#include "simulator.h"

Simulator::Simulator(World& world, WorldView& world_view, int screen_w,
                     int screen_h)
    : world(world), world_view(world_view) {
  window.create(sf::VideoMode(screen_w, screen_h), datam_PROG_NAME);
  window.setPosition(sf::Vector2i(461, 1636));
  window.setFramerateLimit(TARGET_FPS);
  // window.setVerticalSyncEnabled(true);
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

void Simulator::print_simulation_report() {
  LOG(INFO) << "=== Simulation Report ===";

  LOG(INFO) << "   == Tiles ==";
  LOG(INFO) << "      - Growth Mode: " << world.get_growth_mode();
  LOG(INFO) << "      - Tiles: " << world.get_tiles().size();
  LOG(INFO) << "      - Potential tiles: "
            << world.get_potential_tiles_pos().size();
  LOG(INFO) << "   == Vertex Buffers ==";

  for (size_t i_layer = 0; i_layer < NB_GRAPHIC_LAYERS; i_layer += 1) {
    LOG(INFO) << "      - Layer " << i_layer << " :  "
              << world_view.get_vertex_counts()[i_layer] << "/"
              << world_view.get_vertex_buffers_capacity()[i_layer];
  }
}

void Simulator::draw_debug_grid() {
  static std::vector<sf::CircleShape> debug_grid;

  if (debug_grid.size() == 0) {
    for (int x = -3; x < 4; x += 1)
      for (int y = -3; y < 4; y += 1) {
        float radius = 4.;
        sf::CircleShape circle(radius);
        circle.setPosition(world_pos_to_screen_pos({x, y}));
        if (x == 0 && y == 0) {
          circle.setFillColor(sf::Color(200, 100, 255, 170));
        } else
          circle.setFillColor(sf::Color(255, 200, 200, 170));

        circle.setOrigin({radius, radius});
        debug_grid.push_back(circle);
      }
  }

  for (const auto& circle : debug_grid) {
    window.draw(circle);
  }
}

void Simulator::run() {
  bool do_draw_debug_grid = false;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      handle_camera_events(event);
      if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
          case sf::Keyboard::A:
            print_simulation_report();
            break;

          case sf::Keyboard::N:
            world.next();
            world_view.update();
            break;

          case sf::Keyboard::G:
            do_draw_debug_grid = !do_draw_debug_grid;
            break;

          // Toggle edge color mode
          case sf::Keyboard::E:
            world_view.set_glue_color_mode_char(
                !world_view.get_glue_color_mode_char());
            break;

          // Change growth mode
          case sf::Keyboard::T:
            world.set_growth_mode(static_cast<GrowthMode>(
                (world.get_growth_mode() + 1) % NB_GROWTH_MODES));
            LOG(INFO) << "Changed growth to " << world.get_growth_mode();
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
    if (do_draw_debug_grid) draw_debug_grid();
    window.display();
  }
}