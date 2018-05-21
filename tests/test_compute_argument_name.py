#from __future__ import absolute_import, division, print_function
import kimpy

def test_main():

  N = kimpy.compute_argument_name.get_number_of_compute_arguments()
  assert N == 9

  arg0,error0 = kimpy.compute_argument_name.get_compute_argument_name(0)
  dtype0,_ = kimpy.compute_argument_name.get_compute_argument_data_type(arg0)
  arg3,error3 = kimpy.compute_argument_name.get_compute_argument_name(3)
  dtype3,_ = kimpy.compute_argument_name.get_compute_argument_data_type(arg3)
  arg00,error00 = kimpy.compute_argument_name.get_compute_argument_name(0)
  argN,errorN = kimpy.compute_argument_name.get_compute_argument_name(N)

  # class instances
  assert arg0 == arg00
  assert arg0 != arg3
  assert str(arg0) == 'numberOfParticles'
  #assert str(arg3) == 'coordinates'

  # error handling
  assert error0 == False
  assert errorN == True

  # module attributes
  assert arg0 == kimpy.compute_argument_name.numberOfParticles
  assert str(kimpy.compute_argument_name.numberOfParticles) == 'numberOfParticles'

  # data type
  print dtype3
  assert str(dtype0) == 'Integer'
  assert dtype0 == kimpy.data_type.Integer
  assert dtype3 == kimpy.data_type.Double # cannot pass due to Bug in API

  #help(kimpy.compute_argument_name.get_number_of_compute_arguments)
  #help(kimpy.compute_argument_name.get_compute_argument_name)
  #help(kimpy.compute_argument_name.get_compute_argument_data_type)


if __name__ == '__main__':
  test_main()
