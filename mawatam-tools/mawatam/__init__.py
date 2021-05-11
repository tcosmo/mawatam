yaml_spacer = " " * 2


def format_pos(pos):
    return str(pos[0]) + "," + str(pos[1])


def format_square_glues(square_glues):
    if type(square_glues) == str:
        return f'"{square_glues}"'
    return str(square_glues).replace("None", "null").replace('"', "").replace("'", "")


# In order to add tuples together
class C:
    def __init__(self, x, y=0):
        if isinstance(x, tuple):
            self.x = x[0]
            self.y = x[1]
        else:
            self.x = x
            self.y = y

    def t(self):
        return self.x, self.y

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __add__(self, other):
        return C(self.x + other.x, self.y + other.y)

    def __mul__(self, other):
        if isinstance(other, (int, float)):
            return C(other * self.x, other * self.y)
        elif isinstance(other, C):
            return C(other.x * self.x, other.y * self.y)

    def __getitem__(self, key):
        if key % 2 == 0:
            return self.x
        return self.y

    def __str__(self):
        return str(self.t())

    def __hash__(self):
        return self.t().__hash__()


CENTER = C(0, 0)
NORTH = C(0, 1)
EAST = C(1, 0)
SOUTH = C(0, -1)
WEST = C(-1, 0)

from mawatam.configuration import Configuration
from mawatam.tileset import Tileset
from mawatam.tileset_collection import *