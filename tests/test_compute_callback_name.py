#from __future__ import absolute_import, division, print_function
import kimpy

attributes = [
  kimpy.compute_callback_name.GetNeighborList,
  kimpy.compute_callback_name.ProcessDEDrTerm,
  kimpy.compute_callback_name.ProcessD2EDr2Term
]

str_names = [
  'GetNeighborList',
  'ProcessDEDrTerm',
  'ProcessD2EDr2Term'
]

def test_main():

  N = kimpy.compute_callback_name.get_number_of_compute_callbacks()
  assert N == 3

  all_instances = []
  for i in range(N):
    inst,error = kimpy.compute_callback_name.get_compute_callback_name(i)
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
  inst,error = kimpy.compute_callback_name.get_compute_callback_name(N)
  assert error == True

  help(kimpy.compute_callback_name.get_number_of_compute_callbacks)
  help(kimpy.compute_callback_name.get_compute_callback_name)


if __name__ == '__main__':
  test_main()
