#include "Render2D_v1.h"
#include "bezier.h"
#include "matrix.h"
#include "polygon_triangulation.h"
#include "Color.h"

int main()
{

    std::vector<vec2> CP = loadCurve("borboleta.txt");
    std::vector<vec2> P = sample_bezier_spline<3>(CP, 30);
    std::vector<unsigned int> indices = triangulate_polygon(P);

    vec2 v = {400, 400};
    mat3 T = {
        1.0, 0.0, v[0],
        0.0, 1.0, v[1],
        0.0, 0.0, 1.0};

    mat3 Ti = {
        1.0, 0.0, -v[0],
        0.0, 1.0, -v[1],
        0.0, 0.0, 1.0};

    LineStrip L{P.size()};
    Elements<Triangles> Tri{indices};

    ImageRGB G(800, 800);
    G.fill(white);

    float t = (3.14 * 2) / 12;

    for (float i = 0; i < 12; i++)
    {
        mat3 R = {
            cos(i * t), -sin(i * t), 0.0,
            sin(i * t), cos(i * t), 0.0,
            0.0, 0.0, 1.0};

        RGB color = lerp(i / 12, red, yellow);

        render2d((T * R * Ti) * P, L, black, G);
        render2d((T * R * Ti) * P, Tri, color, G);
    }

    G.savePNG("output.png");
}