# MaWaTAM

`mawatam` is a simulator for the Maze-Walking aTAM, a model introduced in the paper `Small tile sets that compute while solving mazes
` by M. Cook, T. Stérin and D. Woods. It is written in C++ and uses the SFML as graphic library.

## Dependencies

We use the SFML, a powerful graphics, game and media library.

On Debian/Ubuntu, you can install SFML with: `sudo apt install libsfml-dev`, please refer to [this guide](https://www.sfml-dev.org/tutorials/2.5/) for other systems.

## Build

```bash
git clone
cd mawatam
mkdir build
cmake ..
make
```

## Run

The simulator can either be run from a `.yml` input file (see input format below):

```bash
./datam -f examples/powers_of_two.yml
```

Or reading from `stdin`, in the same format:

`python3 mawatam-tools/four_tiles_circuits.py prime_circuit 110  | ./mawatam -i`

## Examples

You can simulate the constructions presented in the paper by running the following commands:

### NAND-NXOR tile set

- `python3 mawatam-tools/four_tiles_circuits.py prime_circuit 110  | ./mawatam -i`
- `python3 mawatam-tools/four_tiles_circuits.py crossover 01  | ./mawatam -i`

### Collatz tile set

- `python3 mawatam-tools/Collatz_circuits.py prime_circuit_better 111  | ./mawatam -i`
- `python3 mawatam-tools/Collatz_circuits.py bridge_type_2_in_context 11 | ./mawatam -i`
- `python3 mawatam-tools/Collatz_circuits.py input_x_y_on_east_canonical_gate NAND  | ./mawatam -i`
- `python3 mawatam-tools/Collatz_forward.py Collatz_forward 1001011 | ./mawatam -i`
- `python3 mawatam-tools/powers_of_two.py 100 | ./mawatam -i`

## Controls

- `N`: next simulation step
- `R`: reset simulation
- `arrows`: translate the scene
- `mouse wheel pressed`: translate the scene
- `CTRL + mouse wheel down/up`: zoom in/out
- `+ / -`: zoom in/out
- `G`: change growth mode between asynchronous ordered, asynchronous random, synchronous
- `A`: prints information about the simulation and scene in the terminal
- `D`: dumps the current configuration in `out.yml` file


## Input format

The input format is built on `yaml`. 

```yaml
# This input configuration constructs powers of two in base 3

# The format for specifying glues is:
# <glue_alphabet_name>.<glue_name>: <glue_strength>
# The alphabet name must comply with [_a-zA-Z][_a-zA-Z0-9]*
# The glue name must comply with [a-zA-Z0-9]
# Hence, the complete glue name must match [_a-zA-Z][_a-zA-Z0-9]*\\.[a-zA-Z0-9]
glues:
  bin.0: 1
  bin.1: 1
  ter.0: 1
  ter.1: 1
  ter.2: 1

temperature: 2

# The format for specifying tile types is:
# <tile_type_name>: [<glue_north_name>, <glue_east_name>, <glue_south_name>, <glue_west_name>]
# The tile type name must comply with [a-zA-Z0-9]
tileset_tile_types:
  0: [bin.0, ter.0, bin.0, ter.0]
  1: [bin.0, ter.1, bin.1, ter.0]
  2: [bin.0, ter.2, bin.0, ter.1]
  3: [bin.1, ter.0, bin.1, ter.1]
  4: [bin.1, ter.1, bin.0, ter.2]
  5: [bin.1, ter.2, bin.1, ter.2]

# When you specify a configuration, you can either use anonymous tile types that wont be registered in the tileset:
# <x>,<y>: [<glue_north_name>, <glue_east_name>, <glue_south_name>, <glue_west_name>]
# Or use tile types of the tileset:
# <x>, <y>: <tile_type_name>
configuration:
  0,0: [null, ter.0, null, null]
  0,1: [null, ter.0, null, null]
  0,2: [null, ter.0, null, null]
  0,3: [null, ter.0, null, null]
  0,4: [null, ter.0, null, null]
  0,5: [null, ter.0, null, null]
  0,6: [null, ter.0, null, null]
  0,7: [null, ter.0, null, null]
  0,8: [null, ter.0, null, null]
  0,9: [null, ter.0, null, null]
  0,10: [null, ter.0, null, null]
  0,11: [null, ter.0, null, null]
  0,12: [null, ter.0, null, null]
  0,13: [null, ter.0, null, null]
  0,14: [null, ter.0, null, null]
  0,15: [null, ter.0, null, null]
  0,16: [null, ter.0, null, null]
  1,-1: [bin.1, null, null, null]
  2,-1: [bin.0, null, null, null]
  3,-1: [bin.0, null, null, null]
  4,-1: [bin.0, null, null, null]
  5,-1: [bin.0, null, null, null]
  6,-1: [bin.0, null, null, null]
  7,-1: [bin.0, null, null, null]
  8,-1: [bin.0, null, null, null]
  9,-1: [bin.0, null, null, null]
  10,-1: [bin.0, null, null, null]
  11,-1: [bin.0, null, null, null]
  12,-1: [bin.0, null, null, null]
  13,-1: [bin.0, null, null, null]
  14,-1: [bin.0, null, null, null]
  15,-1: [bin.0, null, null, null]
  16,-1: [bin.0, null, null, null]
  17,-1: [bin.0, null, null, null]
  18,-1: [bin.0, null, null, null]
  19,-1: [bin.0, null, null, null]
  20,-1: [bin.0, null, null, null]
  21,-1: [bin.0, null, null, null]
  22,-1: [bin.0, null, null, null]
  23,-1: [bin.0, null, null, null]

# The two following entries are not implem yet

# You can specify the color with which the simulator will render
# glues that use a given alphabet name:
# <glue_alphabet_name>: #hexcolor or `auto` to let the simulator choose
# By default it is set to auto
glue_alphabets_color:
  bin: auto
  ter: auto

# The view will either display glue colors given their
# alphabet name or glue char can be toggled by pressing `e`
default_glue_color_mode: alphabet
```
