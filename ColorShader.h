#ifndef COLOR_SHADER_H
#define COLOR_SHADER_H

struct ColorShader{
	struct Varying{
		vec4 position;
		vec3 color;
	};

	mat4 M;

	template<class Vertex>
	void vertexShader(Vertex in, Varying& out){
		out.position = M*getPosition(in);
		out.color = getColor(in);
	}

	void fragmentShader(Varying V, RGB& FragColor){
		FragColor = toColor(V.color);
	}
};

#endif
