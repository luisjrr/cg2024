#pragma once

#include <vector>
#include "geometry.h"
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"
#include "Clip3D.h"

template <class VertexAttrib, class Prims, class Shader, class ImageType>
struct Render3D
{
	using Varying = typename Shader::Varying;

	Shader &shader;
	ImageType &image;

	Render3D(const VertexAttrib &V, const Prims &p, Shader &shader, ImageType &image)
		: shader{shader}, image{image}
	{
		// Pipeline de renderização
		for (auto primitive : clip(assemble(p, transform(V))))
			draw(primitive);
	}

	std::vector<Varying> transform(const VertexAttrib &V)
	{
		std::vector<Varying> PV(std::size(V));
		for (unsigned int i = 0; i < std::size(V); i++)
			shader.vertexShader(V[i], PV[i]);
		return PV;
	}

	void draw(Line<Varying> line)
	{
		vec4 P[] = {line[0].position, line[1].position};
		vec2 L[] = {toScreen(P[0]), toScreen(P[1])};

		for (Pixel p : rasterizeLine(L))
		{
			float t = find_mix_param(toVec2(p), L[0], L[1]);
			Varying vi;
			asVec(vi) = (1 - t) * asVec(line[0]) + t * asVec(line[1]);
			paint(p, vi);
		}
	}

	void draw(Triangle<Varying> tri)
	{
		vec4 P[] = {tri[0].position, tri[1].position, tri[2].position};
		vec2 T[] = {toScreen(P[0]), toScreen(P[1]), toScreen(P[2])};
		vec3 iw = {1 / P[0][3], 1 / P[1][3], 1 / P[2][3]}; // correção de perspectiva

		for (Pixel p : rasterizeTriangle(T))
		{
			vec3 t = barycentric_coords(toVec2(p), T);
			t = t * iw;							// correção de perspectiva
			t = 1.0 / (t[0] + t[1] + t[2]) * t; // correção de perspectiva
			Varying vi;
			asVec(vi) = t[0] * asVec(tri[0]) + t[1] * asVec(tri[1]) + t[2] * asVec(tri[2]);
			paint(p, vi);
		}
	}

	vec2 toScreen(vec4 P) const
	{
		float x = P[0];
		float y = P[1];
		float w = P[3];
		return {
			((x / w + 1) * image.width() - 1) / 2,
			((y / w + 1) * image.height() - 1) / 2};
	}

	void paint(Pixel p, Varying v)
	{
		if (testPixel(p, v, image))
			shader.fragmentShader(v, image(p.x, p.y));
	}
};

template <class Varying>
bool testPixel(Pixel p, Varying, ImageRGB &img)
{
	return p.x >= 0 && p.y >= 0 && p.x < img.width() && p.y < img.height();
}