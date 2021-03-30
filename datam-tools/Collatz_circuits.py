import sys
import datam
from datam import C, CENTER, NORTH, EAST, SOUTH, WEST


def bridge_type_2(with_input=None, variant=0):
    # This bridge takes one input from north and one input from east
    # and carries them along: north is to be read on same column than input north
    # and east is to be read on the bottom left most tile

    # the encoding is: on north: `top0 top1 top2 x 0` with top one of the words below (first component)
    #                   on east: `0 y east0 east1 east2` from top to bottom with east one the words below (second component)

    # Found by computer search, ((top, east))
    bridge_variant = [
        ("000", "012"),
        ("000", "100"),
        ("000", "111"),
        ("000", "122"),
        ("000", "210"),
        ("000", "221"),
        ("001", "001"),
        ("001", "012"),
        ("001", "100"),
        ("001", "111"),
        ("010", "001"),
        ("111", "122"),
        ("111", "210"),
        ("111", "221"),
    ]
    conf = datam.Configuration(datam.CollatzTileset)

    # TOP
    conf.add_tile((0, 0)).south("bin.0")

    for i in range(3):
        conf.add_tile((-1 * (i + 2), 0)).south("bin.0")

    # EAST
    conf.add_tile((1, -1)).west("ter.0")

    for i in range(3):
        conf.add_tile((1, -(i + 3))).west(
            f"ter.{bridge_variant[variant%len(bridge_variant)][1][i]}",
        )

    # Output wiring
    bottom_left = conf.bottom_left()
    conf.add_tile(bottom_left + SOUTH + WEST).north("bin.0")
    conf.add_tile(bottom_left + SOUTH + WEST * 2).north("bin.0")
    conf.add_tile(bottom_left + NORTH + WEST)
    conf.add_tile(bottom_left + SOUTH + EAST * 4).west("ter.0")
    conf.add_tile(bottom_left + SOUTH + EAST * 2)

    # input
    if with_input is not None:
        conf.add_tile((-1, 0)).south(f"bin.{with_input[0]}").north(
            f"bin.{with_input[0]}"
        )
        conf.add_tile(C(1, -2)).east(f"ter.{with_input[1]}").west(
            f"ter.{with_input[1]}"
        )

    return conf


def bridge_type_1(with_input=None, variant=0):
    # This bridge takes one input from north and one input from east
    # and carries them along on bottom left most tile

    # the encoding is: on north: `top0 top1 top2 x 0` with top one of the words below (first component)
    #                   on east: `0 y east0 east1 east2` from top to bottom with east one the words below (second component)

    # Found by computer search, ((top, east))
    bridge_variant = [
        ("000", "120"),
        ("001", "010"),
    ]
    conf = datam.Configuration(datam.CollatzTileset)

    # TOP
    conf.add_tile((0, 0)).south("bin.0")

    for i in range(3):
        conf.add_tile((-1 * (i + 2), 0)).south(
            f"bin.{bridge_variant[variant%len(bridge_variant)][0][-(i+1)]}"
        )

    # EAST
    conf.add_tile((1, -1)).west("ter.0")

    for i in range(3):
        conf.add_tile((1, -(i + 3))).west(
            f"ter.{bridge_variant[variant%len(bridge_variant)][1][i]}",
        )

    # Output wiring
    bottom_left = conf.bottom_left()
    conf.add_tile(bottom_left + SOUTH + WEST).north("bin.0")
    conf.add_tile(bottom_left + SOUTH + WEST * 2).north("bin.0")
    conf.add_tile(bottom_left + NORTH + WEST)
    conf.add_tile(bottom_left + SOUTH + EAST * 1).west("ter.0")

    # input
    if with_input is not None:
        conf.add_tile((-1, 0)).south(f"bin.{with_input[0]}").north(
            f"bin.{with_input[0]}"
        )
        conf.add_tile(C(1, -2)).east(f"ter.{with_input[1]}").west(
            f"ter.{with_input[1]}"
        )

    return conf


def input_bridge_type_1(in_, variant=0):
    variant = int(variant)
    return bridge_type_1(in_, variant)


def input_bridge_type_2(in_, variant=0):
    variant = int(variant)
    return bridge_type_2(in_, variant)


if __name__ == "__main__":
    conf = locals()[sys.argv[1]](*sys.argv[2:])
    print(conf)