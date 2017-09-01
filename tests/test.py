from __future__ import print_function
import openkim as km
from inspect import currentframe, getframeinfo
info = getframeinfo(currentframe())


def test_kim_api():
  version, status = km.get_version()
  print('get_version', version, status)

  status = km.report_error(info.lineno, info.filename, 'some message', -1)
  print('report_error', status)



if __name__ == '__main__':
  test_kim_api()
