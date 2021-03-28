#include "view.h"

sf::Vector2f world_pos_to_screen_pos(const sf::Vector2i& pos) {
  return {((float)GRAPHIC_TILE_SIZE) * ((float)pos.x),
          ((float)GRAPHIC_TILE_SIZE) * (-1 * ((float)pos.y))};
}

std::vector<sf::Vertex> get_filled_centered_square_vertices(
    const sf::Vector2f& center, const sf::Color& fill_color, const float size,
    const float transparent_stroke_thick) {
  std::array<sf::Vertex, 4> color_quad;

  sf::Vector2f vec_to_sides = sf::Vector2f{size / 2 - transparent_stroke_thick,
                                           size / 2 - transparent_stroke_thick};

  sf::Vector2f multipliers[4] = {sf::Vector2f{1, 1}, sf::Vector2f{1, -1},
                                 sf::Vector2f{-1, -1}, sf::Vector2f{-1, 1}};
  for (size_t i_side = 0; i_side < 4; i_side += 1) {
    color_quad[i_side].position = center + multipliers[i_side] * vec_to_sides;
    color_quad[i_side].color = fill_color;
  }

  return std::vector<sf::Vertex>(color_quad.begin(), color_quad.end());
}

std::vector<sf::Vertex> get_filled_triangle_vertices(
    const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2f& C,
    const sf::Color& fill_color) {
  // We are in quad mode, we need to add one vertex twice
  std::array<sf::Vertex, 4> color_quad;

  color_quad[0].position = A;
  color_quad[1].position = B;
  color_quad[2].position = C;
  color_quad[3].position = A;

  for (size_t i_side = 0; i_side < 4; i_side += 1)
    color_quad[i_side].color = fill_color;

  return std::vector<sf::Vertex>(color_quad.begin(), color_quad.end());
}