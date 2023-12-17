"""
To update version:
1. update compatibility table ../api_compatibility.txt
2. $ python update_version.py
"""
from pathlib import Path


def get_kimpy_kimapi_versions(path=None):
    """Get the latest kimpy and api version specified in the compatibility table."""
    if path is None:
        this_dir = Path(__file__).resolve().parent
        path = this_dir.parent / "api_compatibility.txt"
    with path.open("r") as fin:
        for line in fin:
            line = line.strip()
            if not line or line[0] == "#":
                continue
            versions = line.split()
    return versions[0], versions[1]


def update_version(version, path, key, in_quotes=False, extra_space=False):
    """Update version given in `key=version` structure."""
    path = Path(path)
    lines = path.read_text().splitlines()
    with path.open("w") as fout:
        for line in lines:
            if key in line:
                try:
                    idx = line.index("=")
                except ValueError:
                    fout.write(line + "\n")
                    continue
                line = line[: idx + 1]
                if extra_space:
                    line += " "
                v = f'"{version}"' if in_quotes else f"{version}"
                fout.write(line + v + "\n")
            else:
                fout.write(line + "\n")


if __name__ == "__main__":
    kimpy_v, api_v = get_kimpy_kimapi_versions()
    kimpy_dir = Path(__file__).resolve().parent.parent

    path = kimpy_dir / "kimpy" / "__init__.py"
    update_version(kimpy_v, path, "__version__", True, True)

    path = kimpy_dir / ".github/workflows/testing.yml"
    update_version(api_v, path, "kim-api", in_quotes=False)

    path = kimpy_dir / ".github/workflows/publish.yml"
    update_version(api_v, path, "kim-api", in_quotes=False)
