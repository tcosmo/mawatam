#include <filesystem>
#include <fstream>

#include "world.h"

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
    printf(
        "File `%s` was not found.\nCurrent working directory is:\n"
        "`%s`.\n"
        "You might want to move or make a symbolic link of the file "
        "here.\nAbort.",
        file_path.c_str(), std::filesystem::current_path().c_str());
    exit(-1);
  }
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  printf("Parsing `%s`...\n", file_path.c_str());
  from_file_content(content);
  printf("Parsing done!");
}

std::string get_enclosed_content(const std::string& container,
                                 const std::string& encloser = "\"") {
  std::string container_ltrim = ltrim(container);

  if (container_ltrim.size() == 0 || container_ltrim.find(encloser) != 0) {
    printf("[parsing error] Content `%s` is not enclosed in `%s`. Abort.\n",
           container.c_str(), encloser.c_str());
    exit(-1);
  }

  container_ltrim.erase(0, encloser.size());

  size_t pos_delim = container_ltrim.find(encloser);
  if (pos_delim == std::string::npos) {
    printf("[parsing error] Content `%s` is not enclosed in `%s`. Abort.\n",
           container.c_str(), encloser.c_str());
    exit(-1);
  }

  return container_ltrim.substr(0, pos_delim);
}

Glue parse_glue(const std::string& line_ltrim,
                std::map<std::string, Glue>& glue_map) {
  size_t pos_delim = line_ltrim.find(":");

  if (pos_delim == std::string::npos) {
    printf("[parsing error] Invalid glue specification: `%s`. Abort.",
           line_ltrim.c_str());
    exit(-1);
  }

  int glue_strength = 0;

  try {
    glue_strength =
        std::stoi(line_ltrim.substr(pos_delim + 1, line_ltrim.length()));
  } catch (std::invalid_argument) {
    printf("[parsing error] Invalid glue strength: `%s`. Abort.",
           line_ltrim.substr(pos_delim + 1, line_ltrim.length()).c_str());
    exit(-1);
  }

  std::string token_glue_name = line_ltrim.substr(0, pos_delim);
  std::string glue_name = get_enclosed_content(token_glue_name);

  if (glue_name == "") {
    printf("[parsing error] Glue name cannot be empty! Abort.\n");
    exit(-1);
  }

  if (glue_map.find(glue_name) != glue_map.end()) {
    printf("[parsing error] Glue name \"%s\" is not unique. Abort.",
           glue_name.c_str());
    exit(-1);
  }

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
    if (curr_side == 4) {
      printf(
          "[parsing error] More than 4 sides are specified in`%s`. "
          "Abort. \n",
          glue_spec_original.c_str());
      exit(-1);
    }

    std::string glue_name_token = glue_spec.substr(0, pos_delim);
    glue_spec.erase(0, pos_delim + 1);

    // Check for null glue
    std::string glue_name_token_ltrim = ltrim(glue_name_token);
    if (glue_name_token_ltrim.length() != 0 &&
        glue_name_token_ltrim.find("null") == 0) {
      glues[curr_side] = NULL_GLUE;
    } else {
      std::string glue_name = get_enclosed_content(glue_name_token);

      if (glue_map.find(glue_name) == glue_map.end()) {
        printf(
            "[parsing error] Glue `%s` is used but does not "
            "exist. Abort.\n",
            glue_name.c_str());
        exit(-1);
      }

      glues[curr_side] = glue_map.at(glue_name);
    }

    curr_side += 1;
  }

  if (curr_side != 4) {
    printf("[parsing error] Not enough glues are specified in `%s`. Abort.\n",
           glue_spec_original.c_str());
    exit(-1);
  }

  return glues;
}

TileType parse_tile_types(const std::string& line_ltrim,
                          const std::map<std::string, Glue>& glue_map,
                          std::map<std::string, bool>& tile_type_names) {
  size_t pos_delim = line_ltrim.find(":");

  if (pos_delim == std::string::npos) {
    printf("[parsing error] Invalid tile type specification: `%s`. Abort.",
           line_ltrim.c_str());
    exit(-1);
  }

  TileType tile_type;

  tile_type.name = get_enclosed_content(line_ltrim.substr(0, pos_delim));

  if (tile_type.name == "") {
    printf("[parsing error] Tile type name cannot be empty! Abort.\n");
    exit(-1);
  }

  if (tile_type_names.find(tile_type.name) != tile_type_names.end()) {
    printf("[parsing error] Tile type name \"%s\" is not unique. Abort.",
           tile_type.name.c_str());
    exit(-1);
  }

  std::string glue_spec = line_ltrim.substr(pos_delim + 1, line_ltrim.size());

  tile_type.glues = parse_glue_spec(glue_spec, glue_map);

  return tile_type;
}

sf::Vector2i parse_coords(const std::string& spec) {
  size_t pos_delim = spec.find(",");

  if (pos_delim == std::string::npos) {
    printf("[parsing error] Wrong coordinates specification: `%s`. Abort.\n",
           spec.c_str());
    exit(-1);
  }

  sf::Vector2i to_ret;

  try {
    to_ret.x = std::stoi(spec.substr(0, pos_delim));
    to_ret.y = std::stoi(spec.substr(pos_delim + 1, spec.size()));
  } catch (std::invalid_argument) {
    printf("[parsing error] Wrong coordinates specification: `%s`. Abort.\n",
           spec.c_str());
    exit(-1);
  }

  return to_ret;
}

void World::from_file_content(const std::string& file_content) {
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
      if (has_scanned_glues || scanning_glues) {
        printf("[parsing error] Only one `glues` section is allowed. Abort.\n");
        exit(-1);
      }
      scanning_glues = true;
      continue;
    }

    if (line_ltrim.find(KW_TILE_TYPES) != std::string::npos) {
      if (!scanning_glues) {
        printf(
            "[parsing error] The `tile_types` section must be right after "
            "the "
            "`glues` section. Abort.\n");
        exit(-1);
      }

      if (has_scanned_tile_types || scanning_tile_types) {
        printf(
            "[parsing error] Only one `tile_types` section is allowed. "
            "Abort.\n");
        exit(-1);
      }

      scanning_glues = false;
      has_scanned_glues = true;
      scanning_tile_types = true;
      continue;
    }

    if (line_ltrim.find(KW_INPUT) != std::string::npos) {
      if (!scanning_tile_types) {
        printf(
            "[parsing error] The `input` section must be right after the "
            "`tile_types` section. Abort.\n");
        exit(-1);
      }

      if (has_scanned_input || scanning_input) {
        printf(
            "[parsing error] Only one `input` section is allowed. "
            "Abort.\n");
        exit(-1);
      }

      scanning_tile_types = false;
      has_scanned_tile_types = true;
      scanning_input = true;
      continue;
    }

    if (scanning_glues) {
      glues.push_back(parse_glue(line_ltrim, glue_map));
      if (DEBUG)
        printf("[parser] Found glue! name: \"%s\", strength: %d\n",
               glues[glues.size() - 1].name.c_str(),
               glues[glues.size() - 1].strength);
    }

    if (scanning_tile_types) {
      tile_types.push_back(
          parse_tile_types(line_ltrim, glue_map, tile_type_names));
      if (DEBUG) {
        printf("[parser] Found tile type! name: \"%s\", glues:",
               tile_types[tile_types.size() - 1].name.c_str());
        for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
          printf(" \"%s\"",
                 tile_types[tile_types.size() - 1].glues[i_dir].name.c_str());
        }
        printf("\n");
      }
    }

    if (scanning_input) {
      size_t pos_delim = line_ltrim.find(":");

      if (pos_delim == std::string::npos) {
        printf("[parsing error] Bad input specification: `%s`. Abort.\n",
               line_ltrim.c_str());
        exit(-1);
      }

      sf::Vector2i coords = parse_coords(line_ltrim.substr(0, pos_delim));
      TileType extra_tile_type;

      extra_tile_type.name =
          "extra_tile_type_" + std::to_string(extra_tile_type_glues.size());

      /* TODO: treat case where glue spec is a tile name. */
      std::string token_glue_spec =
          line_ltrim.substr(pos_delim + 1, line_ltrim.size());

      if (token_glue_spec.find(";") == std::string::npos) {
        printf(
            "[not implemented] specifying inputs with tile types name, which "
            "is what I suppose your tried to do with `%s` is not "
            "implemented yet.",
            token_glue_spec.c_str());
        exit(-1);
      }

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
      printf("[parser] Found square at pos (%d, %d)!\n", coords.x, coords.y);
    }
  }

  tileset.tile_types = std::move(tile_types);
}