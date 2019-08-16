from collections import OrderedDict
from generate_bind_test_commons import generate_bind, generate_test


fields = OrderedDict()
fields['NUMBER_OF_FIELD_NAME'] = 'NUMBER_OF_SUPPORT_STATUSE'
fields['NumberOfFieldName'] = 'NumberOfSupportStatuse'
fields['numberOfFieldName'] = 'numberOfSupportStatuse'
fields['number_of_field_name'] = 'number_of_support_statuse'
fields['FIELD_NAME'] = 'SUPPORT_STATUS'
fields['FieldName'] = 'SupportStatus'
fields['fieldName'] = 'supportStatus'
fields['field_name'] = 'support_status'


attributes = ['requiredByAPI', 'notSupported', 'required', 'optional']


generate_bind(fields, attributes)
generate_test(fields, attributes)
