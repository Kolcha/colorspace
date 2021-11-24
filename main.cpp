#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <string_view>
#include <tuple>

#include "polygon.hpp"

static constexpr const std::initializer_list<point> srgb_color_space = {
  {0.6400, 0.3300}, {0.3000, 0.6000}, {0.1500, 0.0600}
};

static constexpr const std::initializer_list<point> ntsc_color_space = {
  {0.6700, 0.3300}, {0.2100, 0.7100}, {0.1400, 0.0800}
};

// https://en.wikipedia.org/wiki/RGB_color_spaces#RGB_color_space_specifications
static const std::initializer_list<std::tuple<polygon, const char*>> color_spaces = {
  {srgb_color_space,                                       "sRGB"},
  {{{0.6400, 0.3300}, {0.2100, 0.7100}, {0.1500, 0.0600}}, "Adobe RGB"},
  {{{0.6250, 0.3400}, {0.2800, 0.5950}, {0.1550, 0.0700}}, "Apple RGB"},
  {{{0.6800, 0.3200}, {0.2650, 0.6900}, {0.1500, 0.0600}}, "DCI-P3 (Display P3)"},
  {ntsc_color_space,                                       "NTSC-FCC"},
  {{{0.7347, 0.2653}, {0.2738, 0.7174}, {0.1666, 0.0089}}, "CIE RGB"},
  {{{0.7350, 0.2650}, {0.1150, 0.8260}, {0.1570, 0.0180}}, "Wide Gamut"},

  {{{0.6400, 0.3300}, {0.2900, 0.6000}, {0.1500, 0.0600}}, "BT.601 (PAL/SECAM)"},
  {{{0.6300, 0.3400}, {0.3100, 0.5950}, {0.1550, 0.0700}}, "BT.601 (NTSC)"},
  {{{0.6400, 0.3300}, {0.3000, 0.6000}, {0.1500, 0.0600}}, "BT.709"},  // = sRGB
  {{{0.7080, 0.2920}, {0.1700, 0.7970}, {0.1310, 0.0460}}, "BT.2020"},
};


// https://en.wikipedia.org/wiki/Standard_illuminant#White_points_of_standard_illuminants
static constexpr const std::initializer_list<std::tuple<point, int, const char*>> white_points = {
  {{0.44757, 0.40745}, 2856, "A"},
  {{0.34842, 0.35161}, 4874, "B"},
  {{0.31006, 0.31616}, 6774, "C"},
  {{0.34567, 0.35850}, 5003, "D50"},
  {{0.33242, 0.34743}, 5503, "D55"},
  {{0.31271, 0.32902}, 6504, "D65"},
  {{0.29902, 0.31485}, 7504, "D75"},
  {{0.28315, 0.29711}, 9305, "D93"},
  {{0.33333, 0.33333}, 5454, "E"},
  {{0.31310, 0.33727}, 6430, "F1"},
  {{0.37208, 0.37529}, 4230, "F2"},
  {{0.40910, 0.39430}, 3450, "F3"},
  {{0.44018, 0.40329}, 2940, "F4"},
  {{0.31379, 0.34531}, 6350, "F5"},
  {{0.37790, 0.38835}, 4150, "F6"},
  {{0.31292, 0.32933}, 6500, "F7"},
  {{0.34588, 0.35875}, 5000, "F8"},
  {{0.37417, 0.37281}, 4150, "F9"},
  {{0.34609, 0.35986}, 5000, "F10"},
  {{0.38052, 0.37713}, 4000, "F11"},
  {{0.43695, 0.40441}, 3000, "F12"},
  {{0.45600, 0.40780}, 2733, "LED-B1"},
  {{0.43570, 0.40120}, 2998, "LED-B2"},
  {{0.37560, 0.37230}, 4103, "LED-B3"},
  {{0.34220, 0.35020}, 5109, "LED-B4"},
  {{0.31180, 0.32360}, 6598, "LED-B5"},
  {{0.44740, 0.40660}, 2851, "LED-BH1"},
  {{0.45570, 0.42110}, 2840, "LED-RGB1"},
  {{0.45600, 0.45480}, 2724, "LED-V1"},
  {{0.37810, 0.37750}, 4070, "LED-V2"},
};


constexpr static inline bool fuzzy_compare(double p1, double p2)
{
  return (std::abs(p1 - p2) * 1000000000000. <= std::min(std::abs(p1), std::abs(p2)));
}

constexpr static inline bool fuzzy_compare(float p1, float p2)
{
  return (std::abs(p1 - p2) * 100000.f <= std::min(std::abs(p1), std::abs(p2)));
}

static inline bool cmd_option_exists(char** begin, char** end, std::string_view opt)
{
  return std::find(begin, end, opt) != end;
}

static inline float colorspace_coverage(const polygon& space, const polygon& display)
{
  return 100.f * (display & space).area() / space.area();
}


int main(int argc, char* argv[])
{
  using namespace std::literals;

  if (cmd_option_exists(argv, argv + argc, "-h"sv) || cmd_option_exists(argv, argv + argc, "--help"sv)) {
    std::cout << "help!" << std::endl;
    return 0;
  }

  bool compare_to_ntsc = cmd_option_exists(argv, argv + argc, "--ntsc"sv);
  bool compare_to_srgb = cmd_option_exists(argv, argv + argc, "--srgb"sv);

  polygon display(3);
  for (auto& p : display) {
    std::cin >> p.x >> p.y;
  }

  point white_point;
  std::cin >> white_point.x >> white_point.y;

  auto max_inter_percent = 0.f;
  const char* max_inter_name = nullptr;

  auto display_sq = display.area();

  for (const auto&[space, name] : color_spaces) {
    auto space_sq = space.area();
    auto inter_sq = (display & space).area();

    if (fuzzy_compare(inter_sq, space_sq) && display_sq > space_sq)
      continue;

    auto inter_percent = 100 * inter_sq / space_sq;
    if (inter_percent > max_inter_percent) {
      max_inter_percent = inter_percent;
      max_inter_name = name;
    }
  }

  char space = compare_to_ntsc || compare_to_srgb ? '\t' : ' ';

  std::cout << "colorspace coverage: " << max_inter_percent << "%" << space << max_inter_name << std::endl;

  if (compare_to_ntsc) {
    std::cout << "                     " << colorspace_coverage(ntsc_color_space, display) << "%\tNTSC" << std::endl;
  }

  if (compare_to_srgb) {
    std::cout << "                     " << colorspace_coverage(srgb_color_space, display) << "%\tsRGB" << std::endl;
  }


  auto nearest_wp_iter = std::min_element(white_points.begin(), white_points.end(), [&](const auto& lhs, const auto& rhs) {
    const auto&[lp, lt, ln] = lhs;
    const auto&[rp, rt, rn] = rhs;
    return std::hypot(lp.x - white_point.x, lp.y - white_point.y) < std::hypot(rp.x - white_point.x, rp.y - white_point.y);
  });

  const auto&[_, t, name] = *nearest_wp_iter;
  std::cout << "nearest white point: " << name << " " << t << "K" << std::endl;

  return 0;
}
