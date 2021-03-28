#include <SFML/Graphics.hpp>

// A bit of
// https://github.com/chrisBRN/Lots_Of_Colors/blob/master/lots_of_colors_sfml.h

static const sf::Color cga_02{245, 85, 85};    // FF555555
static const sf::Color cga_03{250, 170, 170};  // FFAAAAAA
static const sf::Color cga_05{240, 0, 10};     // FF0000AA
static const sf::Color cga_06{245, 85, 95};    // FF5555FF
static const sf::Color cga_07{240, 10, 160};   // FF00AA00
static const sf::Color cga_08{245, 95, 245};   // FF55FF55
static const sf::Color cga_09{240, 10, 170};   // FF00AAAA
static const sf::Color cga_10{245, 95, 255};   // FF55FFFF
static const sf::Color cga_11{250, 160, 0};    // FFAA0000
static const sf::Color cga_12{255, 245, 85};   // FFFF5555
static const sf::Color cga_13{250, 160, 10};   // FFAA00AA
static const sf::Color cga_14{255, 245, 95};   // FFFF55FF
static const sf::Color cga_15{250, 165, 80};   // FFAA5500
static const sf::Color cga_16{255, 255, 245};  // FFFFFF55

static const sf::Color com64_02(246, 38, 38);    // FF626262
static const sf::Color com64_03(248, 152, 152);  // FF898989
static const sf::Color com64_04(250, 218, 218);  // FFadadad
static const sf::Color com64_06(249, 244, 228);  // FF9f4e44
static const sf::Color com64_07(252, 183, 231);  // FFcb7e75
static const sf::Color com64_08(246, 213, 65);   // FF6d5412
static const sf::Color com64_09(250, 22, 131);   // FFa1683c
static const sf::Color com64_10(252, 157, 72);   // FFc9d487
static const sf::Color com64_11(249, 174, 41);   // FF9ae29b
static const sf::Color com64_12(245, 202, 181);  // FF5cab5e
static const sf::Color com64_13(246, 171, 252);  // FF6abfc6
static const sf::Color com64_14(248, 135, 236);  // FF887ecb
static const sf::Color com64_15(245, 4, 89);     // FF50459b
static const sf::Color com64_16(250, 5, 122);    // FFa057a3

static const sf::Color mac2_02(255, 255, 240);  // FFffff00
static const sf::Color mac2_03(255, 246, 80);   // FFff6500
static const sf::Color mac2_04(253, 192, 0);    // FFdc0000
static const sf::Color mac2_05(255, 240, 9);    // FFff0097
static const sf::Color mac2_06(243, 96, 9);     // FF360097
static const sf::Color mac2_07(240, 0, 12);     // FF0000ca
static const sf::Color mac2_08(240, 9, 127);    // FF0097ff
static const sf::Color mac2_09(240, 10, 128);   // FF00a800
static const sf::Color mac2_10(240, 6, 80);     // FF006500
static const sf::Color mac2_11(246, 83, 96);    // FF653600
static const sf::Color mac2_12(249, 118, 83);   // FF976536
static const sf::Color mac2_13(251, 155, 155);  // FFb9b9b9
static const sf::Color mac2_14(248, 104, 104);  // FF868686
static const sf::Color mac2_15(244, 84, 84);    // FF454545
static const sf::Color mac2_16(240, 0, 0);      // FF000000

static const std::vector<sf::Color> COLOR_WHEEL_SECONDARY = {
    sf::Color::Green,   sf::Color::Red,  sf::Color::Yellow,
    sf::Color::Magenta, sf::Color::Cyan, sf::Color::Blue};

static const std::vector<sf::Color> COLOR_WHEEL = {
    sf::Color::Green, sf::Color::Red, cga_02, cga_03, cga_05, cga_06,
    cga_07,           cga_08,         cga_09, cga_10, cga_11, cga_12,
    cga_13,           cga_14,         cga_15, cga_16};

static const std::vector<sf::Color> COLOR_WHEEL_2 = {
    sf::Color::Green, sf::Color::Red, com64_02, com64_03, com64_04, com64_06,
    com64_07,         com64_08,       com64_09, com64_10, com64_11, com64_12,
    com64_13,         com64_14,       com64_15, com64_16};

static const std::vector<sf::Color> COLOR_WHEEL_3 = {
    sf::Color::Green, sf::Color::Red, mac2_02, mac2_03, mac2_04, mac2_05,
    mac2_06,          mac2_07,        mac2_08, mac2_09, mac2_10, mac2_11,
    mac2_13,          mac2_14,        mac2_14, mac2_15, mac2_16};

static sf::Color get_color_wheel_color(
    size_t i_color,
    const std::vector<sf::Color>& colors = COLOR_WHEEL_SECONDARY) {
  return colors[i_color % colors.size()];
}