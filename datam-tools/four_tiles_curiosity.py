import sys
import datam
from datam import C, CENTER, NORTH, EAST, SOUTH, WEST

conf = datam.Configuration(datam.DamienTileset)

i = -20
while i < 0:
    conf.add_tile((i, 0), [None, None, "bin.0", None])
    i += 1

i = -20
while i < -1:
    conf.add_tile((0, i + 1), [None, None, None, f"bin.{int(i==-2)}"])
    i += 1

print(conf)