import kimpy

attributes = [
    kimpy.compute_argument_name.numberOfParticles,
    kimpy.compute_argument_name.particleSpeciesCodes,
    kimpy.compute_argument_name.particleContributing,
    kimpy.compute_argument_name.coordinates,
    kimpy.compute_argument_name.partialEnergy,
    kimpy.compute_argument_name.partialForces,
    kimpy.compute_argument_name.partialParticleEnergy,
    kimpy.compute_argument_name.partialVirial,
    kimpy.compute_argument_name.partialParticleVirial,
]

str_names = [
    'numberOfParticles',
    'particleSpeciesCodes',
    'particleContributing',
    'coordinates',
    'partialEnergy',
    'partialForces',
    'partialParticleEnergy',
    'partialVirial',
    'partialParticleVirial',
]

data_types = [
    kimpy.data_type.Integer,
    kimpy.data_type.Integer,
    kimpy.data_type.Integer,
    kimpy.data_type.Double,
    kimpy.data_type.Double,
    kimpy.data_type.Double,
    kimpy.data_type.Double,
    kimpy.data_type.Double,
    kimpy.data_type.Double,
]


def test_main():
    N = kimpy.compute_argument_name.get_number_of_compute_argument_names()
    assert N == 9

    all_instances = []
    for i in range(N):
        inst, error = kimpy.compute_argument_name.get_compute_argument_name(i)
        assert not error
        assert inst == attributes[i]
        assert str(inst) == str_names[i]

        all_instances.append(inst)

        dtype, error = \
            kimpy.compute_argument_name.get_compute_argument_data_type(inst)
        assert not error
        assert dtype == data_types[i]

    # test operator overloading
    for i in range(N):
        assert all_instances[i] == all_instances[i]
        for j in range(i + 1, N):
            assert all_instances[i] != all_instances[j]

    # test known
    for inst in all_instances:
        assert inst.known()

    # test out of bound
    inst, error = kimpy.compute_argument_name.get_compute_argument_name(N)
    assert error


if __name__ == '__main__':
    test_main()
