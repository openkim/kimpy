from __future__ import print_function
import numpy as np
from kimpy.calculator import KIMModelCalculator
import kimpy
from ase.lattice.cubic import SimpleCubic, FaceCenteredCubic
import pytest


def test_forces():

  # create calculator
  modelname = 'ex_model_Ar_P_MLJ_C'
  #modelname = 'EAM_Dynamo_Angelo_Moody_NiAlH__MO_418978237058_001'

  # get from calc
  calc = KIMModelCalculator(modelname)
  species = calc.get_kim_model_supported_species()
  species = [str(s) for s in species]
  print ('The species supported by model "{}" is (are): {}'.format(modelname, species))

  # get directly
  species = kimpy.calculator.get_model_species_list(modelname)
  species = [str(s) for s in species]
  print ('The species supported by model "{}" is (are): {}'.format(modelname, species))

if __name__ == '__main__':
  test_forces()
