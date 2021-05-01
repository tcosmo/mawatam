import sys
import datam
from datam import C, CENTER, NORTH, EAST, SOUTH, WEST


def top_binary_and_instr(top_bin, instr):

    conf = datam.Configuration(datam.CollatzTileset)
    curr_pos = C(0, 0)

    for c in top_bin[::-1]:
        conf.add_glue(curr_pos).south(f"bin.{c}")
        curr_pos += WEST

    curr_pos = C(1, -1)
    for c in instr:
        if c == "-":
            curr_pos += WEST
            conf.add_glue(curr_pos)
            continue
        conf.add_glue(curr_pos).west(f"ter.{c}")
        curr_pos += SOUTH

    return conf


def frontierr(in_, repeat=70):
    conf = datam.Configuration(datam.CollatzTileset)

    curr_pos = C(0, 0)

    for r in range(repeat):
        for c in in_:

            if c in ["0", "1"]:
                curr_pos += WEST
                conf.add_glue(curr_pos).north(f"bin.{c}")
            elif c in ["a", "b", "c"]:
                to_p = ord(c) - ord("a")
                conf.add_glue(curr_pos).west(f"ter.{to_p}")
                curr_pos += SOUTH
            else:
                print(f"Invalid frontier character `{c}`", file=sys.stderr)
                exit(-1)

    return conf


def frontier(in_):
    conf = datam.Configuration(datam.CollatzTileset)

    curr_pos = C(0, 0)

    for c in in_:

        if c in ["0", "1"]:
            curr_pos += WEST
            conf.add_glue(curr_pos).north(f"bin.{c}")
        elif c in ["a", "b", "c"]:
            to_p = ord(c) - ord("a")
            conf.add_glue(curr_pos).west(f"ter.{to_p}")
            curr_pos += SOUTH
        else:
            print(f"Invalid frontier character `{c}`", file=sys.stderr)
            exit(-1)

    return conf


def base6_diagonal(in_):
    conf = datam.Configuration(datam.CollatzTileset)
    pos = C(0, 0)

    for i, c in enumerate(in_[::-1]):
        conf.add_tile(pos, str(c))
        pos += NORTH + WEST
    return conf


def base6_anti_diagonal(in_):
    conf = datam.Configuration(datam.CollatzTileset)
    pos = C(0, 0)

    for i, c in enumerate(in_):
        conf.add_tile(pos, str(c))
        pos += NORTH + EAST
    return conf


if __name__ == "__main__":
    conf = locals()[sys.argv[1]](*sys.argv[2:])
    print(conf)