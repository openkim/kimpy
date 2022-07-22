import os
import numpy as np

from kimpy import neighlist as nl
from kimpy import KimPyError


def create_graphite_unit_cell(alat=2.46, d=3.35):

    cell = np.zeros((3, 3), dtype=np.double)
    cell[0][0] = alat
    cell[1][0] = 0.5 * alat
    cell[1][1] = np.sqrt(3) * 0.5 * alat
    cell[2][2] = 2 * d

    coords = np.empty((4, 3), dtype=np.double)
    coords[0] = np.zeros((3), dtype=np.double)
    coords[1] = (cell[0] + cell[1]) / 3.0
    coords[2] = (cell[0] + cell[1]) / 3.0 + cell[2] / 2.0
    coords[3] = (cell[0] + cell[1]) * 2 / 3.0 + cell[2] / 2.0

    species = np.array([1, 1, 2, 2], dtype=np.intc)

    return cell, coords, species


def write_XYZ(fname, lat_vec, species, coords):

    with open(fname, "w") as fout:

        # first line (num of atoms)
        natoms = len(species)
        fout.write("{}\n".format(natoms))

        # second line
        # lattice
        fout.write('Lattice="')
        for line in lat_vec:
            for item in line:
                fout.write("{:.15g} ".format(item))  # g removes trailing zero

        fout.write('" ')
        # PBC
        fout.write('PBC="1 1 1" ')
        # properties
        fout.write("Properties=species:S:1:pos:R:3\n")

        # body
        for i in range(len(species)):
            fout.write("{:3d} ".format(species[i]))
            fout.write(
                "{:13.5e} {:13.5e} {:13.5e}".format(
                    coords[i][0], coords[i][1], coords[i][2]
                )
            )
            fout.write("\n")


def test_main():

    # create contributing atoms
    alat = 2.46
    d = 3.35

    cell, contrib_coords, contrib_species = create_graphite_unit_cell(alat, d)

    # create padding atoms
    cutoffs = np.array([d + 0.01, d + 0.02], dtype=np.double)

    influence_dist = cutoffs[1]

    pbc = np.array([1, 1, 1], dtype=np.intc)

    try:
        pad_coords, pad_species, _ = nl.create_paddings(
            influence_dist, cell, pbc, contrib_coords, contrib_species
        )
    except RuntimeError:
        msg = 'Calling "neighlist.create_paddings" failed.'
        raise KimPyError(msg)

    assert pad_coords.shape == (96, 3)

    coords = np.concatenate((contrib_coords, pad_coords))
    coords = np.asarray(coords, dtype=np.double)

    species = np.concatenate((contrib_species, pad_species))
    species = np.asarray(species, dtype=np.intc)

    fname = "atoms.xyz"
    write_XYZ(fname, cell, species, coords)

    # flag to indicate wheter create neighbor list for an atom
    n_pad = pad_coords.shape[0]
    n_contrib = contrib_coords.shape[0]

    need_neigh = np.concatenate((np.ones(n_contrib), np.zeros(n_pad)))
    need_neigh = np.asarray(need_neigh, dtype=np.intc)

    # create neighbor list
    neigh = nl.create()

    try:
        neigh.build(coords, influence_dist, cutoffs, need_neigh)
    except RuntimeError:
        msg = 'Calling "neighlist.build" failed.'
        raise KimPyError(msg)

    # build again (it will automatically empty previous neigh list)
    try:
        neigh.build(coords, influence_dist, cutoffs, need_neigh)
    except RuntimeError:
        msg = 'Calling "neighlist.build" failed.'
        raise KimPyError(msg)

    # test get neigh function
    neigh_list_index = 0
    particle = 1

    try:
        num_neigh, _ = neigh.get_neigh(cutoffs, neigh_list_index, particle)
    except RuntimeError:
        msg = 'KIM error. Calling "neighlist.get_neigh" failed.'
        raise KimPyError(msg)

    assert num_neigh == 14

    neigh_list_index = 1
    particle = 4

    try:
        num_neigh, _ = neigh.get_neigh(cutoffs, neigh_list_index, particle)
    except RuntimeError:
        msg = 'KIM error. Calling "neighlist.get_neigh" failed.'
        raise KimPyError(msg)

    assert num_neigh == 0

    # remove the created file
    try:
        os.remove(fname)
        os.remove("kim.log")
    except:
        pass


if __name__ == "__main__":
    test_main()
