yaml_spacer = " " * 2


def format_pos(pos):
    return str(pos[0]) + "," + str(pos[1])


def format_square_glues(square_glues):
    return str(square_glues).replace("None", "null").replace('"', "").replace("'", "")


# In order to add tuples together
class C(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return C(self.x + other.x, self.y + other.y)

    def __lmul__(self, other):
        if type(other) == int or type(other) == float:
            return T(other * self.x, other * self.y)
        return T(other.x * self.x, other.y * self.y)

    def __getitem__(self, key):
        if key % 2 == 0:
            return self.x
        return self.y


CENTER = C(0, 0)
NORTH = C(0, 1)
EAST = C(1, 0)
SOUTH = C(0, -1)
WEST = C(-1, 0)

from datam.configuration import Configuration
from datam.tileset import Tileset
from datam.tileset_collection import *