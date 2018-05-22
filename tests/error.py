def check_error(error, message=None):
  if error != 0:
    raise Exception('KIM error. Calling "{}" failed.'.format(message))

def report_error(message=None):
  raise Exception('KIM error. {}.'.format(message))


