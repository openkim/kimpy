from __future__ import print_function
import numpy as np
from kimpy.calculator import KIMModelCalculator
from ase.lattice.cubic import SimpleCubic, FaceCenteredCubic
import time

def print_values(atoms, msg):
  # compute energy and forces
  start = time.time()
  pos = atoms.get_positions()
  energy = atoms.get_potential_energy()
  forces = atoms.get_forces()
  print()
  print('='*80)
  print(msg)
  print('\ncoords:\n', pos)
  print('\nenergy:\n', energy)
  print('\nforces:\n', np.reshape(forces, (-1, 3)))
  print ('\nrunning time:', time.time() - start)


def test_calculator():

  # create calculator
  modelname = 'ex_model_Ar_P_MLJ_C'
  calc = KIMModelCalculator(modelname)

  # create a SC cyrstal
  argon = SimpleCubic(directions=[[1,0,0], [0,1,0], [0,0,1]], size=(2,2,2),
                      symbol='Ar', pbc=(1,1,0), latticeconstant=3.0)

  # attach calculator to atoms
  argon.set_calculator(calc)

  # compute energy and forces
  print_values(argon, 'SC argon, pbc=(1,1,0)')

  # change pbc, and then compute energy and forces
  argon.set_pbc([0,0,0])
  print_values(argon, 'SC argon, pbc=(0,0,0)')

  # create an FCC crystal
  argon2 = FaceCenteredCubic(directions=[[1,0,0], [0,1,0], [0,0,1]], size=(2,2,2),
                             symbol='Ar', pbc=(1,1,0), latticeconstant=3.0)

  # attach the SAME calculator to the new atoms object
  argon2.set_calculator(calc)

  # compute energy and forces
  print_values(argon2, 'FCC argon, pbc=(1,1,0)')



if __name__ == '__main__':
  test_calculator()
