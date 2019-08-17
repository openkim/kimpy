import os


def get_kimpy_kimapi_versions(path=None):
    """Get the latest kimpy and api version specified the compatibility table.

    Returns
    -------
    kimpy_v: str
        kimpy version
    api_v: str
        kim-api version that kimpy targets for
    """
    if path is None:
        this_dir = os.path.dirname(os.path.abspath(__file__))
        path = os.path.join(os.path.dirname(this_dir), 'api_compatibility.txt')
    with open(path, 'r') as fin:
        for line in fin:
            line = line.strip()
            if not line or line[0] == '#':
                continue
            versions = line.split()
    return versions[0], versions[1]


def update_version(version, path, key, in_quotes=False, extra_space=False):
    """Update version given in `key=version` structure."""
    with open(path, 'r') as fin:
        lines = fin.readlines()
    with open(path, 'w') as fout:
        for line in lines:
            if key in line:
                idx = line.index('=')
                line = line[: idx + 1]
                if extra_space:
                    line += ' '
                if in_quotes:
                    v = '"{}"'.format(version)
                else:
                    v = '{}'.format(version)
                fout.write(line + v + '\n')
            else:
                fout.write(line)


if __name__ == '__main__':
    kimpy_v, api_v = get_kimpy_kimapi_versions()
    kimpy_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    path = os.path.join(kimpy_dir, 'kimpy', '__init__.py')
    update_version(kimpy_v, path, '__version__', True, True)
    path = os.path.join(kimpy_dir, '.travis.yml')
    update_version(api_v, path, 'export KIM_API_VERSION')
