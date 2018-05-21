from setuptools import setup, Extension
from distutils.sysconfig import get_config_vars
import sys
import os
import subprocess


# remove `-Wstrict-prototypes' that is for C not C++
cfg_vars = get_config_vars()
for key, value in cfg_vars.items():
  if type(value) == str and '-Wstrict-prototypes' in value:
    cfg_vars[key] = value.replace('-Wstrict-prototypes', '')

# run script to generate files
cwd = os.getcwd()
fname = os.path.join(cwd, 'kimpy', 'generate_SpeciesName_attributes.py')
subprocess.call(['python', fname])


def inquire_kim_api(option, key, mode):
  try:
    config = subprocess.check_output(['kim-api-v2-build-config', option])
  except:
    raise Exception('"kim-api-v2-build-config" not found on PATH; make sure '
                    'kim-api ss installed and "kim-api-v2-build-config" is on PATH.')

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


def get_extension(module_name, sources):
  return Extension(module_name,
  sources = sources,
  include_dirs = get_includes(),
  library_dirs = get_kim_libdirs(),
  libraries = get_kim_ldlibs(),
  extra_compile_args = get_extra_compile_args(),
  extra_link_args = get_kim_extra_link_args(),
  language = 'c++',
  )


model = get_extension(
  'kimpy.model',
  ['kimpy/KIM_Model_bind.cpp']
)
numbering = get_extension(
  'kimpy.numbering',
  ['kimpy/KIM_Numbering_bind.cpp']
)
compute_arguments = get_extension(
  'kimpy.compute_arguments',
  ['kimpy/KIM_ComputeArguments_bind.cpp']
)
compute_argument_name = get_extension(
  'kimpy.compute_argument_name',
  ['kimpy/KIM_ComputeArgumentName_bind.cpp']
)
data_type = get_extension(
  'kimpy.data_type',
  ['kimpy/KIM_DataType_bind.cpp']
)
numbering = get_extension(
  'kimpy.numbering',
  ['kimpy/KIM_Numbering_bind.cpp']
)
species_name = get_extension(
  'kimpy.species_name',
  ['kimpy/KIM_SpeciesName_bind.cpp']
)


setup(name = 'kimpy',
    version = get_version(),
    packages = ['kimpy'],
    ext_modules = [compute_argument_name,
                   data_type,
                   numbering,
                   species_name,
                  ],
    install_requires = ['pybind11>=2.2', 'numpy', 'ase'],

    # metadata
    author = 'Mingjian Wen',
    author_email = 'wenxx151[at]umn.edu',
    url = 'https://github.com/mjwen/kimpy',
    description = 'Python binding of the KIM API to work with ASE',

    zip_safe = False,
    )

