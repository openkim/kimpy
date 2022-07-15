import kimpy


def test_main():
    version = kimpy.sem_ver.get_sem_ver()

    try:
        is_less_than = kimpy.sem_ver.is_less_than("2.0.1", "2.1.0")
    except RuntimeError:
        msg = 'Calling "kimpy.sem_ver.is_less_than" failed.'
        raise kimpy.KimPyError(msg)

    assert is_less_than == 1

    try:
        version_related = kimpy.sem_ver.parse_sem_ver(version)
    except RuntimeError:
        msg = 'Calling "kimpy.sem_ver.parse_sem_ver" failed.'
        raise kimpy.KimPyError(msg)

    version_rebuilt_1 = "{}.{}.{}{}+{}".format(*version_related)
    version_rebuilt_2 = "{}.{}.{}-{}+{}".format(*version_related)

    assert version in (version_rebuilt_1, version_rebuilt_2)


if __name__ == "__main__":
    test_main()
