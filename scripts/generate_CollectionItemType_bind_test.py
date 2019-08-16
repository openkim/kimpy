from generate_bind_test_commons import generate_bind, generate_test


fields = {
    'FIELD_NAME': 'COLLECTION_ITEM_TYPE',
    'FieldName': 'CollectionItemType',
    'fieldName': 'collectionItemType',
    'field_name': 'collection_item_type',
}


attributes = ['modelDriver', 'portableModel', 'simulatorModel']


generate_bind(fields, attributes)
generate_test(fields, attributes)
