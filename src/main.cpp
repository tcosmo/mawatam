#include "cxxopts.hpp"
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
  LOG(INFO) << "Welcome to mawatam!";
}

int main(int argc, char** argv) {
  init_logger();

  cxxopts::Options options("mawatam", "Disconnected-seed aTAM simulator");

  options.add_options()("i,input", "Reads input from stdin")(
      "f,file", "Reads input from file given as arg",
      cxxopts::value<std::string>())("h,help", "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

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

  if (result["input"].as<bool>()) {
    // Reading input from stdin
    std::string line;
    std::string configuration_file_content = "";
    while (std::getline(std::cin, line)) {
      configuration_file_content += line + "\n";
    }
    parser.parse_configuration_file_content(configuration_file_content);
  } else {
    // Reading input from file
    try {
      std::string file_name = result["file"].as<std::string>();
      parser.load_configuration_file(result["file"].as<std::string>());
    } catch (cxxopts::option_has_no_value_exception) {
      LOG(INFO)
          << "No input was given, call `./mawatam -i` to stream input through "
             "stdin or `./mawatam -f <file_path>` to read input from a file";
      std::cout << options.help() << std::endl;
    }
  }

  world.set_view_watcher(&world_view.get_view_watcher());
  world_view.update();

  Simulator sim(parser, world, world_view, 1200, 800);
  sim.run();

  LOG(INFO) << "Goodbye, be well!";
}
