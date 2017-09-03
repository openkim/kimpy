from setuptools import setup, Extension
from distutils.sysconfig import get_config_vars
import subprocess

# remove `-Wstrict-prototypes' that is for C not C++
cfg_vars = get_config_vars()
for key, value in cfg_vars.items():
  if type(value) == str and '-Wstrict-prototypes' in value:
    cfg_vars[key] = value.replace('-Wstrict-prototypes', '')


def inquire_kim_api(option, key, mode):
  config = subprocess.check_output(['kim-api-build-config', option])
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





kimapi_module = Extension('openkim',
    sources = ['src/openkim_bind.cpp'],
    include_dirs = get_kim_includes(),
    library_dirs = get_kim_libdirs(),
    libraries = get_kim_ldlibs(),
    extra_compile_args = ['-std=c++11'],
    extra_link_args = get_kim_extra_link_args(),
    language = 'c++',
    )

neigh_module = Extension('neighborlist',
    sources = ['src/cvec.cpp', 'src/neigh.cpp', 'src/neigh_bind.cpp'],
    include_dirs = get_kim_includes(),
    library_dirs = get_kim_libdirs(),
    libraries = get_kim_ldlibs(),
    extra_compile_args = ['-std=c++11'],
    extra_link_args = get_kim_extra_link_args(),
    language = 'c++',
    )



setup(name = 'ase_kim_calculator',
    version = '0.0.1',
    description = 'This is a demo package',
    packages = ['padding'],
    package_dir = {'padding':'src/'},
    #ext_modules = [kimapi_module, neigh_module],
    ext_modules = [neigh_module],
    install_requires=['pybind11==2.1'],
    setup_requires=['pybind11==2.1'],  # ensures it be downloaded first
    #zip_safe = False,
    )
