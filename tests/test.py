from __future__ import print_function
from kimpy import kimapi as km


def test_kim_api():
  version, status = km.get_version()
  print('get_version', version, status)


if __name__ == '__main__':
  test_kim_api()
