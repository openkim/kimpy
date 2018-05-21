#from __future__ import absolute_import, division, print_function
import kimpy
from species_name_attributes import attributes, str_names

def test_main():

  N = kimpy.species_name.get_number_of_species_names()
  assert N == 139

  all_instances = []
  for i in range(N):
    inst,error = kimpy.species_name.get_species_name(i)
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
  inst,error = kimpy.species_name.get_species_name(N)
  assert error == True

  #help(kimpy.species_name.get_number_of_species_names)
  #help(kimpy.species_name.get_species_name)


if __name__ == '__main__':
  test_main()
