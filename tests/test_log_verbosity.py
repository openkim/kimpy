# This file is generated automatically by generate_LogVerbosity_bind_test.py.
# Do not modify this file, but modify the script instead.
import kimpy

attributes = [
  kimpy.log_verbosity.silent,
  kimpy.log_verbosity.fatal,
  kimpy.log_verbosity.error,
  kimpy.log_verbosity.warning,
  kimpy.log_verbosity.information,
  kimpy.log_verbosity.debug,
]


str_names = [
  "silent",
  "fatal",
  "error",
  "warning",
  "information",
  "debug",
]



def test_main():
    """Main test function."""
    N = kimpy.log_verbosity.get_number_of_log_verbosities()

    assert N == 6

    all_instances = []
    for i in range(N):
        inst = kimpy.log_verbosity.get_log_verbosity(i)

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
        inst = kimpy.log_verbosity.get_log_verbosity(N)
    except RuntimeError:
        capture_out_of_bound_error = True

    assert capture_out_of_bound_error


if __name__ == "__main__":
    test_main()
