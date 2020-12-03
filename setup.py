"""Setup."""

from distutils.sysconfig import get_config_vars
import os
import subprocess
import sys

from setuptools import setup, Extension, find_packages, distutils
from setuptools.command.build_ext import build_ext

from api_compatibility import check_kim_api_compatibility


# remove `-Wstrict-prototypes' that is for C not C++
cfg_vars = get_config_vars()
for _key, _value in cfg_vars.items():
    if isinstance(_value, str) and "-Wstrict-prototypes" in _value:
        cfg_vars[_key] = _value.replace("-Wstrict-prototypes", "")


def generate_files():
    """Run scripts to generate files."""
    dir_path = os.path.dirname(os.path.realpath(__file__))
    generate_script = os.path.join(dir_path, "scripts", "generate_all.py")
    subprocess.call([sys.executable, generate_script])


# Run scripts to generate files.
generate_files()


def inquire_kim_api(kim_api_key):
    """Get compile and link flags of kim-api package."""
    try:
        output = subprocess.check_output(
            ["pkg-config", kim_api_key, "libkim-api"], universal_newlines=True
        )
    except:
        msg = 'Package "libkim-api" was not found in the search path.\n'
        msg += 'Make sure "kim-api" is installed and do not forget to '
        msg += '"source path/to/kim-api-activate".\nSee '
        msg += '"Installing the KIM API" at https://openkim.org/kim-api '
        msg += "for more detailed information."
        raise Exception(msg)

    split_config = output.strip().split()

    # remove identifier
    identifier = kim_api_key[-2:]
    if identifier in ["-I", "-L", "-l"]:
        split_config = [
            s.replace(identifier, "") for s in split_config if s.startswith(identifier)
        ]

    # remove empty string
    split_config = [s for s in split_config if s != ""]
    return split_config


class get_pybind_include:
    """Helper class to determine the pybind11 include path.

    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked.
    """

    def __str__(self):
        import pybind11

        return pybind11.get_include()


def has_flag(compiler, flag_name):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile

    with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False) as f:
        f.write("int main (int argc, char **argv) { return 0; }")
        tempfile_name = f.name

    try:
        compiler.compile([tempfile_name], extra_postargs=[flag_name])
    except distutils.errors.CompileError:
        print(
            f"Checking whether the compiler supports flag: '{flag_name}'. You may see "
            "compilation error (e.g. gcc: error: unrecognized command line option "
            f"{flag_name}). Just ignore it; we are on the right track."
        )
        return False
    finally:
        try:
            os.remove(tempfile_name)
        except OSError:
            pass
    return True


def cpp_flag(compiler_type, compiler):
    """Return the -std=c++[11/14/17] compiler flag.

    The newer version is preferred over c++11 (when it is available).
    """
    if compiler_type == "unix":
        flags = ["-std=c++20", "-std=c++17", "-std=c++14", "-std=c++11"]
    else:
        flags = ["-std=c++20", "-std=c++17"]

    for flag in flags:
        if has_flag(compiler, flag):
            return flag
    if compiler_type == "unix":
        msg = "Unsupported compiler -- at least C++11 support is needed!"
    else:
        msg = "Unsupported compiler -- at least C++17 support is needed!"
    raise Exception(msg)


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""

    c_opts = {
        "msvc": ["/EHsc"],
        "unix": [],
    }

    l_opts = {
        "msvc": [],
        "unix": [],
    }

    if sys.platform == "darwin":
        c_opts["unix"].append("-mmacosx-version-min=10.7")
        l_opts["unix"].append("-mmacosx-version-min=10.7")

    def build_extensions(self):
        compiler_type = self.compiler.compiler_type

        if sys.platform == "darwin":
            if has_flag(self.compiler, "-stdlib=libc++"):
                self.c_opts["unix"].append("-stdlib=libc++")
                self.l_opts["unix"].append("-stdlib=libc++")

        opts = self.c_opts.get(compiler_type, [])
        opts.append(cpp_flag(compiler_type, self.compiler))

        if has_flag(self.compiler, "-fvisibility=hidden"):
            opts.append("-fvisibility=hidden")

        link_opts = self.l_opts.get(compiler_type, [])

        kim_extra_link_args = inquire_kim_api("--libs-only-other")
        for link_arg in kim_extra_link_args:
            link_opts.append(link_arg)

        kim_include_dirs = inquire_kim_api("--cflags-only-I")
        for include_dir in kim_include_dirs:
            self.compiler.add_include_dir(include_dir)

        kim_library_dirs = inquire_kim_api("--libs-only-L")
        for library_dir in kim_library_dirs:
            self.compiler.add_library_dir(library_dir)

        kim_libraries = inquire_kim_api("--libs-only-l")
        for library in kim_libraries:
            self.compiler.add_library(library)

        for ext in self.extensions:
            ext.define_macros = [
                ("VERSION_INFO", '"{}"'.format(self.distribution.get_version()))
            ]
            ext.extra_compile_args = opts
            ext.extra_link_args = link_opts

        build_ext.build_extensions(self)


def get_include_dirs():
    """Return the list of directories that will be searched."""
    include_dirs = inquire_kim_api("--cflags-only-I")
    include_dirs.append(get_pybind_include())
    include_dirs.append(
        os.path.join(os.path.dirname(os.path.abspath(__file__)), "kimpy", "neighlist")
    )
    return include_dirs


def get_kimpy_version():
    """Get the kimpy version.

    Returns:
        str: kimpy version
    """
    kimpy_init_file = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "kimpy", "__init__.py"
    )

    version = None

    with open(kimpy_init_file) as fin:
        for line in fin:
            line = line.strip()
            if "__version__" in line:
                sline = line.split("=")[1]
                # stripe white space, and ' or " in string
                if "'" in sline:
                    version = sline.strip("' ")
                elif '"' in sline:
                    version = sline.strip('" ')
                break

    if version is None:
        msg = "the current kimpy version can not be found."
        raise Exception(msg)

    return version


def get_extension(name):
    """Return the Extension instance, used to describe C/C++ extension modules.

    Args:
        name (str): the name of the extension

    Returns:
        Extension obj: Return an Extension instance
    """
    # the full name of the extension
    module_name = "kimpy.{}".format(name)

    if name == "neighlist":
        sources = [
            os.path.join("kimpy", "neighlist", "neighbor_list.cpp"),
            os.path.join("kimpy", "neighlist", "neighbor_list_bind.cpp"),
        ]
    else:
        tname = name.split("_")
        name = [i.title() for i in tname]
        name = "".join(name)
        sources = [os.path.join("kimpy", "KIM_{}_bind.cpp".format(name))]

    return Extension(
        module_name,
        sources=sources,
        include_dirs=get_include_dirs(),
        library_dirs=inquire_kim_api("--libs-only-L"),
        libraries=inquire_kim_api("--libs-only-l"),
        extra_link_args=inquire_kim_api("--libs-only-other"),
        language="c++",
    )


def get_kimpy_ext_modules():
    """Get kimpy Extension instances.

    Returns:
        list: kimpy Extension instances

    """
    module_names = [
        "model",
        "compute_arguments",
        "compute_argument_name",
        "compute_callback_name",
        "data_type",
        "species_name",
        "language_name",
        "model_routine_name",
        "numbering",
        "support_status",
        "log_verbosity",
        "length_unit",
        "energy_unit",
        "charge_unit",
        "temperature_unit",
        "time_unit",
        "sem_ver",
        "log",
        "collections",
        "collection",
        "collection_item_type",
        "simulator_model",
        "neighlist",
    ]

    kimpy_ext_modules = [get_extension(name) for name in module_names]
    return kimpy_ext_modules


def chech_kim_api_compatibility():
    """Check the KIM-API compatibility."""
    modversion = inquire_kim_api("--modversion")[0]

    if "-" in modversion:
        kim_api_version = modversion.split("-")[0]
    elif "+" in modversion:
        kim_api_version = modversion.split("+")[0]
    else:
        kim_api_version = modversion

    kimpy_version = get_kimpy_version()
    kim_api_compatibility = check_kim_api_compatibility(kimpy_version, kim_api_version)
    if kim_api_compatibility is not None:
        raise Exception(kim_api_compatibility)


# Check kim-api compatibility
chech_kim_api_compatibility()


setup(
    name="kimpy",
    version=get_kimpy_version(),
    packages=find_packages(),
    setup_requires=["pybind11"],
    install_requires=["pybind11", "numpy", "pytest"],
    cmdclass={"build_ext": BuildExt},
    ext_modules=get_kimpy_ext_modules(),
    author="Mingjian Wen",
    author_email="wenxx151@gmail.com",
    url="https://github.com/openkim/kimpy",
    description="Python interface to the KIM-API",
    long_description="Python interface to the KIM-API. For more "
    "information about the KIM-API, please see: "
    "https://openkim.org/kim-api/",
    license="CDDL-1.0",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering",
        "Operating System :: OS Independent",
        "License :: OSI Approved :: Common Development and Distribution "
        "License 1.0 (CDDL-1.0)",
    ],
    keywords=["kimpy"],
    zip_safe=False,
)
