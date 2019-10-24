#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec3 pos;
out vec2 uv;
out vec3 n;


uniform mat4 MVP;


float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
};

void main()
{
  gl_Position = MVP*vec4(position, 1.0) + 2 * rand(uv);
  pos = position;
  uv = texcoord;
  n = normal;
};
