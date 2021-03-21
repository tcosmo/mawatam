#include "simulator.h"

Simulator::Simulator(World& world, WorldView& world_view, int screen_w,
                     int screen_h)
    : world(world), world_view(world_view) {
  window.create(sf::VideoMode(screen_w, screen_h), "ddd");
  window.setFramerateLimit(TARGET_FPS);
  camera_init();
}

bool Simulator::is_ctrl_pressed() {
  return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
}

bool Simulator::is_shift_pressed() {
  return sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
}

bool Simulator::is_alt_pressed() {
  return sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);
}

void Simulator::run() {
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      handle_camera_events(event);
      if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
          case sf::Keyboard::A:
            //print_simulation_report();
            break;

          case sf::Keyboard::N:
            world.next();
            //world_view.update();
            break;

          case sf::Keyboard::R:
            // world.reset();
            // world_view.reset();
            // world_view.update();
            break;

          case sf::Keyboard::Escape:
            window.close();
            break;
        }
      }

      if (event.type == sf::Event::Closed) window.close();
    }

    window.clear(sf::Color(0.2 * 255, 0.2 * 255, 0.2 * 255));
    window.draw(world_view);
    window.display();
  }
}