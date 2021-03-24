#include "world.h"

Glue::Glue(const std::string& name, int strength)
    : name(name), strength(strength) {}

const Glue NULL_GLUE = Glue("", 0);

Glue::Glue() : name(NULL_GLUE.name), strength(NULL_GLUE.strength) {}

TileType::TileType() {
  in_tileset = true;
  for (size_t i = 0; i < 4; i += 1) glues[i] = NULL_GLUE;
}

TileType::TileType(const std::string& name, const std::array<Glue, 4>& glues,
                   bool in_tileset = true)
    : name(name), glues(glues), in_tileset(in_tileset) {}

Tileset::Tileset() {}

std::vector<TileType*> Tileset::query(
    std::vector<std::pair<size_t, Glue>> constraints) {
  std::vector<TileType*> to_return;

  for (TileType& tile_type : tile_types) {
    for (const auto& constraint : constraints) {
      if (tile_type.glues[constraint.first] == constraint.second) {
        to_return.push_back(&tile_type);
      }
    }
  }

  return to_return;
}

Tileset::Tileset(const std::vector<TileType>& tile_types)
    : tile_types(tile_types) {}

World::World() { view_watcher = nullptr; }

void World::set_view_watcher(ViewWatcher* watcher) {
  view_watcher = watcher;

  for (const auto& pos_and_tile : tiles) {
    view_watcher->push_back(pos_and_tile);
  }

  for (const sf::Vector2i& pos : potential_tiles_pos) {
    view_watcher->push_back({pos, nullptr});
  }
}

void World::init_potential_tiles_pos() {
  for (const auto& pos_and_square : tiles) {
    add_neighbors_to_potential_tile_pos(pos_and_square.first);
  }
}

std::vector<std::pair<size_t, Glue>>
World::get_existing_glues_of_potential_tile_pos(const sf::Vector2i& pos) {
  assert(tiles.find(pos) == tiles.end());

  std::vector<std::pair<size_t, Glue>> to_return;
  for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
    sf::Vector2i neigh = pos + CARDINAL_POINTS[i_dir];
    if (tiles.find(neigh) != tiles.end()) {
      to_return.push_back({i_dir, tiles[neigh]->glues[flip_direction(i_dir)]});
    }
  }

  return to_return;
}

void World::add_neighbors_to_potential_tile_pos(const sf::Vector2i& pos) {
  for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
    sf::Vector2i neigh = pos + CARDINAL_POINTS[i_dir];
    if (tiles.find(neigh) == tiles.end()) {
      if (view_watcher &&
          potential_tiles_pos.find(neigh) == potential_tiles_pos.end())
        view_watcher->push_back({neigh, nullptr});
      potential_tiles_pos.insert(neigh);
    }
  }
}

void World::next() {
  for (const sf::Vector2i& pos : potential_tiles_pos) {
    std::vector<std::pair<size_t, Glue>> glues_at_pos =
        get_existing_glues_of_potential_tile_pos(pos);

    std::vector<TileType*> candidates_tile_types = tileset.query(glues_at_pos);

    if (candidates_tile_types.size() == 0) return;

    if (candidates_tile_types.size() == 1) {
      tiles[pos] = candidates_tile_types[0];
      potential_tiles_pos.erase(pos);
      add_neighbors_to_potential_tile_pos(pos);

      if (view_watcher)
        view_watcher->push_back(std::make_pair(pos, candidates_tile_types[0]));

    } else {
      printf("Non determinism at pos (%d,%d)!!", pos.x, pos.y);
    }
  }
}