from setuptools import setup, Extension
from distutils.sysconfig import get_config_vars
import sys
import os
import subprocess
from api_compatibility import check_kim_api_compatibility


# remove `-Wstrict-prototypes' that is for C not C++
cfg_vars = get_config_vars()
for key, value in cfg_vars.items():
    if type(value) == str and '-Wstrict-prototypes' in value:
        cfg_vars[key] = value.replace('-Wstrict-prototypes', '')


# run scripts to generate files
dir_path = os.path.dirname(os.path.realpath(__file__))
fname = os.path.join(dir_path, 'scripts', 'generate_all.py')
subprocess.call([sys.executable, fname])


def inquire_kim_api(key):
    """ Get compile and link flags of kim-api."""
    try:
        config = subprocess.check_output(
            ['pkg-config', key, 'libkim-api'], universal_newlines=True
        )
    except:
        raise Exception(
            '"libkim-api" not found. Make sure "kim-api" is '
            'installed and do not forget to '
            '"source path/to/kim-api-activate".'
        )

    split_config = [s for s in config.strip().split(' ')]

    # remove identifier
    identifier = key[-2:]
    if identifier in ['-I', '-L', '-l']:
        split_config = [
            s.replace(identifier, '') for s in split_config if s.startswith(identifier)
        ]

    # remove empty string
    split_config = [s for s in split_config if s != '']

    return split_config


def get_kim_includes():
    return inquire_kim_api('--cflags-only-I')


def get_kim_libdirs():
    return inquire_kim_api('--libs-only-L')


def get_kim_ldlibs():
    return inquire_kim_api('--libs-only-l')


def get_kim_extra_link_args():
    return inquire_kim_api('--libs-only-other')


class get_pybind11_includes(object):
    """Helper class to determine the pybind11 include path

    The purpose of this class is to postpone importing pybind11 until it is actually
    installed, so that the ``get_include()`` method can be invoked.

    see:
    https://github.com/pybind/python_example/blob/master/setup.py
    https://github.com/pybind/python_example/issues/32
    """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11

        return pybind11.get_include(self.user)


def get_includes():
    kim_inc = get_kim_includes()
    pybind11_inc = [get_pybind11_includes(), get_pybind11_includes(user=True)]
    neighlist_inc = [
        os.path.join(os.path.dirname(os.path.abspath(__file__)), 'kimpy', 'neighlist')
    ]
    return kim_inc + pybind11_inc + neighlist_inc


def get_extra_compile_args():
    return ['-std=c++11', '-fvisibility=hidden']


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
    return Extension(
        module_name,
        sources=sources,
        include_dirs=get_includes(),
        library_dirs=get_kim_libdirs(),
        libraries=get_kim_ldlibs(),
        extra_compile_args=get_extra_compile_args(),
        extra_link_args=get_kim_extra_link_args(),
        language='c++',
    )


def get_extension_2(name):
    module_name = 'kimpy.{}'.format(name)
    name = name.split('_')
    name = [i.title() for i in name]
    name = ''.join(name)
    sources = ['kimpy/KIM_{}_bind.cpp'.format(name)]
    return Extension(
        module_name,
        sources=sources,
        include_dirs=get_includes(),
        library_dirs=get_kim_libdirs(),
        libraries=get_kim_ldlibs(),
        extra_compile_args=get_extra_compile_args(),
        extra_link_args=get_kim_extra_link_args(),
        language='c++',
    )


# check api compatibility
kimpy_v = get_version()
v = inquire_kim_api('--modversion')[0]
if '-' in v:
    kim_api_v = v.split('-')[0]
elif '+' in v:
    kim_api_v = v.split('+')[0]
else:
    kim_api_v = v
msg = check_kim_api_compatibility(kimpy_v, kim_api_v)
if msg is not None:
    raise Exception(msg)


module_names = [
    'model',
    'compute_arguments',
    'compute_argument_name',
    'compute_callback_name',
    'data_type',
    'species_name',
    'language_name',
    'model_routine_name',
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
    'collections',
    'collection',
    'collection_item_type',
    'simulator_model',
]

kimpy_ext_modules = [get_extension_2(name) for name in module_names]

neighlist_ext_module = [
    get_extension(
        'kimpy.neighlist',
        ['kimpy/neighlist/neighbor_list.cpp', 'kimpy/neighlist/neighbor_list_bind.cpp'],
    )
]

setup(
    name='kimpy',
    version=get_version(),
    packages=['kimpy'],
    setup_requires=['pybind11'],
    install_requires=['pybind11', 'numpy', 'pytest'],
    ext_modules=kimpy_ext_modules + neighlist_ext_module,
    author='Mingjian Wen',
    author_email='wenxx151@gmail.com',
    url='https://github.com/openkim/kimpy',
    description='Python interface to the KIM API',
    long_description='Python interface to the KIM API. For more information '
    'about the KIM API, please see: https://openkim.org/kim-api/',
    classifiers=[
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'License :: OSI Approved :: Common Development and Distribution License 1.0 (CDDL-1.0)',
        'Operating System :: OS Independent',
    ],
    zip_safe=False,
)
