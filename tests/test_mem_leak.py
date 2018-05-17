import numpy as np
from kimpy.calculator import KIMModelCalculator
from ase.lattice.cubic import SimpleCubic, FaceCenteredCubic
import pytest

"""
To run:

$ valgrind --leak-check=full --suppressions=valgrind-python.supp python -E -tt ./test_mem_leak.py

"""


def test_forces():

  # create calculator
  modelname = 'ex_model_Ar_P_MLJ_C'
  modelname = 'EMT_Asap_Standard_Jacobsen_Stoltze_Norskov_AlAgAuCuNiPdPt__MO_118428466217_002'
  #modelname = 'Three_Body_Stillinger_Weber_Si__MO_405512056662_003'
  calc = KIMModelCalculator(modelname)

  # create an FCC crystal
  argon = FaceCenteredCubic(directions=[[1,0,0], [0,1,0], [0,0,1]], size=(1,1,1),
                            symbol='Al', pbc=(1,0,0), latticeconstant=3.0)

  # perturb the x coord of the first atom
  argon.positions[0,0] += 0.01

  # attach calculator to the atoms
  argon.set_calculator(calc)

  # get energy and forces
  energy = argon.get_potential_energy()
  forces = argon.get_forces()

if __name__ == '__main__':
  test_forces()
