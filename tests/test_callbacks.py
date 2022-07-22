""" Test callbacks can pass data bi-directionally.

Two atoms are in the configuration, and as a results, each time
`kim_model.compute` is issued, callback `get_neigh` is called twice, but
`process_dEdr` and `process_d2Edr2` are called only once since the LJ612 model
uses half list.

In this test, we call `kim_model.compute` twice.

After the first call, we change the local data to test that we can modifiy data
locally and the modification goes to the data object in the KIM API. Similary,
we test the data modified in the KIM KPI is reflected locally by changing the
data in the callbacks when it is last called.

"""

import numpy as np
import kimpy
from ase import Atoms
import pytest


# create a dimer of two atoms
def get_argon_dimer():
    argon = Atoms(
        "ArAr", positions=[(0, 0, 0), (0.1, 0.2, 0.2)], cell=(10, 10, 10), pbc=(0, 0, 0)
    )
    return argon


# neigh
neigh_data = dict()
neigh_data["cutoff"] = 1.0
neigh_data["num_particles"] = 2
neigh_data["neighbors"] = np.array([[1], [0]], dtype=np.intc)
neigh_data["key"] = 1
neigh_data["num_called"] = 0


def get_neigh(data, cutoffs, neighbor_list_index, particle_number):
    # 1st call of kim_model.compute
    if data["num_called"] < 2:
        assert data["cutoff"] == pytest.approx(1.0, 1e-6)
        assert data["num_particles"] == 2
        assert np.allclose(data["neighbors"], [[1], [0]])
        assert data["key"] == 1
    # 2nd call of kim_model.compute
    else:
        assert data["cutoff"] == pytest.approx(1.0, 1e-6)
        assert data["num_particles"] == 2
        assert np.allclose(data["neighbors"], [[1], [0]])
        assert data["key"] == 2

    # last call of this function
    if data["num_called"] == 3:
        # modify original key
        data["key"] = 3
        # add new key
        data["new_key"] = 1

    data["num_called"] += 1
    neighbors = data["neighbors"][particle_number]

    return (neighbors, 0)


# process dEdr
dEdr_data = dict()
dEdr_data["key"] = 1
dEdr_data["num_called"] = 0

n_dEdr_called = 0


def process_dEdr(data, de, r, dx, i, j):
    assert r == pytest.approx(0.3, 1e-6)

    assert np.allclose(abs(dx), [0.1, 0.2, 0.2])

    # 1st call of kim_model.compute
    if data["num_called"] < 1:
        assert data["key"] == 1
    else:
        assert data["key"] == 2

    # last call of this function
    if data["num_called"] == 1:
        # modify original key
        data["key"] = 3
        # add new key
        data["new_key"] = 1

    data["num_called"] += 1
    return 0


# process d2Edr2
d2Edr2_data = dict()
d2Edr2_data["key"] = 1
d2Edr2_data["num_called"] = 0


def process_d2Edr2(data, de, r, dx, i, j):

    assert np.allclose(r, [0.3, 0.3])

    assert np.allclose(abs(dx), [0.1, 0.2, 0.2, 0.1, 0.2, 0.2])

    # 1st call of kim_model.compute
    if data["num_called"] < 1:
        assert data["key"] == 1
    else:
        assert data["key"] == 2

    # last call of this function
    if data["num_called"] == 1:
        # modify original key
        data["key"] = 3
        # add new key
        data["new_key"] = 1

    data["num_called"] += 1

    return 0


def test_main():

    model_name = "LennardJones612_UniversalShifted__MO_959249795837_003"

    # create model
    try:
        requested_units_accepted, kim_model = kimpy.model.create(
            kimpy.numbering.zeroBased,
            kimpy.length_unit.A,
            kimpy.energy_unit.eV,
            kimpy.charge_unit.e,
            kimpy.temperature_unit.K,
            kimpy.time_unit.ps,
            model_name,
        )
    except RuntimeError:
        raise kimpy.KimPyError('Calling "kimpy.model.create" failed.')

    if not requested_units_accepted:
        msg = "requested units not accepted in kimpy.model.create"
        raise kimpy.KimPyError(msg)

    # create compute arguments
    try:
        compute_arguments = kim_model.compute_arguments_create()
    except RuntimeError:
        msg = 'Calling "kim_model.compute_arguments_create" failed.'
        raise kimpy.KimPyError(msg)

    # register argument
    argon = get_argon_dimer()

    coords = np.asarray(argon.get_positions(), dtype=np.double)
    N = coords.shape[0]
    forces = np.zeros((N, 3), dtype=np.double)
    energy = np.array([0.0], dtype=np.double)
    num_particles = np.array([N], dtype=np.intc)
    species_code = np.zeros(num_particles, dtype=np.intc)
    particle_contributing = np.zeros(num_particles, dtype=np.intc)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.numberOfParticles, num_particles
        )
    except RuntimeError:
        msg = 'Calling "compute_argument.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.particleSpeciesCodes, species_code
        )
    except RuntimeError:
        msg = 'Calling "compute_argument.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.particleContributing, particle_contributing
        )
    except RuntimeError:
        msg = 'Calling "compute_argument.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.coordinates, coords
        )
    except RuntimeError:
        msg = 'Calling "compute_argument.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.partialEnergy, energy
        )
    except RuntimeError:
        msg = 'Calling "compute_argument.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.partialForces, forces
        )
    except RuntimeError:
        msg = 'Calling "compute_argument.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    # species support and code
    try:
        species_support, code = kim_model.get_species_support_and_code(
            kimpy.species_name.Ar
        )
    except RuntimeError:
        msg = 'Calling "kim_model.get_species_support_and_code" failed.'
        raise kimpy.KimPyError(msg)

    if not species_support:
        msg = "Ar species is not supported by this model."
        raise kimpy.KimPyError(msg)

    # setup particle species
    species_code[:] = code

    # setup particleContributing
    particle_contributing[:] = 1

    # register callbacks
    try:
        compute_arguments.set_callback(
            kimpy.compute_callback_name.GetNeighborList, get_neigh, neigh_data
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_callback" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_callback(
            kimpy.compute_callback_name.ProcessDEDrTerm, process_dEdr, dEdr_data
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_callback" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_callback(
            kimpy.compute_callback_name.ProcessD2EDr2Term, process_d2Edr2, d2Edr2_data
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_callback" failed.'
        raise kimpy.KimPyError(msg)

    # 1st call of compute
    try:
        kim_model.compute(compute_arguments)
    except RuntimeError:
        msg = 'Calling "kim_model.compute" failed the 1st time.'
        raise kimpy.KimPyError(msg)

    # 2nd call of compute
    neigh_data["key"] = 2
    dEdr_data["key"] = 2
    d2Edr2_data["key"] = 2

    try:
        kim_model.compute(compute_arguments)
    except RuntimeError:
        msg = 'Calling "kim_model.compute" failed the 2nd time.'
        raise kimpy.KimPyError(msg)

    # check callback can modify local data
    assert neigh_data["key"] == 3
    assert neigh_data["new_key"] == 1
    assert dEdr_data["key"] == 3
    assert dEdr_data["new_key"] == 1
    assert d2Edr2_data["key"] == 3
    assert d2Edr2_data["new_key"] == 1


if __name__ == "__main__":
    test_main()
