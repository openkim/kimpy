import numpy as np
from kimpy.calculator import KIMModelCalculator
from ase.lattice.cubic import SimpleCubic, FaceCenteredCubic
import pytest

energy_ref = 112.921634093

forces_ref = np.array([[ -7.23227863, -139.33816177, -139.33816177],
                       [  3.77011623,  139.34545864, -139.24229951],
                       [  3.77011623, -139.24229951,  139.34545864],
                       [ -0.30795382,  139.23500265,  139.23500265]]
                     )

def test_forces():

  # create calculator
  modelname = 'ex_model_Ar_P_MLJ_C'
  calc = KIMModelCalculator(modelname)

  # create an FCC crystal
  argon = FaceCenteredCubic(directions=[[1,0,0], [0,1,0], [0,0,1]], size=(1,1,1),
                            symbol='Ar', pbc=(1,0,0), latticeconstant=3.0)

  # perturb the x coord of the first atom
  argon.positions[0,0] += 0.01

  # attach calculator to the atoms
  argon.set_calculator(calc)

  # get energy and forces
  energy = argon.get_potential_energy()
  forces = argon.get_forces()

  tol = 1e-6
  assert energy == pytest.approx(energy_ref, tol)
  assert forces == pytest.approx(forces_ref, tol)


if __name__ == '__main__':
  test_forces()
