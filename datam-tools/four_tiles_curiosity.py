import sys
import datam
import random
from datam import C, CENTER, NORTH, EAST, SOUTH, WEST

conf = datam.Configuration(datam.DamienTileset)

i = -20
while i < 0:
    conf.add_tile((i, 0)).south(f"bin.{random.randint(0,1)}")
    i += 1

i = -20
while i < -1:
    conf.add_tile((0, i + 1)).west(f"bin.{random.randint(0,1)}")
    i += 1

print(conf)