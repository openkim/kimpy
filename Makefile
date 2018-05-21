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
	rm -rf build/ dist/ kimpy.egg-info/ *.so kimpy/*.so kimpy/*.pyc \
  kimpy/SpeciesName_attributes.cpp tests/*.pyc tests/species_name_attributes.py \
  tests/__pycache__
