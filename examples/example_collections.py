from __future__ import absolute_import, division, print_function
import kimpy
from error import check_error, report_error


def dirs_for_collection(collection, col):
    for it in [
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ]:
        extent, error = col.cache_list_of_directory_names(collection, it)
        check_error(error, 'cache_list_of_directory_names')
        print(str(collection), ':', str(it), ':')
        for i in range(extent):
            name, error = col.get_directory_name(i)
            check_error(error, 'get_directory_name')
            print('    ', name)


def names_for_collection(collection, col):
    for it in [
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ]:
        extent, error = col.cache_list_of_item_names_by_collection_and_type(
            collection, it
        )
        check_error(error, 'cache_list_of_directory_names')
        print(str(collection), ':', str(it), ':')
        for i in range(extent):
            name, error = col.get_item_name_by_collection_and_type(i)
            check_error(error, 'get_item_name_by_collection_and_type')
            print('    ', name)


def example_main():
    col, error = kimpy.collections.create()
    check_error(error, 'collections.create')

    project, semver = col.get_project_name_and_sem_ver()
    print('Project:', project)
    print('semVer:', semver)

    for it in [
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ]:
        name, error = col.get_environment_variable_name(it)
        check_error(error, 'get_environment_variable_name')
        print(str(it), ' env name:', name)

    name, value = col.get_configuration_file_environment_variable()
    print('config file env name:', name)
    print('config file env value:', value)

    filename = col.get_configuration_file_name()
    print('config file name:', filename)

    for kc in [
        kimpy.collection.system,
        kimpy.collection.user,
        kimpy.collection.environmentVariable,
        kimpy.collection.currentWorkingDirectory,
    ]:
        dirs_for_collection(kc, col)

    for kc in [
        kimpy.collection.system,
        kimpy.collection.user,
        kimpy.collection.environmentVariable,
        kimpy.collection.currentWorkingDirectory,
    ]:
        names_for_collection(kc, col)

    for it in [
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ]:
        extent, error = col.cache_list_of_item_names_by_type(it)
        check_error(error, 'cache_list_of_item_names_by_type')
        print(str(it), ':')
        for i in range(extent):
            name, error = col.get_item_name_by_type(i)
            check_error(error, 'get_item_name_by_type')
            print('    ', name)

    filename, collection, error = col.get_item_library_file_name_and_collection(
        kimpy.collection_item_type.simulatorModel,
        'Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu',
    )
    check_error(error, 'get_item_library_file_name_and_collection')
    print(
        'Simulator Model Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu has library name ',
        '"{}" and is part of the "{}" collection'.format(filename, str(collection)),
    )

    extent, error = col.cache_list_of_item_metadata_files(
        kimpy.collection_item_type.simulatorModel,
        "Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu",
    )
    check_error(error, 'cache_list_of_item_metadata_files')

    for i in range(extent):
        out = col.get_item_metadata_file(i)
        file_name, file_length, file_raw_data, avail_as_str, file_str, error = out
        check_error(error, 'get_item_metadata_file')
        print('Metadata file {} ({}) is of length {}'.format(i, file_name, file_length))
        print(file_str)

    kimpy.collections.destroy(col)


if __name__ == '__main__':
    example_main()
