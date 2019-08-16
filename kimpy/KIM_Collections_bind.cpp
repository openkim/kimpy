#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "KIM_SimulatorHeaders.hpp"
#include "sim_buffer.h"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(collections, module)
{
  module.doc() = "Python binding to KIM_Collections.hpp";

  // C++ class constructor and destructor are private, which can not be wrapped
  // directly.
  // So we need to call the C++ class factory function in py::init, and use
  // `py::nodelete` to avoid calling the destructor when python instance is
  // destroyed. It is crucial that the instance is deallocated by calling the
  // destroy function from the C++ side to avoid memory leaks. For more info,
  // see http://pybind11.readthedocs.io/en/stable/advanced/classes.html

  py::class_<Collections, std::unique_ptr<Collections, py::nodelete> > cl(
      module, "Collections");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([](py::array_t<int> error) {
      Collections * co;
      int * e = error.mutable_data(0);
      e[0] = Collections::Create(&co);
      return co;
    }))

      .def(
          "get_item_type",
          [](Collections & self, std::string const & itemName) {
            CollectionItemType itemType;
            int error = self.GetItemType(itemName, &itemType);

            py::tuple re(2);
            re[0] = itemType;
            re[1] = error;
            return re;
          },
          py::arg("itemName"),
          "Return(itemType, error)")

      .def(
          "get_item_library_file_name_and_collection",
          [](Collections & self,
             CollectionItemType const itemType,
             std::string const & itemName) {
            std::string const * fileName;
            Collection collection;
            int error = self.GetItemLibraryFileNameAndCollection(
                itemType, itemName, &fileName, &collection);

            py::tuple re(3);
            re[0] = *fileName;
            re[1] = collection;
            re[2] = error;
            return re;
          },
          py::arg("itemType"),
          py::arg("itemName"),
          "Return(fileName, collection, error)")

      .def(
          "cache_list_of_item_metadata_files",
          [](Collections & self,
             CollectionItemType const itemType,
             std::string const & itemName) {
            int extent;
            int error = self.CacheListOfItemMetadataFiles(
                itemType, itemName, &extent);

            py::tuple re(2);
            re[0] = extent;
            re[1] = error;
            return re;
          },
          py::arg("itemType"),
          py::arg("itemName"),
          "Return(extent, error)")

      .def(
          "get_item_metadata_file",
          [](Collections & self, int const index) {
            std::string const * fileName;
            unsigned int fileLength;
            unsigned char const * fileRawData;
            int availableAsString;
            std::string const * fileString;
            int error = self.GetItemMetadataFile(index,
                                                 &fileName,
                                                 &fileLength,
                                                 &fileRawData,
                                                 &availableAsString,
                                                 &fileString);

            py::tuple re(6);
            re[0] = *fileName;
            re[1] = fileLength;
            re[2] = fileRawData;
            re[3] = availableAsString;
            re[4] = *fileString;
            re[5] = error;
            return re;
          },
          py::arg("index"),
          "Return(fileName, fileLength, fileRawData, availableAsString, "
          "fileString, error)")

      .def(
          "cache_list_of_item_names_by_type",
          [](Collections & self, CollectionItemType const itemType) {
            int extent;
            int error = self.CacheListOfItemNamesByType(itemType, &extent);

            py::tuple re(2);
            re[0] = extent;
            re[1] = error;
            return re;
          },
          py::arg("itemType"),
          "Return(extent, error)")

      .def(
          "get_item_name_by_type",
          [](Collections & self, int const index) {
            std::string const * itemName;
            int error = self.GetItemNameByType(index, &itemName);

            py::tuple re(2);
            re[0] = *itemName;
            re[1] = error;
            return re;
          },
          py::arg("index"),
          "Return(itemName, error)")

      .def(
          "cache_list_of_item_names_by_collection_and_type",
          [](Collections & self,
             Collection const collection,
             CollectionItemType const itemType) {
            int extent;
            int error = self.CacheListOfItemNamesByCollectionAndType(
                collection, itemType, &extent);

            py::tuple re(2);
            re[0] = extent;
            re[1] = error;
            return re;
          },
          py::arg("collection"),
          py::arg("itemType"),
          "Return(extent, error)")

      .def(
          "get_item_name_by_collection_and_type",
          [](Collections & self, int const index) {
            std::string const * itemName;
            int error = self.GetItemNameByCollectionAndType(index, &itemName);

            py::tuple re(2);
            re[0] = *itemName;
            re[1] = error;
            return re;
          },
          py::arg("index"),
          "Return(itemName, error)")

      .def(
          "get_item_library_file_name_by_collection_and_type",
          [](Collections & self,
             Collection const collection,
             CollectionItemType const itemType,
             std::string const & itemName) {
            std::string const * fileName;
            int error = self.GetItemLibraryFileNameByCollectionAndType(
                collection, itemType, itemName, &fileName);

            py::tuple re(2);
            re[0] = *fileName;
            re[1] = error;
            return re;
          },
          py::arg("collection"),
          py::arg("itemType"),
          py::arg("itemName"),
          "Return(fileName, error)")

      .def(
          "cache_list_of_item_metadata_files_by_collection_and_type",
          [](Collections & self,
             Collection const collection,
             CollectionItemType const itemType,
             std::string const & itemName) {
            int extent;
            int error = self.CacheListOfItemMetadataFilesByCollectionAndType(
                collection, itemType, itemName, &extent);

            py::tuple re(2);
            re[0] = extent;
            re[1] = error;
            return re;
          },
          py::arg("collection"),
          py::arg("itemType"),
          py::arg("itemName"),
          "Return(extent, error)")

      .def(
          "get_item_metadata_file_by_collection_and_type",
          [](Collections & self, int const index) {
            std::string const * fileName;
            unsigned int fileLength;
            unsigned char const * fileRawData;
            int availableAsString;
            std::string const * fileString;
            int error = self.GetItemMetadataFileByCollectionAndType(
                index,
                &fileName,
                &fileLength,
                &fileRawData,
                &availableAsString,
                &fileString);

            py::tuple re(6);
            re[0] = *fileName;
            re[1] = fileLength;
            re[2] = fileRawData;
            re[3] = availableAsString;
            re[4] = *fileString;
            re[5] = error;
            return re;
          },
          py::arg("index"),
          "Return(fileName, fileLength, fileRawData, availableAsString, "
          "fileString, error)")

      .def(
          "get_project_name_and_sem_ver",
          [](Collections & self) {
            std::string const * projectName;
            std::string const * semVer;
            self.GetProjectNameAndSemVer(&projectName, &semVer);

            py::tuple re(2);
            re[0] = *projectName;
            re[1] = *semVer;
            return re;
          },
          "Return(projectName, semVer)")

      .def(
          "get_environment_variable_name",
          [](Collections & self, CollectionItemType const itemType) {
            std::string const * name;
            int error = self.GetEnvironmentVariableName(itemType, &name);

            py::tuple re(2);
            re[0] = *name;
            re[1] = error;
            return re;
          },
          py::arg("itemType"),
          "Return(name, error)")

      .def(
          "get_configuration_file_environment_variable",
          [](Collections & self) {
            std::string const * name;
            std::string const * value;
            self.GetConfigurationFileEnvironmentVariable(&name, &value);

            py::tuple re(2);
            re[0] = *name;
            re[1] = *value;
            return re;
          },
          "Return(name, value)")

      .def(
          "get_configuration_file_name",
          [](Collections & self) {
            std::string const * fileName;
            self.GetConfigurationFileName(&fileName);

            return *fileName;
          },
          "Return fileName")

      .def(
          "cache_list_of_directory_names",
          [](Collections & self,
             Collection const collection,
             CollectionItemType const itemType) {
            int extent;
            int error
                = self.CacheListOfDirectoryNames(collection, itemType, &extent);

            py::tuple re(2);
            re[0] = extent;
            re[1] = error;
            return re;
          },
          py::arg("collection"),
          py::arg("itemType"),
          "Return(extent, error)")

      .def(
          "get_directory_name",
          [](Collections & self, int const index) {
            std::string const * directoryName;
            int error = self.GetDirectoryName(index, &directoryName);

            py::tuple re(2);
            re[0] = *directoryName;
            re[1] = error;
            return re;
          },
          py::arg("index"),
          "Return(directoryName, error)")

      .def("set_log_id", &Collections::SetLogID)

      .def("push_log_verbosity", &Collections::PushLogVerbosity)

      .def("pop_log_verbosity", &Collections::PopLogVerbosity);


  // module functions

  module.def(
      "create",
      []() {
        Collections * co;
        int error = Collections::Create(&co);

        py::tuple re(2);
        re[0] = co;
        re[1] = error;
        return re;
      },
      "Return(Collections, error)");

  module.def("destroy",
             [](Collections * self) { Collections::Destroy(&self); });
}
