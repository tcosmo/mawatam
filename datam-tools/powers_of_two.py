import sys
import datam
from datam import C, CENTER, NORTH, EAST, SOUTH, WEST


def build_powers_of_2(size=10):
    size = int(size)
    conf = datam.Configuration(datam.CollatzTileset)

    conf.add_tile(C(1, -1), ["bin.1", None, None, None])
    for x in range(size):
        conf.add_tile(C(0, x), [None, "ter.0", None, None])
        conf.add_tile(C(x + 2, -1), ["bin.0", None, None, None])

    return conf


if __name__ == "__main__":
    # print(locals()[sys.argv[1]](*sys.argv[2:]))
    conf = build_powers_of_2(*sys.argv[1:])
    print(conf)