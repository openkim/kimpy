from generate_bind_test_commons import generate_bind, generate_test


fields = {
    'FIELD_NAME': 'COMPUTE_CALLBACK_NAME',
    'FieldName': 'ComputeCallbackName',
    'fieldName': 'computeCallbackName',
    'field_name': 'compute_callback_name',
}


attributes = ['GetNeighborList', 'ProcessDEDrTerm', 'ProcessD2EDr2Term']


generate_bind(fields, attributes)
generate_test(fields, attributes)
