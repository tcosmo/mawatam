#include "simulator.h"

int main(int argc, char** argv) {
  World world;
  world.from_file("examples/Collatz_configuration.txt");

  WorldView world_view;

  Simulator sim(world,world_view,1350,900);
  sim.run();
}