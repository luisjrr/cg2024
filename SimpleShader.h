#ifndef SIMPLE_SHADER_H
#define SIMPLE_SHADER_H

#include "matrix.h"
#include "Color.h"
#include "VertexUtils.h"

struct SimpleShader{
	struct Varying{
		vec4 position;
	};

	mat4 M;
	RGB C;

	template<class Vertex>
	void vertexShader(Vertex in, Varying& out){
		out.position = M*getPosition(in);
	}

	void fragmentShader(Varying, RGB& fragColor){
		fragColor = C;
	}
};

#endif
