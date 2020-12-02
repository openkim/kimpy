import os
import numpy as np
import kimpy
from kimpy import check_error, report_error
from ase.lattice.cubic import FaceCenteredCubic


def create_fcc_argon(alat=5.26):
    argon = FaceCenteredCubic(
        directions=[[1, 0, 0], [0, 1, 0], [0, 0, 1]],
        size=(2, 2, 2),
        symbol='Ar',
        pbc=(0, 0, 0),
        latticeconstant=alat,
    )
    return argon


def get_neigh(data, cutoffs, neighbor_list_index, particle_number):
    error = 0

    # we only support one neighbor list
    rcut = data['cutoff']
    if len(cutoffs) != 1 or cutoffs[0] > rcut:
        error = 1
    if neighbor_list_index != 0:
        error = 1

    # invalid id
    number_of_particles = data['num_particles']
    if particle_number >= number_of_particles or particle_number < 0:
        error = 1
    check_error(error, 'get_neigh')

    neighbors = data['neighbors'][particle_number]
    return (neighbors, error)


def create_neigh(coords, cutoff, neigh):
    """naively create a full neighbor list"""

    n = coords.shape[0]
    neighbors = []
    for i in range(n):
        neigh_i = []
        for j in range(n):
            if j == i:
                continue
            dist = np.linalg.norm(coords[i] - coords[j])
            if dist < cutoff:
                neigh_i.append(j)
        neigh_i = np.array(neigh_i, dtype=np.intc)
        neighbors.append(neigh_i)

    neigh['cutoff'] = cutoff
    neigh['num_particles'] = n
    neigh['neighbors'] = neighbors


def test_main():

    modelname = 'ex_model_Ar_P_Morse'
    # modelname = 'Three_Body_Stillinger_Weber_Si__MO_405512056662_004'
    # modelname = 'LennardJones612_Universal__MO_826355984548_002'
    print()
    print('=' * 80)
    print('Matching results for KIM model:', modelname)
    print()

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

    # units
    l_unit, e_unit, c_unit, te_unit, ti_unit = kim_model.get_units()
    check_error(error, 'kim_model.get_units')
    print('Length unit is:', str(l_unit))
    print('Energy unit is:', str(e_unit))
    print('Charge unit is:', str(c_unit))
    print('Temperature unit is:', str(te_unit))
    print('Time unit is:', str(ti_unit))
    print()

    # create compute arguments
    compute_arguments, error = kim_model.compute_arguments_create()
    check_error(error, 'kim_model.compute_arguments_create')

    # check compute arguments
    num_compute_arguments = (
        kimpy.compute_argument_name.get_number_of_compute_argument_names()
    )
    print('Number of compute_arguments:', num_compute_arguments)

    for i in range(num_compute_arguments):

        name, error = \
            kimpy.compute_argument_name.get_compute_argument_name(i)
        check_error(error, 'kim_model.get_compute_argument_name')

        dtype, error = \
            kimpy.compute_argument_name.get_compute_argument_data_type(name)
        check_error(error, 'kim_model.get_compute_argument_data_type')

        support_status, error = \
            compute_arguments.get_argument_support_status(name)
        check_error(error, 'compute_argument.get_argument_support_status')

        n_space_1 = 21 - len(str(name))
        n_space_2 = 7 - len(str(dtype))
        print(
            'Compute Argument name "{}" '.format(name)
            + ' ' * n_space_1
            + 'is of type "{}" '.format(dtype)
            + ' ' * n_space_2
            + 'and has support status "{}".'.format(support_status)
        )

        # can only handle energy and force as a required arg
        if support_status == kimpy.support_status.required:
            if name not in (kimpy.compute_argument_name.partialEnergy,
                            kimpy.compute_argument_name.partialForces):
                report_error('Unsupported required ComputeArgument')

        # must have energy and forces
        if name in (kimpy.compute_argument_name.partialEnergy,
                    kimpy.compute_argument_name.partialForces):
            if support_status not in (kimpy.support_status.required,
                                      kimpy.support_status.optional):
                report_error('Energy or forces not available')
    print()

    # check compute callbacks
    num_callbacks = \
        kimpy.compute_callback_name.get_number_of_compute_callback_names()
    print('Number of callbacks:', num_callbacks)

    for i in range(num_callbacks):

        name, error = kimpy.compute_callback_name.get_compute_callback_name(i)
        check_error(error, 'kim_model.get_compute_callback_name')

        support_status, error = \
            compute_arguments.get_callback_support_status(name)
        check_error(error, 'compute_argument.get_callback_support_status')

        n_space = 18 - len(str(name))
        print(
            'Compute callback "{}"'.format(name)
            + ' ' * n_space
            + 'has support status "{}".'.format(support_status)
        )

        # cannot handle any "required" callbacks
        if support_status == kimpy.support_status.required:
            report_error('Unsupported required ComputeCallback')
    print()

    # parameter
    num_params = kim_model.get_number_of_parameters()
    print('Number of parameters:', num_params)
    print()

    for i in range(num_params):
        out = kim_model.get_parameter_metadata(i)
        dtype, extent, name, description, error = out
        check_error(error, 'kim_model.get_parameter_metadata')
        print('Parameter No.', i)
        print('    data type:', dtype)
        print('    extent:', extent)
        print('    name:', name)
        print('    description:', description)
    print()

    # register argument
    argon = create_fcc_argon()

    coords = np.asarray(argon.get_positions(), dtype=np.double)
    N = coords.shape[0]
    print('Number of particles:', N)
    forces = np.zeros((N, 3), dtype=np.double)
    energy = np.array([0.0], dtype=np.double)
    num_particles = np.array([N], dtype=np.intc)
    species_code = np.zeros(num_particles, dtype=np.intc)
    particle_contributing = np.zeros(num_particles, dtype=np.intc)

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.numberOfParticles,
        num_particles)
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.particleSpeciesCodes,
        species_code)
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.particleContributing,
        particle_contributing)
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.coordinates, coords)
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.partialEnergy, energy)
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    error = compute_arguments.set_argument_pointer(
        kimpy.compute_argument_name.partialForces, forces)
    check_error(error, 'kimpy.compute_argument.set_argument_pointer')

    # neighbor list
    neigh = dict()

    # register get neigh callback
    error = compute_arguments.set_callback(
        kimpy.compute_callback_name.GetNeighborList, get_neigh, neigh)
    check_error(error, 'kimpy.compute_argument.set_callback')

    # influence distance and cutoff of model
    model_influence_dist = kim_model.get_influence_distance()
    out = kim_model.get_neighbor_list_cutoffs_and_hints()
    model_cutoffs, padding_not_require_neigh_hints = out
    print('Model influence distance:', model_influence_dist)
    print('Model cutoffs:', model_cutoffs)
    print('Model padding neighbors hints:', padding_not_require_neigh_hints)
    print()

    # species support and code
    species_support, code, error = kim_model.get_species_support_and_code(
        kimpy.species_name.Ar
    )
    check_error(error or not species_support,
                'kim_model.get_species_support_and_code')
    print('Species Ar is supported and its code is:', code)
    print()

    # setup particle species
    species_code[:] = code

    # setup particleContributing
    particle_contributing[:] = 1

    # compute energy and force for different structures
    alat = 5.26
    min_alat = 0.8 * 5.26
    max_alat = 1.2 * 5.26
    inc_alat = 0.025 * alat
    all_alat = np.arange(min_alat, max_alat, inc_alat)

    print('=' * 80)
    print('Result for KIM model:', modelname)
    print()
    print('       energy          force norm        lattice spacing')
    print()

    for a in all_alat:
        argon = create_fcc_argon(a)
        # NOTE should not change coords address
        np.copyto(coords, argon.get_positions())
        # NOTE safe to change content of neigh
        create_neigh(coords, model_influence_dist, neigh)
        error = kim_model.compute(compute_arguments)
        print('{:18.10e} {:18.10e} {:18.10e}'.format(
            energy[0], np.linalg.norm(forces), a))

    # destory compute arguments
    error = kim_model.compute_arguments_destroy(compute_arguments)
    check_error(error, 'kim_model.compute_arguments_destroy')

    out = kim_model.is_routine_present(
        kimpy.model_routine_name.WriteParameterizedModel)
    present, required, error = out
    check_error(error, 'kim_model.is_routine_present')
    if present:
        kim_model.write_parameterized_model('.', 'Morse_Ar')
        try:
            os.remove('Morse_Ar.params')
            os.remove('CMakeLists.txt')
            os.remove('kim.log')
        except:
            pass

    # destory model
    kimpy.model.destroy(kim_model)



if __name__ == '__main__':
    test_main()
