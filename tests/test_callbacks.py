""" Test callbacks can pass data bi-directionally.

Two atoms are in the configuration, and as a results, each time `kim_model.compute`
is issued, callback `get_neigh` is called twice, but `process_dEdr` and
`process_d2Edr2` are called only once since the LJ612 model uses half list.

In this test, we call `kim_model.compute` twice. After the first call, we change
the local data to test that we can modifiy data locally and the modification
goes to the data object in the KIM API. Similary, we test the data modified in the
KIM KPI is reflected locally by changing the data in the callbacks when it is last
called.
"""

from __future__ import absolute_import, division, print_function
import numpy as np
import kimpy
from error import check_error, report_error
from ase import Atoms
import pytest


# create a dimer of two atoms
def get_dimer():
    argon = Atoms(
        'ArAr', positions=[(0, 0, 0), (0.1, 0.2, 0.2)], cell=(10, 10, 10), pbc=(0, 0, 0)
    )
    return argon


def assert_1d_array(A, B):
    assert len(A) == len(B)
    for i, j in zip(A, B):
        assert i == pytest.approx(j, 1e-6)


def assert_2d_array(A, B):
    A = np.array(A)
    B = np.array(B)
    assert A.shape[0] == B.shape[0]
    assert A.shape[1] == B.shape[1]
    for i, j in zip(A, B):
        for m, n in zip(i, j):
            assert m == pytest.approx(n, 1e-6)


# neigh
neigh_data = dict()
neigh_data['cutoff'] = 1.0
neigh_data['num_particles'] = 2
neigh_data['neighbors'] = np.array([[1], [0]], dtype=np.intc)
neigh_data['key'] = 1
neigh_data['num_called'] = 0


def get_neigh(data, cutoffs, neighbor_list_index, particle_number):
    if data['num_called'] < 2:  # 1st call of kim_model.compute
        assert data['cutoff'] == pytest.approx(1.0, 1e-6)
        assert data['num_particles'] == 2
        assert_2d_array(data['neighbors'], [[1], [0]])
        assert data['key'] == 1
    else:  # 2nd call of kim_model.compute
        assert data['cutoff'] == pytest.approx(1.0, 1e-6)
        assert data['num_particles'] == 2
        assert_2d_array(data['neighbors'], [[1], [0]])
        assert data['key'] == 2

    if data['num_called'] == 3:  # last call of this function
        data['key'] = 3  # modify original key
        data['new_key'] = 1  # add new key
    data['num_called'] += 1
    error = 0
    neighbors = data['neighbors'][particle_number]
    return (neighbors, error)


# process dEdr
dEdr_data = dict()
dEdr_data['key'] = 1
dEdr_data['num_called'] = 0

n_dEdr_called = 0


def process_dEdr(data, de, r, dx, i, j):
    assert r == pytest.approx(0.3, 1e-6)
    assert_1d_array(abs(dx), [0.1, 0.2, 0.2])
    if data['num_called'] < 1:  # 1st call of kim_model.compute
        assert data['key'] == 1
    else:
        assert data['key'] == 2
    if data['num_called'] == 1:  # last call of this function
        data['key'] = 3  # modify original key
        data['new_key'] = 1  # add new key
    data['num_called'] += 1
    error = 0
    return error


# process d2Edr2
d2Edr2_data = dict()
d2Edr2_data['key'] = 1
d2Edr2_data['num_called'] = 0


def process_d2Edr2(data, de, r, dx, i, j):
    assert_1d_array(r, [0.3, 0.3])
    assert_1d_array(abs(dx), [0.1, 0.2, 0.2, 0.1, 0.2, 0.2])
    if data['num_called'] < 1:  # 1st call of kim_model.compute
        assert data['key'] == 1
    else:
        assert data['key'] == 2
    if data['num_called'] == 1:  # last call of this function
        data['key'] = 3  # modify original key
        data['new_key'] = 1  # add new key
    data['num_called'] += 1
    error = 0
    return error


def test_main():

    modelname = 'LennardJones612_UniversalShifted__MO_959249795837_003'

    # create model
    requestedUnitsAccepted, kim_model, error = kimpy.model.create(
        kimpy.numbering.zeroBased,
        kimpy.length_unit.A,
        kimpy.energy_unit.eV,
        kimpy.charge_unit.e,
        kimpy.temperature_unit.K,
        kimpy.time_unit.ps,
        modelname,
    )
    check_error(error, 'kimpy.model.create')
    if not requestedUnitsAccepted:
        report_error('requested units not accepted in kimpy.model.create')

    # create compute arguments
    compute_arguments, error = kim_model.compute_arguments_create()
    check_error(error, 'kim_model.compute_arguments_create')

    # register argument
    argon = get_dimer()

    coords = np.asarray(argon.get_positions(), dtype=np.double)
    N = coords.shape[0]
    forces = np.zeros((N, 3), dtype=np.double)
    energy = np.array([0.0], dtype=np.double)
    num_particles = np.array([N], dtype=np.intc)
    species_code = np.zeros(num_particles, dtype=np.intc)
    particle_contributing = np.zeros(num_particles, dtype=np.intc)

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.numberOfParticles, num_particles
    )
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.particleSpeciesCodes, species_code
    )
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.particleContributing, particle_contributing
    )
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.coordinates, coords
    )
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.partialEnergy, energy
    )
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.partialForces, forces
    )
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    # species support and code
    species_support, code, error = kim_model.get_species_support_and_code(
        kimpy.species_name.Ar
    )
    check_error(error or not species_support, 'kim_model.get_species_support_and_code')

    # setup particle species
    species_code[:] = code

    # setup particleContributing
    particle_contributing[:] = 1

    # register callbacks
    error = compute_arguments.set_callback(
        kimpy.compute_callback_name.GetNeighborList, get_neigh, neigh_data
    )
    check_error(error, 'kimpy.compute_argument.set_callback')

    error = compute_arguments.set_callback(
        kimpy.compute_callback_name.ProcessDEDrTerm, process_dEdr, dEdr_data
    )
    check_error(error, 'kimpy.compute_argument.set_callback')

    error = compute_arguments.set_callback(
        kimpy.compute_callback_name.ProcessD2EDr2Term, process_d2Edr2, d2Edr2_data
    )
    check_error(error, 'kimpy.compute_argument.set_callback')

    # 1st call of compute
    error = kim_model.compute(compute_arguments)
    check_error(error, 'kim_model.compute')

    # 2nd call of compute
    neigh_data['key'] = 2
    dEdr_data['key'] = 2
    d2Edr2_data['key'] = 2
    error = kim_model.compute(compute_arguments)
    check_error(error, 'kim_model.compute')

    # destory compute arguments and model
    error = kim_model.compute_arguments_destroy(compute_arguments)
    check_error(error, 'kim_model.compute_arguments_destroy')
    kimpy.model.destroy(kim_model)

    # check callback can modify local data
    assert neigh_data['key'] == 3
    assert neigh_data['new_key'] == 1
    assert dEdr_data['key'] == 3
    assert dEdr_data['new_key'] == 1
    assert d2Edr2_data['key'] == 3
    assert d2Edr2_data['new_key'] == 1


if __name__ == '__main__':
    test_main()
