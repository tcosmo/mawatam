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
  el::Loggers::getLogger(LOGGER_WORLD);
  el::Loggers::reconfigureAllLoggers(c);
  LOG(INFO) << "Welcome to datam!";
}

int main(int argc, char** argv) {
  init_logger();

  sf::Font default_font;
  // Only trick I found to easily check later on if font was loaded
  sf::Font* font_pointer = nullptr;
  if (!default_font.loadFromFile(DEFAULT_FONT)) {
    LOG(WARNING) << "Could not load default font " << DEFAULT_FONT;
    LOG(WARNING) << "Text will not be rendered";
  } else {
    font_pointer = &default_font;
  }
  World world;
  WorldView world_view(world, font_pointer);

  Parser parser(world, world_view);
  parser.load_configuration_file("examples/Collatz_configuration.yml");

  world.set_view_watcher(&world_view.get_view_watcher());
  world_view.update();

  Simulator sim(parser, world, world_view, 1200, 800);
  sim.run();

  LOG(INFO) << "Goodbye, be well!";
}