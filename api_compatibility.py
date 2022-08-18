"""KimPY compatibility module."""

from os.path import dirname, abspath, join, isfile
from collections import OrderedDict
import sys

sys.path.insert(0, abspath("kimpy"))

from err import KimPyError


def read_compatible_table():
    """Read the compatible table.

    Returns:
        dict: an ordered dictionary
            A, ordered dict with:
                key: kimpy version,
                value['target']: kim-api version that kimpy targets for
                value['backward']: kim-api version that kimpy is backward
                    compatible with
    """
    this_dir = dirname(abspath(__file__))
    file_path = join(this_dir, "api_compatibility.txt")

    if not isfile(file_path):
        msg = "{} is not an existing regular file.".format(file_path)
        raise KimPyError(msg)

    compatible_table = OrderedDict()
    with open(file_path, "r") as fin:
        for line in fin:
            line = line.strip()
            if not line or line[0] == "#":
                continue

            (
                kimpy_version,
                kim_api_target_version,
                kim_api_backward_version,
            ) = line.split()

            compatible_table[kimpy_version] = {
                "target": kim_api_target_version,
                "backward": kim_api_backward_version,
            }

    return compatible_table


def compare_version(x, y):
    """Check the version relation of x and y in the format a.b.c.

    Return:
        str: the version relation of x and y
            '<' if x smaller than y
            '=' if x equal y
            '>' is x larger than y
    """
    x_major, x_minor, x_patch = [int(i) for i in x.split(".")]
    y_major, y_minor, y_patch = [int(i) for i in y.split(".")]

    if x_major > y_major:
        return ">"

    if x_major < y_major:
        return "<"

    if x_minor > y_minor:
        return ">"

    if x_minor < y_minor:
        return "<"

    if x_patch > y_patch:
        return ">"

    if x_patch < y_patch:
        return "<"

    return "="


def suggest_kimpy(kim_api_version, compatible_table):
    """Suggest a kimpy version for the installed kim-api version.

    Always suggest the latest kimpy compatible with the given kim-api version.

    Args:
        kim_api_version (str): kim-api version
        compatible_table (dict): the compatible table

    Returns:
        str, or None: the latest kimpy
            the latest kimpy compatible with the given kim-api version.
    """
    if not isinstance(compatible_table, OrderedDict):
        msg = "The compatible_table is not an ordered `dict`."
        raise KimPyError(msg)

    target = None
    backward = None

    # search backward
    for key, value in compatible_table.items():
        if value["target"] == kim_api_version:
            target = key

        if value["backward"] == kim_api_version:
            backward = key

    if backward:
        return backward

    return target


def check_kim_api_compatibility(kimpy_version, kim_api_version):
    """Check the kimpy version and kim-api version.

    Check the compatibility of the kimpy and kim-api versions.

    Args:
        kimpy_version (str): kimpy version string
        kim_api_version (str): kim-api version string

    Returns:
        str, or None: Error message if not compatible
    """
    compatible_table = read_compatible_table()

    # check whether we have kimpy for the specified api

    # last key in OrderedDict
    latest_kimpy = next(reversed(compatible_table))
    latest_target = compatible_table[latest_kimpy]["target"]

    cmp = compare_version(latest_target, kim_api_version)
    cmp_1 = True
    if cmp == "<":
        cmp_1 = False
    else:
        oldest_target = "2.0.0"
        cmp = compare_version(oldest_target, kim_api_version)
        if cmp == ">":
            cmp_1 = False

    # check whether the current kimpy is compatible with the api
    target = compatible_table[kimpy_version]["target"]
    backward = compatible_table[kimpy_version]["backward"]

    cmp = compare_version(target, kim_api_version)
    cmp_2 = True
    if cmp == "<":
        cmp_2 = False
    else:
        cmp = compare_version(backward, kim_api_version)
        if cmp == ">":
            cmp_2 = False

    if not cmp_1 or not cmp_2:
        msg0 = "\n\n\n{}\n".format("=" * 80)

        msg1 = 'KIM-API version = "{}" is detected, '.format(kim_api_version)
        msg1 += "which is incompatible with the current\n  kimpy version = "
        msg1 += '"{}".\n\nTry one of these options:\n\n'.format(kimpy_version)

        msg2 = "  (1) Update KIM-API.\n      The current kimpy is "
        msg2 += "compatible with KIM-API versions:\n "
        msg2 += '     ["{}" - "{}"] (inclusive).\n'.format(backward, target)

        if not cmp_1:
            msg3 = "  (2) Use the latest kimpy.\n      If you are using the "
            msg3 += "latest kimpy, which is not compatible "
            msg3 += "with this\n      KIM-API version "
            msg3 += '"{}" yet. Please contact\n      '.format(kim_api_version)
            msg3 += "Mingjian Wen (wenxx151@gmail.com)  or\n      "
            msg3 += "raise an issue on github so we update the kimpy.\n"
        else:
            compatible_kimpy = suggest_kimpy(kim_api_version, compatible_table)
            msg3 = "  (2) Switch kimpy version.\n      The detected KIM-API "
            msg3 += "is compatible with kimpy version "
            msg3 += '"{}".\n'.format(compatible_kimpy)

        msg = msg0 + msg1 + msg2 + msg3
    else:
        msg = None

    return msg


if __name__ == "__main__":
    kim_api_compatibility = check_kim_api_compatibility("2.0.1", "2.3.0")
    print(kim_api_compatibility)
