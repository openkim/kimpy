# OpenKIM ASE calculator

An Atomic Simulation Environemnt (ASE) calculator based on the Knowledgebase
of Interatomic Models.

## Installation

0. Virtual environment (Optional)

Create a virtual environment using `conda` (see [here](https://conda.io/miniconda.html)
for more information about `conda` and `Miniconda`)

    $ conda create -n kimpy

and then activate the environment

    $ source activate kimpy

1. install from source

Clone this repo

    $ git clone https://github.com/mjwen/kimpy.git

and then install by

    $ pip install ./kimpy

2. pip (to come)

3. conda (to come)

TO check that `kimpy` is sussessfully installed, you can do

    $ cd kimpy/tests
    $ pytest

and you will get something like
```
...
collected 2 items

test_calculator.py .
test_report_error.py .

==================== 2 passed in 0.18 seconds ====================
```

## Example

After successfully intalling the `simpy` package, the ASE calculator can be
imported from the `calculator` module as

    from kimpy.calculator import KIMModelCalculator

The floowing code snippet (`test_argon.py`) shows how to compute the potential
energy and forces of a FCC crystal using the KIM ASE calculator with the KIM
potentail `ex_model_Ar_P_MLJ_C` (get more KIM potenital Models
[here](https://openkim.org/intro-models/)).

```python
from __future__ import print_function
import numpy as np
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
```

Run this code snippet by

    $ python test_argon.py
