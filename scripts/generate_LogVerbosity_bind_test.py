from collections import OrderedDict
from generate_bind_test_commons import generate_bind, generate_test


fields = OrderedDict()
fields['NUMBER_OF_FIELD_NAME'] = 'NUMBER_OF_LOG_VERBOSITIE'
fields['NumberOfFieldName'] = 'NumberOfLogVerbositie'
fields['numberOfFieldName'] = 'numberOfLogVerbositie'
fields['number_of_field_name'] = 'number_of_log_verbositie'
fields['FIELD_NAME'] = 'LOG_VERBOSITY'
fields['FieldName'] = 'LogVerbosity'
fields['fieldName'] = 'logVerbosity'
fields['field_name'] = 'log_verbosity'


attributes = ['silent', 'fatal', 'error', 'warning', 'information', 'debug']


generate_bind(fields, attributes)
generate_test(fields, attributes)
