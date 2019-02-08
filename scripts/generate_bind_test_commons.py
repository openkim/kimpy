import os


def generate_bind(fields, attributes):
    """ Generate binding code by replacing fields and writing attributes.

    Arguments
    ---------

    fields: dictionary whose key to be replaced by value.
      e.g.
      length_unit = {
        'FIELD_NAME': 'LENGTH_UNIT',
        'FieldName': 'LengthUnit',
        'filedName': 'lengthUnit',
        'field_name': 'length_unit'}

    attributes: list of str
      e.g.
      length_attributes = [
        'unused',
        'A',
        'Bohr',
        'cm',
        'm',
        'nm']

    output: files with `fname` below
    """

    fname = 'KIM_FieldName_bind.cpp-template'

    dir_path = os.path.dirname(os.path.realpath(__file__))
    template_name = os.path.join(dir_path, fname)
    with open(template_name, 'r') as fin:
        template = fin.read()

    # add attributes
    attr_str = ''
    for attr in attributes:
        attr_str += '  module.attr("{0}") = FIELD_NAME::{0};\n'.format(attr)
    template = template.replace('rpls_attributes', attr_str)

    # replace field names
    for key, value in fields.items():
        template = template.replace(key, value)
        fname = fname.replace(key, value)

    # write to file
    parent_path = os.path.dirname(dir_path)
    fname = os.path.join(parent_path, 'kimpy', fname.split('-')[0])
    with open(fname, 'w') as fout:
        fout.write('// This file is generated automatically by scripts\n')
        fout.write(template)


def generate_test(fields, attributes):
    """ Generate test for binding code by replacing fields and writing attributes.

    Argument
    --------

    fields: dictionary with three name to repalce
      e.g.
      length_unit = {
        'FIELD_NAME': 'LENGTH_UNIT',
        'FieldName': 'LengthUnit',
        'filedName': 'lengthUnit',
        'field_name': 'length_unit'}

    attributes: list of str
      e.g.
      length_attributes = [
        'unused',
        'A',
        'Bohr',
        'cm',
        'm',
        'nm']

    output: files with `fname` below
    """

    fname = 'test_field_name.py-template'

    dir_path = os.path.dirname(os.path.realpath(__file__))
    template_name = os.path.join(dir_path, fname)
    with open(template_name, 'r') as fin:
        template = fin.read()

    # add attributes
    attr_str = 'attributes = [\n'
    for attr in attributes:
        attr_str += '  kimpy.field_name.{},\n'.format(attr)
    attr_str += ']\n'
    template = template.replace('rpls_attributes', attr_str)

    # add str names
    attr_str = 'str_names = [\n'
    for attr in attributes:
        attr_str += '  "{}",\n'.format(attr)
    attr_str += ']\n'
    template = template.replace('rpls_str_names', attr_str)

    # replace field names
    for key, value in fields.items():
        template = template.replace(key, value)
        fname = fname.replace(key, value)

    # replace number of attributes
    template = template.replace('rpls_num_attributes', str(len(attributes)))

    # write to file
    parent_path = os.path.dirname(dir_path)
    fname = os.path.join(parent_path, 'tests', fname.split('-')[0])
    with open(fname, 'w') as fout:
        fout.write('# This file is generated automatically by scripts\n')
        fout.write(template)
