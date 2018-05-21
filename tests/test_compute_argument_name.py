#from __future__ import absolute_import, division, print_function
import kimpy

def test_main():

  N = kimpy.compute_argument_name.get_number_of_compute_arguments()
  assert N == 9

  arg0,error0 = kimpy.compute_argument_name.get_compute_argument_name(0)
  arg00,_ = kimpy.compute_argument_name.get_compute_argument_name(0)
  arg1,_ = kimpy.compute_argument_name.get_compute_argument_name(1)
  arg10,error10 = kimpy.compute_argument_name.get_compute_argument_name(10)

  # class instances
  assert arg0 == arg00
  assert arg0 != arg1
  assert str(arg0) == 'numberOfParticles'
  assert str(arg1) == 'particleSpeciesCodes'

  # error handling
  assert error0 == False
  assert error10 == True

  # module attributes
  assert arg0 == kimpy.compute_argument_name.numberOfParticles
  assert str(kimpy.compute_argument_name.numberOfParticles) == 'numberOfParticles'

  #help(kimpy.compute_argument_name.get_number_of_compute_arguments)
  #help(kimpy.compute_argument_name.get_compute_argument_name)
  #help(kimpy.compute_argument_name.get_compute_argument_data_type)


if __name__ == '__main__':
  test_main()
