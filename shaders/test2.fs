#version 330 core

in vec2 uv;
in vec3 n;
in vec3 pos;
uniform float time;
uniform vec3 POS_CAM;
uniform sampler2D textureSampler;
out vec4 color;
float Ia = 0.2;
float Kd = 0.8;
float ks = 0.2;
float es = 38;
float Id;
float Is;

vec3 lum = vec3(10,-10,10); 
vec3 ul = normalize(lum -pos);
vec3 t = normalize(POS_CAM - pos);
vec4 nt0 = normalize(texture( textureSampler, uv + time/25 * vec2(1,0) ));
vec3 nt = normalize(vec3(nt0.xyz));
vec3 s = normalize(reflect(ul, nt));


void main()
{
    Id = Kd * clamp(dot(nt, ul),0,1);
    Is = ks * pow( clamp(dot(s,t),0,1), es);
   
    color = (Ia + Id) *  texture(textureSampler, uv) + Is* vec4(1,1,1,1);
};