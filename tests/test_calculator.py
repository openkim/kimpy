from __future__ import print_function
import numpy as np
from kimpy import kimapi as km
from kimpy.calculator import KIMModelCalculator
from ase.lattice.cubic import SimpleCubic

def test_calculator():

  # create atoms object
  argon = SimpleCubic(directions=[[1,0,0], [0,1,0], [0,0,1]], size=(2,2,2),
                      symbol='Ar', pbc=(1,1,0), latticeconstant=3.0)

  # create calculator
  modelname = 'ex_model_Ar_P_MLJ_C'
  calc = KIMModelCalculator(modelname)

  # attach calculator to atoms
  argon.set_calculator(calc)

  pos = argon.get_positions()
  energy = argon.get_potential_energy()
  forces = argon.get_forces()

  print('KIM model:', modelname)
  print('\ncoords:\n', pos)
  print('\nenergy:\n', energy)
  print('\nforces:\n', np.reshape(forces, (-1, 3)))

if __name__ == '__main__':
  test_calculator()
