#version 330 core
in float inValue;
out float outValue;


uniform mat4 MVP;

void main()
{
  outValue = 0.2f * inValue;
};
