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
};

// The NULL_GLUE is a default ('',0) glue which is used to specify
// sides where nothing can attach (useful for polyominos in disconnected seeds
// for instance)
extern const Glue NULL_GLUE;

class TileType {
 public:
  std::string name;
  std::array<Glue, 4> glues;

  TileType();
  TileType(const std::string& name, const std::array<Glue, 4>& glues);
};

class Tileset {
 public:
  std::vector<TileType> tile_types;
  std::vector<TileType*> query(
      std::vector<std::pair<size_t, Glue>> constraints);

  Tileset();
  Tileset(const std::vector<TileType>& tile_types);
};

class World {
 public:
  Tileset tileset;
  std::map<sf::Vector2i, TileType*, CompareSfVector2i> tiles;
  World(const Tileset& tileset,
        const std::map<sf::Vector2i, TileType*, CompareSfVector2i>& tiles);

  void next();

 private:
  // Positions neighboring at least one tile
  std::set<sf::Vector2i, CompareSfVector2i> potential_tiles_pos;
  void add_neighbors_to_potential_tile_pos(const sf::Vector2i& pos);
  void init_potential_tiles_pos();

  // Return a vector of (`i_dir`, `glue`) where `glue` is the glue sitting on
  // side `i_dir` \in {0,1,2,3}. If there is no glue on a side, the pair wont be
  // in the vector.
  std::vector<std::pair<size_t, Glue>> get_existing_glues_of_potential_tile_pos(
      const sf::Vector2i& pos);
};
