from generate_bind_test_commons import generate_bind, generate_test


fields = {
    'FIELD_NAME': 'DATA_TYPE',
    'FieldName': 'DataType',
    'fieldName': 'dataType',
    'field_name': 'data_type',
}


attributes = ['Integer', 'Double']


generate_bind(fields, attributes)
generate_test(fields, attributes)
