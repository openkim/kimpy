#from __future__ import absolute_import, division, print_function
import numpy as np
import kimpy
from error import check_error

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

  kim_compute_arguments, error = kim_model.compute_arguments_create()
  check_error(error, 'kim_model.compute_arguments_create')
  error = kim_model.compute_arguments_destroy(kim_compute_arguments)

  #help(kimpy.model)
  help(kimpy.model.Model)


if __name__ == '__main__':
  test_main()
