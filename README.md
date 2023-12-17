# kimpy

![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/openkim/kimpy/testing.yml)
[![Anaconda-Server Badge](https://img.shields.io/conda/vn/conda-forge/kimpy.svg)](https://anaconda.org/conda-forge/kimpy)
[![PyPI](https://img.shields.io/pypi/v/kimpy.svg)](https://pypi.python.org/pypi/kimpy)
[![License](https://img.shields.io/badge/license-CDDL--1.0-blue)](LICENSE.CDDL)

kimpy is a Python interface to the [KIM API](https://openkim.org/kim-api).

## Installing kimpy

`kimpy` requires [KIM AIP](https://openkim.org/kim-api/) and `Python 3.7` or later.

### Using conda

The recommended (and the easiest) way to install `kimpy` is to use the conda package manager to install both the KIM API and kimpy from the conda-forge channel.

```sh
conda install -c conda-forge kim-api kimpy
```

### Using pip

This requires that the KIM API alreay be installed on your system. See the KIM-API [docs](https://openkim.org/kim-api/) for instructions on how to do this.

```sh
pip install kimpy
```

### From source

This also requires that the KIM API alreay be installed on your system. See above.

```sh
git clone https://github.com/openkim/kimpy.git
pip install -e ./kimpy
```

## Example

For an example of using `kimpy`, take a look at `kimpy/tests/test_model.py`.

## Help

kimpy is designed to closely mimic the C++ KIM API with only a few changes of
names. In case one wants to know the names and arguments of a class or
function, Use `help(object)` to get help for any objects in the package.

For exampe:

1. To list all the modules in the package, do

   ```sh
   $ python
   >>> import kimpy
   >>> help(kimpy)
   ```

   and then you can find all the available modules under `PACKAGE CONTENTS` as

   ```sh
   PACKAGE CONTENTS
       charge_unit
       collection
       collection_item_type
       collections
       compute_argument_name
       ...
       temperature_unit
       time_unit
   ```

2. To inspect the `compute_argument_name` module, do

   ```sh
   $ python
   >>> import kimpy
   >>> help(kimpy.compute_argument_name)
   ```

   All the functions are listed under `FUNCTIONS`. For example,

   ```sh
   FUNCTIONS
       get_compute_argument_data_type(...) method of builtins.PyCapsule instance
           get_compute_argument_data_type(compute_argument_name: kimpy.compute_argument_name.ComputeArgumentName) -> KIM::DataType


           Get the data_type of each defined standard compute_argument_name.

           Returns:
               DataType: data_type
       get_compute_argument_name(...) method of builtins.PyCapsule instance
           get_compute_argument_name(index: int) -> kimpy.compute_argument_name.ComputeArgumentName


           Get the identity of each defined standard compute_argument_name.

           Returns:
               ComputeArgumentName: compute_argument_name

       get_number_of_compute_argument_names(...) method of builtins.PyCapsule instance
           get_number_of_compute_argument_names() -> int


           Get the number of standard compute_argument_name's defined by the KIM-API.

           Returns:
               int: number_of_compute_arguments
   ```

   shows that the function `get_compute_argument_name` takes an integer
   `index` as input, and returns an output: `compute_argument_name`. You can
   refer to `KIM API` docs for further information on the input and outputs.

   All the attributes of the module are listed under `DATA`. For example,

   ```sh
   DATA
       coordinates = coordinates
       numberOfParticles = numberOfParticles
       ...
       particleSpeciesCodes = particleSpeciesCodes
   ```

## Copyright

Copyright (c) 2017-2023, Regents of the University of Minnesota.\
All Rights Reserved

## Contributing

Contributors:\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mingjian Wen\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Yaser Afshar
