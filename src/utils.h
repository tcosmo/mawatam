#pragma once

#include <assert.h>

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>

#include "config.h"

struct CompareSfVector2i {
  bool operator()(const sf::Vector2<int> &a, const sf::Vector2<int> &b) const {
    if (a.x == b.x) return a.y < b.y;
    return a.x < b.x;
  }
};

// model
static const sf::Vector2i NORTH = sf::Vector2i{0, 1};
static const sf::Vector2i EAST = sf::Vector2i{-1, 0};
static const sf::Vector2i SOUTH = sf::Vector2i{0, -1};
static const sf::Vector2i WEST = sf::Vector2i{1, 0};

static const sf::Vector2i CARDINAL_POINTS[4] = {NORTH, EAST, SOUTH, WEST};
static size_t flip_direction(size_t i_dir) { return (i_dir + 2) % 4; }

// view
static const sf::Vector2f VIEW_NORTH = sf::Vector2f({0, -1});
static const sf::Vector2f VIEW_EAST = sf::Vector2f({1, 0});
static const sf::Vector2f VIEW_SOUTH = sf::Vector2f({0, 1});
static const sf::Vector2f VIEW_WEST = sf::Vector2f({-1, 0});

/* Some geometry helpers */

// Multiplying a vector by a scalar
template <typename T, typename U>
static sf::Vector2<T> operator*(U scalar, const sf::Vector2<T> &vector) {
  sf::Vector2<T> toReturn = vector;
  return {toReturn.x * scalar, toReturn.y * scalar};
}

// Multiplying two vectors component by component
template <typename T>
static sf::Vector2<T> operator*(const sf::Vector2<T> &a,
                                const sf::Vector2<T> &b) {
  return {a.x * b.x, a.y * b.y};
}

inline float Euclidean_norm(const sf::Vector2f &vect) {
  return sqrt(vect.x * vect.x + vect.y * vect.y);
}

inline sf::Vector2f get_normal_unit_vector(const sf::Vector2f &vect) {
  return {vect.y / Euclidean_norm(vect), -1 * vect.x / Euclidean_norm(vect)};
}