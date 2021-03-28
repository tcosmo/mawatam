#pragma once

#include <SFML/Graphics.hpp>

#include "utils.h"
#include "world.h"

// Some graphic parameters
const static float GRAPHIC_TILE_STROKE_THICK = 2;
const static float GRAPHIC_EDGE_LENGTH = 64;
const static float GRAPHIC_TILE_SIZE = GRAPHIC_EDGE_LENGTH;
const static int GRAPHIC_TILE_TEXT_SIZE = (int)(GRAPHIC_EDGE_LENGTH / 2);
const static int GRAPHIC_EDGE_TEXT_SIZE = (int)(GRAPHIC_EDGE_LENGTH / 3);

const static int EDGE_CORLOR_TRANSPARENCY = 160;
const static sf::Color COLOR_POTENTIAL_TILE = sf::Color(0, 100, 255, 140);
const static sf::Color COLOR_ANONYMOUS_TILE_TYPES =
    sf::Color(0.4 * 255, 0.4 * 255, 0.4 * 255);
const static sf::Color COLOR_TILE_TYPES =
    sf::Color(0.6 * 255, 0.6 * 255, 0.6 * 255);

// Graphic layers
const static size_t INITIAL_CAPACITY = 10;
const static size_t CAPACITY_GROWTH_FACTOR = 2;
const static size_t NB_GRAPHIC_LAYERS = 7;
const static size_t LAYER_POTENTIAL_TILES = 0;
const static size_t LAYER_TILES_BACKGROUND = 1;
const static size_t LAYER_EDGES_COLOR_PER_ALPHABET = 2;
const static size_t LAYER_EDGES_COLOR_PER_CHAR = 3;
const static size_t LAYER_EDGES_TEXT = 4;
const static size_t LAYER_TILES_TEXT = 5;
const static size_t LAYER_TILES_COLOR = 6;

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

  void set_glue_color_mode_char(bool to_set) { glue_color_mode_char = to_set; }
  bool get_glue_color_mode_char() { return glue_color_mode_char; }

  void set_glue_char_color(std::map<char, sf::Color> p_glue_char_colors);
  void set_glue_alphabet_colors(
      std::map<std::string, sf::Color> p_glue_alphabet_colors);

 private:
  const World& world;
  ViewWatcher view_watcher;

  bool glue_color_mode_char;

  std::array<size_t, NB_GRAPHIC_LAYERS> vertex_buffers_capacity;
  std::array<size_t, NB_GRAPHIC_LAYERS> vertex_counts;

  std::array<std::vector<sf::Vertex>, NB_GRAPHIC_LAYERS> vertex_memory;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  std::vector<sf::Vertex> vertices_for_layer_and_tile(
      size_t i_layer,
      const std::pair<sf::Vector2i, const TileType*>& pos_and_tile);
  void update_layer(size_t i_layer);
  void update_vertex_buffer(size_t i_layer,
                            const std::vector<sf::Vertex>& vertices_to_add);

  std::array<sf::VertexBuffer, NB_GRAPHIC_LAYERS> vertex_buffers;

  std::vector<sf::Vertex> get_edges_vertices(const sf::Vector2i& tile_pos,
                                             const TileType* tile_type,
                                             bool color_per_alphabet);

  sf::Color get_glue_alphabet_color(const Glue& glue);
  std::map<std::string, sf::Color> glue_alphabet_colors;

  sf::Color get_glue_char_color(const Glue& glue);
  std::map<char, sf::Color> glue_char_colors;
};