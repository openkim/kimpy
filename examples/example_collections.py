import kimpy


def dirs_for_collection(collection, collections):

    for it in (
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ):
        try:
            extent = collections.cache_list_of_directory_names(collection, it)
        except RuntimeError:
            msg = 'Calling "collections.cache_list_of_directory_names" failed.'
            raise kimpy.KimPyError(msg)

        print("{}:{}:".format(str(collection), str(it)))

        for i in range(extent):
            try:
                name = collections.get_directory_name(i)
            except RuntimeError:
                msg = 'Calling "collections.get_directory_name" for index = '
                msg += "{} failed.".format(i)
                raise kimpy.KimPyError(msg)

            print("    ", name)


def names_for_collection(collection, collections):

    for it in (
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ):
        try:
            extent = collections.cache_list_of_item_names_by_collection_and_type(
                collection, it
            )
        except RuntimeError:
            msg = 'Calling "collections.'
            msg += 'cache_list_of_item_names_by_collection_and_type" failed.'
            raise kimpy.KimPyError(msg)

        print("{}:{}:".format(str(collection), str(it)))

        for i in range(extent):
            try:
                name = collections.get_item_name_by_collection_and_type(i)
            except RuntimeError:
                msg = 'Calling "collections.'
                msg += 'get_item_name_by_collection_and_type" for index = '
                msg += "{} failed.".format(i)
                raise kimpy.KimPyError(msg)

            print("    ", name)


def example_main():

    try:
        collections = kimpy.collections.create()
    except RuntimeError:
        msg = 'Calling "kimpy.collections.create" failed.'
        raise kimpy.KimPyError(msg)

    project, semver = collections.get_project_name_and_sem_ver()

    print("Project: {}".format(project))
    print("semVer: {}".format(semver))

    for it in (
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ):
        try:
            name = collections.get_environment_variable_name(it)
        except RuntimeError:
            msg = 'Calling "collections.get_environment_variable_name" failed.'
            raise kimpy.KimPyError(msg)

        print("{} env name:{}".format(str(it), name))

    name, value = collections.get_configuration_file_environment_variable()

    print("config file env name: {}".format(name))
    print("config file env value: {}".format(value))

    filename = collections.get_configuration_file_name()

    print("config file name: {}".format(filename))

    for kc in (
        kimpy.collection.system,
        kimpy.collection.user,
        kimpy.collection.environmentVariable,
        kimpy.collection.currentWorkingDirectory,
    ):
        dirs_for_collection(kc, collections)

    for kc in (
        kimpy.collection.system,
        kimpy.collection.user,
        kimpy.collection.environmentVariable,
        kimpy.collection.currentWorkingDirectory,
    ):
        names_for_collection(kc, collections)

    for it in (
        kimpy.collection_item_type.modelDriver,
        kimpy.collection_item_type.portableModel,
        kimpy.collection_item_type.simulatorModel,
    ):
        try:
            extent = collections.cache_list_of_item_names_by_type(it)
        except RuntimeError:
            msg = 'Calling "collections.'
            msg += 'cache_list_of_item_names_by_type" failed.'
            raise kimpy.KimPyError(msg)

        print("{}:".format(str(it)))

        for i in range(extent):
            try:
                name = collections.get_item_name_by_type(i)
            except RuntimeError:
                msg = 'Calling "collections.get_item_name_by_type" '
                msg += "for index = {} failed.".format(i)
                raise kimpy.KimPyError(msg)

            print("    {}".format(name))

    try:
        filename, collection = collections.get_item_library_file_name_and_collection(
            kimpy.collection_item_type.simulatorModel,
            "Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu",
        )
    except RuntimeError:
        msg = 'Calling "collections.'
        msg += 'get_item_library_file_name_and_collection" failed.'
        raise kimpy.KimPyError(msg)

    msg = "Simulator Model Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu "
    msg += 'has library name "{}" and is part of the '.format(filename)
    msg += '"{}" collection'.format(str(collection))
    print(msg)

    try:
        extent = collections.cache_list_of_item_metadata_files(
            kimpy.collection_item_type.simulatorModel,
            "Sim_LAMMPS_LJcut_AkersonElliott_Alchemy_PbAu",
        )
    except RuntimeError:
        msg = 'Calling "collections.'
        msg += 'cache_list_of_item_metadata_files" failed.'
        raise kimpy.KimPyError(msg)

    for i in range(extent):
        try:
            (
                file_name,
                file_length,
                file_raw_data,
                avail_as_str,
                file_str,
            ) = collections.get_item_metadata_file(i)
        except RuntimeError:
            msg = 'Calling "collections.get_item_metadata_file" '
            msg += "for index = {} failed.".format(i)
            raise kimpy.KimPyError(msg)

        msg = "Metadata file {} ({}) ".format(i, file_name)
        msg += "is of length {}".format(file_length)

        print(msg)
        print(file_str)


if __name__ == "__main__":
    example_main()
