from __future__ import print_function
import openkim as km
import inspect

# NOTE
# It would be good to get the stderr as string, and then assert it.
# Anyway, the stdout and stderr after running this script should look like:
"""
* Error: at line 17 in test_report_error.py
    Message: some message 1
    KIM_STATUS_MSG: symbol is not among supported particle symbols
report_error_wrapper 0

* Error: at line 20 in test_report_error.py
    Message: some message 2
    KIM_STATUS_MSG: argument name provided is not in KIM API object
report_error 0
"""

def get_frame():
  callerframerecord = inspect.stack()[1]    # 0 represents this line
                                            # 1 represents line at caller
  frame = callerframerecord[0]
  info = inspect.getframeinfo(frame)
  return info


def test_error():
  """ Test the correctness of the two report error function.
  """

  info = get_frame()
  status = km.report_error_wrapper(info.lineno+1, info.filename, 'some message 1', -2)
  print('report_error_wrapper', status)
  print()

  status = km.report_error('some message 2', -1)
  print('report_error', status)



if __name__ == '__main__':
  test_error()
