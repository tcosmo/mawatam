#pragma once

#include <SFML/Graphics.hpp>

#include "utils.h"
#include "world.h"

// Some graphic parameters
const static float GRAPHIC_EDGE_THICK = 5;
const static float GRAPHIC_EDGE_LENGTH = 64;
const static float GRAPHIC_TILE_SIZE = GRAPHIC_EDGE_LENGTH;
const static int GRAPHIC_TILE_TEXT_SIZE = (int)(GRAPHIC_EDGE_LENGTH / 2);
const static int GRAPHIC_EDGE_TEXT_SIZE = (int)(GRAPHIC_EDGE_LENGTH / 3);

const static sf::Color COLOR_POTENTIAL_TILE = sf::Color(0, 100, 255, 140);
const static sf::Color COLOR_ANONYMOUS_TILE_TYPES =
    sf::Color(0.4 * 255, 0.4 * 255, 0.4 * 255);
const static sf::Color COLOR_TILE_TYPES =
    sf::Color(0.8 * 255, 0.8 * 255, 0.8 * 255);

// Graphic layers
const static size_t INITIAL_CAPACITY = 10;
const static size_t CAPACITY_GROWTH_FACTOR = 2;
const static size_t NB_GRAPHIC_LAYERS = 4;
const static size_t LAYER_POTENTIAL_TILES = 0;
const static size_t LAYER_TILES = 1;
const static size_t LAYER_EDGES_TEXT = 1;
const static size_t LAYER_TILES_TEXT = 2;
const static size_t LAYER_TILES_COLOR = 3;

sf::Vector2f world_pos_to_screen_pos(const sf::Vector2i& pos);

class WorldView : public sf::Drawable, public sf::Transformable {
 public:
  WorldView(const World& world);
  ~WorldView();

  void update();

  ViewWatcher& get_view_watcher() { return view_watcher; }
  const std::array<size_t, NB_GRAPHIC_LAYERS>& get_vertex_buffers_capacity() {
    return vertex_buffers_capacity;
  }
  const std::array<size_t, NB_GRAPHIC_LAYERS>& get_vertex_counts() {
    return vertex_counts;
  }

 private:
  const World& world;
  ViewWatcher view_watcher;

  std::array<size_t, NB_GRAPHIC_LAYERS> vertex_buffers_capacity;
  std::array<size_t, NB_GRAPHIC_LAYERS> vertex_counts;

  std::array<std::vector<sf::Vertex>, NB_GRAPHIC_LAYERS> vertex_memory;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  std::vector<sf::Vertex> vertices_for_layer_and_tile(
      size_t i_layer, const std::pair<sf::Vector2i, TileType*>& pos_and_tile);
  void update_layer(size_t i_layer);
  void update_vertex_buffer(size_t i_layer,
                            const std::vector<sf::Vertex>& vertices_to_add);

  std::array<sf::VertexBuffer, NB_GRAPHIC_LAYERS> vertex_buffers;
};