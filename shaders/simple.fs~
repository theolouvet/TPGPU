#version 330 core

in vec2 uv;
in vec3 n;
in vec3 pos;
uniform vec3 POS_CAM;
out vec4 color;

uniform float time;
uniform sampler2D textureSampler;
uniform sampler2D textureSamplerM;

float Ia = 0.2;
float Kd = 0.8;
float ks = 0.2;
float es = 128;
float Is;
float Id;
float Ist;

vec3 lum = vec3(10,-10,10);
vec3 ul = normalize(lum -pos);
vec3 t = normalize(POS_CAM - pos);
vec4 nt4 = normalize(texture( textureSamplerM, uv + time/10 * vec2(0, 1) ));
vec4 nt0 = normalize(texture( textureSampler, uv ));
vec3 nt = normalize(vec3(nt4.xyz));
vec3 s = normalize(reflect(ul, nt));
vec3 st = normalize(reflect(ul, nt0.xyz));

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
};

void main()
{
    Id = Kd * clamp(dot(nt, ul),0,1);
    Is = ks * pow( clamp(dot(s,t),0,1), es);
    Ist = ks * pow( clamp(dot(st,t),0,1), es);
    if(length(texture(textureSampler, uv).rgb) > 0.9f){
	color = (Ia + Id) * vec4(0,0,1,1)  + Is* vec4(1,1,1,1) ;
	}
    else{
	color = (Ia + Id) *texture(textureSampler, uv) + Ist* vec4(1,1,1,1) ;
	}
   

};
