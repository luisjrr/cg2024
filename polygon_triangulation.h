#pragma once

#include <algorithm>
#include <list>
#include <numeric>
#include "geometry.h"
#include "VertexUtils.h"

// previous iterator (wrapping around) 
template<class It>
It prev(It a, It b, It e){
	if(a == b)
		a = e;
	return --a;
}

// next iterator (wrapping around) 
template<class It>
It next(It a, It b, It e){
	++a;
	if(a == e)
		a = b;
	return a;
}

// Check if a polygon is in clockwise orientation.
template<class Vertex>
bool is_clockwise(const std::list<unsigned int>& indices, const std::vector<Vertex>& P){
	const auto 
		b = indices.begin(),
		e = indices.end(),
		it = std::min_element(b, e,
			[&](unsigned int i, unsigned int j){
				vec2 u = get2DPosition(P[i]);
				vec2 v = get2DPosition(P[j]);
				return u[0] < v[0] || (u[0]==v[0] && u[1]<v[1]);
			}
		),
		pr = prev(it, b, e),
		nx = next(it, b, e);

	return tri_area(get2DPosition(P[*pr]), get2DPosition(P[*it]), get2DPosition(P[*nx])) > 0;
}
			
// Get a list of vertex indices from polygon P
// without duplicate consecutive items (including first and last)
// in clockwise orientation.
template<class Vertex>
std::list<unsigned int> get_polygon_indices(const std::vector<Vertex>& P){
	// indices = {0, 1, 2, ... , n-1}
	std::list<unsigned int> indices(P.size());
	std::iota(indices.begin(), indices.end(), 0);

	auto close_points  = [&](unsigned int i, unsigned int j){
		return norm2(get2DPosition(P[i]) - get2DPosition(P[j])) < 1e-20;
	};
	// remove duplicate vertices
	indices.unique(close_points);
	if(close_points(indices.front(), indices.back()))
		indices.erase(indices.begin());

	// make sure polygon is clockwise
	if(!is_clockwise(indices, P))
		indices.reverse();
		
	return indices;
}

// check if triangle is an ear in a polygon.
template<class Vertex>
bool is_ear(
	unsigned int a, unsigned int b, unsigned int c, 
	const std::list<unsigned int>& indices, 
	const std::vector<Vertex>& P
){
	const vec2 T[3] = {get2DPosition(P[a]), get2DPosition(P[b]), get2DPosition(P[c])};
	return 
		tri_area(T[0], T[1], T[2]) > 0 && 
		std::none_of(indices.begin(), indices.end(), 
			[&](unsigned int i){
				return i!=a && i !=b && i!=c && is_inside(get2DPosition(P[i]), T);
			}
		);
}

// get a list of triangle indices from a polygon P. 
template<class Vertex>
std::vector<unsigned int> triangulate_polygon(const std::vector<Vertex>& P){
	if(P.size() < 3)
		return {};

	std::list<unsigned int> polygon_indices = get_polygon_indices(P);
	auto pr = polygon_indices.begin();
	auto it = std::next(pr);
	auto nx = std::next(it);

	std::vector<unsigned int> triangles_indices;
	while(polygon_indices.size() >= 3){
		if(is_ear(*pr, *it, *nx, polygon_indices, P)){
			triangles_indices.insert(triangles_indices.end(), {*pr, *it, *nx});
			polygon_indices.erase(it);
		}else
			pr = it;

		it = nx;
		nx = next(it, polygon_indices.begin(), polygon_indices.end());
	}
	return triangles_indices;
}
