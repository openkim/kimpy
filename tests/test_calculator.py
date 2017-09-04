from __future__ import print_function
from kimpy import kimapi as km
from kimpy.calculator import KIMModelCalculator
from ase.lattice.cubic import SimpleCubic

def test_calculator():

  # create atoms object
  argon = SimpleCubic(directions=[[1,0,0], [0,1,0], [0,0,1]], size=(2,2,2),
                      symbol='Ar', pbc=(1,1,0), latticeconstant=3.0)
  print(argon.get_cell())
  print(argon.get_positions())

  # create calculator
  modelname = 'ex_model_Ar_P_MLJ_C'
  calc = KIMModelCalculator(modelname)

  # attach calculator to atoms
  argon.set_calculator(calc)

  energy = argon.get_potential_energy()
  forces = argon.get_forces()
  print('energy =', energy)
  print('forces =', forces)


if __name__ == '__main__':
  test_calculator()
