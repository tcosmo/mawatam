#pragma once

#include "utils.h"

#include <SFML/Graphics.hpp>

// Some graphic parameters
const static size_t GRAPHIC_EDGE_THICK = 5;
const static size_t GRAPHIC_EDGE_WIDTH = 64;
const static size_t GRAPHIC_TILE_SIZE = GRAPHIC_EDGE_WIDTH;
const static size_t GRAPHIC_TILE_TEXT_SIZE = GRAPHIC_EDGE_WIDTH / 2;
const static size_t GRAPHIC_EDGE_TEXT_SIZE = GRAPHIC_EDGE_WIDTH / 3;

class WorldView : public sf::Drawable, public sf::Transformable {
 public:

 WorldView();
 ~WorldView();

 private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};