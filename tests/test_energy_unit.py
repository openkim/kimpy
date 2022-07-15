# This file is generated automatically by generate_UnitSystem_bind_test.py.
# Do not modify this file, but modify the script instead.
import kimpy

attributes = [
  kimpy.energy_unit.unused,
  kimpy.energy_unit.amu_A2_per_ps2,
  kimpy.energy_unit.erg,
  kimpy.energy_unit.eV,
  kimpy.energy_unit.Hartree,
  kimpy.energy_unit.J,
  kimpy.energy_unit.kcal_mol,
]


str_names = [
  "unused",
  "amu_A2_per_ps2",
  "erg",
  "eV",
  "Hartree",
  "J",
  "kcal_mol",
]



def test_main():
    """Main test function."""
    N = kimpy.energy_unit.get_number_of_energy_units()

    assert N == 7

    all_instances = []
    for i in range(N):
        inst = kimpy.energy_unit.get_energy_unit(i)

        all_instances.append(inst)

        assert inst == attributes[i]
        assert str(inst) == str_names[i]

    # test operator overloading
    for i in range(N):
        assert all_instances[i] == all_instances[i]

        for j in range(i + 1, N):
            assert all_instances[i] != all_instances[j]

    # test known
    for inst in all_instances:
        assert inst.known() == True

    # test out of bound
    capture_out_of_bound_error = False

    try:
        inst = kimpy.energy_unit.get_energy_unit(N)
    except RuntimeError:
        capture_out_of_bound_error = True

    assert capture_out_of_bound_error


if __name__ == "__main__":
    test_main()
