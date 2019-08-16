from __future__ import absolute_import, division, print_function
import kimpy
import os
import subprocess
from error import check_error, report_error


def example_main():
    modelname = 'Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu'
    sm, error = kimpy.simulator_model.create(modelname)
    check_error(error, 'simulator_model.create')

    name, version = sm.get_simulator_name_and_version()
    print('Simulator name:', name)
    print('Simulator version:', version)

    extent = sm.get_number_of_supported_species()
    print('Simulator supports {} species.'.format(extent))
    for i in range(extent):
        species, error = sm.get_supported_species(i)
        check_error(error, 'get_supported_species')
        print('{}  {}'.format(i, species))

    error = sm.add_template_map('atom-type-sym-list', 'Pb Pb Au Pb')
    check_error(error, 'add_template_map')
    sm.close_template_map()
    number_fields = sm.get_number_of_simulator_fields()
    print('Simulator model has {} fields.'.format(number_fields))

    for i in range(number_fields):
        extent, field_name, error = sm.get_simulator_field_metadata(i)
        check_error(error, 'get_simulator_field_metadata')
        print('Field {} is {} and has lines:'.format(i, field_name, extent))
        for j in range(extent):
            field_line, error = sm.get_simulator_field_line(i, j)
            check_error(error, 'get_simulator_field_line')
            print('    ', field_line)

    dirname = sm.get_parameter_file_directory_name()
    print('Simulator model parameter directory name is:', dirname)

    specname = sm.get_specification_file_name()
    print('Simulator model specification file name is:', specname)
    fname = os.path.join(dirname, specname)
    subprocess.call(['cat', fname])

    num_param_files = sm.get_number_of_parameter_files()
    print('Simulator model has {} parameter files.'.format(num_param_files))
    for i in range(num_param_files):
        paramname, error = sm.get_parameter_file_name(i)
        check_error(error, 'get_parameter_file_name')
        print('Parameter file {} has name: {}'.format(i, paramname))
        fname = os.path.join(dirname, paramname)
        subprocess.call(['cat', fname])

    kimpy.simulator_model.destroy(sm)


if __name__ == '__main__':
    example_main()
