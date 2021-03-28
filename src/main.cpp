
#include "parser.h"
#include "simulator.h"

INITIALIZE_EASYLOGGINGPP

void init_logger() {
  el::Configurations c;
  c.setToDefault();
  el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
  c.parseFromText(LOGGER_CONF);
  el::Loggers::getLogger(LOGGER_PARSER);
  el::Loggers::getLogger(LOGGER_NOT_IMPLEM);
  el::Loggers::getLogger(LOGGER_VIEW);
  el::Loggers::reconfigureAllLoggers(c);
  LOG(INFO) << "Welcome to datam!";
}

int main(int argc, char** argv) {
  init_logger();

  World world;
  WorldView world_view(world);

  Parser parser(world, world_view);
  parser.load_configuration_file("examples/Collatz_configuration.yml");

  world.set_view_watcher(&world_view.get_view_watcher());
  world_view.update();

  Simulator sim(world, world_view, 1200, 800);
  sim.run();

  LOG(INFO) << "Goodbye, be well!";
}