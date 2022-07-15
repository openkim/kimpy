"""Format all .py and CPP source files using `black` and `clang-format` respectively.

This assumes both `black` (https://github.com/python/black) and `clang-format`
(https://clang.llvm.org/docs/ClangFormat.html) are installed.

To run:
$ python format_sources.py
"""

import os
import subprocess


def get_files(path, extension=[".cpp", ".hpp", ".h"]):
    all_srcs = []
    path = os.path.abspath(path)
    for root, dirs, files in os.walk(path):
        for f in files:
            ext = os.path.splitext(f)[1]
            if ext in extension:
                all_srcs.append(os.path.join(root, f))
    return all_srcs


def format_py_code(path):
    path = os.path.abspath(path)
    print(
        'Formating .py files in directory "{}" and its subdirectories using '
        '"black"...'.format(path)
    )
    subprocess.call(
        ["black", "--quiet", "--line-length", "90", "--skip-string-normalization", path]
    )
    print("Formatting .py files done.")


def format_cpp_code(path, exclude=["KIM_ComputeArguments_bind.cpp"]):
    path = os.path.abspath(path)
    print(
        'Formating CPP files in directory "{}" and its subdirectories using '
        '"clang-format"...'.format(path)
    )
    files = get_files(path)
    for f in files:
        ignore = False
        for i in exclude:
            if i in f:
                ignore = True
                break
        if ignore:
            continue
        subprocess.call(["clang-format", "-i", "-style=file", f])
    print("Formatting CPP files done.")


if __name__ == "__main__":
    kimpy_root_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
    format_py_code(kimpy_root_dir)
    format_cpp_code(kimpy_root_dir)
