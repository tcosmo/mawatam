#pragma once

#include <SFML/Graphics.hpp>

#include "utils.h"

// Some graphic parameters
const static size_t GRAPHIC_EDGE_THICK = 5;
const static size_t GRAPHIC_EDGE_WIDTH = 64;
const static size_t GRAPHIC_TILE_SIZE = GRAPHIC_EDGE_WIDTH;
const static size_t GRAPHIC_TILE_TEXT_SIZE = GRAPHIC_EDGE_WIDTH / 2;
const static size_t GRAPHIC_EDGE_TEXT_SIZE = GRAPHIC_EDGE_WIDTH / 3;

// Graphic layers
const static size_t NB_GRAPHIC_LAYERS = 4;
const static size_t LAYER_TILES = 0;
const static size_t LAYER_EDGES_TEXT = 1;
const static size_t LAYER_TILES_TEXT = 2;
const static size_t LAYER_TILES_COLOR = 3;
const static size_t LAYER_POTENTIAL_TILES = 4;

class WorldView : public sf::Drawable, public sf::Transformable {
 public:
  WorldView();
  ~WorldView();

 private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  std::array<sf::VertexBuffer, NB_GRAPHIC_LAYERS> vertex_buffers;
  std::array<size_t, NB_GRAPHIC_LAYERS> vertex_counts;
};