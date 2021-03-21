# datam

`datam` is a simulator for the datam. It is written in C++ and uses the SFML as graphic library.

## Dependencies

We use the SFML, a powerful graphics, game and media library.

On Debian/Ubuntu, you can install SFML with: `sudo apt install libsfml-dev`, please refer to [this guide](https://www.sfml-dev.org/tutorials/2.5/) for other systems.

## Build

```bash
git clone
cd datam
mkdir build
cmake ..
make
./datam <input_file.txt>
```

## Input format

Here is an example input file:

```
; Collatz tileset
tileset:
  ; format for glues is:
  ; "<glue_name>": <glue_strength>
  ; There is an implicit glue present in all systems: null of strength 0
  glues:
    "bin,0": 1
    "bin,1": 1
    "ter,0": 1
    "ter,1": 1
    "ter,2": 1

  ; format for tile types is:
  ; "<tile_type_name>": <north_glue_name>; <east_glue_name>; <south_glue_name>; <east_glue_name>;
  tile_types:
    "0": "bin,0"; "ter,0"; "bin,0"; "ter,0";
    "1": "bin,0"; "ter,1"; "bin,1"; "ter,0";
    "2": "bin,0"; "ter,2"; "bin,0"; "ter,1";
    "3": "bin,1"; "ter,0"; "bin,1"; "ter,1";
    "4": "bin,1"; "ter,1"; "bin,0"; "ter,2";
    "5": "bin,1"; "ter,2"; "bin,1"; "ter,2";

; format for input is:
; <x>,<y>: <north_glue_name>; <east_glue_name>; <south_glue_name>; <east_glue_name>;
input:
  0,0: null; null; null; "ter,0";
  -1,-1: "bin,0"; null; null; null;
```
