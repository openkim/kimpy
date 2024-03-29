# Change Log

## v2.1.0 (2022/12/06)

- Fixing inverse function bug in neighbor list, which can result in incorrect
  padding
- Using pyproject.toml, and refactoring build and test requires

## v2.0.1 (2022/08/18)

- Include generated binding and testing file in the repo, instead of generating
  them in the setup.py file. This means each time we modify the source template
  file, we need to run `$ python scripts/generate_all.py` manually
- Drop python3.6 support
- Update to be compatible with kim-api-2.3.0

## v2.0.0 (2021/04/26)

This release is not backward compatible.

- Remove the manual memory management. There is no need to destroy the KIM-API
  objects in Python explicitly. By using smart pointers with custom deleters,
  Python controls the ownership of newly created objects, and the garbage
  collector module is tracking their status.
- The interface is updated, and no error message returns from the KIM-API to the
  users. It throws a RuntimeError exception with a print indicating what went
  wrong in case of an error.
- Input and return arguments are now in lower case.
- Methods are provided with a docstring explaining the purpose.
- NeighList module is updated. One uses NeighList() class to create an instance
  of the NeighList object and calls its methods.
- Find example usage at kimpy/examples/

## v0.3.3 (2019/10/28)

- Update setup.py to not explicitly use `pip` and `python`

- Remove pybind11==2.2.3 version dependence, since the `get_include()` of
  pybind11 is fixed
