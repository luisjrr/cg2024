#ifndef CLIP3D_H
#define CLIP3D_H
#include "vec.h"
#include "VertexUtils.h"
#include "Primitives.h"

inline std::array<vec4, 6> normals()
{
	return {
		vec4{0, 0, -1, 1},
		vec4{0, 0, 1, 1},
		vec4{1, 0, 0, 1},
		vec4{-1, 0, 0, 1},
		vec4{0, 1, 0, 1},
		vec4{0, -1, 0, 1}};
}

template <class Varying>
bool clip(Line<Varying> &line)
{
	vec4 A = line[0].position;
	vec4 B = line[1].position;

	float t_in = 0;
	float t_out = 1;

	for (vec4 n : normals())
	{
		float an = dot(A, n);
		float bn = dot(B, n);

		bool Ain = an >= 0;
		bool Bin = bn >= 0;

		if (!Ain && !Bin)
			return false;

		if (Ain && Bin)
			continue;

		float t = an / (an - bn);

		if (Ain && !Bin)
			t_out = std::min(t_out, t);
		else
			t_in = std::max(t_in, t);

		if (t_in > t_out)
			return false;
	}

	Line<Varying> copy = line;
	asVec(line[0]) = lerp(t_in, asVec(copy[0]), asVec(copy[1]));
	asVec(line[1]) = lerp(t_out, asVec(copy[0]), asVec(copy[1]));

	return true;
}

template <class Varying>
std::vector<Line<Varying>> clip(const std::vector<Line<Varying>> &lines)
{
	std::vector<Line<Varying>> res;
	for (Line<Varying> line : lines)
		if (clip(line))
			res.push_back(line);
	return res;
}

/******************************************************************************/

template <class Varying>
std::vector<Varying> clip(const std::vector<Varying> &polygon, vec4 n)
{
	std::vector<Varying> R;
	for (unsigned int i = 0; i < polygon.size(); i++)
	{
		Varying P = polygon[i];
		Varying Q = polygon[(i + 1) % polygon.size()];
		vec4 p = getPosition(P);
		vec4 q = getPosition(Q);

		float dotP = dot(p, n);
		float dotQ = dot(q, n);

		bool Pin = dotP >= 0;
		bool Qin = dotQ >= 0;

		if (Pin != Qin)
		{
			float t = dotP / (dotP - dotQ);
			Varying interp;
			asVec(interp) = (1.0 - t) * asVec(P) + t * asVec(Q);
			R.push_back(interp);
		}
		if (Qin)
		{
			R.push_back(Q);
		}
	}
	return R;
}

template <class Varying>
std::vector<Varying> clip(const std::vector<Varying> &polygon)
{
	std::vector<Varying> R = polygon;

	for (vec4 n : normals())
		R = clip(R, n);

	return R;
}

template <class Varying>
std::vector<Triangle<Varying>> clip(const std::vector<Triangle<Varying>> &tris)
{
	std::vector<Triangle<Varying>> res;

	for (Triangle<Varying> tri : tris)
	{
		std::vector<Varying> polygon = {tri[0], tri[1], tri[2]};
		polygon = clip(polygon);
		TriangleFan T{polygon.size()};
		for (Triangle<Varying> tri : assemble(T, polygon))
			res.push_back(tri);
	}
	return res;
}

#endif
