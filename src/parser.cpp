#include <filesystem>
#include <fstream>

#include "world.h"
#include "yaml/Yaml.hpp"

#define DEBUG_PARSER_LOG CLOG_IF(DEBUG_PARSER, DEBUG, "parser")
#define PARSER_LOG(LVL) CLOG(LVL, "parser")

/* Our format keywords */
const std::string KW_GLUE_ALPHA_COLOR = "glue_alphabets_color";
const std::string KW_GLUES = "glues";
const std::string KW_TILESET_TILE_TYPES = "tileset_tile_types";
const std::string KW_INPUT = "input";

const std::array SECTION_KW = {KW_GLUE_ALPHA_COLOR, KW_GLUES,
                               KW_TILESET_TILE_TYPES, KW_INPUT};

void World::from_file(std::string file_path) {
  std::ifstream ifs(file_path);

  if (!ifs) {
    PARSER_LOG(FATAL)
        << "File " << file_path
        << " was not found. Current working directory is: `"
        << std::filesystem::current_path() << "`. "
        << "You might want to move or make a symbolic link of the file "
           "here";
  }
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  PARSER_LOG(INFO) << "Input file: " << file_path;
  from_file_content(content);
}

GlueName GlueName::parse(const std::string& repr) {
  std::regex glue_name_regex = std::regex(GlueName::glue_name_format);
  GlueName to_return;
  std::smatch m;

  if (std::regex_search(repr, m, glue_name_regex)) {
    assert(m.size() >= 3);
    if (m.size() > 3)
      LOG(WARNING) << "Regex matcher gets more than 3 groups, thats weird...";
    to_return.alphabet_name = m[1].str();
    to_return.name = m[2].str()[0];
  } else {
    throw std::invalid_argument(Formatter()
                                << "Representation `" << repr
                                << "` does not match glue name format: `"
                                << GlueName::glue_name_format + "`");
  }
  return to_return;
}

Glue Glue::parse(const std::pair<const std::string&, Yaml::Node&> key_value) {
  Glue to_return;
  try {
    to_return.name = GlueName::parse(key_value.first);
  } catch (std::invalid_argument e) {
    throw;
  }

  try {
    std::string strength_string = key_value.second.As<std::string>();
    to_return.strength = std::stoi(strength_string);

  } catch (Yaml::Exception e) {
    throw std::invalid_argument(Formatter() << "Could not parse glue strength: "
                                            << e.what());
  } catch (std::exception e) {
    std::string strength_string = key_value.second.As<std::string>();
    throw std::invalid_argument(
        Formatter() << "Could not parse integer glue strength `"
                    << strength_string << "` because of: " << e.what());
  }

  DEBUG_PARSER_LOG << "Successfully parsed glue: `" << to_return << "`";

  return to_return;
}

void World::from_file_content(const std::string& file_content) {
  PARSER_LOG(INFO) << "Parsing...";

  Yaml::Node root;

  try {
    Yaml::Parse(root, file_content);
  } catch (const Yaml::Exception e) {
    PARSER_LOG(FATAL) << "Exception " << e.Type() << ": " << e.what();
  }

  // Checking that all sections are here
  for (const std::string& kw : SECTION_KW) {
    if (root[kw].IsNone()) {
      if (kw == KW_GLUE_ALPHA_COLOR) continue;
      if (kw == KW_INPUT) {
        PARSER_LOG(WARNING)
            << "No input was given, add an `input` section to your input file";
      } else {
        PARSER_LOG(FATAL) << "Mandatory section `" << kw
                          << "` is missing from input file";
      }
    }
  }

  // Section glues
  std::set<std::string> all_glue_alphabet_names;
  std::set<std::string> all_glue_names;
  std::map<std::string, Glue> all_glues;

  Yaml::Node node_glues = root[KW_GLUES];
  for (auto it = node_glues.Begin(); it != node_glues.End(); it++) {
    Glue glue;

    try {
      glue = Glue::parse((*it));
    } catch (const std::invalid_argument e) {
      PARSER_LOG(FATAL) << e.what();
    }
  }

  // Section glue alphabets color
  for (auto itN = root[KW_GLUE_ALPHA_COLOR].Begin();
       itN != root[KW_GLUE_ALPHA_COLOR].End(); itN++) {
    DEBUG_PARSER_LOG << (*itN).first;
  }
  PARSER_LOG(INFO) << "Parsing successful!";
}