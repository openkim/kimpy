# This file is generated automatically by generate_ComputeCallbackName_bind_test.py.
# Do not modify this file, but modify the script instead.
import kimpy

attributes = [
  kimpy.compute_callback_name.GetNeighborList,
  kimpy.compute_callback_name.ProcessDEDrTerm,
  kimpy.compute_callback_name.ProcessD2EDr2Term,
]


str_names = [
  "GetNeighborList",
  "ProcessDEDrTerm",
  "ProcessD2EDr2Term",
]



def test_main():
    """Main test function."""
    N = kimpy.compute_callback_name.get_number_of_compute_callback_names()

    assert N == 3

    all_instances = []
    for i in range(N):
        inst = kimpy.compute_callback_name.get_compute_callback_name(i)

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
        inst = kimpy.compute_callback_name.get_compute_callback_name(N)
    except RuntimeError:
        capture_out_of_bound_error = True

    assert capture_out_of_bound_error


if __name__ == "__main__":
    test_main()
