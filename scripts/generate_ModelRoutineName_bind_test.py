from generate_bind_test_commons import generate_bind, generate_test


fields = {
    'FIELD_NAME': 'MODEL_ROUTINE_NAME',
    'FieldName': 'ModelRoutineName',
    'fieldName': 'modelRoutineName',
    'field_name': 'model_routine_name',
}


attributes = [
    'Create',
    'ComputeArgumentsCreate',
    'Compute',
    'Extension',
    'Refresh',
    'WriteParameterizedModel',
    'ComputeArgumentsDestroy',
    'Destroy',
]


generate_bind(fields, attributes)
generate_test(fields, attributes)
