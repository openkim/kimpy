from generate_bind_test_commons import generate_bind, generate_test


length_unit = {
    'FIELD_NAME': 'LENGTH_UNIT',
    'FieldName': 'LengthUnit',
    'filedName': 'lengthUnit',
    'field_name': 'length_unit',
}
length_attributes = ['unused', 'A', 'Bohr', 'cm', 'm', 'nm']

energy_unit = {
    'FIELD_NAME': 'ENERGY_UNIT',
    'FieldName': 'EnergyUnit',
    'filedName': 'energyUnit',
    'field_name': 'energy_unit',
}
energy_attributes = ['unused', 'amu_A2_per_ps2', 'erg', 'eV', 'Hartree', 'J', 'kcal_mol']

charge_unit = {
    'FIELD_NAME': 'CHARGE_UNIT',
    'FieldName': 'ChargeUnit',
    'filedName': 'chargeUnit',
    'field_name': 'charge_unit',
}
charge_attributes = ['unused', 'C', 'e', 'statC']

temperature_unit = {
    'FIELD_NAME': 'TEMPERATURE_UNIT',
    'FieldName': 'TemperatureUnit',
    'FIELD_NAME': 'TEMPERATURE_UNIT',
    'FieldName': 'TemperatureUnit',
    'filedName': 'temperatureUnit',
    'field_name': 'temperature_unit',
}
temperature_attributes = ['unused', 'K']

time_unit = {
    'FIELD_NAME': 'TIME_UNIT',
    'FieldName': 'TimeUnit',
    'filedName': 'timeUnit',
    'field_name': 'time_unit',
}
time_attributes = ['unused', 'fs', 'ps', 'ns', 's']


all_units = [length_unit, energy_unit, charge_unit, temperature_unit, time_unit]
all_attributes = [
    length_attributes,
    energy_attributes,
    charge_attributes,
    temperature_attributes,
    time_attributes,
]


for fields, attributes in zip(all_units, all_attributes):
    generate_bind(fields, attributes)
    generate_test(fields, attributes)
