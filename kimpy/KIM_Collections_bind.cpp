#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <memory>
#include <string>

#include "KIM_SimulatorHeaders.hpp"

namespace py = pybind11;
using namespace KIM;


namespace
{
struct PyCollectionsDestroy
{
  void operator()(Collections * collections) const
  {
    Collections::Destroy(&collections);
  }
};
}  // namespace


PYBIND11_MODULE(collections, module)
{
  module.doc() = "Python binding to KIM_Collections.hpp";

  py::class_<Collections, std::unique_ptr<Collections, PyCollectionsDestroy> >
      cl(module, "Collections");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([]() {
    Collections * collections;

    int error = Collections::Create(&collections);
    if (error == 1)
    {
      throw std::runtime_error(
          "Unable to create a new KIM-API Collections object!");
    }

    return std::unique_ptr<Collections, PyCollectionsDestroy>(
        std::move(collections));
    }))
    .def("get_item_type",
         [](Collections &self, std::string const &item_name) {
    CollectionItemType collection_item_type;

    int error = self.GetItemType(item_name, &collection_item_type);
    if (error == 1)
    {
      throw std::runtime_error(
          "An item with the specificed name cannot be found!");
    }

    return collection_item_type;
    }, R"pbdoc(
       Get the collection_item_type of the item in the KIM-API Collections with
       a specific name.

       Returns:
           CollectionItemType: collection_item_type
       )pbdoc",
       py::arg("item_name"))
    .def("get_item_library_file_name_and_collection",
         [](Collections &self,
            CollectionItemType const &item_type,
            std::string const &item_name) {
    std::string const * file_name;

    Collection collection;

    int error = self.GetItemLibraryFileNameAndCollection(
        item_type, item_name, &file_name, &collection);
    if (error == 1)
    {
      throw std::runtime_error(
          "item_type is unknown!  or\nan item with the specified type "
          "and name cannot be found!");
    }

    py::tuple re(2);
    re[0] = *file_name;
    re[1] = collection;
    return re;
    }, R"pbdoc(
       Get the item's library file name and its KIM::Collection.

       Returns:
           str, Collection: file_name, collection
       )pbdoc",
       py::arg("item_type"),
       py::arg("item_name"))
    .def("cache_list_of_item_metadata_files",
         [](Collections &self,
            CollectionItemType const &item_type,
            std::string const &item_name) {
    int extent;

    int error
        = self.CacheListOfItemMetadataFiles(item_type, item_name, &extent);
    if (error == 1)
    {
      throw std::runtime_error(
          "item_type is unknown!  or\nthe list is not successfully cached "
          "for some reason!");
    }

    return extent;
    }, R"pbdoc(
       Cache a list of an item's metadata files.

       Returns:
           int: extent
       )pbdoc",
       py::arg("item_type"),
       py::arg("item_name"))
    .def("get_item_metadata_file",
         [](Collections &self, int const index) {
    std::string const * file_name;
    unsigned int file_length;
    unsigned char const * file_raw_data;
    int available_as_string;
    std::string const * file_string;

    int error = self.GetItemMetadataFile(index,
                                         &file_name,
                                         &file_length,
                                         &file_raw_data,
                                         &available_as_string,
                                         &file_string);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    py::tuple re(5);
    re[0] = *file_name;
    re[1] = file_length;
    re[2] = file_raw_data;
    re[3] = available_as_string;
    re[4] = *file_string;
    return re;
    }, R"pbdoc(
       Get the name and content of one of an item's metadata files.

       Returns:
           str, int, str, int, str: file_name, file_length, file_raw_data,
                available_as_string, file_string
       )pbdoc",
       py::arg("index"))
    .def("cache_list_of_item_names_by_type",
         [](Collections &self, CollectionItemType const &item_type) {
    int extent;

    int error = self.CacheListOfItemNamesByType(item_type, &extent);
    if (error == 1) { throw std::runtime_error("item_type is unknown!"); }

    return extent;
    }, R"pbdoc(
       Cache a list of all item names of a specific type in the KIM-API
       Collections.

       Returns:
           int: extent
       )pbdoc",
       py::arg("item_type"))
    .def("get_item_name_by_type", [](Collections &self, int const index) {
    std::string const * item_name;

    int error = self.GetItemNameByType(index, &item_name);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    return *item_name;
    }, R"pbdoc(
       Get the name of an item from the cached list.

       Returns:
           str: item_name
       )pbdoc",
       py::arg("index"))
    .def("cache_list_of_item_names_by_collection_and_type",
         [](Collections &self,
            Collection const &collection,
            CollectionItemType const &item_type) {
    int extent;

    int error = self.CacheListOfItemNamesByCollectionAndType(
        collection, item_type, &extent);
    if (error == 1)
    {
      throw std::runtime_error("collection or item_type are unknown!");
    }

    return extent;
    }, R"pbdoc(
       Cache a list of all item names of a specific type in a specific
       collection.

       Returns:
           int: extent
       )pbdoc",
       py::arg("collection"),
       py::arg("item_type"))
    .def("get_item_name_by_collection_and_type",
         [](Collections &self, int const index) {
    std::string const * item_name;

    int error = self.GetItemNameByCollectionAndType(index, &item_name);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    return *item_name;
    }, R"pbdoc(
       Get the name of an item from the cached list.

       Returns:
           str: item_name
       )pbdoc",
       py::arg("index"))
    .def("get_item_library_file_name_by_collection_and_type",
         [](Collections &self,
            Collection const &collection,
            CollectionItemType const &item_type,
            std::string const &item_name) {
    std::string const * file_name;

    int error = self.GetItemLibraryFileNameByCollectionAndType(
        collection, item_type, item_name, &file_name);
    if (error == 1)
    {
      throw std::runtime_error(
          "collection or item_type are unknown, or\nan item "
          "with the specified type and name cannot be found "
          "in the specified collection!");
    }

    return *file_name;
    }, R"pbdoc(
       Get the item's library file name.

       Returns:
           str: file_name
       )pbdoc",
       py::arg("collection"),
       py::arg("item_type"),
       py::arg("item_name"))
    .def("cache_list_of_item_metadata_files_by_collection_and_type",
         [](Collections &self,
            Collection const &collection,
            CollectionItemType const &item_type,
            std::string const &item_name) {
    int extent;

    int error = self.CacheListOfItemMetadataFilesByCollectionAndType(
        collection, item_type, item_name, &extent);
    if (error == 1)
    {
      throw std::runtime_error(
          "collection or item_type are unknown, or\nthe list "
          "is not successfully cached for some reason!");
    }

    return extent;
    }, R"pbdoc(
       Cache a list of an item's metadata files.

       Returns:
           int: extent
       )pbdoc",
       py::arg("collection"),
       py::arg("item_type"),
       py::arg("item_name"))
    .def("get_item_metadata_file_by_collection_and_type",
         [](Collections &self, int const index) {
    std::string const * file_name;
    unsigned int file_length;
    unsigned char const * file_raw_data;
    int available_as_string;
    std::string const * file_string;

    int error
        = self.GetItemMetadataFileByCollectionAndType(index,
                                                      &file_name,
                                                      &file_length,
                                                      &file_raw_data,
                                                      &available_as_string,
                                                      &file_string);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    py::tuple re(5);
    re[0] = *file_name;
    re[1] = file_length;
    re[2] = file_raw_data;
    re[3] = available_as_string;
    re[4] = *file_string;
    return re;
    }, R"pbdoc(
       Get the name and content of one of an item's metadata files.

       Returns:
           str, int, str, int, int: file_name, file_length, file_raw_data,
                available_as_string, file_string
       )pbdoc",
       py::arg("index"))
    .def("get_project_name_and_sem_ver", [](Collections &self) {
    std::string const * project_name;
    std::string const * sem_ver;

    self.GetProjectNameAndSemVer(&project_name, &sem_ver);

    py::tuple re(2);
    re[0] = *project_name;
    re[1] = *sem_ver;
    return re;
    }, R"pbdoc(
       Get the KIM-API project name and full Semantic Version string.

       Returns:
           str, str: project_name, sem_ver
       )pbdoc")
    .def("get_environment_variable_name",
         [](Collections &self, CollectionItemType const &item_type) {
    std::string const * name;

    int error = self.GetEnvironmentVariableName(item_type, &name);
    if (error == 1) { throw std::runtime_error("item_type is unknown!"); }

    return *name;
    }, R"pbdoc(
       Get the names of environment variables that store configuration
       settings for the KIM::COLLECTION::environmentVariable collection.

       Returns:
           str: name
       )pbdoc",
       py::arg("item_type"))
    .def("get_configuration_file_environment_variable",
         [](Collections &self) {
    std::string const * name;
    std::string const * value;

    self.GetConfigurationFileEnvironmentVariable(&name, &value);

    py::tuple re(2);
    re[0] = *name;
    re[1] = *value;
    return re;
    }, R"pbdoc(
       Get the name and value of the environment variable that stores the
       name of the KIM-API user configuration file.

       Returns:
           str, str: name, value
       )pbdoc")
    .def("get_configuration_file_name", [](Collections &self) {
    std::string const * file_name;

    self.GetConfigurationFileName(&file_name);

    return *file_name;
    }, R"pbdoc(
       Get the absolute file and path name of the KIM-API user configuration
       file.

       Returns:
           str: file_name
       )pbdoc")
    .def("cache_list_of_directory_names",
         [](Collections &self,
            Collection const &collection,
            CollectionItemType const &item_type) {
    int extent;

    int error = self.CacheListOfDirectoryNames(collection, item_type, &extent);
    if (error == 1)
    {
      throw std::runtime_error(
          "collection or item_type are unknown! or\n"
          "the list is not successfully cached for some reason!");
    }

    return extent;
    }, R"pbdoc(
       Cache a list of directory names where a specific KIM-API
       Collection stores library files for a specific item type.

       Returns:
           int: extent
       )pbdoc",
       py::arg("collection"),
       py::arg("item_type"))
    .def("get_directory_name", [](Collections &self, int const index) {
    std::string const * directory_name;

    int error = self.GetDirectoryName(index, &directory_name);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    return *directory_name;
    }, R"pbdoc(
       Get the name of a directory from the cached list.

       Returns:
           str: directory_name
       )pbdoc",
       py::arg("index"))
    .def("set_log_id", &Collections::SetLogID,
         "Set the identity of the Log object associated with the "
         "collections object.",
         py::arg("log_id"))
    .def("push_log_verbosity", &Collections::PushLogVerbosity,
         "Push a new log_verbosity onto the collections object's Log object "
         "verbosity stack.",
         py::arg("log_verbosity"))
    .def("pop_log_verbosity", &Collections::PopLogVerbosity,
         "Pop a log_verbosity from the collections object's Log object "
         "verbosity stack.");

  module.def(
      "create",
      []() {
    Collections * collections;

    int error = Collections::Create(&collections);
    if (error == 1)
    {
      throw std::runtime_error(
          "Unable to create a new KIM-API Collections object!");
    }

    return std::unique_ptr<Collections, PyCollectionsDestroy>(
        std::move(collections));
      }, R"pbdoc(
         Create a new KIM-API Collections object.

         Returns:
             Collections: collections
         )pbdoc"
      );
}
