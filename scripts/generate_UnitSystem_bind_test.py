import os


length_unit = {
  'FIELD_NAME': 'LENGTH_UNIT',
  'FieldName': 'LengthUnit',
  'filedName': 'lengthUnit',
  'field_name': 'length_unit',
}
length_attributes = [
  'unused',
  'A',
  'Bohr',
  'cm',
  'm',
  'nm'
]

energy_unit = {
  'FIELD_NAME': 'ENERGY_UNIT',
  'FieldName': 'EnergyUnit',
  'filedName': 'energyUnit',
  'field_name': 'energy_unit',
}
energy_attributes = [
  'unused',
  'amu_A2_per_ps2',
  'erg',
  'eV',
  'Hartree',
  'J',
  'kcal_mol',
]

charge_unit = {
  'FIELD_NAME': 'CHARGE_UNIT',
  'FieldName': 'ChargeUnit',
  'filedName': 'chargeUnit',
  'field_name': 'charge_unit',
}
charge_attributes = [
  'unused',
  'C',
  'e',
  'statC',
]

temperature_unit = {
  'FIELD_NAME': 'TEMPERATURE_UNIT',
  'FieldName': 'TemperatureUnit',
  'FIELD_NAME': 'TEMPERATURE_UNIT',
  'FieldName': 'TemperatureUnit',
  'filedName': 'temperatureUnit',
  'field_name': 'temperature_unit',
}
temperature_attributes = [
  'unused',
  'K',
]

time_unit = {
  'FIELD_NAME': 'TIME_UNIT',
  'FieldName': 'TimeUnit',
  'filedName': 'timeUnit',
  'field_name': 'time_unit',
}
time_attributes = [
  'unused',
  'fs',
  'ps',
  'ns',
  's',
]


all_units = [
  length_unit,
  energy_unit,
  charge_unit,
  temperature_unit,
  time_unit
]
all_attributes = [
  length_attributes,
  energy_attributes,
  charge_attributes,
  temperature_attributes,
  time_attributes
]


dir_path = os.path.dirname(os.path.realpath(__file__))
parent_path = os.path.dirname(dir_path)

# write binding
template_name = os.path.join(dir_path, 'KIM_FieldName_bind.cpp-template')
with open(template_name, 'r') as fin:
  template = fin.read()

for units, attributes in zip(all_units, all_attributes):

  temp = template
  fname = os.path.join(parent_path, 'kimpy', os.path.basename(template_name).split('-')[0])

  # add attributes
  attr_str = ''
  for attr in attributes:
    attr_str += '  module.attr("{0}") = FIELD_NAME::{0};\n'.format(attr)
  temp = temp.replace('rpls_attributes', attr_str)

  # replace field names
  for key, value in units.iteritems():
    temp = temp.replace(key, value)
    fname = fname.replace(key, value)

  # write to file
  with open(fname, 'w') as fout:
    fout.write(temp)


# write tests
template_name = os.path.join(dir_path, 'test_field_name.py-template')
with open(template_name, 'r') as fin:
  template = fin.read()

for units, attributes in zip(all_units, all_attributes):

  temp = template
  fname = os.path.join(parent_path, 'tests', os.path.basename(template_name).split('-')[0])

  # add attributes
  attr_str = 'attributes = [\n'
  for attr in attributes:
    attr_str += '  kimpy.field_name.{},\n'.format(attr)
  attr_str += ']\n'
  temp = temp.replace('rpls_attributes', attr_str)

  # add str names
  attr_str = 'str_names = [\n'
  for attr in attributes:
    attr_str += '  "{}",\n'.format(attr)
  attr_str += ']\n'
  temp = temp.replace('rpls_str_names', attr_str)

  # replace field names
  for key, value in units.iteritems():
    temp = temp.replace(key, value)
    fname = fname.replace(key, value)

  # replace number of attributes
  temp = temp.replace('rpls_num_attributes', str(len(attributes)))

  # write to file
  with open(fname, 'w') as fout:
    fout.write(temp)


