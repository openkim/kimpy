#from __future__ import absolute_import, division, print_function
import kimpy

attributes = [
  kimpy.data_type.Integer,
  kimpy.data_type.Double
]

str_names = [
  'Integer',
  'Double'
]

def test_main():

  N = kimpy.data_type.get_number_of_data_types()
  assert N == 2

  all_instances = []
  for i in range(N):
    inst,error = kimpy.data_type.get_data_type(i)
    all_instances.append(inst)

    assert error == False
    assert inst == attributes[i]
    assert str(inst) == str_names[i]

  # test operator overloading
  for i in range(N):
    assert all_instances[i] == all_instances[i]
    for j in range(i+1, N):
      assert all_instances[i] != all_instances[j]

  # test out of bound
  inst,error = kimpy.data_type.get_data_type(N)
  assert error == True

  #help(kimpy.data_type.get_number_of_data_types)
  #help(kimpy.data_type.get_data_type)


if __name__ == '__main__':
  test_main()
