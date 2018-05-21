#from __future__ import absolute_import, division, print_function
import kimpy

attributes = [
  kimpy.support_status.requiredByAPI,
  kimpy.support_status.notSupported,
  kimpy.support_status.required,
  kimpy.support_status.optional
]

str_names = [
  'requiredByAPI',
  'notSupported',
  'required',
  'optional'
]

def test_main():

  N = kimpy.support_status.get_number_of_support_statuses()
  assert N == 4

  all_instances = []
  for i in range(N):
    inst,error = kimpy.support_status.get_support_status(i)
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
  inst,error = kimpy.support_status.get_support_status(N)
  assert error == True

  #help(kimpy.support_status.get_number_of_support_statuses)
  #help(kimpy.support_status.get_support_status)


if __name__ == '__main__':
  test_main()
