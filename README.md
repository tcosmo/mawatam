# datam-sim

`datam-sim` is a simulator for the datam. It is written in C++ and uses the SFML as graphic library.

## Dependencies

We use the SFML, a powerful graphics, game and media library.

We use conan to manage our dependencies, in particular SDL: [https://docs.conan.io/en/latest/getting_started.html](https://docs.conan.io/en/latest/getting_started.html).

You can install conan using python (>3):

```bash
python3 -m pip install conan
export PATH="${HOME}/.local/bin":$PATH
```

SFML is not available on the standard conan repository, you have to add the `bincrafters` repository:

```bash
conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
conan config set general.revisions_enabled=1
```

## Build

```bash
git clone
cd datam-sim
mkdir build
conan install ..
cmake ..
make
(cd bin && ./bin/datam-sim <configuration_file.txt>)
```

## Input format

Here is an example configuration file:

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
