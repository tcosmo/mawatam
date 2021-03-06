#include "parser.h"

#include <filesystem>
#include <fstream>

#include "yaml/Yaml.hpp"

// Color wheels
#include "wheels.h"

#define DEBUG_PARSER_LOG CLOG_IF(DEBUG_PARSER, DEBUG, LOGGER_PARSER)
#define PARSER_LOG(LVL) CLOG(LVL, LOGGER_PARSER)

/* Our format keywords */
const std::string KW_GLUE_ALPHA_COLOR = "glue_alphabets_color";
const std::string KW_GLUES = "glues";
const std::string KW_TILESET_TILE_TYPES = "tileset_tile_types";
const std::string KW_CONFIGURATION = "configuration";
const std::string KW_TEMPERATURE = "temperature";
const std::string KW_NULL = "null";

const std::array SECTION_KW = {KW_GLUE_ALPHA_COLOR, KW_GLUES,
                               KW_TILESET_TILE_TYPES, KW_CONFIGURATION,
                               KW_TEMPERATURE};

Parser::Parser(World& world, WorldView& view) : world(world), view(view) {
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
  parse_configuration_file_content(content);
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
      glue_alphabet_char.insert(glue.name._char);
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

TileType TileType::parse(const std::pair<const std::string&, Yaml::Node&>&
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

TileType TileType::parse(Yaml::Node& node_square_glues,
                         const std::map<std::string, Glue>& all_glues) {
  TileType to_return;
  to_return.glues = SquareGlues::parse(node_square_glues, all_glues);

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
        all_tile_types_name[tile_type.name] = tile_type.glues.__str__();
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

sf::Vector2i parse_coordinates(const std::string& repr) {
  std::regex coordinates_regex("( *-* *[0-9]+) *,( *-* *[0-9]+ *)");
  std::smatch m;
  sf::Vector2i to_ret;
  if (std::regex_match(repr, m, coordinates_regex)) {
    assert(m.size() == 3);
    try {
      to_ret.x = std::stoi(m[1]);
      to_ret.y = std::stoi(m[2]);
    } catch (std::invalid_argument e) {
      throw std::invalid_argument(Formatter()
                                  << "Invalid coordinates representation: "
                                  << repr << " " << e.what());
    }
  } else {
    throw std::invalid_argument(
        Formatter() << "Invalid coordinates representation: " << repr);
  }
  return to_ret;
}

void Parser::parse_configuration_file_world_section_configuration(
    Yaml::Node& root) {
  Yaml::Node node_tileset_tile_types = root[KW_CONFIGURATION];
  check_section_is_map(node_tileset_tile_types, KW_CONFIGURATION);

  for (auto it = node_tileset_tile_types.Begin();
       it != node_tileset_tile_types.End(); it++) {
    try {
      sf::Vector2i coordinates = parse_coordinates((*it).first);
      TileType tile_type;
      if ((*it).second.As<std::string>().size() == 1) {
        char tile_name = (*it).second.As<std::string>()[0];
        if (all_tile_types_name.find(tile_name) == all_tile_types_name.end()) {
          PARSER_LOG(FATAL) << "Tileset tile type with name`" << tile_name
                            << "` does not exist";
        }
        tile_type = all_tile_types[all_tile_types_name[tile_name]];
      } else {
        tile_type = TileType::parse((*it).second, all_glues);
      }

      all_tile_types[tile_type.glues.__str__()] = tile_type;
      tmp_tile_map_repr[coordinates] = tile_type.glues.__str__();

      DEBUG_PARSER_LOG << "Successfully parsed configuration tile: "
                       << coordinates << " " << tile_type;

    } catch (std::invalid_argument e) {
      PARSER_LOG(FATAL) << e.what();
    }
  }

  int temperature = root[KW_TEMPERATURE].As<int>(2);
  PARSER_LOG(INFO) << "Growing at temperature " << temperature;
  world.set_temperature(temperature);

  set_world_tile_types_and_tiles();
}

void Parser::reset_world_to_initial_configuration() {
  set_world_tile_types_and_tiles();
}

void Parser::set_world_tile_types_and_tiles() {
  /* Converting the tile map from pointing to tile types repr to the actual tile
   * type and filling the world initial configuation. */
  std::vector<std::unique_ptr<TileType>> tile_types;
  std::map<sf::Vector2i, const TileType*, CompareSfVector2i> tiles;

  std::map<std::string, size_t> tile_types_to_their_unique_ptr_loc;
  size_t i = 0;
  for (const auto& name_and_tile_type : all_tile_types) {
    tile_types.push_back(std::make_unique<TileType>(name_and_tile_type.second));

    if (!name_and_tile_type.second.is_anonymous())
      DEBUG_PARSER_LOG << "Tile type " << name_and_tile_type.second.__str__()
                       << " belongs to the tileset";

    tile_types_to_their_unique_ptr_loc[name_and_tile_type.first] = i;
    i += 1;
  }

  for (const auto& coord_and_tile_type_repr : tmp_tile_map_repr) {
    size_t loc =
        tile_types_to_their_unique_ptr_loc[coord_and_tile_type_repr.second];
    tiles[coord_and_tile_type_repr.first] = tile_types[loc].get();
  }

  world.set_tile_types_and_set_tiles(std::move(tile_types), std::move(tiles));
}

void Parser::parse_configuration_file_world(Yaml::Node& root) {
  // Checking that all sections are here
  for (const std::string& kw : SECTION_KW) {
    if (root[kw].IsNone()) {
      if (kw == KW_GLUE_ALPHA_COLOR) continue;
      if (kw == KW_CONFIGURATION) {
        PARSER_LOG(WARNING) << "No configuration was set, add a "
                               "`configuration` section to your input file";
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

  // Section `configuration`
  parse_configuration_file_world_section_configuration(root);
}

void Parser::parse_configuration_file_view(Yaml::Node& root) {
  /* TODO: view parameters are hardcoded at the minute, they should be parsed.
   */

  std::map<char, sf::Color> tiles_colors;
  std::map<std::string, sf::Color> glue_alphabet_colors;
  std::map<char, sf::Color> glue_char_colors;

  size_t i_color = 0;

  for (const auto& tile_char_and_repr : all_tile_types_name) {
    tiles_colors[tile_char_and_repr.first] =
        get_color_wheel_color(i_color, COLOR_WHEEL_SECONDARY);
    i_color += 1;
  }

  for (const std::string& alpha_name : glue_alphabet_names) {
    glue_alphabet_colors[alpha_name] =
        get_color_wheel_color(i_color, COLOR_WHEEL_SECONDARY);
    i_color += 1;
  }

  i_color = 0;
  for (char glue_char : glue_alphabet_char) {
    glue_char_colors[glue_char] =
        get_color_wheel_color(i_color, COLOR_WHEEL_4, 0);
    i_color += 1;
  }

  view.set_tiles_colors(std::move(tiles_colors));
  view.set_glue_alphabet_colors(std::move(glue_alphabet_colors));
  view.set_glue_char_color(std::move(glue_char_colors));
}

void Parser::parse_configuration_file_content(
    const std::string& p_configuration_file_content) {
  configuration_file_content = p_configuration_file_content;

  DEBUG_PARSER_LOG << "File content: \n\n" << configuration_file_content;

  PARSER_LOG(INFO) << "Parsing...";

  Yaml::Node root;

  try {
    Yaml::Parse(root, configuration_file_content);
  } catch (const Yaml::Exception e) {
    PARSER_LOG(FATAL) << "Exception " << e.Type() << ": " << e.what();
  }

  parse_configuration_file_world(root);
  parse_configuration_file_view(root);

  // Section glue alphabets color
  // for (auto itN = root[KW_GLUE_ALPHA_COLOR].Begin();
  //      itN != root[KW_GLUE_ALPHA_COLOR].End(); itN++) {
  //   DEBUG_PARSER_LOG << (*itN).first;
  // }
  PARSER_LOG(INFO) << "Parsing successful!";
}

std::string pos_to_yaml_key(sf::Vector2i pos) {
  std::stringstream to_return;
  to_return << pos.x << "," << pos.y;
  return to_return.str();
}

void Parser::world_dump() {
  std::ofstream out_file;
  out_file.open("out.yml");
  out_file << world_dumps();
  out_file.close();
}

std::string Parser::world_dumps() {
  std::stringstream to_return;
  std::string yaml_spacer = "  ";
  to_return << KW_TEMPERATURE << ": " << world.get_temperature() << "\n\n";
  to_return << KW_GLUES << ": "
            << "\n";

  for (const auto& glue : all_glues)
    if (!(glue.second == NULL_GLUE))
      to_return << yaml_spacer << glue.second << "\n";
  to_return << "\n";

  to_return << KW_TILESET_TILE_TYPES << ": "
            << "\n";
  for (const auto& tile_type : all_tile_types)
    if (!tile_type.second.is_anonymous())
      to_return << yaml_spacer << tile_type.second << "\n";
  to_return << "\n";

  to_return << KW_CONFIGURATION << ": "
            << "\n";

  for (const auto& tile : world.get_tiles()) {
    if (tile.second)
      to_return << yaml_spacer << pos_to_yaml_key(tile.first) << ": "
                << tile.second->glues << "\n";
  }
  to_return << "\n";
  return to_return.str();
}
