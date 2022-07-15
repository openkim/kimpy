import os
from generate_bind_test_commons import generate_bind, generate_test

fields = {
    "FIELD_NAME": "NUMBERING",
    "FieldName": "Numbering",
    "fieldName": "numbering",
    "field_name": "numbering",
}

attributes = ["zeroBased", "oneBased"]

generate_bind(fields, attributes, os.path.basename(__file__))
generate_test(fields, attributes, os.path.basename(__file__))
