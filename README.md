# kimpy

This is the Python interface to the KIM API V2.x.x.

- TODO add a short description of KIM.

For more information about the KIM API, please see: https://openkim.org/kim-api/

## Installation

0. Virtual environment (optional)

Create a virtual environment using `conda` (see [here](https://conda.io/miniconda.html) for more information about `conda` and `Miniconda`)
```
$ conda create -n kimpy
```
and then activate the environment
```
$ source activate kimpy
```
1. install from source

Clone this repo
```
$ git clone https://github.com/mjwen/kimpy.git
```
and then install by
```
$ pip install -e ./kimpy
```

2. pip (to come)

3. conda (to come)

To check that `kimpy` is sussessfully installed, you can do
```
$ cd kimpy/tests
$ pytest
```
and you will get something like
```
...
collected 13 items

test_charge_unit.py .
.
.
.
test_time_unit.py .

==================== 13 passed in 0.18 seconds ====================
```

## Example

For an example of using the python interface, take a look at `kimpy/tests/test_model.py`.

## Help

Use `help(object)` to get help for any objects in the package (including the `kimpy` package itself and any module, class, and function in the package.)

For exampe:

1. To list all the modules in the package, do

```
$ python
>>> import kimpy
>>> help(kimpy)
```

and then you can find all the available modules under `PACKAGE CONTENTS` as

```
PACKAGE CONTENTS
	charge_unit
	compute_argument_name
	...
	time_unit
```

2. To inspect the `compute_argument_name` module, do

```
$ python
>>> import kimpy
>>> help(kimpy.compute_argument_name)
```

All the functions are listed under `FUNCTIONS`. For example,

``` get_compute_argument_name(...)
FUNCTIONS
	get_compute_argument_name(...)
		get_compute_argument_name(index: int) -> tuple

		Return(ComputeArgumentName, error)
```

shows that the function `get_compute_argument_name` takes an integer `index` as input, and returns a tuple of two outputs: `ComputeArgumentName` and `error`. You can refer to`KIM API` docs for the meaning of the input and outputs.

All the attributes of the module are listed under `DATA`. For example,

    DATA
    	coordinates = coordinates
    	numberOfParticles = numberOfParticles
    	...
    	particleSpeciesCodes = particleSpeciesCodes

## API

The python interface is designed to closely mimic the C++ API with only a few exceptions. These are mainly related to functions that return pointers, and in the python interface we convert these functions to return data. Explicitly, these includes:

- `GetNeighborListCutoffsPointer`, a member funciton of class `Model`,  defined in C++ API as

```cpp
void GetNeighborListCutoffsPointer(int * const numberOfCutoffs, double const ** const cutoffs) const;
```


In the python interface it is converted to

```python
cutoffs = Model.get_neighbor_list_cutoffs()
```

where `cutoffs` is a numpy 1D array contains `numberOfCutoffs = len(cutoffs)` values.

## Contact

Mingjian Wen (wenxx151@umn.edu)
