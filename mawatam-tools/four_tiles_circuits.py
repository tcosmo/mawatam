import sys
import mawatam
from mawatam import C, CENTER, NORTH, EAST, SOUTH, WEST
import random


def diago_135(in_word):
    conf = mawatam.Configuration(mawatam.DamienTileset)
    curr_pos = CENTER

    for c in in_word:
        conf.add_tile(curr_pos, c)
        curr_pos += EAST
        curr_pos += SOUTH

    return conf


def diago_135_big(size="100"):
    size = int(size)

    return diago_135(random.choices(["A", "B", "C", "D"], k=size))


def self_nand_gate(size=3):
    conf = mawatam.Configuration(mawatam.DamienTileset)
    curr_pos = CENTER

    for i in range(size):
        conf.add_glue(CENTER + NORTH + WEST * (i + 1)).south("bin.1")
        if i < size - 2:
            curr_pos += WEST

    conf.add_tile(curr_pos + SOUTH + EAST).west("bin.1")
    conf.add_tile(curr_pos + SOUTH * 2 + EAST).west("bin.1")
    conf.add_tile(curr_pos + SOUTH + WEST).south("bin.1").west("bin.1")

    return conf


def crossover(with_input=None):
    conf = mawatam.Configuration(mawatam.DamienTileset)

    if with_input is not None:
        conf.add_glue((-1, 0)).west(f"bin.{with_input[0]}")
        conf.add_glue((-1, -2)).west(f"bin.{with_input[1]}")

    for i in range(2, 7):
        conf.add_glue((-1 * i, 1)).south("bin.1")

    # conf.add_glue((-1, -1)).south("bin.1")
    conf.add_glue((-2, -1)).south("bin.1").west("bin.1")

    # conf.add_glue((-2, -3)).south("bin.1")
    conf.add_glue((-3, -3)).south("bin.1")
    conf.add_glue((-4, -3)).south("bin.1").west("bin.1")

    conf.add_glue((-4, -1)).south("bin.0")
    conf.add_glue((-5, -1)).south("bin.1").west("bin.1")

    conf.add_glue((-1, -3)).west("bin.1")
    conf.add_glue((-1, -4)).west("bin.1")
    conf.add_glue((-6, -3))

    conf.add_glue((-7, -1)).south("bin.0")
    conf.add_glue((-6, -3)).south("bin.0")
    return conf


def prime_circuit(with_input=None):
    conf = mawatam.Configuration(mawatam.DamienTileset)

    if with_input is not None:
        conf.add_glue((0, 0)).west(f"bin.{with_input[0]}")
        conf.add_glue((0, -2)).west(f"bin.{with_input[1]}")
        conf.add_glue((0, -6)).west(f"bin.{with_input[2]}")

    conf.add_sub_conf(crossover().translate(EAST))

    conf.add_glue((-6, 1)).south("bin.1")
    conf.add_glue((-7, 1)).south("bin.0")
    conf.add_glue((-7, -1)).south("bin.1")

    conf.add_glue((-8, 1)).south("bin.1")
    conf.add_glue((-7, -1)).west("bin.1")

    conf.add_glue((-7, -3)).west("bin.1")

    conf.add_glue((-6, -3)).south("bin.1")
    conf.add_glue((-5, -5)).west("bin.1")

    for x in range(1, 6):
        conf.add_glue((-1 * x, -5)).south("bin.1")

    conf.add_glue((-5, -7)).west("bin.1")
    conf.add_glue((-5, -8)).west("bin.1")
    conf.add_glue((-5, -9)).west("bin.1")
    conf.add_glue((-7, -8)).south("bin.1")

    for y in range(4, 9):
        conf.add_glue((-7, y * -1)).west("bin.1")

    return conf


def or_gate(with_input=None):
    conf = mawatam.Configuration(mawatam.DamienTileset)
    conf.add_tile(CENTER).west("bin.1").south("bin.0")

    if with_input is not None:
        conf.add_tile(CENTER + WEST + NORTH).south(f"bin.{with_input[0]}")
        conf.add_tile(CENTER + SOUTH + EAST).west(f"bin.{with_input[1]}")

    return conf


def or_gate_complex(with_input=None):
    conf = mawatam.Configuration(mawatam.DamienTileset)
    self_nand_size = 3
    conf.add_sub_conf(self_nand_gate(self_nand_size))
    bottom_left = conf.bottom_left()
    conf.add_tile(bottom_left + SOUTH + EAST).west("bin.1")
    conf.add_tile(bottom_left + WEST).south("bin.1")
    conf.add_tile(bottom_left + WEST * 2).south("bin.1")

    south_translate = 6
    second_self_nand = self_nand_gate().translate(SOUTH * south_translate)
    conf.add_sub_conf(second_self_nand)
    conf.add_tile(second_self_nand.bottom_left() + SOUTH + EAST).west("bin.1")

    for i in range(5):
        if i < 4:
            conf.add_tile(bottom_left + WEST + SOUTH * (i + 2)).west("bin.1")
        else:
            conf.add_tile(bottom_left + WEST + SOUTH * (i + 2)).south("bin.1").west(
                "bin.1"
            )

    # inputs
    if with_input is not None:
        conf.add_tile(CENTER).west(f"bin.{with_input[0]}")
        conf.add_tile(SOUTH * south_translate).west(f"bin.{with_input[1]}")

    return conf


def wires_fan_out_nand(with_input=None):
    conf = mawatam.Configuration(mawatam.DamienTileset)

    # input
    if with_input is not None:
        conf.add_tile(C(1, -1)).west(f"bin.{with_input}")

    # Horz wire up
    hwire_up_length = 4
    for i in range(hwire_up_length):
        conf.add_tile(C(0, 0) + WEST * i).south("bin.1")

    conf.add_tile(C(0, 0) + WEST * (hwire_up_length - 1) + SOUTH * 2)

    # Vertical wire
    vert_length = 5
    for i in range(vert_length + 2):
        conf.add_tile(C(0, 0) + WEST + SOUTH * (i + 2)).west("bin.1")

    curr = C(0, 0) + WEST + SOUTH * (vert_length + 1)

    conf.add_tile(curr + SOUTH * 2 + WEST * 2)

    # Horz wire down
    hwire_down_length = 2
    for i in range(hwire_down_length):
        conf.add_tile(curr + WEST * (i + 2)).south("bin.1")

    return conf


def horizontal_wire(with_input=None, size="4"):
    size = int(size)
    conf = mawatam.Configuration(mawatam.DamienTileset)

    for i in range(size):
        conf.add_glue((-1 * i, 1)).south("bin.1")

    if with_input is not None:
        conf.add_glue((1, 0)).west(f"bin.{with_input}")

    return conf


def Four_tileset():
    conf = mawatam.Configuration(mawatam.DamienTileset)
    conf.add_tile((0, 0), "A")
    conf.add_tile((2, 0), "B")
    conf.add_tile((0, -2), "C")
    conf.add_tile((2, -2), "D")
    return conf


def input_or_gate(in_):
    return or_gate(in_)


if __name__ == "__main__":
    conf = locals()[sys.argv[1]](*sys.argv[2:])
    print(conf)