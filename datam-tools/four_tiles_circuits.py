import sys
import datam
from datam import C, CENTER, NORTH, EAST, SOUTH, WEST


def self_nand_gate(size=3):
    conf = datam.Configuration(datam.DamienTileset)
    curr_pos = CENTER

    for i in range(size):
        conf.add_tile(CENTER + NORTH + WEST * (i + 1), [None, None, "bin.1", None])
        if i < size - 2:
            curr_pos += WEST

    conf.add_tile(curr_pos + SOUTH + EAST, [None, None, None, "bin.1"])
    conf.add_tile(curr_pos + SOUTH * 2 + EAST, [None, None, None, "bin.1"])
    conf.add_tile(curr_pos + SOUTH + WEST, [None, None, "bin.1", "bin.1"])

    return conf


def or_gate(with_input=None):
    conf = datam.Configuration(datam.DamienTileset)

    self_nand_size = 3
    conf.add_sub_conf(self_nand_gate(self_nand_size))
    bottom_left = conf.bottom_left()
    conf.add_tile(bottom_left + SOUTH + EAST, [None] * 3 + ["bin.1"])
    conf.add_tile(bottom_left + WEST, [None] * 2 + ["bin.1", None])
    conf.add_tile(bottom_left + WEST * 2, [None] * 2 + ["bin.1", None])

    south_translate = 6
    second_self_nand = self_nand_gate().translate(SOUTH * south_translate)
    conf.add_sub_conf(second_self_nand)
    conf.add_tile(second_self_nand.bottom_left() + SOUTH + EAST, [None] * 3 + ["bin.1"])

    for i in range(5):
        if i < 4:
            conf.add_tile(bottom_left + WEST + SOUTH * (i + 2), [None] * 3 + ["bin.1"])
        else:
            conf.add_tile(
                bottom_left + WEST + SOUTH * (i + 2), [None, None, "bin.1", "bin.1"]
            )
    # inputs
    if with_input is not None:
        conf.add_tile(CENTER, [None, None, None, f"bin.{with_input[0]}"])
        conf.add_tile(
            SOUTH * south_translate, [None, None, None, f"bin.{with_input[1]}"]
        )

    return conf


def input_or_gate(in_):
    return or_gate(in_)


if __name__ == "__main__":
    conf = locals()[sys.argv[1]](*sys.argv[2:])
    print(conf)