# kimpy

[![Build Status](https://travis-ci.org/openkim/kimpy.svg?branch=master)](https://travis-ci.org/openkim/kimpy)
[![Python package](https://github.com/openkim/kimpy/workflows/Python%20package/badge.svg)](https://github.com/openkim/kimpy/actions)
[![Anaconda-Server Badge](https://img.shields.io/conda/vn/conda-forge/kimpy.svg)](https://anaconda.org/conda-forge/kimpy)
[![PyPI](https://img.shields.io/pypi/v/kimpy.svg)](https://pypi.python.org/pypi/kimpy)
[![License](https://img.shields.io/badge/license-CDDL--1.0-blue)](LICENSE.CDDL)

kimpy is a Python interface to the [KIM API](https://openkim.org/kim-api).

## Installing kimpy

### Requirements

- KIM API 2 package

To install `kimpy`, you need [KIM API](https://openkim.org/kim-api). The
easiest option for obtaining the KIM API is to install the `kim-api`
pre-compiled binary package for your preferred operating system or package
manager. See
[Obtaining KIM Models](https://openkim.org/doc/usage/obtaining-models) for
instructions to install the KIM API and the models from OpenKIM on your system.

You can
[install](https://openkim.org/doc/usage/obtaining-models#installing_api)
the KIM API from source.

- Python 3.6 or later

You need Python 3.6 or later to run `kimpy`. You can have multiple
Python versions (2.x and 3.x) installed on the same system without problems.

To install Python 3 for different Linux flavors, macOS and Windows, packages
are available at\
[https://www.python.org/getit/](https://www.python.org/getit/)

### Using pip

**pip** is the most popular tool for installing Python packages, and the one
included with modern versions of Python.

`kimpy` can be installed with `pip`:

```sh
pip install kimpy
```

**Note:**

Depending on your Python installation, you may need to use `pip3` instead of
`pip`.

```sh
pip3 install kimpy
```

Depending on your configuration, you may have to run `pip` like this:

```sh
python3 -m pip install kimpy
```

### Using pip (GIT Support)

`pip` currently supports cloning over `git`

```sh
pip install git+https://github.com/openkim/kimpy.git
```

For more information and examples, see the
[pip install](https://pip.pypa.io/en/stable/reference/pip_install/#id18)
reference.

### Using conda

**conda** is the package management tool for Anaconda Python installations.

Installing `kimpy` from the `conda-forge` channel can be achieved by adding
`conda-forge` to your channels with:

```sh
conda config --add channels conda-forge
```

Once the `conda-forge` channel has been enabled, `kimpy` can be installed with:

```sh
conda install kimpy
```

It is possible to list all of the versions of `kimpy` available on your
platform with:

```sh
conda search kimpy --channel conda-forge
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
        compute_argument_name
        ...
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
    get_compute_argument_name(...)

    FUNCTIONS
        get_compute_argument_name(...)
        get_compute_argument_name(index: int) -> tuple
        Return(ComputeArgumentName, error)
    ```

    shows that the function `get_compute_argument_name` takes an integer
    `index` as input, and returns a tuple of two outputs: `ComputeArgumentName`
    and `error`. You can refer to `KIM API` docs for the meaning of the input
    and outputs.

    All the attributes of the module are listed under `DATA`. For example,

    ```sh
    DATA
        coordinates = coordinates
        numberOfParticles = numberOfParticles
        ...
        particleSpeciesCodes = particleSpeciesCodes
    ```

## Copyright

Copyright (c) 2017-2020, Regents of the University of Minnesota.\
All Rights Reserved

## Contributing

Contributors:\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mingjian Wen\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Yaser Afshar

## Contact

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mingjian Wen (wenxx151@umn.edu)
