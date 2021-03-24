#include <filesystem>
#include <fstream>

#include "world.h"

#define DEBUG_PARSER_LOG CLOG_IF(DEBUG_PARSER, DEBUG, "parser")
#define PARSER_LOG(LVL) CLOG(LVL, "parser")

const std::string WHITESPACE = " \n\r\t\f\v";
const char LINE_DELIMITER = '\n';
const char COMMENT_TOKEN = ';';
const std::string KW_GLUES = "glues";
const std::string KW_TILE_TYPES = "tile_types";
const std::string KW_INPUT = "input";

std::string ltrim(const std::string& s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

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

std::string get_enclosed_content(const std::string& container,
                                 const std::string& encloser = "\"") {
  std::string container_ltrim = ltrim(container);

  if (container_ltrim.size() == 0 || container_ltrim.find(encloser) != 0)
    PARSER_LOG(FATAL) << "Content " << container << " is not enclosed is `"
                      << encloser << "`";

  container_ltrim.erase(0, encloser.size());

  size_t pos_delim = container_ltrim.find(encloser);
  if (pos_delim == std::string::npos)
    PARSER_LOG(FATAL) << "Content " << container << " is not enclosed is `"
                      << encloser << "`";

  return container_ltrim.substr(0, pos_delim);
}

Glue parse_glue(const std::string& line_ltrim,
                std::map<std::string, Glue>& glue_map) {
  size_t pos_delim = line_ltrim.find(":");

  if (pos_delim == std::string::npos)
    PARSER_LOG(FATAL) << "Invalid glue specification: `" << line_ltrim << "`";

  int glue_strength = 0;

  try {
    glue_strength =
        std::stoi(line_ltrim.substr(pos_delim + 1, line_ltrim.length()));
  } catch (std::invalid_argument) {
    PARSER_LOG(FATAL) << "Invalid glue strength: `"
                      << line_ltrim.substr(pos_delim + 1, line_ltrim.length())
                      << "`";
  }

  std::string token_glue_name = line_ltrim.substr(0, pos_delim);
  std::string glue_name = get_enclosed_content(token_glue_name);

  if (glue_name == "") PARSER_LOG(FATAL) << "Glue name cannot be empty!";

  if (glue_map.find(glue_name) != glue_map.end())
    PARSER_LOG(FATAL) << "Glue name \"" << glue_name << "\" is not unique!";

  Glue glue(glue_name, glue_strength);

  glue_map[glue_name] = glue;

  return glue;
}

std::array<Glue, 4> parse_glue_spec(
    std::string glue_spec, const std::map<std::string, Glue>& glue_map) {
  size_t pos_delim;
  size_t curr_side = 0;
  std::array<Glue, 4> glues;

  std::string glue_spec_original = glue_spec;

  while ((pos_delim = glue_spec.find(";")) != std::string::npos) {
    if (curr_side == 4)
      PARSER_LOG(FATAL) << "More than 4 sides are specified in `"
                        << glue_spec_original << "`";

    std::string glue_name_token = glue_spec.substr(0, pos_delim);
    glue_spec.erase(0, pos_delim + 1);

    // Check for null glue
    std::string glue_name_token_ltrim = ltrim(glue_name_token);
    if (glue_name_token_ltrim.length() != 0 &&
        glue_name_token_ltrim.find("null") == 0) {
      glues[curr_side] = NULL_GLUE;
    } else {
      std::string glue_name = get_enclosed_content(glue_name_token);

      if (glue_map.find(glue_name) == glue_map.end())
        PARSER_LOG(FATAL) << "Glue \"" << glue_name
                          << "\" is used but does not exist";

      glues[curr_side] = glue_map.at(glue_name);
    }

    curr_side += 1;
  }

  if (curr_side != 4)
    PARSER_LOG(FATAL) << "Not enough glues are specified in "
                      << glue_spec_original;

  return glues;
}

TileType parse_tile_types(const std::string& line_ltrim,
                          const std::map<std::string, Glue>& glue_map,
                          std::map<std::string, bool>& tile_type_names) {
  size_t pos_delim = line_ltrim.find(":");

  if (pos_delim == std::string::npos)
    PARSER_LOG(FATAL) << "Invalid tile type specification: `" << line_ltrim
                      << "`";

  TileType tile_type;

  tile_type.name = get_enclosed_content(line_ltrim.substr(0, pos_delim));

  if (tile_type.name == "")
    PARSER_LOG(FATAL) << "Tile type name cannot be empty!";

  if (tile_type_names.find(tile_type.name) != tile_type_names.end())
    PARSER_LOG(FATAL) << " Tile type name \"" << tile_type.name
                      << "\" is not unique";

  std::string glue_spec = line_ltrim.substr(pos_delim + 1, line_ltrim.size());

  tile_type.glues = parse_glue_spec(glue_spec, glue_map);

  return tile_type;
}

sf::Vector2i parse_coords(const std::string& spec) {
  size_t pos_delim = spec.find(",");

  if (pos_delim == std::string::npos)
    PARSER_LOG(FATAL) << "Wrong coordinates specifications: `" << spec << "`";

  sf::Vector2i to_ret;

  try {
    to_ret.x = std::stoi(spec.substr(0, pos_delim));
    to_ret.y = std::stoi(spec.substr(pos_delim + 1, spec.size()));
  } catch (std::invalid_argument) {
    PARSER_LOG(FATAL) << "Wrong coordinates specifications: `" << spec << "`";
  }

  return to_ret;
}

void World::from_file_content(const std::string& file_content) {
  PARSER_LOG(INFO) << "Parsing...";
  std::string file_content_copy = file_content;
  size_t pos = 0;
  std::string token;

  bool has_scanned_glues = false;
  bool scanning_glues = false;

  bool has_scanned_tile_types = false;
  bool scanning_tile_types = false;

  bool has_scanned_input = false;
  bool scanning_input = false;

  std::vector<Glue> glues;
  std::map<std::string, Glue> glue_map;

  std::vector<TileType> tile_types;
  std::map<std::string, bool> tile_type_names;

  std::map<std::array<Glue, 4>, TileType*> extra_tile_type_glues;

  while ((pos = file_content_copy.find(LINE_DELIMITER)) != std::string::npos) {
    token = file_content_copy.substr(0, pos);
    file_content_copy.erase(0, pos + 1);

    // Treating case where file does not end with \n
    if (file_content_copy.size() != 0 &&
        file_content_copy.find(LINE_DELIMITER) == std::string::npos) {
      file_content_copy.append("\n");
    }

    std::string line_ltrim = ltrim(token);

    if (line_ltrim.size() == 0 || line_ltrim[0] == COMMENT_TOKEN) continue;

    if (line_ltrim.find(KW_GLUES) != std::string::npos) {
      if (has_scanned_glues || scanning_glues)
        PARSER_LOG(FATAL) << "Only one `glues` section is allowed";

      scanning_glues = true;
      continue;
    }

    if (line_ltrim.find(KW_TILE_TYPES) != std::string::npos) {
      if (!scanning_glues)
        PARSER_LOG(FATAL) << "The `tile_types` section must be right after the "
                             "`glues` section";

      if (has_scanned_tile_types || scanning_tile_types)
        PARSER_LOG(FATAL) << "Only one `tile_types` section is allowed";

      scanning_glues = false;
      has_scanned_glues = true;
      scanning_tile_types = true;
      continue;
    }

    if (line_ltrim.find(KW_INPUT) != std::string::npos) {
      if (!scanning_tile_types)
        PARSER_LOG(FATAL) << "The `input` section must be right after the "
                             "`tile_types` section";

      if (has_scanned_input || scanning_input)
        PARSER_LOG(FATAL) << "Only one `input` section is allowed";

      scanning_tile_types = false;
      has_scanned_tile_types = true;
      scanning_input = true;
      continue;
    }

    if (scanning_glues) {
      glues.push_back(parse_glue(line_ltrim, glue_map));
      // DEBUG_PARSER_LOG << "Found glue: " << glues[glues.size() - 1];
    }

    if (scanning_tile_types) {
      tile_types.push_back(
          parse_tile_types(line_ltrim, glue_map, tile_type_names));

      // DEBUG_PARSER_LOG << "Found tile type: "
      //                << tile_types[tile_types.size() - 1];
    }

    if (scanning_input) {
      size_t pos_delim = line_ltrim.find(":");

      if (pos_delim == std::string::npos)
        PARSER_LOG(FATAL) << "Bad input specification: " << line_ltrim;

      sf::Vector2i coords = parse_coords(line_ltrim.substr(0, pos_delim));
      TileType extra_tile_type;

      extra_tile_type.name =
          "extra_tile_type_" + std::to_string(extra_tile_type_glues.size());

      /* TODO: treat case where glue spec is a tile name. */
      std::string token_glue_spec =
          line_ltrim.substr(pos_delim + 1, line_ltrim.size());

      if (token_glue_spec.find(";") == std::string::npos)
        NOT_IMPLEM_LOG << "specifying inputs with tile types name, which "
                          "is what I suppose your tried to do with `"
                       << token_glue_spec << "` is not implemented yet";

      extra_tile_type.in_tileset = false;

      extra_tile_type.glues = parse_glue_spec(token_glue_spec, glue_map);

      TileType* to_point = nullptr;

      if (extra_tile_type_glues.find(extra_tile_type.glues) !=
          extra_tile_type_glues.end()) {
        to_point = extra_tile_type_glues[extra_tile_type.glues];
      } else {
        input_specific_tile_types.push_back(extra_tile_type);
        to_point =
            &input_specific_tile_types[input_specific_tile_types.size() - 1];
        extra_tile_type_glues[extra_tile_type.glues] = to_point;
      }

      tiles[coords] = to_point;
      DEBUG_PARSER_LOG << "Found square at pos " << coords;
    }
  }

  tileset.tile_types = std::move(tile_types);
  init_potential_tiles_pos();
  PARSER_LOG(INFO) << "Parsing successful!";
}