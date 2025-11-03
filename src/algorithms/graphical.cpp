#include "graphical.hpp"

namespace Maths::Graphical {

vector<Point> createOuterPolygon(vector<Point> points) {
    int n = points.size();
    if (n <= 1) return points;

    sort(points.begin(), points.end());

    vector<Point> hull;

    // Построение нижней оболочки
    for (int i = 0; i < n; ++i) {
        while (hull.size() >= 2) {
            Point& a = hull[hull.size()-2];
            Point& b = hull[hull.size()-1];
            Point& c = points[i];

            // Векторное произведение (b-a) × (c-b)
            int cross = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
            if (cross > 0) break;
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    // Построение верхней оболочки
    int lower_size = hull.size();
    for (int i = n - 2; i >= 0; --i) {
        while (hull.size() > lower_size) {
            Point& a = hull[hull.size()-2];
            Point& b = hull[hull.size()-1];
            Point& c = points[i];

            int cross = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
            if (cross > 0) break;
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    // Удаляем дублирующую точку
    if (hull.size() > 1) hull.pop_back();

    return hull;
}


}
