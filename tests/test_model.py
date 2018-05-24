from __future__ import absolute_import, division, print_function
import numpy as np
import kimpy
from neighlist import neighlist as nl
from error import check_error, report_error
from ase.lattice.cubic import FaceCenteredCubic


def create_fcc_argon():
  argon = FaceCenteredCubic(
    directions=[[1,0,0], [0,1,0], [0,0,1]], size=(2,2,2), symbol='Ar',
    pbc=(0,0,0), latticeconstant=5.26)
  return argon


def test_main():

  modelname = 'ex_model_Ar_P_Morse_07C'

  # create model
  requestedUnitsAccepted, kim_model, error = kimpy.model.create(
     kimpy.numbering.zeroBased,
     kimpy.length_unit.A,
     kimpy.energy_unit.eV,
     kimpy.charge_unit.e,
     kimpy.temperature_unit.K,
     kimpy.time_unit.ps,
     modelname
  )
  check_error(error, 'kimpy.model.create')


  # units
  l_unit,e_unit,c_unit,te_unit,ti_unit = kim_model.get_units()
  check_error(error, 'kim_model.get_units')
  print('Length unit is:', str(l_unit))
  print('Energy unit is:', str(e_unit))
  print('Charge unit is:', str(c_unit))
  print('Temperature unit is:', str(te_unit))
  print('Time unit is:', str(ti_unit))
  print()



  # create compute arguments
  compute_arguments, error = kim_model.compute_arguments_create()
  check_error(error, 'kim_model.compute_arguments_create')


  # check compute arguments
  num_compute_arguments = kimpy.compute_argument_name.get_number_of_compute_arguments()
  print('Number of compute_arguments:', num_compute_arguments)

  for i in range(num_compute_arguments):

    name,error = kimpy.compute_argument_name.get_compute_argument_name(i)
    check_error(error, 'kim_model.get_compute_argument_name')

    dtype,error = kimpy.compute_argument_name.get_compute_argument_data_type(name)
    check_error(error, 'kim_model.get_compute_argument_data_type')

    support_status, error = compute_arguments.get_argument_support_status(name)
    check_error(error, 'compute_argument.get_argument_support_status')

    print('Compute Argument name "{}" of type: "{}" and has support status '
        '"{}".'.format(name, dtype, support_status))

    # can only handle energy and force as a required arg
    if support_status == kimpy.support_status.required:
      if (name != kimpy.compute_argument_name.partialEnergy or
          name != kimpy.compute_argument_name.partialForces):
        report_error('Unsupported required ComputeArgument')

    # must have energy and forces
    if (name == kimpy.compute_argument_name.partialEnergy or
        name == kimpy.compute_argument_name.partialForces):
      if (not (support_status == kimpy.support_status.required or
               support_status == kimpy.support_status.optional)):
        report_error('Energy or forces not available')
  print()


  # check compute callbacks
  num_callbacks = kimpy.compute_callback_name.get_number_of_compute_callbacks()
  print('Number of callbacks:', num_callbacks)

  for i in range(num_callbacks):

    name,error = kimpy.compute_callback_name.get_compute_callback_name(i)
    check_error(error, 'kim_model.get_compute_callback_name')

    support_status, error = compute_arguments.get_callback_support_status(name)
    check_error(error, 'compute_argument.get_callback_support_status')

    print('Compute callback "{}" of has support status "{}".'.format(
        name, support_status))

    # cannot handle any "required" callbacks
    if support_status == kimpy.support_status.required:
      report_error('Unsupported required ComputeCallback')
  print()


  # parameter
  num_params = kim_model.get_number_of_parameters()
  print('Number of parameters:', num_params)
  print()

  for i in range(num_params):
    out = kim_model.get_parameter_data_type_extend_and_description(i)
    dtype, extent, description, error = out
    check_error(error, 'kim_model.get_parameter_data_type_extend_and_description')

    print('Parameter No. {} has data type "{}" with extent and description: '
      '"{}".'.format(dtype, extent, description))


  # register argument
  argon = create_fcc_argon()

  coords = np.asarray(argon.get_positions(), dtype='double')
  N = coords.shape[0]
  forces = np.zeros((N, 3), dtype='double')
  energy = np.array([0.], dtype='double')
  num_particles = np.array([N], dtype='intc')
  species_code = np.zeros(num_particles, dtype='intc')
  particle_contributing = np.zeros(num_particles, dtype='intc')

  error = compute_arguments.set_argument_pointer(
      kimpy.compute_argument_name.numberOfParticles, num_particles)
  check_error(error, 'kimpy.compute_argument.set_argument_pointer')

  error = compute_arguments.set_argument_pointer(
      kimpy.compute_argument_name.particleSpeciesCodes, species_code)
  check_error(error, 'kimpy.compute_argument.set_argument_pointer')

  error = compute_arguments.set_argument_pointer(
      kimpy.compute_argument_name.particleContributing, particle_contributing)
  check_error(error, 'kimpy.compute_argument.set_argument_pointer')

  error = compute_arguments.set_argument_pointer(
      kimpy.compute_argument_name.coordinates, coords)
  check_error(error, 'kimpy.compute_argument.set_argument_pointer')

  error = compute_arguments.set_argument_pointer(
      kimpy.compute_argument_name.partialEnergy, energy)
  check_error(error, 'kimpy.compute_argument.set_argument_pointer')

  error = compute_arguments.set_argument_pointer(
      kimpy.compute_argument_name.partialForces, forces)
  check_error(error, 'kimpy.compute_argument.set_argument_pointer')


  # create neighbor list
  neigh = nl.initialize()

  # register get neigh callback

  error = compute_arguments.set_callback_pointer(
      kimpy.compute_callback_name.GetNeighborList,
      kimpy.language_name.cpp,
      nl.get_neigh_kim(),
      neigh
    )   #TODO ask Ryan to add python
  check_error(error, 'kimpy.compute_argument.set_callback_pointer')


  # influence distance and cutoff of model
  model_influence_dist = kim_model.get_influence_distance()
  model_cutoffs = kim_model.get_neighbor_list_cutoffs()
  if(model_cutoffs.size != 1):
    report_error('too many cutoffs')
  print('Model influence distance:', model_influence_dist)
  print('Model cutoff:', model_cutoffs)
  print()

   # species support and code
  species_support, code, error = kim_model.get_species_support_and_code(
      kimpy.species_name.Ar)
  check_error(error or not species_support, 'kim_model.get_species_support_and_code')
  print('Species Ar is supported and its code is:', code)
  print()


  # setup particle species
  species_code[:] = code

  # setup particleContributing
  particle_contributing[:] = 1

  # setup neighbor list
  need_neigh = np.ones(N, dtype='intc')
  error = nl.build(neigh, model_influence_dist, coords, need_neigh)
  check_error(error, 'nl.build')


  error = kim_model.compute(compute_arguments)
  check_error(error, 'kim_mode.compute')
  print('energy:', energy)
  print('force norm:', np.linalg.norm(forces))


  # destory compute arguments
  error = kim_model.compute_arguments_destroy(compute_arguments)
  check_error(error, 'kim_model.compute_arguments_destroy')

  # destory model
  kimpy.model.destroy(kim_model)


if __name__ == '__main__':
  test_main()
