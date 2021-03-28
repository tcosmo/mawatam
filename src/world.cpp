#include "world.h"

#define DEBUG_WORLD_LOG CLOG_IF(DEBUG_WORLD, DEBUG, LOGGER_WORLD)
#define WORLD_LOG(LVL) CLOG(LVL, LOGGER_WORLD)

World::World() {
  view_watcher = nullptr;
  growth_mode = ASYNC_ORDERED;
}

World::World(std::vector<std::unique_ptr<TileType>>& tile_types,
             std::map<sf::Vector2i, const TileType*, CompareSfVector2i>& tiles,
             int temperature, GrowthMode growth_mode)
    : tile_types(std::move(tile_types)),
      tiles(std::move(tiles)),
      temperature(temperature),
      growth_mode(growth_mode) {
  view_watcher = nullptr;

  // Have to use this->tile_types because of move
  for (const auto& tile_ptr : this->tile_types) {
    if (!tile_ptr.get()->is_anonymous()) {
      tile_types_in_tileset.push_back(tile_ptr.get());
    }
  }

  init_potential_tiles_pos();
}

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
    add_tile_neighbors_as_potential_tile_pos(pos_and_square.first);
  }
}

void World::add_tile_neighbors_as_potential_tile_pos(const sf::Vector2i& pos) {
  assert(tiles.find(pos) != tiles.end());
  for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
    sf::Vector2i neigh = pos + CARDINAL_POINTS[i_dir];
    if (tiles.find(neigh) == tiles.end() &&
        !(tiles[pos]->glues.at(i_dir) == NULL_GLUE)) {
      if (view_watcher &&
          potential_tiles_pos.find(neigh) == potential_tiles_pos.end())
        view_watcher->push_back({neigh, nullptr});
      potential_tiles_pos.insert(neigh);
    }
  }
}

SquareGlues World::get_glues_surrounding_potential_tile_pos(
    const sf::Vector2i& pos) {
  assert(tiles.find(pos) == tiles.end());

  SquareGlues to_return;
  for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
    sf::Vector2i neigh = pos + CARDINAL_POINTS[i_dir];
    if (tiles.find(neigh) != tiles.end())
      to_return[i_dir] = tiles[neigh]->glues.at(flip_direction(i_dir));
  }

  return to_return;
}

std::vector<const TileType*> World::tileset_query(
    const SquareGlues& constraints) {
  std::vector<const TileType*> to_return;

  for (const TileType* tile_type : tile_types_in_tileset) {
    int match_strength = 0;
    for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
      if (constraints.at(i_dir) == NULL_GLUE) continue;
      if (tile_type->glues.at(i_dir) == constraints.at(i_dir))
        match_strength += tile_type->glues.at(i_dir).strength;
    }

    DEBUG_WORLD_LOG << "Matching strength for tile `" << tile_type->name
                    << "` is " << match_strength;

    if (match_strength >= temperature) {
      to_return.push_back(tile_type);
      DEBUG_WORLD_LOG << "Above temperature threshold";
    } else {
      DEBUG_WORLD_LOG << "Below temperature threshold";
    }
  }

  return to_return;
}

void World::next() {
  std::vector<std::pair<sf::Vector2i, const TileType*>> tiles_to_add;
  for (const sf::Vector2i& pos : potential_tiles_pos) {
    SquareGlues glues_at_pos = get_glues_surrounding_potential_tile_pos(pos);

    DEBUG_WORLD_LOG << "Solving " << pos;
    DEBUG_WORLD_LOG << "Constraints: " << glues_at_pos;

    std::vector<const TileType*> candidates_tile_types =
        tileset_query(glues_at_pos);

    if (candidates_tile_types.size() == 0) {
      DEBUG_WORLD_LOG << "No solution";
      continue;
    }

    if (candidates_tile_types.size() >= 1) {
      if (candidates_tile_types.size() == 1) {
        DEBUG_WORLD_LOG << "One solution: " << *candidates_tile_types[0];
        tiles_to_add.push_back(std::make_pair(pos, candidates_tile_types[0]));
      } else {
        DEBUG_WORLD_LOG << "Several solutions (" << candidates_tile_types.size()
                        << "): non determinism!";
      }

      // We do several updates iff we are in async growth mode
      if (growth_mode == ASYNC_ORDERED) break;
    }
  }

  bool apply_all = (growth_mode == SYNC);
  size_t apply_only_i = 0;
  if (growth_mode == ASYNC_RANDOM)
    apply_only_i = std::rand() % tiles_to_add.size();

  size_t i = 0;
  for (const auto& pos_and_tile : tiles_to_add) {
    if (apply_all || i == apply_only_i) {
      tiles[pos_and_tile.first] = pos_and_tile.second;
      potential_tiles_pos.erase(pos_and_tile.first);
      add_tile_neighbors_as_potential_tile_pos(pos_and_tile.first);
      if (view_watcher) view_watcher->push_back(pos_and_tile);
      if (!apply_all) break;
    }
    i += 1;
  }
}