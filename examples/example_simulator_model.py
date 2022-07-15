from os.path import join
import kimpy


def example_main():

    modelname = "Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu"

    try:
        sm = kimpy.simulator_model.create(modelname)
    except RuntimeError:
        msg = 'Calling "kimpy.simulator_model.create" failed.'
        raise kimpy.KimPyError(msg)

    name, version = sm.get_simulator_name_and_version()

    print("Simulator name: {}".format(name))
    print("Simulator version: {}".format(version))

    extent = sm.get_number_of_supported_species()
    print("Simulator supports {} species.".format(extent))

    for i in range(extent):
        try:
            species = sm.get_supported_species(i)
        except RuntimeError:
            msg = 'Calling "sm.get_supported_species" for '
            msg += "index = {} failed.".format(i)
            raise kimpy.KimPyError(msg)

        print("{}  {}".format(i, species))

    try:
        sm.add_template_map("atom-type-sym-list", "Pb Pb Au Pb")
    except RuntimeError:
        raise kimpy.KimPyError('Calling "sm.add_template_map" failed.')

    sm.close_template_map()

    number_fields = sm.get_number_of_simulator_fields()
    print("Simulator model has {} fields.".format(number_fields))

    for i in range(number_fields):
        try:
            extent, field_name = sm.get_simulator_field_metadata(i)
        except RuntimeError:
            msg = 'Calling "sm.get_simulator_field_metadata" for '
            msg += "fieldIndex = {} failed.".format(i)
            raise kimpy.KimPyError(msg)

        print("Field {} is {} and has lines: {}".format(i, field_name, extent))

        for j in range(extent):
            try:
                field_line = sm.get_simulator_field_line(i, j)
            except RuntimeError:
                msg = 'Calling "sm.get_simulator_field_line" for '
                msg += "fieldIndex = {} & lineIndex = ".format(i)
                msg += "{} failed.".format(j)
                raise kimpy.KimPyError(msg)

            print("    ", field_line)

    dirname = sm.get_parameter_file_directory_name()
    print("Simulator model parameter directory name is:", dirname)

    specname = sm.get_specification_file_name()
    print("Simulator model specification file name is:", specname)

    fname = join(dirname, specname)
    with open(fname, "r") as f:
        print(f.read())

    num_param_files = sm.get_number_of_parameter_files()
    print("Simulator model has {} parameter files.".format(num_param_files))

    for i in range(num_param_files):
        try:
            paramname = sm.get_parameter_file_name(i)
        except RuntimeError:
            msg = 'Calling "sm.get_parameter_file_name" for '
            msg += "index = {} failed.".format(i)
            raise kimpy.KimPyError(msg)

        print("Parameter file {} has name: {}".format(i, paramname))

        fname = join(dirname, paramname)
        with open(fname, "r") as f:
            print(f.read())


if __name__ == "__main__":
    example_main()
