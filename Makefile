build:
	python setup.py build
dev:
	python setup.py develop
inplace:
	python setup.py build_ext --inplace
install:
	python setup.py install
clean:
	python setup.py clean
clean_all:
	rm -rf build/ dist/ ase_kim_calculator.egg-info/ *.so kimpy/*.so kimpy/*pyc
