import numpy as np
import kimpy
from kimpy import neighlist as nl
from ase.lattice.cubic import FaceCenteredCubic


def create_fcc_argon(alat=5.26):
    argon = FaceCenteredCubic(
        directions=[[1, 0, 0], [0, 1, 0], [0, 0, 1]],
        size=(2, 2, 2),
        symbol="Ar",
        pbc=(0, 0, 0),
        latticeconstant=alat,
    )
    return argon


def test_main():
    modelname = "ex_model_Ar_P_Morse_07C"
    print()
    print("=" * 80)
    print("Matching results for KIM model:", modelname)
    print()

    # create model
    try:
        requestedUnitsAccepted, kim_model = kimpy.model.create(
            kimpy.numbering.zeroBased,
            kimpy.length_unit.A,
            kimpy.energy_unit.eV,
            kimpy.charge_unit.e,
            kimpy.temperature_unit.K,
            kimpy.time_unit.ps,
            modelname,
        )
    except RuntimeError:
        raise kimpy.KimPyError('Calling "kimpy.model.create" failed.')

    if not requestedUnitsAccepted:
        msg = "requested units not accepted in kimpy.model.create"
        raise kimpy.KimPyError(msg)

    # units
    l_unit, e_unit, c_unit, te_unit, ti_unit = kim_model.get_units()

    print("Length unit is:", str(l_unit))
    print("Energy unit is:", str(e_unit))
    print("Charge unit is:", str(c_unit))
    print("Temperature unit is:", str(te_unit))
    print("Time unit is:", str(ti_unit))
    print()

    # create compute arguments
    try:
        compute_arguments = kim_model.compute_arguments_create()
    except RuntimeError:
        msg = 'Calling "kim_model.compute_arguments_create" failed.'
        raise kimpy.KimPyError(msg)

    # check compute arguments
    num_compute_arguments = (
        kimpy.compute_argument_name.get_number_of_compute_argument_names()
    )
    print("Number of compute_arguments:", num_compute_arguments)

    for i in range(num_compute_arguments):
        try:
            name = kimpy.compute_argument_name.get_compute_argument_name(i)
        except RuntimeError:
            msg = 'Calling "kimpy.compute_argument_name.'
            msg += 'get_compute_argument_name" for index = '
            msg += "{} failed.".format(i)
            raise kimpy.KimPyError(msg)

        try:
            dtype = kimpy.compute_argument_name.get_compute_argument_data_type(name)
        except RuntimeError:
            msg = 'Calling "kimpy.compute_argument_name.'
            msg += 'get_compute_argument_data_type" for computeArgumentName = '
            msg += "{} failed.".format(name)
            raise kimpy.KimPyError(msg)

        try:
            support_status = compute_arguments.get_argument_support_status(name)
        except RuntimeError:
            msg = 'Calling "compute_arguments.get_argument_support_status" '
            msg += "for computeArgumentName = {} failed.".format(name)
            raise kimpy.KimPyError(msg)

        n_space_1 = 21 - len(str(name))
        n_space_2 = 7 - len(str(dtype))

        print(
            'Compute Argument name "{}" '.format(name)
            + " " * n_space_1
            + 'is of type "{}" '.format(dtype)
            + " " * n_space_2
            + 'and has support status "{}".'.format(support_status)
        )

        # can only handle energy and force as a required arg
        if support_status == kimpy.support_status.required:
            if name not in (
                kimpy.compute_argument_name.partialEnergy,
                kimpy.compute_argument_name.partialForces,
            ):
                msg = "Unsupported required ComputeArgumentName = "
                msg += "{}".format(name)
                raise kimpy.KimPyError(msg)

        # must have energy and forces
        if name in (
            kimpy.compute_argument_name.partialEnergy,
            kimpy.compute_argument_name.partialForces,
        ):
            if support_status not in (
                kimpy.support_status.required,
                kimpy.support_status.optional,
            ):
                raise kimpy.KimPyError("Energy or forces not available")

    print()

    # check compute callbacks
    num_callbacks = kimpy.compute_callback_name.get_number_of_compute_callback_names()

    print("Number of callbacks:", num_callbacks)

    for i in range(num_callbacks):
        try:
            name = kimpy.compute_callback_name.get_compute_callback_name(i)
        except RuntimeError:
            msg = 'Calling "kimpy.compute_callback_name.'
            msg += 'get_compute_callback_name" for index = '
            msg += "{} failed.".format(i)
            raise kimpy.KimPyError(msg)

        try:
            support_status = compute_arguments.get_callback_support_status(name)
        except RuntimeError:
            msg = 'Calling "compute_arguments.get_callback_support_status" '
            msg += "for computeArgumentName = {} failed.".format(name)
            raise kimpy.KimPyError(msg)

        n_space = 18 - len(str(name))

        print(
            'Compute callback "{}"'.format(name)
            + " " * n_space
            + 'has support status "{}".'.format(support_status)
        )

        # cannot handle any "required" callbacks
        if support_status == kimpy.support_status.required:
            raise kimpy.KimPyError("Unsupported required ComputeCallback")

    print()

    # parameter
    num_params = kim_model.get_number_of_parameters()

    print("Number of parameters:", num_params)

    print()

    for i in range(num_params):
        try:
            dtype, extent, name, description = kim_model.get_parameter_metadata(i)
        except RuntimeError:
            msg = 'Calling "kim_model.get_parameter_metadata" '
            msg += "for parameterIndex = {} failed.".format(i)
            raise kimpy.KimPyError(msg)

        print("Parameter No.", i)
        print("    data type:", dtype)
        print("    extent:", extent)
        print("    name:", name)
        print("    description:", description)

    print()

    # register argument
    argon = create_fcc_argon()

    coords = np.asarray(argon.get_positions(), dtype=np.double)
    N = coords.shape[0]
    print("Number of particles:", N)
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
        msg = 'Calling "compute_arguments.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.particleSpeciesCodes, species_code
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.particleContributing, particle_contributing
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.coordinates, coords
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.partialEnergy, energy
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    try:
        compute_arguments.set_argument_pointer(
            kimpy.compute_argument_name.partialForces, forces
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_argument_pointer" failed.'
        raise kimpy.KimPyError(msg)

    # create neighbor list
    neigh = nl.create()

    # register get neigh callback
    try:
        compute_arguments.set_callback_pointer(
            kimpy.compute_callback_name.GetNeighborList, nl.get_neigh_kim(), neigh
        )
    except RuntimeError:
        msg = 'Calling "compute_arguments.set_callback_pointer" failed.'
        raise kimpy.KimPyError(msg)

    # influence distance and cutoff of model
    model_influence_dist = kim_model.get_influence_distance()
    print("Model influence distance:", model_influence_dist)

    (
        model_cutoffs,
        padding_not_require_neigh_hints,
    ) = kim_model.get_neighbor_list_cutoffs_and_hints()
    print("Model cutoffs:", model_cutoffs)
    print("Model padding neighbors hints:", padding_not_require_neigh_hints)
    print()

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

    print("Species Ar is supported and its code is:", code)
    print()

    # setup particle species
    species_code[:] = code

    # setup particleContributing
    particle_contributing[:] = 1

    # setup neighbor list
    need_neigh = np.ones(N, dtype="intc")

    # compute energy and force for different structures
    alat = 5.26
    min_alat = 0.8 * 5.26
    max_alat = 1.2 * 5.26
    inc_alat = 0.025 * alat
    all_alat = np.arange(min_alat, max_alat, inc_alat)

    print("=" * 80)
    print("Result for KIM model:", modelname)
    print()
    print("       energy          force norm        lattice spacing")
    print()

    for a in all_alat:
        argon = create_fcc_argon(a)

        # NOTE cannot change coords address
        np.copyto(coords, argon.get_positions())

        try:
            neigh.build(coords, model_influence_dist, model_cutoffs, need_neigh)
        except RuntimeError:
            raise kimpy.KimPyError('Calling "neighlist.build" failed.')

        try:
            kim_model.compute(compute_arguments)
        except RuntimeError:
            raise kimpy.KimPyError('Calling "kim_model.compute" failed.')

        print(
            "{:18.10e} {:18.10e} {:18.10e}".format(energy[0], np.linalg.norm(forces), a)
        )


if __name__ == "__main__":
    test_main()
