#pragma once

#include <vector>
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"

struct Render2dPipeline{
	ImageRGB& image;

	template<class Vertices, class Prims>
	void run(const Vertices& V, const Prims& P, RGB color){
		for(auto primitive: assemble(P, V))
			draw(primitive, color);
	}

	void paint(Pixel p, RGB c){
		if(p.x >= 0 && p.y >= 0 && p.x < image.width() && p.y < image.height())
			image(p.x,p.y) = c;
	}

	template<class Vertex>
	void draw(Line<Vertex> line, RGB color){
		vec2 L[] = {get2DPosition(line[0]), get2DPosition(line[1])};
		for(Pixel p: rasterizeLine(L))
			paint(p, color);
	}
	
	template<class Vertex>
	void draw(Triangle<Vertex> tri, RGB color){
		vec2 T[] = {get2DPosition(tri[0]), get2DPosition(tri[1]), get2DPosition(tri[2])};
		for(Pixel p: rasterizeTriangle(T))
			paint(p, color);
	}
};

template<class Vertices, class Prims>
void render2d(const Vertices& V, const Prims& P, RGB color, ImageRGB& image){
	Render2dPipeline pipeline{image};
	pipeline.run(V, P, color);
}
