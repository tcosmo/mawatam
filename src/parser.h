#pragma once

#include "view.h"
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
  Parser(World& world, WorldView& view);

  void load_configuration_file(const std::string& p_configuration_file_path);
  void parse_configuration_file(
      const std::string& p_configuration_file_content);

 private:
  std::string configuration_file_path;
  std::string configuration_file_content;

  void parse_configuration_file_world(Yaml::Node& root);

  void parse_configuration_file_world_section_glues(Yaml::Node& root);
  std::set<std::string> glue_alphabet_names;
  std::set<char> glue_alphabet_char;
  std::map<std::string, Glue> all_glues;

  void parse_configuration_file_world_section_tileset_tile_types(
      Yaml::Node& root);
  std::map<std::string, TileType> all_tile_types;
  std::map<char, std::string> all_tile_types_name;

  void parse_configuration_file_world_section_configuration(Yaml::Node& root);

  World& world;

  void parse_configuration_file_view(Yaml::Node& root);
  WorldView& view;
};