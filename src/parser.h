#pragma once

#include "world.h"

struct ParseNullGlue : public std::exception {
  const char* what() const throw() { return "The null glue was parsed"; }
};

struct InvalidListRepr : public std::exception {
 private:
  std::string msg;

 public:
  explicit InvalidListRepr(const std::string& repr)
      : msg(std::string("List representation " + repr + " is not valid")) {}

  const char* what() const noexcept override { return msg.c_str(); }
};

class Parser {
 public:
  Parser(World& world);

  void load_configuration_file(const std::string& p_configuration_file_path);
  void parse_configuration_file(
      const std::string& p_configuration_file_content);

 private:
  std::string configuration_file_path;
  std::string configuration_file_content;

  void parse_configuration_file_world(Yaml::Node& root);

  void parse_configuration_file_world_section_glues(Yaml::Node& root);
  std::set<std::string> glue_alphabet_names;
  std::map<std::string, Glue> all_glues;

  void parse_configuration_file_world_section_tileset_tile_types(
      Yaml::Node& root);
  std::map<std::string, TileType> all_tile_types;

  World& world;
};