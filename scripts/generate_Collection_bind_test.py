from generate_bind_test_commons import generate_bind, generate_test


fields = {
    'FIELD_NAME': 'COLLECTION',
    'FieldName': 'Collection',
    'fieldName': 'collection',
    'field_name': 'collection',
}


attributes = ['system', 'user', 'environmentVariable', 'currentWorkingDirectory']


generate_bind(fields, attributes)
generate_test(fields, attributes)
