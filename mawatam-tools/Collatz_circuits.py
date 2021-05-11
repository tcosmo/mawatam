import sys
import mawatam
from mawatam import C, CENTER, NORTH, EAST, SOUTH, WEST


def bridge_type_2_in_context(with_input=None):
    conf = bridge_type_2()

    for i in range(4):
        conf.add_glue((0, i)).west("ter.0")

    for i in range(3):
        conf.add_tile((0, -7 - i)).west("ter.0")

    for i in range(2):
        conf.add_glue((2 - i, -3)).north("bin.0")
        conf.add_glue((-7 - i, -6)).north("bin.0")

    if with_input is not None:
        conf.add_glue((-1, 4)).south(f"bin.{with_input[0]}")
        conf.add_glue((3, -2)).west(f"ter.{with_input[1]}")

    return conf

def circuit_from_spec(spec_file, with_input=None):
    conf = mawatam.Configuration(mawatam.CollatzTileset)
    with open(spec_file, "r") as f:
        spec_content = f.read()

    spec_split = spec_split.split("\n")

    nodes_index = {}
    nodes = []

    for i, line in enumerate(spec_split):
        space_split = line.split(" ")
        node_name = space_split[0]
        nodes_index[node_name] = i
        if len(space_split) == 1:
            # input node
            nodes.append((True, node_name))
        else:
            # computation node
            # <name> <gate> <node x> <node y>
            nodes.append(
                (False, node_name, space_split[1], space_split[2], space_split[3])
            )

    conf.add_tile((0, 0))
    return conf


def circuit_prime_3_bits(with_input=None):
    # (not x and y) or (x and z)
    conf = mawatam.Configuration(mawatam.CollatzTileset)

    if with_input is not None:
        # x
        conf.add_tile(C(0, 0) + EAST).west(f"ter.{with_input[0]}")
        # top horizontal wire for x
        conf.add_tile(C(0, 0) + SOUTH).north("bin.1")
        conf.add_tile(C(0, 0) + WEST + SOUTH).north("bin.1")
        conf.add_tile(C(0, 0) + WEST * 2 + SOUTH).north("bin.1")

        # y
        conf.add_tile(C(0, 0) + SOUTH * 3 + EAST).west(f"ter.{with_input[1]}")

        # horz wire for y
        now = C(0, 0) + SOUTH * 3
        conf.add_glue(now + SOUTH).north("bin.1")
        conf.add_glue(now + WEST + SOUTH).north("bin.1")

        # z
        conf.add_tile(C(0, 0) + SOUTH * 9 + EAST).west(f"ter.{with_input[2]}")

        # horz wire for z
        now = C(0, 0) + SOUTH * 9
        conf.add_glue(now + SOUTH).north("bin.1")
        conf.add_glue(now + WEST + SOUTH).north("bin.1")

    # turn south for x
    conf.add_tile(C(0, 0) + WEST * 3 + NORTH).south("bin.1")
    conf.add_tile(C(0, 0) + WEST * 4 + NORTH).south("bin.0")
    conf.add_glue(C(0, 0) + WEST * 5 + SOUTH).north("bin.1")
    conf.add_glue(C(0, 0) + WEST * 2 + SOUTH).west("ter.0")

    # growth blocker
    conf.add_tile(C(0, 0) + WEST * 4 + SOUTH)

    # bridge x y
    now = C(0, 0) + WEST * 3 + SOUTH
    conf.add_sub_conf(bridge_type_2(minimal=True).translate(C(-2, -1)))

    # bridge x z
    now = C(0, 0) + WEST * 2 + SOUTH * 7
    conf.add_glue(now).west("ter.0")
    conf.add_sub_conf(bridge_type_2(minimal=True).translate(now))

    # brige y z
    now = C(0, 0) + WEST * 7 + SOUTH * 13
    conf.add_tile(now).north("bin.1")
    conf.add_tile(now + WEST).north("bin.1")
    now += WEST * 2 + NORTH * 3
    conf.add_sub_conf(bridge_type_2(minimal=True).translate(now))

    # turn for y
    now = now = C(0, 0) + WEST * 7 + SOUTH * 7
    conf.add_glue(now).north("bin.1")
    conf.add_glue(now + WEST).north("bin.1")
    conf.add_glue(now + WEST * 2).north("bin.1")
    conf.add_glue(now + WEST * 3 + NORTH * 2).south("bin.1")
    # conf.add_glue(now + WEST * 4 + NORTH * 2).south("bin.0")
    # conf.add_glue(now + WEST * 5).north("bin.1")

    # y goes south to bridge
    for i in range(4):
        conf.add_glue(now + WEST * 2 + SOUTH * i).west("ter.0")

    now = C(0, 0) + SOUTH * 15 + WEST
    # bring x to gate
    for i in range(4):
        conf.add_glue(now + NORTH * 2 + WEST + SOUTH * i).west("ter.0")
        conf.add_glue(now + NORTH * 2 + WEST * 3 + SOUTH * 4 + WEST * i).north("bin.1")
    conf.add_tile(now + NORTH * 2 + WEST * 3 + SOUTH * 4 + NORTH * 2)
    conf.add_glue(now + NORTH * 2 + WEST * 3 + SOUTH * 4 + NORTH * 2 + WEST * 4).south(
        "bin.1"
    )

    conf.add_sub_conf(
        input_x_y_on_top_canonical_gate("AND").translate(now + WEST * 7 + SOUTH)
    )
    conf.add_glue(now + WEST * 7 + SOUTH + WEST).west("ter.0")

    # bridge (x,z)
    conf.add_glue(C(-16, -13) + EAST + SOUTH * 3).north("bin.1")
    conf.add_glue(C(-16, -13) + EAST * 2 + SOUTH * 3).north("bin.1")
    conf.add_sub_conf(bridge_type_2(minimal=True).translate((-16, -13)))

    # bridge (x, AND x y)
    conf.add_glue((-16, -19)).west("ter.0")
    conf.add_sub_conf(bridge_type_2(minimal=True).translate((-16, -19)))

    # x coming to bridges (x,z) (x, AND x y)
    for i in range(6, 17):
        conf.add_glue((-1 * i, -1)).north("bin.1")
    conf.add_glue((-17, 1)).south("bin.1")

    for i in range(1, 14):
        conf.add_glue((-16, -1 * i)).west("ter.0")

    # (AND x y) coming to x bridge we need a not to correct parity
    conf.add_glue((-12, -16)).south("bin.1")
    for i in range(18, 22):
        conf.add_glue((-11, -1 * i)).west("ter.0")

    for i in range(13, 16):
        conf.add_glue((-1 * i, -22)).north("bin.1")

    conf.add_tile((-13, -20))

    # bridge z, (AND x y)
    conf.add_glue((-21, -25)).north("bin.1")
    conf.add_glue((-22, -25)).north("bin.1")
    conf.add_sub_conf(bridge_type_2(minimal=True).translate((-23, -22)))

    # z coming to brige z, (AND x y)
    for i in range(21, 24):
        conf.add_glue((-1 * i, -19)).north("bin.1")

    conf.add_glue((-24, -17)).south("bin.1")

    for i in range(19, 23):
        conf.add_glue((-23, -1 * i)).west("ter.0")

    # (AND x z)
    conf.add_sub_conf(input_x_y_on_top_canonical_gate("AND").translate((-22, -28)))

    # bringing x to (AND x z)
    conf.add_glue((-23, -28)).west("ter.0")
    for i in range(25, 29):
        conf.add_glue((-16, -1 * i)).west("ter.0")

    for i in range(18, 22):
        conf.add_glue((-1 * i, -29)).north("bin.1")
    conf.add_tile((-18, -27))

    # OR
    conf.add_glue((-22, -27)).south("bin.0")
    conf.add_glue((-26, -28)).south("bin.0")
    conf.add_sub_conf(input_x_y_on_top_canonical_gate("OR").translate((-26, -29)))

    # bring AND(x,y) to OR gate
    conf.add_glue((-28, -26)).south("bin.0")
    conf.add_glue((-27, -28)).west("ter.0")
    conf.add_glue((-27, -29)).west("ter.0")

    # light bulb
    for i in range(30, 36):
        conf.add_glue((-1 * i, -33)).south("bin.0")
    for i in range(35, 40):
        conf.add_glue((-29, -1 * i)).west("ter.0")

    return conf


def fanout_two_even_pos(with_input=None):
    conf = mawatam.Configuration(mawatam.CollatzTileset)
    if with_input is not None:
        conf.add_glue((0, 0)).west(f"ter.{with_input[0]}")
    conf.add_glue((-1, -1)).north("bin.1")
    conf.add_glue((-2, -1)).north("bin.1").west("ter.1")
    conf.add_glue(C(-2, -1) + NORTH * 2 + WEST).south("bin.1")
    return conf


def bridge_type_2_inverter(with_input=None, variant=0, minimal=False):
    bridge_variant = [
        ("000", "021"),
    ]
    conf = mawatam.Configuration(mawatam.CollatzTileset)

    # TOP
    conf.add_glue((0, 0)).south("bin.0")

    for i in range(3):
        conf.add_glue((-1 * (i + 2), 0)).south("bin.0")

    # EAST
    conf.add_glue((1, -1)).west("ter.0")

    for i in range(3):
        conf.add_glue((1, -(i + 3))).west(
            f"ter.{bridge_variant[variant%len(bridge_variant)][1][i]}",
        )

    # Output wiring
    bottom_left = conf.bottom_left()
    if not minimal:
        conf.add_glue(bottom_left + SOUTH + WEST).north("bin.0")
        conf.add_glue(bottom_left + SOUTH + WEST * 2).north("bin.0")
        conf.add_glue(bottom_left + SOUTH + EAST * 4).west("ter.0")

    conf.add_glue(bottom_left + NORTH + WEST)
    conf.add_glue(bottom_left + SOUTH + EAST * 2)

    # input
    if with_input is not None:
        if with_input[0] is not None:
            conf.add_glue((-1, 0)).south(f"bin.{with_input[0]}").north(
                f"bin.{with_input[0]}"
            )
        if with_input[1] is not None:
            conf.add_glue(C(1, -2)).east(f"ter.{with_input[1]}").west(
                f"ter.{with_input[1]}"
            )

    return conf


def bridge_type_2(with_input=None, variant=0, minimal=False):
    # This bridge takes one input from north and one input from east
    # and carries them along: north is to be read on same column than input north
    # and east is to be read on the bottom left most tile

    # if minimal=False a short 2 base wire will be added for west output

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
    conf = mawatam.Configuration(mawatam.CollatzTileset)

    # TOP
    conf.add_glue((0, 0)).south("bin.0")

    for i in range(3):
        conf.add_glue((-1 * (i + 2), 0)).south("bin.0")

    # EAST
    conf.add_glue((1, -1)).west("ter.0")

    for i in range(3):
        conf.add_glue((1, -(i + 3))).west(
            f"ter.{bridge_variant[variant%len(bridge_variant)][1][i]}",
        )

    # Output wiring
    bottom_left = conf.bottom_left()
    if not minimal:
        conf.add_glue(bottom_left + SOUTH + WEST).north("bin.0")
        conf.add_glue(bottom_left + SOUTH + WEST * 2).north("bin.0")
        conf.add_glue(bottom_left + SOUTH + EAST * 4).west("ter.0")

    conf.add_glue(bottom_left + NORTH + WEST)
    conf.add_glue(bottom_left + SOUTH + EAST * 2)

    # input
    if with_input is not None:
        if with_input[0] is not None:
            conf.add_glue((-1, 0)).south(f"bin.{with_input[0]}").north(
                f"bin.{with_input[0]}"
            )
        if with_input[1] is not None:
            conf.add_glue(C(1, -2)).east(f"ter.{with_input[1]}").west(
                f"ter.{with_input[1]}"
            )

    return conf


# python3 mawatam-tools/Collatz_circuits.py input_two_bridges_type_2 110 0 0 | ./mawatam -i
def two_bridges_type_2(with_input=None, variant1=0, variant2=0):

    if len(with_input) != 3:
        print("Need three input for the two bridges configuration", file=sys.stderr)
        exit(-1)

    conf = mawatam.Configuration(mawatam.CollatzTileset)
    conf.add_sub_conf(bridge_type_2(with_input[:2], variant1))
    bottom_left = conf.bottom_left()
    conf.add_sub_conf(
        bridge_type_2((with_input[2], None), variant2).translate(
            bottom_left + NORTH * 3 + WEST * 1
        )
    )
    conf.add_glue(bottom_left).north("bin.0")
    return conf


# python3 mawatam-tools/Collatz_circuits.py input_two_bridges_type_2_tighter_gap 110 0 0 | ./mawatam -i
def two_bridges_type_2_tighter_gap(with_input=None, variant1=0, variant2=0):

    if len(with_input) != 3:
        print("Need three input for the two bridges configuration", file=sys.stderr)
        exit(-1)

    conf = mawatam.Configuration(mawatam.CollatzTileset)
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
    conf = mawatam.Configuration(mawatam.CollatzTileset)

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
    conf = mawatam.Configuration(mawatam.CollatzTileset)

    total_top_word = prefix_top + " " + middle_top + " "

    curr_in = 1
    for i in range(len(total_top_word)):
        c = total_top_word[::-1][i]
        if c == " ":
            if with_input is None:
                continue

            c = with_input[curr_in]
            curr_in -= 1
            # conf.add_glue((-1 * i, 0)).north(f"bin.{c}")

        conf.add_glue((-1 * i, 0)).south(f"bin.{c}")

    for i in range(len(right_word)):
        conf.add_glue(C(0, -1 * i) + SOUTH + EAST).west(f"ter.{right_word[i]}")

    return conf


def input_x_y_on_top_gate(in_, right_word, middle_top="", prefix_top=""):
    return x_y_on_top_gate(in_, right_word, middle_top, prefix_top)


def input_x_y_on_top_canonical_gate(gate_name, in_=None):

    nicknames = {
        "OR": "0111",
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
        "1101": ("000201", 1),  # can do better with ('1','01',1)
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


def x_y_on_east_gate(with_input, top, east, middle_east):

    conf = mawatam.Configuration(mawatam.CollatzTileset)

    total_east_word = " " + middle_east + " " + east

    curr_in = 0
    for i in range(len(total_east_word)):
        c = total_east_word[i]
        if c == " ":
            if with_input is None:
                continue

            c = with_input[curr_in]
            curr_in += 1

        conf.add_glue((0, -1 * i)).west(f"ter.{c}")

    for i in range(len(top)):
        conf.add_glue(C(-1 * i, 0) + NORTH + WEST).south(f"bin.{top[::-1][i]}")

    return conf


def input_x_y_on_east_canonical_gate(gate_name, in_=None):
    """size_dict, int(frep), frep, top, east, middle_east, score
    1 4 0100 01  0 12
    2 13 1101 10  0 12
    3 3 0011 000  0 16
    4 0 0000 0000  0 20
    5 1 0001 0010  0 20
    6 15 1111 0110  0 20
    7 7 0111 1101  0 20
    8 6 0110 0 0 0 10
    9 9 1001 1 0 0 10
    9 1 0001 00 0 0 15
    10 12 1100 0101 0 0 25
    10 7 0111 00 1 0 15
    11 8 1000 10 00 0 18
    12 14 1110 10 01 0 18
    13 5 0101 0000 000 0 35
    14 10 1010 0011 000 0 35
    15 2 0010 1101 0000 0 40
    16 11 1011 1111 0001 0 40
    """
    nicknames = {
        "OR": "0111",
        "AND": "0001",
        "NOR": "1000",
        "NAND": "1110",
        "XOR": "0110",
        "NXOR": "1001",
    }

    # All 2x1 boolean gates in a '0'*g[1] + x + "0" + y with g[0] as right word
    # and g in:
    canonical_gates = {
        "0000": ("0000", "", "0"),
        "0001": ("00", "0", "0"),
        "0010": ("1101", "0000", "0"),
        "0011": ("000", "", "0"),
        "0100": ("01", "", "0"),
        "0101": ("0000", "000", "0"),
        "0110": ("0", "0", "0"),
        "0111": ("00", "1", "0"),
        "1000": ("10", "00", "0"),
        "1001": ("1", "0", "0"),
        "1010": ("0011", "000", "0"),
        "1011": ("1111", "0001", "0"),
        "1100": ("0101", "0", "0"),
        "1101": ("10", "", "0"),
        "1110": ("10", "01", "0"),
        "1111": ("0110", "", "0"),
    }

    if gate_name in nicknames:
        gate_name = nicknames[gate_name]

    g = canonical_gates[gate_name]

    return x_y_on_east_gate(in_, g[0], g[1], g[2])


def fanout(even=True):
    conf = mawatam.Configuration(mawatam.CollatzTileset)
    conf.add_glue((0, -1)).west(f"ter.{int(even)}")
    conf.add_glue((-1, 1)).south("bin.1")
    conf.add_glue((-2, 1)).south("bin.0")
    conf.add_glue((-2, -1))
    return conf


def not_gate(in_=None):
    conf = mawatam.Configuration(mawatam.CollatzTileset)
    if in_ is not None:
        conf.add_glue((0, 0)).west(f"ter.{in_[0]}")
    conf.add_glue((-1, 1)).south("bin.1")
    conf.add_glue((0, -1)).west("ter.0")
    return conf


def prime_circuit_better(in_=None):
    conf = mawatam.Configuration(mawatam.CollatzTileset)

    if in_ is not None:
        conf.add_glue((1, 0)).west(f"ter.{in_[0]}")
        conf.add_glue((1, -3)).west(f"ter.{in_[1]}")
        conf.add_glue((1, -11)).west(f"ter.{in_[2]}")

    conf.add_glue((0, -1)).north(f"bin.1")
    conf.add_sub_conf(fanout(even=False))  # even true because we want not

    conf.add_sub_conf(bridge_type_2(minimal=True).translate(SOUTH))

    # not x and y
    conf.add_sub_conf(input_x_y_on_east_canonical_gate("AND").translate((-6, -4)))

    # not x to and
    conf.add_glue((-3, -1)).north("bin.1")
    conf.add_glue((-4, -1)).north("bin.1")
    conf.add_glue((-5, -1)).north("bin.1")

    for i in range(1, 5):
        conf.add_glue((-5, -1 * i)).west("ter.0")

    conf.add_glue((-6, 1)).south("bin.1")

    # y to and
    conf.add_glue((-5, -7)).north("bin.1")
    conf.add_glue((-6, -7)).north("bin.1")

    # x and z
    conf.add_sub_conf(input_x_y_on_east_canonical_gate("AND").translate((-6, -10)))

    # x to and
    conf.add_glue((0, -7)).west("ter.0")
    conf.add_glue((0, -8)).west("ter.0")
    conf.add_glue((0, -9)).west("ter.0")
    conf.add_glue((-2, -8))

    for i in range(2, 6):
        conf.add_glue((-1 * i, -10)).north("bin.1")

    conf.add_sub_conf(not_gate().translate((-4, -11)))
    conf.add_sub_conf(not_gate().translate((-5, -9)))
    # z to and
    for i in range(0, 5):
        conf.add_glue((-1 * i, -12)).north("bin.1")
    conf.add_glue((-1, -10))
    conf.add_glue((-6, -13)).north("bin.1")
    conf.add_glue((-5, -10)).south("bin.0")
    conf.add_glue((-6, -8)).south("bin.0")
    conf.add_glue((-9, -6))
    conf.add_sub_conf(input_x_y_on_east_canonical_gate("OR").translate((-10, -11)))
    conf.add_glue((-9, -14)).north("bin.1")
    conf.add_glue((-10, -14)).north("bin.1")
    conf.add_glue((-9, -12))

    conf.add_glue((-9, -8)).north("bin.1")

    conf.add_glue((-10, -6)).south("bin.1")
    for i in range(8, 12):
        conf.add_glue((-9, -1 * i)).west("ter.0")
    return conf


def Collatz_tileset():
    conf = mawatam.Configuration(mawatam.CollatzTileset)
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