#ifndef RASTERIZATION_H
#define RASTERIZATION_H

#include <algorithm>
#include <cmath>
#include "vec.h"
#include "geometry.h"

//////////////////////////////////////////////////////////////////////////////

struct Pixel
{
	int x, y;
};

inline Pixel toPixel(vec2 u)
{
	return {(int)round(u[0]), (int)round(u[1])};
}

inline vec2 toVec2(Pixel p)
{
	return {(float)p.x, (float)p.y};
}

//////////////////////////////////////////////////////////////////////////////

template <class Line>
std::vector<Pixel> rasterizeLine(const Line &P)
{
	// return simple(P[0], P[1]);

	return dda(P[0], P[1]);

	// return bresenham(toPixel(P[0]), toPixel(P[1]));
}

//////////////////////////////////////////////////////////////////////////////

inline std::vector<Pixel> simple(vec2 A, vec2 B)
{
	std::vector<Pixel> out;
	vec2 d = B - A;
	float m = d[1] / d[0];
	float b = A[1] - m * A[0];

	int x0 = (int)roundf(A[0]);
	int x1 = (int)roundf(B[0]);

	for (int x = x0; x <= x1; x++)
	{
		int y = (int)roundf(m * x + b);
		out.push_back({x, y});
	}
	return out;
}

//////////////////////////////////////////////////////////////////////////////

inline std::vector<Pixel> dda(vec2 A, vec2 B)
{
	vec2 dif = B - A;
	float delta = std::max(fabs(dif[0]), fabs(dif[1]));

	vec2 d = (1 / delta) * dif;
	vec2 p = A;

	std::vector<Pixel> out;
	for (int i = 0; i <= delta; i++)
	{
		out.push_back(toPixel(p));
		p = p + d;
	}
	return out;
}

//////////////////////////////////////////////////////////////////////////////

inline std::vector<Pixel> bresenham_base(int dx, int dy)
{
	std::vector<Pixel> out;

	int D = 2 * dy - dx;
	int y = 0;
	for (int x = 0; x <= dx; x++)
	{
		out.push_back({x, y});
		if (D > 0)
		{
			y++;
			D -= 2 * dx;
		}
		D += 2 * dy;
	}
	return out;
}

inline std::vector<Pixel> bresenham(int dx, int dy)
{
	std::vector<Pixel> out;

	if (dx >= dy)
	{
		out = bresenham_base(dx, dy);
	}
	else
	{
		out = bresenham_base(dy, dx);
		for (Pixel &p : out)
			p = {p.y, p.x};
	}
	return out;
}

inline std::vector<Pixel> bresenham(Pixel p0, Pixel p1)
{
	if (p0.x > p1.x)
		std::swap(p0, p1);

	std::vector<Pixel> out = bresenham(p1.x - p0.x, abs(p1.y - p0.y));

	int s = (p0.y <= p1.y) ? 1 : -1;

	for (Pixel &p : out)
		p = {p0.x + p.x, p0.y + s * p.y};

	return out;
}

//////////////////////////////////////////////////////////////////////////////

template <class Tri>
std::vector<Pixel> rasterizeTriangle(const Tri &P)
{
	// return simple_rasterize_triangle(P);
	return scanline(P);
}

template <class Tri>
std::vector<Pixel> simple_rasterize_triangle(const Tri &P)
{
	vec2 A = P[0];
	vec2 B = P[1];
	vec2 C = P[2];

	int xmin = ceil(std::min({A[0], B[0], C[0]}));
	int xmax = floor(std::max({A[0], B[0], C[0]}));
	int ymin = ceil(std::min({A[1], B[1], C[1]}));
	int ymax = floor(std::max({A[1], B[1], C[1]}));

	std::vector<Pixel> out;
	Pixel p;
	for (p.y = ymin; p.y <= ymax; p.y++)
		for (p.x = xmin; p.x <= xmax; p.x++)
			if (is_inside(toVec2(p), P))
				out.push_back(p);
	return out;
}

float intersection(vec2 A, vec2 B, int ys)
{

	float x = (((B[0] - A[0]) * (ys - A[1])) / (B[1] - A[1])) + A[0];
	// Aresta horizontal -> está contida na scanline
	if ((ys == B[1]) && (ys == A[1]))
	{
		return A[0];
		// Scanline fora do intervalo [ymin,ymax]
	}
	else if ((B[1] > ys && A[1] > ys) || (B[1] < ys && A[1] < ys))
	{
		return NAN;
		// Retorna a abscissa do ponto
	}
	else
	{
		return x;
	}
}

template <class Tri>
std::vector<Pixel> scanline(const Tri &P)
{
	vec2 A = P[0];
	vec2 B = P[1];
	vec2 C = P[2];

	// calcula ymin e ymax
	int ymin = ceil(std::min({A[1], B[1], C[1]}));
	int ymax = floor(std::max({A[1], B[1], C[1]}));

	std::vector<Pixel> out;
	Pixel p;
	// calcula as intersecções entre as scanlines e as arestas usando a função intersection
	for (p.y = ymin; p.y <= ymax; p.y++)
	{
		float ABx = intersection(A, B, p.y);
		float BCx = intersection(B, C, p.y);
		float CAx = intersection(C, A, p.y);

		// dentro do intervalo determinado pela intersecção calcula xmin e xmax
		int xmin = ceil(fmin(ABx, fmin(BCx, CAx)));
		int xmax = floor(fmax(ABx, fmax(BCx, CAx)));

		// pinta os pixels dentro do intervalo [xmin,xmax]
		for (p.x = xmin; p.x <= xmax; p.x++)
		{
			out.push_back(p);
		}
	}

	return out;
}

#endif
