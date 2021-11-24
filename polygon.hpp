#pragma once

#include <vector>

struct point {
  float x;
  float y;
};

inline point& operator+=(point& lhs, const point& rhs)
{
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

inline point operator+(point lhs, const point& rhs) { return lhs += rhs; }


using polygon_underlying_type = std::vector<point>;

class polygon : public polygon_underlying_type
{
public:
  using polygon_underlying_type::polygon_underlying_type;

  float area() const ;

  bool contains(const point& p) const;
};

polygon intersect_polygons(const polygon& poly0, const polygon& poly1);

inline polygon& operator&=(polygon& lhs, const polygon& rhs)
{
  lhs = intersect_polygons(lhs, rhs);
  return lhs;
}

inline polygon operator&(polygon lhs, const polygon& rhs) { return lhs &= rhs; }
