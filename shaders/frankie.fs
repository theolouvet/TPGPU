#version 330 core

in vec2 uv;
in vec3 n;
in vec3 pos;
uniform vec3 POS_CAM;
out vec4 color;

uniform sampler2D textureSampler;

float Ia = 0.2;
float Kd = 0.8;
float ks = 0.2;
float es = 28;
float Is;
float Id;
vec3 lum = vec3(0,1,0);
vec3 ul = normalize(pos-lum);
vec3 t = normalize(pos-POS_CAM);
vec3 s = normalize(reflect(ul, n));

void main()
{
    Id = Kd * dot(ul , n);
    
    Is = ks * clamp(pow(dot(s,t),es),0,1);	
    color = (Ia + Id) * texture( textureSampler, uv ) + Is* vec4(1,1,1,1) ;
};
