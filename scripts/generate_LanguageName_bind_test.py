from generate_bind_test_commons import generate_bind, generate_test


fields = {
    'FIELD_NAME': 'LANGUAGE_NAME',
    'FieldName': 'LanguageName',
    'fieldName': 'languageName',
    'field_name': 'language_name',
}


attributes = ['cpp', 'c', 'fortran']


generate_bind(fields, attributes)
generate_test(fields, attributes)
