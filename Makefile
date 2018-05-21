build:
	python setup.py build
develop:
	python setup.py develop
inplace:
	python setup.py build_ext --inplace
#install:
#	python setup.py install
clean:
	python setup.py clean
clean_all:
	rm -rf build/ dist/ *.so \
  kimpy.egg-info/kimpy/*.so \
  kimpy/*.pyc \
  kimpy/SpeciesName_attributes.cpp \
  kimpy/*Unit_bind.cpp \
  tests/*.pyc \
  tests/species_name_attributes.py \
  tests/*_unit.py \
  tests/__pycache__ \
