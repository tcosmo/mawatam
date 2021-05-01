#include "view.h"

sf::Vector2f world_pos_to_screen_pos(const sf::Vector2i& pos) {
  return {((float)GRAPHIC_TILE_SIZE) * ((float)pos.x),
          ((float)GRAPHIC_TILE_SIZE) * (-1 * ((float)pos.y))};
}

sf::Vector2i screen_pos_to_world_pos(const sf::Vector2f& coords) {
  int sign_x = (coords.x >= 0) ? 1 : -1;
  int sign_y = (coords.y >= 0) ? -1 : 1;
  int x = (coords.x + sign_x * GRAPHIC_TILE_SIZE / 2) / GRAPHIC_TILE_SIZE;
  int y = (-1 * coords.y + sign_y * GRAPHIC_TILE_SIZE / 2) / GRAPHIC_TILE_SIZE;
  return {x, y};
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

std::vector<sf::Vertex> get_glyph_vertices(const sf::Vector2f& glyph_center,
                                           char glyph_char,
                                           const sf::Font& font, int text_size,
                                           sf::Color text_color, bool bold) {
  sf::Glyph glyph = font.getGlyph(glyph_char, text_size, bold);
  sf::Vector2f glyph_rectangle =
      sf::Vector2f{glyph.bounds.width, glyph.bounds.height};

  std::array<sf::Vertex, 4> text_quad;
  sf::Vector2f top_left = glyph_center + glyph_rectangle * -0.5f;
  text_quad[0].position = top_left;

  text_quad[1].position = top_left + glyph_rectangle * VIEW_EAST;
  text_quad[2].position = top_left + glyph_rectangle * (VIEW_EAST + VIEW_SOUTH);
  text_quad[3].position = top_left + glyph_rectangle * VIEW_SOUTH;

  sf::Vector2f top_left_text = {(float)glyph.textureRect.left,
                                (float)glyph.textureRect.top};
  sf::Vector2f text_size_vec = {(float)glyph.textureRect.width,
                                (float)glyph.textureRect.height};

  text_quad[0].texCoords = top_left_text;
  text_quad[1].texCoords = top_left_text + VIEW_EAST * text_size_vec;

  text_quad[2].texCoords = top_left_text + text_size_vec;
  text_quad[3].texCoords = top_left_text + VIEW_SOUTH * text_size_vec;

  for (size_t i_dir = 0; i_dir < 4; i_dir += 1)
    text_quad[i_dir].color = text_color;

  return std::vector<sf::Vertex>(text_quad.begin(), text_quad.end());
}