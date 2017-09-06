import numpy as np
from kimpy.calculator import KIMModelCalculator
from ase.lattice.cubic import SimpleCubic, FaceCenteredCubic
import pytest


def test_forces():

  # create calculator
  modelname = 'ex_model_Ar_P_MLJ_C'
  calc = KIMModelCalculator(modelname)

  species = calc.get_kim_model_supported_species()

  print species

if __name__ == '__main__':
  test_forces()
