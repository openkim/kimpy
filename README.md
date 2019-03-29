# kimpy

[![Build Status](https://travis-ci.com/openkim/kimpy.svg?branch=master)](https://travis-ci.com/openkim/kimpy)

Kimpy is the Python interface to the KIM API. For more information about the KIM API, see: https://openkim.org/kim-api/


## Installation

### Package managers

```
$ pip install kimpy
```

### From source

Get the source
```
$ git clone https://github.com/mjwen/kimpy.git
```
and then install by
```
$ pip install -e ./kimpy
```


## Example

For an example of using `kimpy`, take a look at `kimpy/tests/test_model.py`.

In the above example, the neighbor list is built within python. We provide a [neighbor list building library](https://github.com/mjwen/neighlist) that accelerates the creation of neighbor list and works seamlessly with `kimpy`. See `kimpy/tests/test_model_neigh_library.py`for an example of using this library.


## Help

Kimpy is designed to closely mimic the C++ KIM API with only a few changes of names. In case one wants to know the names and arguments of a class or function, Use `help(object)` to get help for any objects in the package.

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

shows that the function `get_compute_argument_name` takes an integer `index` as input, and returns a tuple of two outputs: `ComputeArgumentName` and `error`. You can refer to `KIM API` docs for the meaning of the input and outputs.

All the attributes of the module are listed under `DATA`. For example,

    DATA
    	coordinates = coordinates
    	numberOfParticles = numberOfParticles
    	...
    	particleSpeciesCodes = particleSpeciesCodes

