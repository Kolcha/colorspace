#include "polygon.hpp"

#include <assert.h>
#include <math.h>

#include <algorithm>
#include <numeric>

float polygon::area() const
{
  float area0 = 0.f;
  auto& pt = *this;
  size_t n = size();
  for (size_t i = 0; i < n; i++) {
    int j = (i + 1) % n;
    area0 += pt[i].x * pt[j].y;
    area0 -= pt[i].y * pt[j].x;
  }
  return 0.5f * fabs(area0);
}

bool polygon::contains(const point& p) const
{
  auto& points = *this;
  size_t n = size();
  bool c = false;
  for (int i = 0, j = n - 1; i < n; j = i++) {
    if ( ( (points[i].y >= p.y) != (points[j].y >= p.y) ) &&
         (p.x <= (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
       )
      c = !c;
  }
  return c;
}


namespace {

bool intersect_segements(const point& p0, const point& p1, const point& p2, const point& p3, point* intersection)
{
  float s10_x = p1.x - p0.x;
  float s10_y = p1.y - p0.y;
  float s32_x = p3.x - p2.x;
  float s32_y = p3.y - p2.y;
  float denom = s10_x * s32_y - s32_x * s10_y;

  if (denom == 0) {
    return false;
  }

  bool denom_positive = denom > 0.f;

  float s02_x = p0.x - p2.x;
  float s02_y = p0.y - p2.y;
  float s_numer = s10_x * s02_y - s10_y * s02_x;

  if ((s_numer < 0.f) == denom_positive) {
    return false;
  }

  float t_numer = s32_x * s02_y - s32_y * s02_x;
  if ((t_numer < 0.f) == denom_positive) {
    return false;
  }

  if ((s_numer > denom) == denom_positive || (t_numer > denom) == denom_positive) {
    return false;
  }

  float t = t_numer / denom;

  *intersection = point{p0.x + (t * s10_x), p0.y + (t * s10_y)};
  return true;
}

point get_polygon_center(const polygon& p)
{
  point center = std::accumulate(p.begin(), p.end(), point{0.f, 0.f});
  assert(!p.empty());
  center.x /= p.size();
  center.y /= p.size();
  return center;
}

void sort_polygon_points(polygon& p)
{
  if (p.empty()) return;
  point center = get_polygon_center(p);
  auto angle = [&](const point& pt) { return atan2f((pt.y - center.y), (pt.x - center.x)); };
  std::sort(p.begin(), p.end(), [&](const auto& lhs, const auto& rhs) { return angle(lhs) < angle(rhs); });
}

} // namespace

polygon intersect_polygons(const polygon& poly0, const polygon& poly1)
{
  polygon inter;

  for (const auto& pt : poly0) {
    if (poly1.contains(pt)) {
      inter.push_back(pt);
    }
  }

  for (const auto& pt : poly1) {
    if (poly0.contains(pt)) {
      inter.push_back(pt);
    }
  }

  const size_t n0 = poly0.size();
  const size_t n1 = poly1.size();

  for (size_t i = 0; i < n0; i++) {
    const auto& p0 = poly0[i];
    const auto& p1 = poly0[(i+1) % n0];
    for (size_t j = 0; j < n1; j++) {
      const auto& p2 = poly1[j];
      const auto& p3 = poly1[(j+1) % n1];
      point pinter;
      if (intersect_segements(p0, p1, p2, p3, &pinter)) {
        inter.push_back(std::move(pinter));
      }
    }
  }

  sort_polygon_points(inter);

  return inter;
}
