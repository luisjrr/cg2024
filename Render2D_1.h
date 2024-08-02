#pragma once

#include <vector>
#include "geometry.h"
#include "Image.h"
#include "VertexUtils.h"
#include "Primitives.h"
#include "rasterization.h"
#include "Clip2D.h"

std::vector<Mat<3, 1>> vertices3d;

struct Render2dPipeline
{
	ImageRGB &image;

	template <class Vertices, class Prims>
	void run(const Vertices &V, const Prims &P)
	{
		ClipRectangle R = {-0.5f, -0.5f, image.width() - 0.5f, image.height() - 0.5f};
		for (auto primitive : clip(assemble(P, V), R))
			draw(primitive);
	}

	void paint(Pixel p, RGB c)
	{
		if (p.x >= 0 && p.y >= 0 && p.x < image.width() && p.y < image.height())
			image(p.x, p.y) = c;
	}

	void draw(Line<Vec2Col> line)
	{
		vec2 L[] = {line[0].position, line[1].position};
		RGB C[] = {line[0].color, line[1].color};

		for (Pixel p : rasterizeLine(L))
		{
			float t = find_mix_param(toVec2(p), L[0], L[1]);
			RGB color = lerp(t, C[0], C[1]);
			paint(p, color);
		};
	}

	void draw(Triangle<Vec2Col> tri)
	{

		vec2 T[] = {tri[0].position, tri[1].position, tri[2].position};
		RGB C[] = {tri[0].color, tri[1].color, tri[2].color};
		vec3 c[] = {toVec(C[0]), toVec(C[1]), toVec(C[2])};

		for (Pixel p : rasterizeTriangle(T))
		{
			vec3 alpha = barycentric_coords(toVec2(p), T);
			vec3 cor = alpha[0] * c[0] + alpha[1] * c[1] + alpha[2] * c[2];
			RGB color = toColor(cor);
			paint(p, color);
		}
	}
};

template <class Vertices, class Prims>
void render2d(const Vertices &V, const Prims &P, ImageRGB &image)
{
	Render2dPipeline pipeline{image};
	pipeline.run(V, P);
}

template <class Vertices, class Prims>
void render2d(const Vertices &V, const Prims &P, RGB color, ImageRGB &image)
{
	std::vector<vec2> vertices2d;
	for (const auto &v : vertices3d)
	{
		vertices2d.push_back(toVec2<3, 1>(v.get<3, 1>()));
	}
	render2d(vertices2d, P, red, image);
}