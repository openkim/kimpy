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
    config = subprocess.check_output(['kim-api-build-config', option])
  except:
    raise Exception('"kim_api_build_config" not found on PATH; make sure kim-api '
                    'is installed and "kim_api_build_config" is on PATH.')

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
  if sys.platform == 'linux2':
    return ['-std=c++11']
  if sys.platform == 'darwin':
    return ['-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.7']


kimapi_module = Extension('kimpy.kimapi',
    sources = ['kimpy/kim_api_bind.cpp'],
    include_dirs = get_kim_includes(),
    library_dirs = get_kim_libdirs(),
    libraries = get_kim_ldlibs(),
    extra_compile_args = get_extra_compile_args(),
    extra_link_args = get_kim_extra_link_args(),
    language = 'c++',
    )


neigh_module = Extension('kimpy.neighborlist',
    sources = ['kimpy/cvec.cpp', 'kimpy/neigh.cpp', 'kimpy/neigh_bind.cpp'],
    include_dirs = get_kim_includes(),
    library_dirs = get_kim_libdirs(),
    libraries = get_kim_ldlibs(),
    extra_compile_args = get_extra_compile_args(),
    extra_link_args = get_kim_extra_link_args(),
    language = 'c++',
    )


setup(name = 'ase_kim_calculator',
    version = '0.0.1',
    description = 'This is a demo package',
    packages = ['kimpy'],
    ext_modules = [kimapi_module, neigh_module],
    install_requires=['pybind11>=2.2'],
    setup_requires=['pybind11>=2.2'],  # ensures it be downloaded first
    zip_safe = False,
    )

