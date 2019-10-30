#version 330 core

in vec2 uv;
in vec3 n;
in vec3 pos;
uniform vec3 POS_CAM;
out vec4 color;

uniform float time;
uniform sampler2D textureSampler;
uniform sampler2D textureSamplerM;
uniform sampler2D textureGrass;
uniform sampler2D textureGrass2;

float Ia = 0.2;
float Kd = 0.8;
float ks = 0.2;
float es = 38;
float Is;
float Id;
float Ist;

vec3 lum = vec3(10,-10,10);
vec3 ul = normalize(lum -pos);
vec3 t = normalize(POS_CAM - pos);
vec4 nt4 = normalize(texture( textureSamplerM, uv + time/50 * vec2(1, 0)));
vec4 nt0 = normalize(texture( textureSamplerM, uv ));
vec3 nt = normalize(vec3(nt4.xyz));
vec3 s = normalize(reflect(ul, nt));
vec3 st = normalize(reflect(ul, nt0.xyz));

//Grass illumination and bump map

vec4 ntGrass = normalize(texture( textureGrass, uv ));
vec3 sGrass = normalize(reflect(ul, ntGrass.xyz));
float IdG;
float IsG;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
};

void main()
{
    Id = Kd * clamp(dot(nt, ul),0,1);
    Is = ks * pow( clamp(dot(s,t),0,1), es);
    Ist = ks * pow( clamp(dot(st,t),0,1), es);

    if(pow(uv.x - 0.5 ,2) + pow(uv.y - 0.5,2) < pow(0.25, 2)  ){
        color = (Ia + Id) * vec4(0,0.47,0.75,1)  + Is* vec4(1,1,1,1) ;
        
        //if(length(texture(textureSampler, uv).rgb) < 0.9f){
        color = 0.1*color + 0.9*((Ia + Id) *texture(textureSampler, uv) + Ist* vec4(1,1,1,1)) ;
	    //}
    }else{
        IdG =  Kd * clamp(dot(ntGrass.xyz, ul),0,1);
        IsG =  ks * pow( clamp(dot(sGrass,t),0,1), es);
        //color =;
        color = (Ia + IdG) *  texture(textureGrass, uv) + IsG* vec4(1,1,1,1);
    }

   

};
