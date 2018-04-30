from setuptools import setup, Extension
from distutils.sysconfig import get_config_vars
import sys, subprocess


# remove `-Wstrict-prototypes' that is for C not C++
cfg_vars = get_config_vars()
for key, value in cfg_vars.items():
  if type(value) == str and '-Wstrict-prototypes' in value:
    cfg_vars[key] = value.replace('-Wstrict-prototypes', '')


def inquire_kim_api(option, key, mode):
  try:
    config = subprocess.check_output(['kim-api-v1-build-config', option])
  except:
    raise Exception('"kim-api-v1-build-config" not found on PATH; make sure kim-api '
                    'is installed and "kim-api-v1-build-config" is on PATH.')

  # remove `\n' at end and then split at white space
  split_config = [s for s in config.strip().split(' ')]
  if mode == 0:
    # only collect item starting with key, and remove key in the string
    split_config= [s.replace(key, '') for s in split_config if s.startswith(key)]
  elif mode == 1:
    # only collect item not starting with key
    split_config= [s for s in split_config if not s.startswith(key)]

  return split_config


def get_kim_includes():
  return inquire_kim_api('--includes', '-I', 0)

def get_kim_libdirs():
  return inquire_kim_api('--ldflags', '-L', 0)

def get_kim_ldlibs():
  return inquire_kim_api('--ldlibs', '-l', 0)

#TODO not sure whether we need to add this
def get_kim_extra_link_args():
  return inquire_kim_api('--ldflags', '-L', 1)

def get_extra_compile_args():
  return ['-std=c++11']


class get_pybind11_includes(object):
  """Helper class to determine the pybind11 include path

  The purpose of this class is to postpone importing pybind11 until it is actually
  installed, so that the ``get_include()`` method can be invoked.

  Borrowd from: https://github.com/pybind/python_example/blob/master/setup.py
  """

  def __init__(self, user=False):
    self.user = user

  def __str__(self):
    import pybind11
    return pybind11.get_include(self.user)


def get_includes():
  kim_inc = get_kim_includes()
  pybind11_inc =[get_pybind11_includes(), get_pybind11_includes(user=True)]
  return kim_inc + pybind11_inc


def get_version(fname='kimpy/__init__.py'):
  with open(fname) as fin:
    for line in fin:
      line = line.strip()
      if '__version__' in line:
        v = line.split('=')[1]
        # stripe white space, and ' or " in string
        if "'" in v:
          version = v.strip("' ")
        elif '"' in v:
          version = v.strip('" ')
        break
  return version


kimapi_module = Extension('kimpy.kimapi',
    sources = ['kimpy/kim_api_bind.cpp'],
    include_dirs = get_includes(),
    library_dirs = get_kim_libdirs(),
    libraries = get_kim_ldlibs(),
    extra_compile_args = get_extra_compile_args(),
    extra_link_args = get_kim_extra_link_args(),
    language = 'c++',
    )


neigh_module = Extension('kimpy.neighborlist',
    sources = ['kimpy/neigh.cpp', 'kimpy/padding.cpp', 'kimpy/neigh_bind.cpp'],
    include_dirs = get_includes(),
    library_dirs = get_kim_libdirs(),
    libraries = get_kim_ldlibs(),
    extra_compile_args = get_extra_compile_args(),
    extra_link_args = get_kim_extra_link_args(),
    language = 'c++',
    )


setup(name = 'kimpy',
    version = get_version(),
    packages = ['kimpy'],
    ext_modules = [kimapi_module, neigh_module],
    install_requires = ['pybind11>=2.2', 'numpy', 'ase'],

    # metadata
    author = 'Mingjian Wen',
    author_email = 'wenxx151[at]umn.edu',
    url = 'https://github.com/mjwen/kimpy',
    description = 'Python binding of the KIM API to work with ASE',

    zip_safe = False,
    )

