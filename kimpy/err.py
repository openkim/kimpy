"""Error module."""

import inspect

__all__ = [
    'KimPyError',
    'check_error',
    'report_error'
]


class KimPyError(Exception):
    """Raise an exception.

    It raises an exception when receives an error message.

    msg (str): The error message.

    """

    def __init__(self, msg):
        """Constuctor."""
        _msg = '\nERROR(@' + \
            inspect.currentframe().f_back.f_code.co_name + '): ' + msg
        Exception.__init__(self, _msg)
        self.msg = _msg

    def __reduce__(self):
        """Efficient pickling."""
        return self.__class__, (self.msg)

    def __str__(self):
        """Message string representation."""
        return self.msg


def check_error(error, message=None):
    """Check KIM-API return error.

    Args:
        error (int): 0 indicate everything went well
        message (str, optional): error message. Defaults to None.
    """
    if error != 0 and error is not None:
        msg = 'KIM error. Calling "{}" failed.'.format(message)
        raise KimPyError(msg)


def report_error(message=None):
    """Check KIM-API return error.

    Args:
        message (str, optional): error message. Defaults to None.
    """
    msg = 'KIM error. {}.'.format(message)
    raise KimPyError(msg)
