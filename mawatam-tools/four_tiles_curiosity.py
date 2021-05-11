import sys
import mawatam
import random
from mawatam import C, CENTER, NORTH, EAST, SOUTH, WEST

conf = mawatam.Configuration(mawatam.DamienTileset)

i = -20
while i < 0:
    conf.add_tile((i, 0)).south(f"bin.{random.randint(0,1)}")
    i += 1

# i = -20
# while i < -1:
#     conf.add_tile((0, i + 1)).west(f"bin.{random.randint(0,1)}")
#     i += 1

conf.add_tile((0, -1)).west("bin.0")
conf.add_tile((0, -2)).west("bin.1")
conf.add_tile((0, -3)).west("bin.0")
conf.add_tile((0, -4)).west("bin.1")
conf.add_tile((0, -5)).west("bin.0")
conf.add_tile((0, -6)).west("bin.1")
conf.add_tile((0, -7)).west("bin.0")
conf.add_tile((0, -8)).west("bin.1")

print(conf)