#include "view.h"

sf::Vector2f world_pos_to_screen_pos(const sf::Vector2i& pos) {
  return {((float)GRAPHIC_TILE_SIZE) * ((float)pos.x),
          ((float)GRAPHIC_TILE_SIZE) * (-1 * ((float)pos.y))};
}

std::vector<sf::Vertex> get_filled_square_vertices(const sf::Vector2i& tile_pos,
                                                   const sf::Color& fill_color,
                                                   const float size,
                                                   const float offset) {
  std::array<sf::Vertex, 4> color_quad;
  color_quad[0].position =
      world_pos_to_screen_pos(tile_pos) - sf::Vector2f{offset, offset};

  color_quad[1].position =
      color_quad[0].position + sf::Vector2f{0, -1.0f * (size - offset)};
  color_quad[2].position =
      color_quad[1].position + sf::Vector2f{-1.0f * (size - offset), 0};
  color_quad[3].position =
      color_quad[0].position + sf::Vector2f{-1.0f * (size - offset), 0};

  color_quad[0].color = fill_color;
  color_quad[1].color = fill_color;

  color_quad[2].color = fill_color;
  color_quad[3].color = fill_color;

  return std::vector<sf::Vertex>(color_quad.begin(), color_quad.end());
}