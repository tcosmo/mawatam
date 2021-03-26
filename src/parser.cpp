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

void check_section_is_map(Yaml::Node& section_node,
                          const std::string& section_kw) {
  if (!section_node.IsMap())
    PARSER_LOG(FATAL) << "Section `" << section_kw << "` must be a map";
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
  static std::regex glue_name_regex = std::regex(GlueName::glue_name_format);
  GlueName to_return;
  std::smatch m;

  if (std::regex_match(repr, m, glue_name_regex)) {
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
        "`null` glue cannot be redefined, it is already given by the model "
        "(strength 0)");
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

void Parser::parse_configuration_file_world_section_glues(Yaml::Node& root) {
  Yaml::Node node_glues = root[KW_GLUES];

  check_section_is_map(node_glues, KW_GLUES);

  for (auto it = node_glues.Begin(); it != node_glues.End(); it++) {
    try {
      Glue glue = Glue::parse((*it));
      glue_alphabet_names.insert(glue.name.alphabet_name);
      DEBUG_PARSER_LOG << "Successfully parsed glue: `" << glue << "`";

      // No need to check for glue.name() unicity: yaml parser
      // will only pick take one version of redundant keys
      all_glues[glue.name()] = std::move(glue);
    } catch (const std::invalid_argument e) {
      PARSER_LOG(FATAL) << e.what();
    }
  }
}

std::vector<std::string> parse_python_style_lists(const std::string& repr) {
  std::string striped_repr = strip(repr);
  if (striped_repr.size() == 0) throw InvalidListRepr(repr);
  if (striped_repr[0] != '[') throw InvalidListRepr(repr);

  size_t pos_end = striped_repr.find(']');
  if (pos_end == std::string::npos) throw InvalidListRepr(repr);
  if (pos_end + 1 != striped_repr.size()) throw InvalidListRepr(repr);

  striped_repr.erase(0, 1);
  striped_repr.erase(striped_repr.size() - 1);

  std::vector<std::string> to_return;

  size_t pos_delim = striped_repr.find(',');

  while ((pos_delim = striped_repr.find(',')) != std::string::npos) {
    std::string token = strip(striped_repr.substr(0, pos_delim));
    to_return.push_back(token);
    striped_repr.erase(0, pos_delim + 1);
  }

  std::string last_token = strip(striped_repr);
  if (last_token.size() != 0) to_return.push_back(last_token);

  return to_return;
}

SquareGlues SquareGlues::parse(Yaml::Node& node_square_glues,
                               const std::map<std::string, Glue>& all_glues) {
  std::vector<std::string> parsed_sequence;

  if (node_square_glues.IsSequence()) {
    for (auto it = node_square_glues.Begin(); it != node_square_glues.End();
         it++) {
      parsed_sequence.push_back(strip((*it).second.As<std::string>()));
    }
  } else {
    /* There is a bug in the yaml library: they interpret [bin.0, ter.1, bin.1,
        ter.0] as a string and not a sequence...
        Hence we have to do our own parsing if inline sequences are given
    */
    parsed_sequence =
        parse_python_style_lists(node_square_glues.As<std::string>());
  }

  if (parsed_sequence.size() != 4) {
    PARSER_LOG(FATAL)
        << "The glues of a tile type must be an array with 4 components";
  }

  SquareGlues to_return;

  size_t i = 0;
  for (const std::string& glue_name : parsed_sequence) {
    if (all_glues.find(glue_name) == all_glues.end())
      PARSER_LOG(FATAL)
          << "The glue " << glue_name
          << " does not exist, make sure to register it before you use it";
    to_return[i] = all_glues.at(glue_name);
    i += 1;
  }

  return to_return;
}

TileType TileType::parse(const std::pair<const std::string&, Yaml::Node&>
                             tile_type_name_and_square_glues,
                         const std::map<std::string, Glue>& all_glues) {
  static std::regex tile_type_name_regex =
      std::regex(TileType::tile_type_name_format);
  TileType to_return;
  std::smatch m;

  if (std::regex_match(tile_type_name_and_square_glues.first, m,
                       tile_type_name_regex)) {
    assert(m.size() == 1);
    to_return.name = m[0].str()[0];
  } else {
    throw std::invalid_argument(Formatter()
                                << "Representation `"
                                << tile_type_name_and_square_glues.first
                                << "` does not match glue name format: `"
                                << TileType::tile_type_name_format + "`");
  }

  to_return.glues =
      SquareGlues::parse(tile_type_name_and_square_glues.second, all_glues);

  return to_return;
}

void Parser::parse_configuration_file_world_section_tileset_tile_types(
    Yaml::Node& root) {
  Yaml::Node node_tileset_tile_types = root[KW_TILESET_TILE_TYPES];

  check_section_is_map(node_tileset_tile_types, KW_TILESET_TILE_TYPES);

  for (auto it = node_tileset_tile_types.Begin();
       it != node_tileset_tile_types.End(); it++) {
    try {
      TileType tile_type = TileType::parse((*it), all_glues);

      if (all_tile_types.find(tile_type.glues.__str__()) ==
          all_tile_types.end()) {
        DEBUG_PARSER_LOG << "Successfully parsed tile type: `" << tile_type
                         << "`";
        all_tile_types[tile_type.glues.__str__()] = std::move(tile_type);
      } else {
        PARSER_LOG(WARNING)
            << "Two tile types have identical glues: " << tile_type;
      }

    } catch (const std::invalid_argument e) {
      PARSER_LOG(FATAL) << e.what();
    }
  }
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
  parse_configuration_file_world_section_tileset_tile_types(root);

  // Section `input`
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