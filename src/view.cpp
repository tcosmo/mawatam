#include "view.h"

#define DEBUG_VIEW_LOG CLOG_IF(DEBUG_VIEW, DEBUG, LOGGER_VIEW)
#define VIEW_LOG(LVL) CLOG(LVL, LOGGER_VIEW)

std::vector<sf::Vertex> get_filled_square_vertices(
    const sf::Vector2i& tile_pos, const sf::Color& fill_color,
    const float size = GRAPHIC_TILE_SIZE,
    const float offset = GRAPHIC_EDGE_THICK / 2);

WorldView::WorldView(const World& world) : world(world) {
  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1) {
    vertex_buffers[i_layer].setPrimitiveType(sf::Quads);
    vertex_buffers[i_layer].setUsage(sf::VertexBuffer::Usage::Dynamic);
    vertex_buffers[i_layer].create(INITIAL_CAPACITY);
    vertex_buffers_capacity[i_layer] = INITIAL_CAPACITY;
    vertex_counts[i_layer] = 0;
  }
}

WorldView::~WorldView() {}

std::vector<sf::Vertex> WorldView::vertices_for_layer_and_tile(
    size_t i_layer, const std::pair<sf::Vector2i, TileType*>& pos_and_tile) {
  std::vector<sf::Vertex> to_ret;

  switch (i_layer) {
    case LAYER_POTENTIAL_TILES:
      // Null tile type means potential type
      if (!pos_and_tile.second)
        return get_filled_square_vertices(pos_and_tile.first,
                                          COLOR_POTENTIAL_TILE);
      break;

    case LAYER_TILES:

      return get_filled_square_vertices(pos_and_tile.first, COLOR_INPUT_SQUARE);
      break;

    default:
      break;
  }

  return to_ret;
}

void WorldView::update_layer(size_t i_layer) {
  std::vector<sf::Vertex> vertices_to_add;
  for (const auto& pos_and_tile : view_watcher) {
    DEBUG_VIEW_LOG << "[layer " << i_layer << "] "
                   << "Adding vertices for tile at position: "
                   << pos_and_tile.first;
    std::vector<sf::Vertex> vertices =
        vertices_for_layer_and_tile(i_layer, pos_and_tile);
    for (const sf::Vertex& vertex : vertices) {
      vertices_to_add.push_back(vertex);
    }
  }
  update_vertex_buffer(i_layer, vertices_to_add);
}

void WorldView::update() {
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

    if (vertex_buffers[i_layer].update(&vertex_memory[i_layer][0],
                                       vertex_buffers_capacity[i_layer], 0)) {
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
    vertex_buffers[i_layer].update(&vertices_to_add[0], vertex_counts[i_layer],
                                   vertices_to_add.size());
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
    // if (i_layer == LAYER_EDGES) {
    //   states.texture = &font.getTexture(GRAPHIC_EDGE_TEXT_SIZE);
    // } else if (i_layer == LAYER_TILES) {
    //   states.texture = &font.getTexture(GRAPHIC_TILE_TEXT_SIZE);
    // }
    target.draw(vertex_buffers[i_layer], states);
  }
}