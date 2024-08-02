#ifndef SAMPLER2D_H
#define SAMPLER2D_H

#include "Image.h"

enum Filter{NEAREST, BILINEAR};
enum WrapMode{CLAMP, REPEAT, MIRRORED_REPEAT};

class Sampler2D{
	public:
	ImageRGB img;
	Filter filter;
	WrapMode wrapX, wrapY;
	RGB default_color = magenta;

	RGB sample(vec2 texCoords) const{
		if(img.width() == 0 || img.height() == 0)
			return default_color;

		vec2 s = {
			normalizeValue(texCoords[0], wrapX),
			normalizeValue(texCoords[1], wrapY),
		};	

		vec2 p = {
			s[0]*img.width()  - 0.5f,
			s[1]*img.height() - 0.5f
		};

		return filter==NEAREST? sampleNearest(p): sampleBilinear(p);
	}

	private:

	float normalizeValue(float v, WrapMode mode) const{
		if(mode == CLAMP)
			return clamp(v, 0.0, 1.0);

		int n = floor(v);
		float r = v - n;

		if(mode == MIRRORED_REPEAT && n%2 != 0)
			r = 1-r;

		return r;		
	}

	RGB sampleNearest(vec2 p) const{
		int x = clamp(round(p[0]), 0, img.width()-1);
		int y = clamp(round(p[1]), 0, img.height()-1);
		return img(x, y);
	}
		
	RGB sampleBilinear(vec2 p) const{
		int x = floor(p[0]);
		int y = floor(p[1]);
		float u = p[0] - x;
		float v = p[1] - y;
		
		int x0 = limitCoord(x, img.width(),  wrapX);
		int y0 = limitCoord(y, img.height(), wrapY);
		int x1 = limitCoord(x+1, img.width(),  wrapX);
		int y1 = limitCoord(y+1, img.height(), wrapY);

		return bilerp(u, v,
			img(x0, y0), img(x1, y0), 
			img(x0, y1), img(x1, y1)
		);
	}

	int limitCoord(int a, int len, WrapMode mode) const{
		if(a < 0)
			return mode == REPEAT? len-1: 0;
		
		if(a >= len)
			return mode == REPEAT? 0: len-1;
		
		return a;
	}
};

#endif
