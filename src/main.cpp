#include "world.h"

int main(int argc, char** argv) {
  World world;
  world.from_file("examples/Collatz_configuration.txt");
}