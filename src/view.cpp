#include "view.h"

WorldView::WorldView() {

}

WorldView::~WorldView() {

}

void WorldView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  // apply the transform
  states.transform *= getTransform();
}