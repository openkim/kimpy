# This file is generated automatically by generate_UnitSystem_bind_test.py.
# Do not modify this file, but modify the script instead.
import kimpy

attributes = [
  kimpy.temperature_unit.unused,
  kimpy.temperature_unit.K,
]


str_names = [
  "unused",
  "K",
]



def test_main():
    """Main test function."""
    N = kimpy.temperature_unit.get_number_of_temperature_units()

    assert N == 2

    all_instances = []
    for i in range(N):
        inst = kimpy.temperature_unit.get_temperature_unit(i)

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
        inst = kimpy.temperature_unit.get_temperature_unit(N)
    except RuntimeError:
        capture_out_of_bound_error = True

    assert capture_out_of_bound_error


if __name__ == "__main__":
    test_main()
