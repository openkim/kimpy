# This file is generated automatically by generate_SupportStatus_bind_test.py.
# Do not modify this file, but modify the script instead.
import kimpy

attributes = [
  kimpy.support_status.requiredByAPI,
  kimpy.support_status.notSupported,
  kimpy.support_status.required,
  kimpy.support_status.optional,
]


str_names = [
  "requiredByAPI",
  "notSupported",
  "required",
  "optional",
]



def test_main():
    """Main test function."""
    N = kimpy.support_status.get_number_of_support_statuses()

    assert N == 4

    all_instances = []
    for i in range(N):
        inst = kimpy.support_status.get_support_status(i)

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
        inst = kimpy.support_status.get_support_status(N)
    except RuntimeError:
        capture_out_of_bound_error = True

    assert capture_out_of_bound_error


if __name__ == "__main__":
    test_main()
