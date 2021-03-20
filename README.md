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
(cd bin && ./bin/datam-sim)
```
