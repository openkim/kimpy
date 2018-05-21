#from __future__ import absolute_import, division, print_function
import kimpy

def test_main():

  N = kimpy.data_type.get_number_of_data_types()
  assert N == 2

  arg0,error0 = kimpy.data_type.get_data_type(0)
  arg00,_ = kimpy.data_type.get_data_type(0)
  arg1,_ = kimpy.data_type.get_data_type(1)
  argN,errorN = kimpy.data_type.get_data_type(N)

  # class instances
  assert arg0 == arg00
  assert arg0 != arg1
  assert str(arg0) == 'Integer'
  assert str(arg1) == 'Double'

  # error handling
  assert error0 == False
  assert errorN == True

  # module attributes
  assert arg0 == kimpy.data_type.Integer
  assert str(kimpy.data_type.Integer) == 'Integer'

  #help(kimpy.data_type.get_number_of_data_types)
  #help(kimpy.data_type.get_data_type)


if __name__ == '__main__':
  test_main()
