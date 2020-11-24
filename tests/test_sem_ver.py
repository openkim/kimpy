import kimpy
from kimpy import check_error


def test_main():

    version = kimpy.sem_ver.get_sem_ver()
    ls_less_than, error = kimpy.sem_ver.is_less_than('2.0.1', '2.1.0')
    check_error(error, 'kimpy.sem_ver.is_less_than')
    out = kimpy.sem_ver.parse_sem_ver(version)
    version_related = out[:-1]
    version_rebuilt_1 = '{}.{}.{}{}+{}'.format(*version_related)
    version_rebuilt_2 = '{}.{}.{}-{}+{}'.format(*version_related)

    assert ls_less_than == 1
    assert version in (version_rebuilt_1, version_rebuilt_2)


if __name__ == '__main__':
    test_main()
