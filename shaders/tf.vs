#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
uniform float time;
out vec3 pos;
out vec3 n;
out vec2 uv;
mat3 rotation;
float t;

void main()
{
  t = time/50 * 2 * 3.14;
  rotation[0] = vec3(cos(t),0,-sin(t));
  rotation[1] = vec3(0,1,0);
  rotation[2] = vec3(sin(t),0,cos(t));
	pos = rotation * position ;
	n = normal;
	uv = texcoord;
};
