build:
	python setup.py build
develop:
	python setup.py develop
inplace:
	python setup.py build_ext --inplace
install:
	python setup.py install
clean:
	python setup.py clean
clean_all:
	rm -rf build/ dist/ kimpy.egg-info/ */*.so */*.pyc */__pycache__  \
  kimpy/*Unit_bind.cpp \
  kimpy/KIM_DataType_bind.cpp kimpy/KIM_Numbering_bind.cpp \
  kimpy/KIM_SpeciesName_bind.cpp kimpy/KIM_LanguageName_bind.cpp \
  kimpy/KIM_ComputeCallbackName_bind.cpp  kimpy/KIM_LogVerbosity_bind.cpp \
  kimpy/KIM_Collection_bind.cpp  kimpy/KIM_CollectionItemType_bind.cpp \
  tests/*_unit.py \
  tests/test_data_type.py tests/test_numbering.py tests/test_species_name.py \
  tests/test_language_name.py tests/test_compute_callback_name.py \
  tests/test_log_verbosity.py tests/test_support_status.py \
  tests/test_collection.py tests/test_collection_item_type.py \
