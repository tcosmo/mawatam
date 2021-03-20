#pragma once

#include <assert.h>

#include <SFML/Graphics.hpp>
#include <cstdio>

struct CompareSfVector2i {
  bool operator()(const sf::Vector2<int> &a, const sf::Vector2<int> &b) const {
    if (a.x == b.x) return a.y < b.y;
    return a.x < b.x;
  }
};

static const sf::Vector2i NORTH = sf::Vector2i{0, 1};
static const sf::Vector2i EAST = sf::Vector2i{-1, 0};
static const sf::Vector2i SOUTH = sf::Vector2i{0, -1};
static const sf::Vector2i WEST = sf::Vector2i{1, 0};

static const sf::Vector2i CARDINAL_POINTS[4] = {NORTH, EAST, SOUTH, WEST};

static size_t flip_direction(size_t i_dir) { return (i_dir + 2) % 4; }