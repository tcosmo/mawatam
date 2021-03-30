#include "view.h"

#define DEBUG_VIEW_LOG CLOG_IF(DEBUG_VIEW, DEBUG, LOGGER_VIEW)
#define VIEW_LOG(LVL) CLOG(LVL, LOGGER_VIEW)

sf::Vector2f world_pos_to_screen_pos(const sf::Vector2i& pos);

std::vector<sf::Vertex> get_filled_centered_square_vertices(
    const sf::Vector2f& center, const sf::Color& fill_color,
    const float size = GRAPHIC_TILE_SIZE,
    const float transparent_stroke_thick = GRAPHIC_TILE_STROKE_THICK);

std::vector<sf::Vertex> get_filled_triangle_vertices(
    const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2f& C,
    const sf::Color& fill_color);

std::vector<sf::Vertex> get_glyph_vertices(const sf::Vector2f& glyph_center,
                                           char glyph_char,
                                           const sf::Font& font, int text_size,
                                           sf::Color text_color,
                                           bool bold = false);

WorldView::WorldView(const World& world, const sf::Font* font)
    : world(world), font(font) {
  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1) {
    vertex_buffers[i_layer].setPrimitiveType(sf::Quads);
    vertex_buffers[i_layer].setUsage(sf::VertexBuffer::Usage::Dynamic);
    vertex_buffers[i_layer].create(INITIAL_CAPACITY);
    vertex_buffers_capacity[i_layer] = INITIAL_CAPACITY;
    vertex_counts[i_layer] = 0;
  }

  glue_alphabet_colors[NULL_GLUE_ALPHA_NAME] = sf::Color::Transparent;
  glue_char_colors[NULL_GLUE_CHAR] = sf::Color::Transparent;

  show_layer[LAYER_POTENTIAL_TILES] = true;
  show_layer[LAYER_TILES_BACKGROUND] = true;
  show_layer[LAYER_EDGES_COLOR_PER_ALPHABET] = false;
  show_layer[LAYER_EDGES_COLOR_PER_CHAR] = true;
  show_layer[LAYER_EDGES_TEXT] = true;
  show_layer[LAYER_TILES_TEXT] = true;
  show_layer[LAYER_TILES_COLOR] = false;

  camera_zoom = 1.0f;
}

void WorldView::set_zoom(float zoom) {
  if (zoom < LOD_TEXT_THRESHOLD)
    DEBUG_VIEW_LOG << "Disabling text because zoom too far out";
  camera_zoom = zoom;
}

void WorldView::toggle_show_layer(size_t i_layer, bool set_value, bool value) {
  if (i_layer >= show_layer.size()) return;
  if (!set_value)
    show_layer[i_layer] = !show_layer[i_layer];
  else
    show_layer[i_layer] = value;
}

void WorldView::reset() {
  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1) {
    sf::VertexBuffer tmp_buff;
    tmp_buff.setPrimitiveType(sf::Quads);
    tmp_buff.setUsage(sf::VertexBuffer::Usage::Dynamic);
    tmp_buff.create(INITIAL_CAPACITY);

    vertex_buffers[i_layer].create(0);
    vertex_buffers[i_layer].create(INITIAL_CAPACITY);
    vertex_buffers[i_layer].update(tmp_buff);
    vertex_buffers_capacity[i_layer] = INITIAL_CAPACITY;
    vertex_counts[i_layer] = 0;
    vertex_memory[i_layer].clear();

    pos_used_by_layer[i_layer].clear();
  }
  view_watcher.clear();
}

void WorldView::set_tiles_colors(std::map<char, sf::Color> p_tiles_colors) {
  tiles_colors = p_tiles_colors;
  for (const auto& name_and_color : p_tiles_colors) {
    tiles_colors[name_and_color.first].a = COLOR_TRANSPARENCY_TILE_COLOR;
  }
}

void WorldView::set_glue_alphabet_colors(
    std::map<std::string, sf::Color> p_glue_alphabet_colors) {
  glue_alphabet_colors = p_glue_alphabet_colors;
  glue_alphabet_colors[NULL_GLUE_ALPHA_NAME] = sf::Color::Transparent;

  for (const auto& name_and_color : glue_alphabet_colors) {
    if (name_and_color.first != NULL_GLUE_ALPHA_NAME)
      glue_alphabet_colors[name_and_color.first].a = COLOR_TRANSPARENCY_EDGE;
  }
}

void WorldView::set_glue_char_color(
    std::map<char, sf::Color> p_glue_char_colors) {
  glue_char_colors = p_glue_char_colors;
  glue_char_colors[NULL_GLUE_CHAR] = sf::Color::Transparent;
  for (auto& name_and_color : glue_char_colors) {
    if (name_and_color.first != NULL_GLUE_CHAR)
      glue_char_colors[name_and_color.first].a = COLOR_TRANSPARENCY_EDGE;
  }
}

sf::Color WorldView::get_tiles_color(char tile_name) {
  if (tiles_colors.find(tile_name) == tiles_colors.end()) {
    VIEW_LOG(WARNING) << "No color was registered for tile `" << tile_name
                      << "`";
    return sf::Color::Black;
  }
  return tiles_colors[tile_name];
}

sf::Color WorldView::get_glue_alphabet_color(const Glue& glue) {
  if (glue_alphabet_colors.find(glue.name.alphabet_name) ==
      glue_alphabet_colors.end()) {
    VIEW_LOG(WARNING) << "No color was registered for glue alphabet "
                      << glue.name.alphabet_name;
    return sf::Color::Black;
  }
  return glue_alphabet_colors[glue.name.alphabet_name];
}

sf::Color WorldView::get_glue_char_color(const Glue& glue) {
  if (glue_char_colors.find(glue.name._char) == glue_char_colors.end()) {
    VIEW_LOG(WARNING) << "No color was registered for glue char "
                      << glue.name._char;
    return sf::Color::Black;
  }
  return glue_char_colors[glue.name._char];
}

WorldView::~WorldView() {}

std::vector<sf::Vertex> WorldView::get_edges_vertices(
    const sf::Vector2i& tile_pos, const TileType& tile_type,
    bool color_per_alphabet) {
  std::vector<sf::Vertex> to_return;

  sf::Vector2f tile_center = world_pos_to_screen_pos(tile_pos);

  sf::Vector2f multipliers[4] = {
      sf::Vector2f{-0.5f, -0.5f}, sf::Vector2f{0.5f, -0.5f},
      sf::Vector2f{0.5f, 0.5f}, sf::Vector2f{-0.5f, 0.5f}};

  for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
    sf::Color fill_color =
        (color_per_alphabet)
            ? get_glue_alphabet_color(tile_type.glues.at(i_dir))
            : get_glue_char_color(tile_type.glues.at(i_dir));

    std::vector<sf::Vertex> to_add = get_filled_triangle_vertices(
        tile_center,
        tile_center + (GRAPHIC_TILE_SIZE - GRAPHIC_TILE_STROKE_THICK) *
                          multipliers[i_dir],
        tile_center + (GRAPHIC_TILE_SIZE - GRAPHIC_TILE_STROKE_THICK) *
                          multipliers[(i_dir + 1) % 4],
        fill_color);

    to_return.insert(to_return.begin(), to_add.begin(), to_add.end());
  }

  return to_return;
}

std::vector<sf::Vertex> WorldView::get_edges_text(const sf::Vector2i& tile_pos,
                                                  const TileType& tile_type) {
  sf::Vector2f tile_center = world_pos_to_screen_pos(tile_pos);
  std::vector<sf::Vertex> to_return;

  for (size_t i_dir = 0; i_dir < 4; i_dir += 1) {
    char to_draw = tile_type.glues.at(i_dir).name._char;

    if (to_draw == NULL_GLUE_CHAR) continue;

    sf::Vector2f glyph_center =
        tile_center + VIEW_CARDINAL_POINTS[i_dir] * (GRAPHIC_TILE_SIZE / 2);

    if (pos_used_by_layer[LAYER_EDGES_TEXT].find(glyph_center) !=
        pos_used_by_layer[LAYER_EDGES_TEXT].end()) {
      continue;
    }

    pos_used_by_layer[LAYER_EDGES_TEXT].insert(glyph_center);

    std::vector<sf::Vertex> to_add =
        get_glyph_vertices(glyph_center, to_draw, *font, GRAPHIC_EDGE_TEXT_SIZE,
                           COLOR_EDGE_TEXT, true);
    to_return.insert(to_return.begin(), to_add.begin(), to_add.end());
    DEBUG_VIEW_LOG << "Adding character glyph `" << to_draw << "` for edge "
                   << DIRECTIONS[i_dir] << " on tile " << tile_pos;
  }

  return to_return;
}

std::vector<sf::Vertex> WorldView::get_tile_text(const sf::Vector2i& tile_pos,
                                                 const TileType& tile_type) {
  sf::Vector2f tile_center = world_pos_to_screen_pos(tile_pos);
  std::vector<sf::Vertex> to_return;
  if (tile_type.is_anonymous()) return to_return;
  char to_draw = tile_type.name;

  if (pos_used_by_layer[LAYER_TILES_TEXT].find(tile_center) !=
      pos_used_by_layer[LAYER_TILES_TEXT].end()) {
    return to_return;
  }

  pos_used_by_layer[LAYER_TILES_TEXT].insert(tile_center);

  std::vector<sf::Vertex> to_add =
      get_glyph_vertices(tile_center, to_draw, *font, GRAPHIC_TILE_TEXT_SIZE,
                         COLOR_TILE_TEXT, true);
  to_return.insert(to_return.begin(), to_add.begin(), to_add.end());
  DEBUG_VIEW_LOG << "Adding character glyph `" << to_draw << "` for tile "
                 << tile_pos;

  return to_return;
}

std::vector<sf::Vertex> WorldView::vertices_for_layer_and_tile(
    size_t i_layer,
    const std::pair<sf::Vector2i, const TileType*>& pos_and_tile) {
  std::vector<sf::Vertex> no_vertices;

  std::string tile_name_or_anonymous = "";
  if (pos_and_tile.second)
    tile_name_or_anonymous =
        (pos_and_tile.second->is_anonymous())
            ? "anonymous"
            : std::string("named `") +
                  std::string(1, pos_and_tile.second->name) + "`";

  switch (i_layer) {
    case LAYER_POTENTIAL_TILES:
      // Null tile type means potential tile
      if (!pos_and_tile.second)
        return get_filled_centered_square_vertices(
            world_pos_to_screen_pos(pos_and_tile.first), COLOR_POTENTIAL_TILE);
      break;

    case LAYER_TILES_BACKGROUND:
      if (!pos_and_tile.second) return no_vertices;

      DEBUG_VIEW_LOG << "Adding vertices for " << tile_name_or_anonymous
                     << " tile at position: " << pos_and_tile.first;
      if (pos_and_tile.second->is_anonymous())
        return get_filled_centered_square_vertices(
            world_pos_to_screen_pos(pos_and_tile.first),
            COLOR_ANONYMOUS_TILE_TYPES, GRAPHIC_TILE_SIZE, 0);
      else
        return get_filled_centered_square_vertices(
            world_pos_to_screen_pos(pos_and_tile.first), COLOR_TILE_TYPES,
            GRAPHIC_TILE_SIZE, 0);

      break;

    case LAYER_EDGES_COLOR_PER_ALPHABET:
      // This layer is bugging on big configurations, let's ignore it for now
      return no_vertices;
      if (!pos_and_tile.second) return no_vertices;
      return get_edges_vertices(pos_and_tile.first, *pos_and_tile.second, true);

    case LAYER_EDGES_COLOR_PER_CHAR:
      if (!pos_and_tile.second) return no_vertices;
      return get_edges_vertices(pos_and_tile.first, *pos_and_tile.second,
                                false);

    case LAYER_EDGES_TEXT:
      if (!pos_and_tile.second || !font) return no_vertices;
      return get_edges_text(pos_and_tile.first, *pos_and_tile.second);

    case LAYER_TILES_TEXT:
      if (!pos_and_tile.second || !font) return no_vertices;
      return get_tile_text(pos_and_tile.first, *pos_and_tile.second);

    default:
      if (!pos_and_tile.second || pos_and_tile.second->is_anonymous())
        return no_vertices;
      return get_filled_centered_square_vertices(
          world_pos_to_screen_pos(pos_and_tile.first),
          get_tiles_color(pos_and_tile.second->name));
      break;
  }
  return no_vertices;
}

void WorldView::update_layer(size_t i_layer) {
  std::vector<sf::Vertex> vertices_to_add;
  for (const auto& pos_and_tile : view_watcher) {
    std::vector<sf::Vertex> vertices =
        vertices_for_layer_and_tile(i_layer, pos_and_tile);
    for (const sf::Vertex& vertex : vertices) {
      vertices_to_add.push_back(vertex);
    }
  }
  update_vertex_buffer(i_layer, vertices_to_add);
}

void WorldView::update() {
  if (view_watcher.size() == 0) return;
  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1) {
    DEBUG_VIEW_LOG << "[layer " << i_layer << "] " << view_watcher.size()
                   << " tiles to add";
    update_layer(i_layer);
  }

  view_watcher.clear();
}

void WorldView::update_vertex_buffer(
    size_t i_layer, const std::vector<sf::Vertex>& vertices_to_add) {
  DEBUG_VIEW_LOG << "[layer " << i_layer << "] "
                 << "Updating vertex buffer with " << vertices_to_add.size()
                 << " new vertices";
  if (i_layer >= NB_GRAPHIC_LAYERS || vertices_to_add.size() == 0) return;

  // Updating memory
  vertex_memory[i_layer].insert(std::end(vertex_memory[i_layer]),
                                std::begin(vertices_to_add),
                                std::end(vertices_to_add));

  // If it does not fit, update capacities
  if (vertex_memory[i_layer].size() > vertex_buffers_capacity[i_layer]) {
    while (vertex_memory[i_layer].size() > vertex_buffers_capacity[i_layer]) {
      vertex_buffers_capacity[i_layer] =
          CAPACITY_GROWTH_FACTOR * vertex_buffers_capacity[i_layer];
    }

    if (vertex_buffers[i_layer].create(vertex_buffers_capacity[i_layer])) {
      vertex_buffers[i_layer].update(&vertex_memory[i_layer][0],
                                     vertex_memory[i_layer].size(), 0);
      DEBUG_VIEW_LOG << "[layer " << i_layer << "] New vertex buffer capacity: "
                     << vertex_buffers_capacity[i_layer];
      vertex_counts[i_layer] = vertex_memory[i_layer].size();

    } else {
      VIEW_LOG(WARNING) << "[layer " << i_layer << "] "
                        << "Could not update vertex buffer to fit "
                        << vertex_memory[i_layer].size()
                        << " vertices at capacity "
                        << vertex_buffers_capacity[i_layer];
      return;
    }
  } else {
    vertex_buffers[i_layer].update(&vertices_to_add[0], vertices_to_add.size(),
                                   vertex_counts[i_layer]);
    vertex_counts[i_layer] += vertices_to_add.size();
  }

  DEBUG_VIEW_LOG << "[layer " << i_layer
                 << "] Vertex buffer usage: " << vertex_counts[i_layer] << "/"
                 << vertex_buffers_capacity[i_layer];
}

void WorldView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  // apply the transform
  states.transform *= getTransform();

  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1) {
    // apply the layer's texture
    // states.texture = &font.getTexture(GRAPHIC_EDGE_TEXT_SIZE);

    if (vertex_memory[i_layer].size() != 0) {
      if (i_layer == LAYER_EDGES_TEXT) {
        states.texture = &(font->getTexture(GRAPHIC_EDGE_TEXT_SIZE));
        if (camera_zoom < LOD_TEXT_THRESHOLD) continue;
      }
      if (i_layer == LAYER_TILES_TEXT) {
        states.texture = &(font->getTexture(GRAPHIC_TILE_TEXT_SIZE));
        if (camera_zoom < LOD_TEXT_THRESHOLD) continue;
      }

      if (show_layer[i_layer] || i_layer == LAYER_EDGES_COLOR_PER_CHAR)
        target.draw(vertex_buffers[i_layer], states);
    }
  }
}