import sys
import datam
from datam import C, CENTER, NORTH, EAST, SOUTH, WEST



def base6_diagonal(in_):
    conf = datam.Configuration(datam.CollatzTileset)
    pos = C(0,0)

    for i,c in enumerate(in_[::-1]):
        conf.add_tile(pos,str(c))
        pos += NORTH + WEST
    return conf

def base6_anti_diagonal(in_):
    conf = datam.Configuration(datam.CollatzTileset)
    pos = C(0,0)

    for i,c in enumerate(in_):
        conf.add_tile(pos,str(c))
        pos += NORTH + EAST
    return conf


if __name__ == "__main__":
    conf = locals()[sys.argv[1]](*sys.argv[2:])
    print(conf)