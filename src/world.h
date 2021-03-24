#pragma once

#include <array>
#include <map>
#include <set>
#include <vector>

#include "utils.h"

class Glue {
 public:
  std::string name;
  int strength;

  Glue();
  Glue(const std::string& name, int strength);

  bool operator==(const Glue& other) const {
    return name == other.name && strength == other.strength;
  }

  bool operator<(const Glue& other) const {
    if (name == other.name) return strength < other.strength;
    return name < other.name;
  }

  friend std::ostream& operator<<(std::ostream& os, Glue const& m) {
    return os << "m.name
  }
};

// The NULL_GLUE is a default ('',0) glue which is used to specify
// sides where nothing can attach (useful for polyominos in disconnected seeds
// for instance)
extern const Glue NULL_GLUE;

class TileType {
 public:
  std::string name;
  std::array<Glue, 4> glues;
  // Set to false if the tiletype was only used to specify input
  bool in_tileset;

  TileType();
  TileType(const std::string& name, const std::array<Glue, 4>& glues,
           bool in_tileset);
};

class Tileset {
 public:
  std::vector<TileType> tile_types;
  std::vector<TileType*> query(
      std::vector<std::pair<size_t, Glue>> constraints);

  Tileset();
  Tileset(const std::vector<TileType>& tile_types);
};

// Allows the view to know what's new
typedef std::vector<std::pair<sf::Vector2i, TileType*>> ViewWatcher;

class World {
 public:
  Tileset tileset;
  std::map<sf::Vector2i, TileType*, CompareSfVector2i> tiles;
  // Positions neighboring at least one tile
  std::set<sf::Vector2i, CompareSfVector2i> potential_tiles_pos;

  World();

  void next();

  void from_file(std::string file_path);
  void from_file_content(const std::string& file_content);

  void set_view_watcher(ViewWatcher* watcher);

 private:
  void add_neighbors_to_potential_tile_pos(const sf::Vector2i& pos);
  void init_potential_tiles_pos();

  // Return a vector of (`i_dir`, `glue`) where `glue` is the glue sitting on
  // side `i_dir` \in {0,1,2,3}. If there is no glue on a side, the pair wont be
  // in the vector.
  std::vector<std::pair<size_t, Glue>> get_existing_glues_of_potential_tile_pos(
      const sf::Vector2i& pos);

  // In the datam, the input seed is a disconnected set of polyominos
  // each square of each polyomino has glues from the tileset (potentially null)
  // assigned those tile types are generally not in the tileset.
  // This structure keeps track of these extra tile types.
  std::vector<TileType> input_specific_tile_types;

  ViewWatcher* view_watcher;
};
