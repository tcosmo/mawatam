#include "parser.h"

#include <filesystem>
#include <fstream>

#include "yaml/Yaml.hpp"

#define DEBUG_PARSER_LOG CLOG_IF(DEBUG_PARSER, DEBUG, "parser")
#define PARSER_LOG(LVL) CLOG(LVL, "parser")

/* Our format keywords */
const std::string KW_GLUE_ALPHA_COLOR = "glue_alphabets_color";
const std::string KW_GLUES = "glues";
const std::string KW_TILESET_TILE_TYPES = "tileset_tile_types";
const std::string KW_INPUT = "input";
const std::string KW_NULL = "null";

const std::array SECTION_KW = {KW_GLUE_ALPHA_COLOR, KW_GLUES,
                               KW_TILESET_TILE_TYPES, KW_INPUT};

Parser::Parser(World& world) : world(world) {
  // Register the null glue as a glue
  all_glues[NULL_GLUE.name()] = NULL_GLUE;
}

void Parser::load_configuration_file(
    const std::string& p_configuration_file_path) {
  configuration_file_path = p_configuration_file_path;
  std::ifstream ifs(configuration_file_path);

  if (!ifs) {
    PARSER_LOG(FATAL)
        << "File " << configuration_file_path
        << " was not found. Current working directory is: `"
        << std::filesystem::current_path() << "`. "
        << "You might want to move or make a symbolic link of the file "
           "here";
  }
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  PARSER_LOG(INFO) << "Input file: " << configuration_file_path;
  parse_configuration_file(content);
}

GlueName GlueName::parse(const std::string& repr) {
  std::regex glue_name_regex = std::regex(GlueName::glue_name_format);
  GlueName to_return;
  std::smatch m;

  if (std::regex_search(repr, m, glue_name_regex)) {
    if (repr == KW_NULL) {
      throw ParseNullGlue();
    }
    assert(m.size() >= 3);
    if (m.size() > 3)
      LOG(WARNING) << "Regex matcher gets more than 3 groups, thats weird...";
    to_return.alphabet_name = m[1].str();
    to_return._char = m[2].str()[0];
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
  } catch (ParseNullGlue e) {
    throw std::invalid_argument(
        "Cannot use `null` glue name when defining tileset glues. It is "
        "reserved for specifying null input glues.");
  }

  catch (std::invalid_argument e) {
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

  return to_return;
}

TileType TileType::parse(const std::pair<const std::string&, Yaml::Node&>
                             tile_type_name_and_square_glues,
                         const std::map<std::string, Glue>& all_glues) {
  // std::regex tile_type_name = std::regex(GlueName::glue_name_format);
  // GlueName to_return;
  // std::smatch m;

  // if (std::regex_search(repr, m, glue_name_regex)) {
  //   if (repr == KW_NULL) {
  //     throw ParseNullGlue();
  //   }
  //   assert(m.size() >= 3);
  //   if (m.size() > 3)
  //     LOG(WARNING) << "Regex matcher gets more than 3 groups, thats
  //     weird...";
  //   to_return.alphabet_name = m[1];
  // }
  return TileType();
}

void Parser::parse_configuration_file_world_section_glues(Yaml::Node& root) {
  Yaml::Node node_glues = root[KW_GLUES];
  for (auto it = node_glues.Begin(); it != node_glues.End(); it++) {
    try {
      Glue glue = Glue::parse((*it));
      glue_alphabet_names.insert(glue.name.alphabet_name);
      DEBUG_PARSER_LOG << "Successfully parsed glue: `" << glue << "`";
      all_glues[glue.name()] = std::move(glue);
    } catch (const std::invalid_argument e) {
      PARSER_LOG(FATAL) << e.what();
    }
  }
}

std::vector<TileType>
Parser::parse_configuration_file_world_section_tileset_tile_types(
    Yaml::Node& root) {
  std::vector<TileType> to_return;
  Yaml::Node node_tileset_tile_types = root[KW_GLUES];
  for (auto it = node_tileset_tile_types.Begin();
       it != node_tileset_tile_types.End(); it++) {
    try {
      to_return.push_back(TileType::parse((*it), all_glues));
      DEBUG_PARSER_LOG << "Successfully parsed tile type: `"
                       << to_return[to_return.size() - 1] << "`";
    } catch (const std::invalid_argument e) {
      PARSER_LOG(FATAL) << e.what();
    }
  }

  return to_return;
}

void Parser::parse_configuration_file_world(Yaml::Node& root) {
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

  // Section `glues`
  parse_configuration_file_world_section_glues(root);

  // Section `tileset_tile_types`
  // std::vector<TileType> tileset_tile_types =
  //     parse_configuration_file_world_section_tileset_tile_types(root);
}

void Parser::parse_configuration_file(
    const std::string& p_configuration_file_content) {
  configuration_file_content = p_configuration_file_content;
  PARSER_LOG(INFO) << "Parsing...";

  Yaml::Node root;

  try {
    Yaml::Parse(root, configuration_file_content);
  } catch (const Yaml::Exception e) {
    PARSER_LOG(FATAL) << "Exception " << e.Type() << ": " << e.what();
  }

  parse_configuration_file_world(root);

  // Section glue alphabets color
  // for (auto itN = root[KW_GLUE_ALPHA_COLOR].Begin();
  //      itN != root[KW_GLUE_ALPHA_COLOR].End(); itN++) {
  //   DEBUG_PARSER_LOG << (*itN).first;
  // }
  PARSER_LOG(INFO) << "Parsing successful!";
}