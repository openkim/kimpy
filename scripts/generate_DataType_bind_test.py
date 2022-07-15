import os
from generate_bind_test_commons import generate_bind, generate_test

fields = {
    "FIELD_NAME": "DATA_TYPE",
    "FieldName": "DataType",
    "fieldName": "dataType",
    "field_name": "data_type",
}

attributes = ["Integer", "Double"]

generate_bind(fields, attributes, os.path.basename(__file__))
generate_test(fields, attributes, os.path.basename(__file__))
