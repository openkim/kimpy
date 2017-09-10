import numpy as np
from kimpy.calculator import KIMModelCalculator
from ase import Atoms
import pytest

energy_ref = 99210.8414132
forces_ref =np.array([[-1190915.62324912,  0.,  0.],
                      [ 1190915.62324912,  0.,  0.]]
                    )

def test_1D():

  model = 'ex_model_Ar_P_MLJ_C'
  calc = KIMModelCalculator(model,debug=True)

  dimer = Atoms('ArAr', positions=[(0,0,0),(1,0,0)], cell=(10,10,10), pbc=(1,1,1))
  dimer.set_calculator(calc)

  energy = dimer.get_potential_energy()
  forces = dimer.get_forces()

  tol = 1e-6
  assert energy == pytest.approx(energy_ref, tol)
  assert forces == pytest.approx(forces_ref, tol)

if __name__ == '__main__':
  test_1D()
