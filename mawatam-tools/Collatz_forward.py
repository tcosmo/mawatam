import sys
import mawatam
from mawatam import C, CENTER, NORTH, EAST, SOUTH, WEST

def Collatz_forward(binary_input, number_odd_steps = "5"):
    conf = mawatam.Configuration(mawatam.xCollatzTileset)
    number_odd_steps = len(binary_input)
    curr_x = 0
    for c in binary_input:
        conf.add_glue((curr_x,0)).south(f"bin.{c}")
        curr_x += 1
    
    for i in range(int(number_odd_steps)):
        conf.add_glue((curr_x,-1*(i+1))).west("x.S")

    return conf

if __name__ == "__main__":
    conf = locals()[sys.argv[1]](*sys.argv[2:])
    print(conf)