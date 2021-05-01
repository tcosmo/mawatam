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

const static int COLOR_TRANSPARENCY_EDGE = 200;
const static int COLOR_TRANSPARENCY_TILE_COLOR = 170;

const static sf::Color COLOR_POTENTIAL_TILE = sf::Color(0, 100, 255, 140);
const static sf::Color COLOR_ANONYMOUS_TILE_TYPES =
    sf::Color(0.4 * 255, 0.4 * 255, 0.4 * 255);
const static sf::Color COLOR_TILE_TYPES =
    sf::Color(0.6 * 255, 0.6 * 255, 0.6 * 255);

const static sf::Color COLOR_EDGE_TEXT = sf::Color(0, 102, 255, 255);
const static sf::Color COLOR_TILE_TEXT = sf::Color::Black;

// Graphic layers
const static size_t INITIAL_CAPACITY = 10000;
const static size_t CAPACITY_GROWTH_FACTOR = 2;
const static size_t NB_GRAPHIC_LAYERS = 7;
const static size_t LAYER_POTENTIAL_TILES = 0;
const static size_t LAYER_TILES_BACKGROUND = 1;
const static size_t LAYER_EDGES_COLOR_PER_ALPHABET = 2;
const static size_t LAYER_EDGES_COLOR_PER_CHAR = 3;
const static size_t LAYER_TILES_COLOR = 4;
const static size_t LAYER_EDGES_TEXT = 5;
const static size_t LAYER_TILES_TEXT = 6;

// Level of detail
const static float LOD_TEXT_THRESHOLD = 0.28;

sf::Vector2f world_pos_to_screen_pos(const sf::Vector2i& pos);
sf::Vector2i screen_pos_to_world_pos(const sf::Vector2f& coords);

class WorldView : public sf::Drawable, public sf::Transformable {
 public:
  WorldView(const World& world, const sf::Font* font);
  ~WorldView();

  void update();
  void reset();

  ViewWatcher& get_view_watcher() { return view_watcher; }
  const std::array<size_t, NB_GRAPHIC_LAYERS>& get_vertex_buffers_capacity() {
    return vertex_buffers_capacity;
  }
  const std::array<size_t, NB_GRAPHIC_LAYERS>& get_vertex_counts() {
    return vertex_counts;
  }

  // Simulator's camera zoom needed for Level of Details purpose
  void set_zoom(float zoom);

  void set_tiles_colors(std::map<char, sf::Color> p_tiles_colors);
  void set_glue_char_color(std::map<char, sf::Color> p_glue_char_colors);
  void set_glue_alphabet_colors(
      std::map<std::string, sf::Color> p_glue_alphabet_colors);

  void toggle_show_layer(size_t i_layer, bool set_value = false,
                         bool value = false);
  bool get_show_layer(size_t i_layer) {
    return (i_layer < show_layer.size()) ? show_layer[i_layer] : false;
  }

 private:
  const World& world;
  const sf::Font* font;
  ViewWatcher view_watcher;

  // Need information for Level Of Details purpose
  float camera_zoom;

  std::array<size_t, NB_GRAPHIC_LAYERS> vertex_buffers_capacity;
  std::array<size_t, NB_GRAPHIC_LAYERS> vertex_counts;

  std::array<std::vector<sf::Vertex>, NB_GRAPHIC_LAYERS> vertex_memory;

  // To avoid redundant vertices, not necessarily used by all layers;
  std::set<sf::Vector2f, CompareSfVector2f>
      pos_used_by_layer[NB_GRAPHIC_LAYERS];

  std::array<bool, NB_GRAPHIC_LAYERS> show_layer;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  std::vector<sf::Vertex> vertices_for_layer_and_tile(
      size_t i_layer,
      const std::pair<sf::Vector2i, const TileType*>& pos_and_tile);
  void update_layer(size_t i_layer);
  void update_vertex_buffer(size_t i_layer,
                            const std::vector<sf::Vertex>& vertices_to_add);

  std::array<sf::VertexBuffer, NB_GRAPHIC_LAYERS> vertex_buffers;

  std::vector<sf::Vertex> get_edges_vertices(const sf::Vector2i& tile_pos,
                                             const TileType& tile_type,
                                             bool color_per_alphabet);

  std::vector<sf::Vertex> get_edges_text(const sf::Vector2i& tile_pos,
                                         const TileType& tile_type);

  std::vector<sf::Vertex> get_tile_text(const sf::Vector2i& tile_pos,
                                        const TileType& tile_type);

  sf::Color get_tiles_color(char tile_name);
  std::map<char, sf::Color> tiles_colors;

  sf::Color get_glue_alphabet_color(const Glue& glue);
  std::map<std::string, sf::Color> glue_alphabet_colors;

  sf::Color get_glue_char_color(const Glue& glue);
  std::map<char, sf::Color> glue_char_colors;
};