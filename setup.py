from setuptools import setup, Extension
from distutils.sysconfig import get_config_vars
import subprocess

# remove `-Wstrict-prototypes' that is for C not C++
cfg_vars = get_config_vars()
for key, value in cfg_vars.items():
  if type(value) == str and '-Wstrict-prototypes' in value:
    cfg_vars[key] = value.replace('-Wstrict-prototypes', '')


def inquire_kim_api(option, key):
  config = subprocess.check_output(['kim-api-build-config', option])
  # remove `\n' at end and then split at white space
  split_config = [s for s in config.strip().split(' ')]
  # only collect item starting with key, and remove key in the string
  split_config= [s.replace(key, '') for s in split_config if s.startswith(key)]
  return split_config

def get_kim_includes():
  return inquire_kim_api('--includes', '-I')

def get_kim_ldflags():
  return inquire_kim_api('--ldflags', '-L')

def get_kim_ldlibs():
  return inquire_kim_api('--ldlibs', '-l')


kimapi_module = Extension('openkim',
    sources = ['openkim.cpp'],
    include_dirs = get_kim_includes(),
    library_dirs = get_kim_ldflags(),
    libraries = get_kim_ldlibs(),
    extra_compile_args=['-std=c++11'],
    language = 'c++',
    )

setup(name = 'ase_kim_calculator',
    version = '0.0.1',
    description = 'This is a demo package',
    ext_modules = [kimapi_module],
    install_requires=['pybind11==2.1'],
    #zip_safe = False,
    )
