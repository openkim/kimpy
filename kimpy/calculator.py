from __future__ import absolute_import, division
import numpy as np
from ase.calculators.calculator import Calculator
from kimpy import kimapi as km
from kimpy import neighborlist as nl

__version__ = '0.0.1'
__anthor__ = 'Mingjian Wen'



class KIMModelCalculator(Calculator):
  """ An ASE calculator based on Knowledge base of Interatomic Models.

  Parameter
  ---------

  modelname: str
    KIM model name

  padding_need_neigh: bool
    Flag to indicate whether need to create neighbors for padding atoms.
  """

  implemented_properties = ['energy', 'forces']


  def __init__(self, modelname, padding_need_neigh=False, **kwargs):
    Calculator.__init__(self, **kwargs)

    # kim attributes
    self.modelname = modelname
    self.padding_need_neigh = padding_need_neigh
    self.pkim = None

    # pointers registed to kim object
    self.km_nparticles = None
    self.km_nspecies = None
    self.km_particle_code = None
    self.km_coords = None
    self.km_cutoff = None

    # number of contributing atoms and padding image
    self.ncontrib = None
    self.pad_image = None


  def set_atoms(self, atoms):
    """Initialize KIM object.
    Called by Atoms class in function set_calculator.

    Parameter
    ---------

    atoms: ASE Atoms instance
    """
    if self.pkim is not None:
      self.free_kim()
    else:
      self.init_kim(atoms)


  def free_kim(self):
    """Free KIM neigh object, KIM Model and KIM object. """
    nl.clean(self.pkim)
    km.model_destroy(self.pkim)
    km.free(self.pkim)
    km.pkim = None


  def init_kim(self, atoms):
    """Initialize KIM object.

    Parameter
    ---------

    atoms: ASE Atoms instance
    """

    # get info form Atoms object
    nparticles = atoms.get_number_of_atoms()
    coords = np.ravel(atoms.get_positions())
    cell = atoms.get_cell()
    pbc = atoms.get_pbc()
    particle_species = atoms.get_chemical_symbols()
    unique_species = list(set(particle_species))
    nspecies = len(unique_species)

    self.ncontrib = nparticles

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


    # init kim data
    if any(pbc):
      # create padding atoms if necessary
      # NOTE this is implemented in python, replace by c for effience
      pad_coords, pad_species, self.pad_image = set_padding(
          cell, pbc, particle_species, coords, self.km_cutoff[0])
      npad = len(pad_species)

      self.km_nparticles = np.array([nparticles + npad], dtype=np.intc)
      km_particle_species = np.concatenate((particle_species, pad_species))
      self.km_coords = np.concatenate((coords, pad_coords)).astype(np.double)
      is_padding = np.zeros(self.km_nparticles[0], dtype=np.intc)
      is_padding[nparticles:] = np.ones(npad, dtype=np.intc)

    else:
      self.km_nparticles = np.array([nparticles], dtype=np.intc)
      km_particle_species = particle_species
      self.km_coords = np.array(coords, dtype=np.double)
      is_padding = np.zeros(nparticles, dtype=np.intc)

    # NOTE for debug use only
    #write_extxyz(cell, particle_species, self.km_coords, fname='check_set_padding.xyz')


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


    # set KIM API object input pointers
    km.set_data_int(self.pkim, "numberOfParticles", self.km_nparticles)
    km.set_data_double(self.pkim, "coordinates", self.km_coords)
    km.set_data_int(self.pkim, "numberOfSpecies", self.km_nspecies)
    km.set_data_int(self.pkim, "particleSpecies", self.km_particle_code)

    # initialize energy and forces and register their KIM pointer (output of KIM object)
    self.km_energy = np.array([0.], dtype=np.double)
    self.km_forces = np.array([0.0]*(3*self.km_nparticles[0]), dtype=np.double)
    km.set_data_double(self.pkim, "energy", self.km_energy)
    km.set_data_double(self.pkim, "forces", self.km_forces)


    # initialize and create neighbor list
    status = nl.initialize(self.pkim)
    if status != km.STATUS_OK:
      km.report_error('nl.initialize', status)

    nl.build_neighborlist(self.pkim, is_padding, self.padding_need_neigh)
    if status != km.STATUS_OK:
      km.report_error('nl.build_neighborlist', status)


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

    status = km.model_compute(self.pkim)
    if status != km.STATUS_OK:
      km.report_error('km.model_compute', status)

    energy = self.km_energy[0]
    forces = self.km_forces
    forces = assemble_padding_forces(forces, self.ncontrib, self.pad_image)

    # return values
    self.results['energy'] = energy
    if 'forces' in properties:
      self.results['forces'] = forces



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
  kimstr += 'reinit   method  none    []\n'
  kimstr += 'destroy  method  none    []\n'
  kimstr += 'cutoff   double  length  []\n'
  kimstr += 'energy   double  energy  []\n'
  kimstr += 'forces   double  force   [numberOfParticles,3]\n'
# TODO we may want to enable particleEnergy
  #kimstr += 'particleEnergy  double  energy  [numberOfParticles]\n'

  return kimstr


def assemble_padding_forces(forces, Ncontrib, pad_image):
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

  forces = np.reshape(forces, (-1, DIM))
  pad_forces = forces[Ncontrib:, :]
  pad_iamge = np.array(pad_image)

  for i in xrange(Ncontrib):
    # idx: the indices of padding atoms that are images of contributing atom i
    idx = np.where(pad_image == i)
    forces[i] += np.sum(pad_forces[idx], axis=0)

  # return forces of contributing atoms
  return np.reshape(forces[:Ncontrib], DIM*Ncontrib)



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
  for i in range(-size0, size0+1):
    for j in range(-size1, size1+1):
      for k in range(-size2, size2+1):
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


