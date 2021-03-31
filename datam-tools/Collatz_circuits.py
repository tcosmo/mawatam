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
        if with_input[0] is not None:
            conf.add_tile((-1, 0)).south(f"bin.{with_input[0]}").north(
                f"bin.{with_input[0]}"
            )
        if with_input[1] is not None:
            conf.add_tile(C(1, -2)).east(f"ter.{with_input[1]}").west(
                f"ter.{with_input[1]}"
            )

    return conf


# python3 datam-tools/Collatz_circuits.py input_two_bridges_type_2 110 0 0 | ./datam -i
def two_bridges_type_2(with_input=None, variant1=0, variant2=0):

    if len(with_input) != 3:
        print("Need three input for the two bridges configuration", file=sys.stderr)
        exit(-1)

    conf = datam.Configuration(datam.CollatzTileset)
    conf.add_sub_conf(bridge_type_2(with_input[:2], variant1))
    bottom_left = conf.bottom_left()
    conf.add_sub_conf(
        bridge_type_2((with_input[2], None), variant2).translate(
            bottom_left + NORTH * 3 + WEST * 1
        )
    )
    conf.add_glue(bottom_left).north("bin.0")
    return conf


# python3 datam-tools/Collatz_circuits.py input_two_bridges_type_2_tighter_gap 110 0 0 | ./datam -i
def two_bridges_type_2_tighter_gap(with_input=None, variant1=0, variant2=0):

    if len(with_input) != 3:
        print("Need three input for the two bridges configuration", file=sys.stderr)
        exit(-1)

    conf = datam.Configuration(datam.CollatzTileset)
    conf.add_sub_conf(bridge_type_2(with_input[:2], variant1))
    bottom_left = conf.bottom_left()

    conf.add_sub_conf(
        bridge_type_2((with_input[2], None), variant2).translate(
            bottom_left + NORTH * 3
        )
    )
    conf.delete_tile(bottom_left)
    conf.add_glue(bottom_left + EAST).north("bin.1")
    conf.delete_tile(conf.bottom_left())
    conf.add_glue(conf.bottom_left()).north("bin.1")
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


# Any gate of the form <prefix_top> x <middle_top> y <right_word> going down
def x_y_on_top_gate(with_input=None, right_word="", middle_top="", prefix_top=""):
    conf = datam.Configuration(datam.CollatzTileset)

    total_top_word = prefix_top + " " + middle_top + " "

    curr_in = 1
    for i in range(len(total_top_word)):
        c = total_top_word[::-1][i]
        if c == " ":
            if with_input is None:
                continue

            c = with_input[curr_in]
            curr_in -= 1
            conf.add_glue((-1 * i, 0)).north(f"bin.{c}")

        conf.add_glue((-1 * i, 0)).south(f"bin.{c}")

    for i in range(len(right_word)):
        conf.add_tile(C(0, -1 * i) + SOUTH + EAST).west(f"ter.{right_word[i]}")

    return conf


def input_x_y_on_top_gate(in_, right_word, middle_top="", prefix_top=""):
    return x_y_on_top_gate(in_, right_word, middle_top, prefix_top)


def input_x_y_on_top_canonical_gate(gate_name, in_):

    nicknames = {
        "0R": "0111",
        "AND": "0001",
        "NOR": "1000",
        "NAND": "1110",
        "XOR": "0110",
        "NXOR": "1001",
    }

    # All 2x1 boolean gates in a '0'*g[1] + x + "0" + y with g[0] as right word
    # and g in:
    canonical_gates = {
        "0011": ("0", 0),
        "0110": ("10", 0),
        "1100": ("110", 0),
        "1001": ("0120", 0),
        "0000": ("0", 1),
        "0001": ("1", 1),
        "1101": ("000201", 1),
        "1000": ("1000", 1),
        "1010": ("1001", 1),
        "1110": ("1011", 1),
        "0111": ("00111", 1),
        "1111": ("00121", 1),
        "0100": ("000110", 1),
        "0101": ("012", 2),
        "0010": ("0000210", 2),
        "1011": ("0001112", 2),
    }

    if gate_name in nicknames:
        gate_name = nicknames[gate_name]

    g = canonical_gates[gate_name]

    return x_y_on_top_gate(in_, g[0], "0", "0" * g[1])


def Collatz_tileset():
    conf = datam.Configuration(datam.CollatzTileset)
    conf.add_tile((0, 0), "0")
    conf.add_tile((2, 0), "1")
    conf.add_tile((4, 0), "2")
    conf.add_tile((0, -2), "3")
    conf.add_tile((2, -2), "4")
    conf.add_tile((4, -2), "5")
    return conf


def input_bridge_type_1(in_, variant=0):
    variant = int(variant)
    return bridge_type_1(in_, variant)


def input_bridge_type_2(in_, variant=0):
    variant = int(variant)
    return bridge_type_2(in_, variant)


def input_two_bridges_type_2(in_, variant1=0, variant2=0):
    variant1 = int(variant1)
    variant2 = int(variant2)
    return two_bridges_type_2(in_, int(variant1), int(variant2))


def input_two_bridges_type_2_tighter_gap(in_, variant1=0, variant2=0):
    variant1 = int(variant1)
    variant2 = int(variant2)
    return two_bridges_type_2_tighter_gap(in_, int(variant1), int(variant2))


if __name__ == "__main__":
    conf = locals()[sys.argv[1]](*sys.argv[2:])
    print(conf)