from __future__ import absolute_import, division, print_function
import numpy as np
import kimpy
from error import check_error, report_error

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


  # species support and code
  species_support, code, error = kim_model.get_species_support_and_code(
      kimpy.species_name.Ar)
  check_error(error or not species_support, 'kim_model.get_species_support_and_code')
  print('Species Ar is supported and its code is:', code)
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

  for i in range(num_params):
    out = kim_model.get_parameter_data_type_extend_and_description(i)
    dtype, extent, description, error = out
    check_error(error, 'kim_model.get_parameter_data_type_extend_and_description')

    print('Parameter No. {} has data type "{}" with extent and description: '
      '"{}".'.format(dtype, extent, description))




  error = kim_model.compute_arguments_destroy(compute_arguments)


  #help(kimpy.model)
  #help(kimpy.model.Model)


if __name__ == '__main__':
  test_main()
