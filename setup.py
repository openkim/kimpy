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


# run scripts to generate files
dir_path = os.path.dirname(os.path.realpath(__file__))
fname = os.path.join(dir_path, 'scripts', 'generate_all.py')
subprocess.call(['python', fname])


def inquire_kim_api(option, key, mode):
  """ Get compile and link flags of kim-api."""
  try:
    py_version = sys.version_info[0]
    if py_version == 2:
      config = subprocess.check_output(['kim-api-v2-build-config', option])
    else:
      config = subprocess.check_output(['kim-api-v2-build-config', option]).decode('utf-8')
  except:
    raise Exception('"kim-api-v2-build-config" not found on PATH; make sure '
                    'kim-api is installed and "kim-api-v2-build-config" is on PATH.')

  # remove `\n' at end and then split at white space
  split_config = [s for s in config.strip().split(' ')]
  if mode == 0:
    # collect item starting with key, and remove key in the string
    split_config= [s.replace(key, '') for s in split_config if s.startswith(key)]
  elif mode == 1:
    # collect item not starting with key
    split_config= [s for s in split_config if not s.startswith(key)]

  return split_config


def get_kim_includes():
  return inquire_kim_api('--includes', '-I', 0)

def get_kim_libdirs():
  return inquire_kim_api('--ldflags', '-L', 0)

def get_kim_ldlibs():
  return inquire_kim_api('--ldlibs', '-l', 0)

def get_kim_extra_link_args():
  return inquire_kim_api('--ldflags', '-L', 1)


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


def get_extra_compile_args():
  return ['-std=c++11']


def get_version(fname=os.path.join('kimpy', '__init__.py')):
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


def get_extension_2(name):

  module_name = 'kimpy.{}'.format(name)
  name = name.split('_')
  name = [i.title() for i in name]
  name = ''.join(name)
  sources = ['kimpy/KIM_{}_bind.cpp'.format(name)]
  return Extension(
    module_name,
    sources = sources,
    include_dirs = get_includes(),
    library_dirs = get_kim_libdirs(),
    libraries = get_kim_ldlibs(),
    extra_compile_args = get_extra_compile_args(),
    extra_link_args = get_kim_extra_link_args(),
    language = 'c++',
  )

module_names = [
  'model',
  'compute_arguments',
  'compute_argument_name',
  'compute_callback_name',
  'data_type',
  'species_name',
  'language_name',
  'numbering',
  'support_status',
  'log_verbosity',
  'length_unit',
  'energy_unit',
  'charge_unit',
  'temperature_unit',
  'time_unit',
  'sem_ver',
  'log',
]

kimpy_ext_modules = [get_extension_2(name) for name in module_names]


setup(name = 'kimpy',
  version = get_version(),
  packages = ['kimpy'],
  ext_modules = kimpy_ext_modules,
  install_requires = ['pybind11', 'numpy', 'pytest'],
  author = 'Mingjian Wen',
  author_email = 'wenxx151@umn.edu',
  url = 'https://github.com/mjwen/kimpy',
  description = 'Python interface to the KIM API',
  classifiers = (
    'Programming Language :: Python :: 2.7',
    'Programming Language :: Python :: 3.5',
    'Programming Language :: Python :: 3.6',
    'Programming Language :: Python :: 3.7',
    'License :: OSI Approved :: Common Development and Distribution License 1.0 (CDDL-1.0)',
    'Operating System :: OS Independent',
  ),
  zip_safe = False,
)

