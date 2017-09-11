from __future__ import absolute_import, division, print_function
import numpy as np
from ase.calculators.calculator import Calculator
from kimpy import kimapi as km
from kimpy import neighborlist as nl


__anthor__ = 'Mingjian Wen'


class KIMModelCalculator(Calculator):
  """ An ASE calculator based on Knowledge base of Interatomic Models.

  Parameter
  ---------

  modelname: str
    KIM model name

  neigh_skin_ratio: double
    The neighbor list is build using r_neigh = (1+neigh_skin_ratio)*rcut.

  padding_need_neigh: bool
    Flag to indicate whether need to create neighbors for padding atoms.

  debug: bool
    Flag to indicate wheter to use debug mode or not. If Yes, the latest
    configuraton with padding atoms setting will be written to 'config.xyz'.
  """

  implemented_properties = ['energy', 'forces']


  def __init__(self, modelname, neigh_skin_ratio=0.2, padding_need_neigh=False,
               debug=False, **kwargs):
    Calculator.__init__(self, **kwargs)

    self.modelname = modelname
    self.debug = debug

    # neigh attributes
    if neigh_skin_ratio < 0:
      neigh_skin_ratio = 0
    self.neigh_skin_ratio = neigh_skin_ratio
    self.padding_need_neigh = padding_need_neigh
    self.skin = None
    self.cutoff = None
    self.last_update_positions = None

    # padding related
    self.is_padding = None
    self.pad_image = None
    self.ncontrib = None

    # pointers registed to kim object
    self.pkim = None
    self.km_nparticles = None
    self.km_nspecies = None
    self.km_particle_code = None
    self.km_coords = None
    self.km_cutoff = None


  def set_atoms(self, atoms):
    """Initialize KIM object.
    Called by set_calculator() of Atoms class.

    Parameter
    ---------

    atoms: ASE Atoms instance
    """
    if self.pkim is not None:
      self.free_neigh_and_kim()
    self.init_kim_and_neigh(atoms)


  def init_kim_and_neigh(self, atoms):
    """Initialize KIM object and neighbor list.

    Parameter
    ---------

    atoms: ASE Atoms instance
    """

    # get species info from Atoms object
    particle_species = atoms.get_chemical_symbols()
    unique_species = list(set(particle_species))
    nspecies = len(unique_species)

    # create kim object
    test_kimstr = generate_kimstr(self.modelname, unique_species)
    self.pkim, status = km.string_init(test_kimstr, self.modelname)
    if status != km.STATUS_OK:
      km.report_error('km.string_init', status)

    # init model
    # memory for `cutoff' must be allocated and registered before calling model_init
    self.km_cutoff = np.array([0.], dtype=np.double)
    status = km.set_data_double(self.pkim, "cutoff", self.km_cutoff)
    if status != km.STATUS_OK:
      km.report_error('km.set_data_double', status)

    status = km.model_init(self.pkim)
    if status != km.STATUS_OK:
      km.report_error('km.model_init', status)

    # set cutoff
    self.skin = self.neigh_skin_ratio * self.km_cutoff[0]
    self.cutoff = (1+self.neigh_skin_ratio) * self.km_cutoff[0]

    # initialize neighbor list
    status = nl.initialize(self.pkim)
    if status != km.STATUS_OK:
      km.report_error('nl.initialize', status)


  def update_kim(self, atoms):
    """ Register KIM input and output data pointers, and build neighbor list.

    Each time called, new KIM input data arrays will be created according to the
    atoms object, and then registered in KIM API.

    Parameter
    ---------

    atoms: ASE Atoms instance
    """
#NOTE ask Prof. Elliott, about garbage collection. New pointers will be registered
# in the KIM object, Will the previous registerd one be freed automatically?
# Provided that KIM object is the only one that has reference to the data?
# 1. python is passed by reference.
# 2. Maintain reference count. when referneced, counts++, when deferenced, counts--,
# when counts == 0. Free.
#
# What I did, pass numpy array, and obtained the pointer to the numpy array data, and
# then register in KIM.
# Q: does this counts are reference, and when pointer deattached, does it count as
# dereference.

    # get info from Atoms object
    nparticles = atoms.get_number_of_atoms()
    coords = np.ravel(atoms.get_positions())
    cell = atoms.get_cell()
    pbc = atoms.get_pbc()
    particle_species = atoms.get_chemical_symbols()
    unique_species = list(set(particle_species))
    nspecies = len(unique_species)
    self.ncontrib = nparticles

    # init KIM API input data

    # create padding atoms if necessary
    if any(pbc):
      # NOTE this is implemented in python, replace by c for effience
      pad_coords, pad_species, self.pad_image = set_padding(
          cell, pbc, particle_species, coords, self.cutoff)
      npad = len(pad_species)

      self.km_nparticles = np.array([nparticles + npad], dtype=np.intc)
      km_particle_species = np.concatenate((particle_species, pad_species))
      self.km_coords = np.concatenate((coords, pad_coords)).astype(np.double)
      self.is_padding = np.zeros(self.km_nparticles[0], dtype=np.intc)
      self.is_padding[nparticles:] = np.ones(npad, dtype=np.intc)

    else:
      self.pad_image = None
      self.km_nparticles = np.array([nparticles], dtype=np.intc)
      km_particle_species = particle_species
      self.km_coords = np.array(coords, dtype=np.double)
      self.is_padding = np.zeros(nparticles, dtype=np.intc)

    if self.debug:
      # write configuratons with paddings
      write_extxyz(cell, km_particle_species, self.km_coords, fname='config.xyz')


    # species code
    self.km_nspecies = np.array([nspecies], dtype=np.intc)

    species_map = dict()
    for s in unique_species:
      code, status = km.get_species_code(self.pkim, s)
      if status != km.STATUS_OK:
        km.report_error('km.get_species_code', status)
      species_map[s] = code
    tmp = [species_map[s] for s in km_particle_species]
    self.km_particle_code = np.array(tmp, dtype=np.intc)


    # register KIM API object input pointers
    km.set_data_int(self.pkim, "numberOfParticles", self.km_nparticles)
    km.set_data_double(self.pkim, "coordinates", self.km_coords)
    km.set_data_int(self.pkim, "numberOfSpecies", self.km_nspecies)
    km.set_data_int(self.pkim, "particleSpecies", self.km_particle_code)


    # initialize and register KIM API object output pointers
    self.km_energy = np.array([0.], dtype=np.double)
    self.km_forces = np.zeros(3*self.km_nparticles[0], dtype=np.double)
    km.set_data_double(self.pkim, "energy", self.km_energy)
    km.set_data_double(self.pkim, "forces", self.km_forces)


  def update_neigh(self):
    """ (Re-)create the neighbor list"""
    status = nl.build_neighborlist(self.pkim, self.cutoff, self.is_padding,
                                   self.padding_need_neigh)
    if status != km.STATUS_OK:
      km.report_error('nl.build_neighborlist', status)


  def free_neigh_and_kim(self):
    """Free KIM neigh object, KIM Model and KIM object. """
    nl.clean(self.pkim)
    km.model_destroy(self.pkim)
    km.free(self.pkim)
    self.pkim = None


  def calculate(self, atoms=None,
                properties=['energy'],
                system_changes=['positions', 'numbers', 'cell', 'pbc']):
    """
    Inherited method from the ase Calculator class that is called by get_property().

    Parameters
    ----------

    atoms: ASE Atoms instance

    properties: list of str
      List of what needs to be calculated.  Can be any combination
      of 'energy' and 'forces'.

    system_changes: list of str
      List of what has changed since last calculation.  Can be
      any combination of these six: 'positions', 'numbers', 'cell',
      and 'pbc'.
    """

    Calculator.calculate(self, atoms, properties, system_changes)

    need_update_neigh = True
    if len(system_changes) == 1 and 'positions' in system_changes:
      if self.last_update_positions is not None:
        a = self.last_update_positions
        b = atoms.positions
        if a.shape == b.shape:
          delta = np.linalg.norm(a - b, axis=1)
          ind = np.argpartition(delta, -2)[-2:]   # indices of the two largest element
          if sum(delta[ind]) <= self.skin:
            need_update_neigh = False

    # update KIM API input data and neighbor list if necessary
    if system_changes:
      self.update_kim(atoms)
      if need_update_neigh:
        if self.debug:
          print ('neighbor list updated')
        self.update_neigh()
        self.last_update_positions = atoms.get_positions()
      status = km.model_compute(self.pkim)
      if status != km.STATUS_OK:
        km.report_error('km.model_compute', status)

    energy = self.km_energy[0]
    forces = self.km_forces
    forces = assemble_padding_forces(forces, self.ncontrib, self.pad_image)

    # return values
    self.results['energy'] = energy
    self.results['forces'] = forces


  def get_kim_model_supported_species(self):
    return get_model_species_list(self.modelname)


  def __str__(self):
    """Print this object shows the following message."""
    return 'KIMModelCalculator(modelname = {})'.format(self.modelname)


  def __del__(self):
    """Garbage collects the KIM neigh objects and KIM object."""
    if self.pkim is not None:
      self.free_neigh_and_kim()



def generate_kimstr(modelname, species):
  """
  Creates a valid KIM string that will be used to initialize the KIM object.

  Parameters
  ----------

  modelname: str
    KIM Model name

  species: list of str
    atomic species of all atoms

  Return
  ------

  kimstring: str
      a string of the KIM file for the configuration object
  """

  # version and units
  kimstr  = 'KIM_API_Version := 1.9.0\n'
  kimstr += 'Unit_length := A\n'
  kimstr += 'Unit_energy := eV\n'
  kimstr += 'Unit_charge := e\n'
  kimstr += 'Unit_temperature := K\n'
  kimstr += 'Unit_time := ps\n'

  # particle species ('code' does not matter)
  kimstr += 'PARTICLE_SPECIES:\n'
  kimstr += '# Symbol/name    Type    code\n'
  for i,s in enumerate(species):
      kimstr += '{}    spec    {}\n'.format(s, i)

  # conversions
  kimstr += 'CONVENTIONS:\n'
  kimstr += 'ZeroBasedLists  flag\n'
  kimstr += 'Neigh_LocaAccess  flag\n'
  kimstr += 'NEIGH_PURE_F  flag\n'

  # model input
  kimstr += 'MODEL_INPUT:\n'
  kimstr += 'numberOfParticles  integer  none    []\n'
  kimstr += 'numberOfSpecies    integer  none    []\n'
  kimstr += 'particleSpecies    integer  none    [numberOfParticles]\n'
  kimstr += 'coordinates        double   length  [numberOfParticles,3]\n'
  kimstr += 'get_neigh          method   none    []\n'
  kimstr += 'neighObject        pointer  none    []\n'

  # model output
  kimstr += "MODEL_OUTPUT:\n"
  kimstr += 'compute  method  none    []\n'
  #kimstr += 'reinit   method  none    []\n'
  kimstr += 'destroy  method  none    []\n'
  kimstr += 'cutoff   double  length  []\n'
  kimstr += 'energy   double  energy  []\n'
  kimstr += 'forces   double  force   [numberOfParticles,3]\n'
# TODO we may want to enable particleEnergy
  #kimstr += 'particleEnergy  double  energy  [numberOfParticles]\n'

  return kimstr


def get_model_species_list(modelname):
  """Create a temporary KIM object to inqure the supported species of the model.

  Return
  ------

  species_list: list of str
    A list of all the supported species by the KIM model.
  """

  pkim, status = km.model_info(modelname)
  if status != km.STATUS_OK:
    km.report_error('km.model_info', status)

  # number of model species
  numberSpecies, maxStrLen, status = km.get_num_model_species(pkim)
  if status != km.STATUS_OK:
    km.report_error('km.get_num_model_species', status)

  # get species list
  species_list = []
  for i in xrange(numberSpecies):
    spec, status = km.get_model_species(pkim, i)
    if status != km.STATUS_OK:
      km.report_error('km.get_model_species', status)
    species_list.append(spec)

  # destroy the temporary model and the KIM object
  km.free(pkim)

  return species_list


def assemble_padding_forces(forces, Ncontrib, pad_image=None):
  """
  Assemble forces on padding atoms back to contributing atoms.

  Parameters
  ----------

  forces: 1D array
    forces on both contributing and padding atoms

  Ncontrib: int
    number of contributing atoms

  pad_iamge, list of int
    atom number, of which the padding atom is an image


  Return
  ------
    forces on contributing atoms with padding forces added.
  """

  DIM = 3

  forces = np.array(forces).reshape(-1, DIM)
  contrib_forces = forces[:Ncontrib]

  if pad_image is None:
    return contrib_forces

  else:
    pad_forces = forces[Ncontrib:]
    pad_image = np.array(pad_image)

    for i in xrange(Ncontrib):
      # idx: the indices of padding atoms that are images of contributing atom i
      indices = np.where(pad_image == i)
      contrib_forces[i] += np.sum(pad_forces[indices], axis=0)

    # return forces of contributing atoms
    return contrib_forces



def set_padding(cell, PBC, species, coords, rcut):
  """ Create padding atoms for PURE PBC so as to generate neighbor list.
  This works no matter rcut is larger or smaller than the boxsize.

  Parameters
  ----------

  cell: 2D array
    supercell lattice vector

  PBC: list
    flag to indicate whether periodic or not in x,y,z diretion

  species: list of string
    atom species symbol

  coords: list
    atom coordiantes

  rcut: float
    cutoff

  Returns
  -------

  abs_coords: list
    absolute (not fractional) coords of padding atoms

  pad_spec: list of string
    species of padding atoms

  pad_image: list of int
    atom number, of which the padding atom is an image
  """

  # transform coords into fractional coords
  coords = np.reshape(coords, (-1, 3))
  tcell = np.transpose(cell)
  fcell = np.linalg.inv(tcell)
  frac_coords = np.dot(coords, fcell.T)
  xmin = min(frac_coords[:,0])
  ymin = min(frac_coords[:,1])
  zmin = min(frac_coords[:,2])
  xmax = max(frac_coords[:,0])
  ymax = max(frac_coords[:,1])
  zmax = max(frac_coords[:,2])

  # compute distance between parallelpiped faces
  volume = np.dot(cell[0], np.cross(cell[1], cell[2]))
  dist0 = volume/np.linalg.norm(np.cross(cell[1], cell[2]))
  dist1 = volume/np.linalg.norm(np.cross(cell[2], cell[0]))
  dist2 = volume/np.linalg.norm(np.cross(cell[0], cell[1]))
  ratio0 = rcut/dist0
  ratio1 = rcut/dist1
  ratio2 = rcut/dist2
  # number of bins to repate in each direction
  size0 = int(np.ceil(ratio0))
  size1 = int(np.ceil(ratio1))
  size2 = int(np.ceil(ratio2))

  # creating padding atoms assume ratio0, 1, 2 < 1
  pad_coords = []
  pad_spec = []
  pad_image = []
  for i in xrange(-size0, size0+1):
    for j in xrange(-size1, size1+1):
      for k in xrange(-size2, size2+1):
        if i==0 and j==0 and k==0:  # do not create contributing atoms
          continue
        if not PBC[0] and i != 0:   # apply BC
          continue
        if not PBC[1] and j != 0:
          continue
        if not PBC[2] and k != 0:
          continue
        for at,(x,y,z) in enumerate(frac_coords):

          # select the necessary atoms to repeate for the most outside bin
          if i == -size0 and x - xmin < size0 - ratio0:
            continue
          if i == size0  and xmax - x < size0 - ratio0:
            continue
          if j == -size1 and y - ymin < size1 - ratio1:
            continue
          if j == size1  and ymax - y < size1 - ratio1:
            continue
          if k == -size2 and z - zmin < size2 - ratio2:
            continue
          if k == size2  and zmax - z < size2 - ratio2:
            continue

          pad_coords.append([i+x,j+y,k+z])
          pad_spec.append(species[at])
          pad_image.append(at)

  # transform fractional coords to abs coords
  if not pad_coords:  # no padding atoms (could be due to non-periodic)
      abs_coords = []
  else:
      abs_coords = np.dot(pad_coords, tcell.T).ravel()

  return abs_coords, pad_spec, pad_image



def write_extxyz(cell, species, coords, fname='config.xyz'):
  """Output configurations as xyz file.

  Parameters
  ----------

  cell: 3x3 array
    Supercell lattice vectors.

  species: list of str
    atom species

  coords: 1D array of size 3*natoms
    atomic coordinates
  """
  with open (fname, 'w') as fout:
    # first line (num of atoms)
    natoms = len(species)
    fout.write('{}\n'.format(natoms))

    # second line
    # lattice
    fout.write('Lattice="')
    for line in cell:
      for item in line:
        fout.write('{} '.format(item))
    fout.write('" ')
    # properties
    fout.write('Properties=species:S:1:pos:R:3\n')

    # species, coords
    if natoms != len(coords)//3:
      print ('Number of atoms is inconsistent from species nad coords.')
      print ('len(specis)=', natoms)
      print ('len(coords)=', len(coords)//3)
      sys.exit(1)
    for i in range(natoms):
      fout.write('{:4}'.format(species[i]))
      fout.write('{:12.5e} '.format(coords[3*i+0]))
      fout.write('{:12.5e} '.format(coords[3*i+1]))
      fout.write('{:12.5e}\n'.format(coords[3*i+2]))

