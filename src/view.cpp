#include "view.h"

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
    std::vector<sf::Vertex> vertices =
        vertices_for_layer_and_tile(i_layer, pos_and_tile);
    for (const sf::Vertex& vertex : vertices) {
      vertices_to_add.push_back(vertex);
    }
  }
  update_vertex_buffer(i_layer, vertices_to_add);
}
void WorldView::update() {
  if (DEBUG_MODE)
    for (const auto& pos_and_tile : view_watcher) {
      printf("[view] Adding vertices for square at position: (%d, %d)\n",
             pos_and_tile.first.x, pos_and_tile.first.y);

      printf("[view] Is potential tile: %d\n",
             (int)(pos_and_tile.second == nullptr));
      if (pos_and_tile.second) {
      }
      printf("\n");
    }

  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1)
    update_layer(i_layer);

  view_watcher.clear();
}

void WorldView::update_vertex_buffer(
    size_t i_layer, const std::vector<sf::Vertex> vertices_to_add) {
  if (i_layer >= NB_GRAPHIC_LAYERS || vertices_to_add.size() == 0) return;

  if (vertex_counts[i_layer] + vertices_to_add.size() >
      VERTEX_BUFFER_MAX_SIZE) {
    printf(
        "The maximum number of drawable vertices, `%zu`, has been reached on "
        "layer `%zu`, new "
        "updates are ignored. You can increase the value of "
        "`VERTEX_BUFFER_MAX_SIZE` in the file `config.h.in`.",
        (size_t)VERTEX_BUFFER_MAX_SIZE, i_layer);
    return;
  }

  bool to_update = false;
  while (vertex_counts[i_layer] + vertices_to_add.size() >
         vertex_buffers_capacity[i_layer]) {
    to_update = true;
    vertex_buffers_capacity[i_layer] =
        std::min(CAPACITY_GROWTH_FACTOR * vertex_buffers_capacity[i_layer],
                 (size_t)VERTEX_BUFFER_MAX_SIZE);
  }

  if (to_update) {
    vertex_buffers[i_layer].create(0);
    vertex_buffers[i_layer].create(vertex_buffers_capacity[i_layer]);
    printf("[vertex buffer %zu] Growing, new size %zu\n", i_layer,
           vertex_buffers_capacity[i_layer]);
  }

  vertex_buffers[i_layer].update(&vertices_to_add[0], vertices_to_add.size(),
                                 vertex_counts[i_layer]);
  vertex_counts[i_layer] += vertices_to_add.size();
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