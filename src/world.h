#pragma once

#include <array>
#include <map>
#include <regex>
#include <set>
#include <vector>

#include "utils.h"

// For parsing purpose
namespace Yaml {
class Node;
}

struct GlueName {
  std::string alphabet_name;
  char name;

  inline static const std::string glue_name_format =
      "([_a-zA-Z][_a-zA-Z0-9]*)\\.([a-zA-Z0-9])|null";

  GlueName() {}
  GlueName(const std::string& alphabet_name, char name)
      : alphabet_name(alphabet_name), name(name) {}

  static GlueName parse(const std::string& repr);

  std::string __str__() const {
    return alphabet_name + "." + std::string(1, name);
  }

  bool operator==(const GlueName& other) const {
    return alphabet_name == other.alphabet_name && name == other.name;
  }

  bool operator<(const GlueName& other) const {
    if (name == other.name) return alphabet_name < other.alphabet_name;
    return name < other.name;
  }

  friend std::ostream& operator<<(std::ostream& os, const GlueName& m) {
    return os << m.__str__();
  }
};

struct Glue {
 public:
  // The alphabet name allows us to easily display glues differently in the UI
  GlueName name;
  int strength;

  Glue() {}
  Glue(const GlueName& name, int strength) : name(name), strength(strength) {}

  static Glue parse(const std::pair<const std::string&, Yaml::Node&> key_value);

  bool operator==(const Glue& other) const {
    return name == other.name && strength == other.strength;
  }

  bool operator<(const Glue& other) const {
    if (name == other.name) return strength < other.strength;
    return name < other.name;
  }

  std::string __str__() const {
    return name.__str__() + ": " + std::to_string(strength);
  }

  friend std::ostream& operator<<(std::ostream& os, Glue const& m) {
    return os << m.__str__();
  }
};

// The NULL_GLUE is a default ('', '\0',0) glue which is used to specify
// the default side of polyominos squares input
static const Glue NULL_GLUE({{"", '\0'}, 0});

struct SquareGlues {
  std::array<Glue, 4> glues;

  SquareGlues() {}
  SquareGlues(const std::array<Glue, 4>& glues) : glues(glues) {}

  bool operator==(const SquareGlues& other) const {
    return glues == other.glues;
  }

  bool operator<(const SquareGlues& other) const { return glues < other.glues; }

  std::string __str__() const {
    return "[" + glues[0].__str__() + ", " + glues[1].__str__() + ", " +
           glues[2].__str__() + ", " + glues[3].__str__() + "]";
  }

  friend std::ostream& operator<<(std::ostream& os, SquareGlues const& m) {
    return os << m.__str__();
  }

  // A convenience for not having to type glues.glues all the time
  std::array<Glue, 4>& operator()() { return glues; }
};

struct TileType {
  // If tile type name is equal to '\0', tile type is anonymous
  // and considered as not being part of the tileset
  char name;
  SquareGlues glues;

  // C++17 feature
  inline static const std::regex name_format = std::regex("[a-zA-Z0-9]");

  TileType() {}
  TileType(const char& name, const SquareGlues& glues)
      : name(name), glues(glues){};

  bool operator==(const TileType& other) const {
    return name == other.name && glues == other.glues;
  }

  bool operator<(const TileType& other) const {
    if (name == other.name) return glues < other.glues;
    return name < other.name;
  }

  std::string __str__() const {
    return std::string(1, name) + ": " + glues.__str__();
  }

  friend std::ostream& operator<<(std::ostream& os, TileType const& m) {
    return os << m.__str__();
  }
};

// Allow the view to know what's new
typedef std::vector<std::pair<sf::Vector2i, TileType*>> ViewWatcher;

class World {
 public:
  /* In the datam, not all tile types belong to the tileset
     Some are only used to specify the input configuration
     They are "anoymous tile types". The only constraint is that
     They use the same glues as in the tileset or a default null glue of
     strength 0. */
  std::vector<TileType> tile_types;
  std::map<sf::Vector2i, TileType*, CompareSfVector2i> tiles;

  std::map<std::string, sf::Color> custom_glue_alphabets_colors;

  // Positions neighboring at least one tile
  std::set<sf::Vector2i, CompareSfVector2i> potential_tiles_pos;

  World();

  // void next();

  void from_file(std::string file_path);
  void from_file_content(const std::string& file_content);

  // std::vector<TileType*> tileset_query(
  //    std::vector<std::pair<size_t, Glue>> constraints);

  void set_view_watcher(ViewWatcher* watcher);

 private:
  void add_neighbors_to_potential_tile_pos(const sf::Vector2i& pos);
  void init_potential_tiles_pos();

  /* Returns a vector of (`i_dir`, `glue`) where `glue` is the glue sitting on
     side `i_dir` \in {0,1,2,3}. If there is no glue on a side, the pair wont be
     in the vector. */
  std::vector<std::pair<size_t, Glue>> get_glues_surrounding_potential_tile_pos(
      const sf::Vector2i& pos);

  ViewWatcher* view_watcher;

  std::vector<TileType*> tile_types_in_tileset;
};
