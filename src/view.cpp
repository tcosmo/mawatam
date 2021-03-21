#include "view.h"

WorldView::WorldView() {
  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1) {
    vertex_buffers[i_layer].setPrimitiveType(sf::Quads);
    vertex_buffers[i_layer].setUsage(sf::VertexBuffer::Usage::Dynamic);
    if (i_layer != LAYER_POTENTIAL_TILES) {
      vertex_buffers[i_layer].create(VERTEX_BUFFER_MAX_SIZE);
    } else {
      vertex_buffers[i_layer].create(VERTEX_BUFFER_MAX_SIZE / 10);
    }
    vertex_counts[i_layer] = 0;
  }
}

WorldView::~WorldView() {}

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