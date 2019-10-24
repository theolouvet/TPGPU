/* Lava 2.0 by Viktor Heisenberger, TheVico87, 2013.09.28.
 *
 * Uses 3d simplex noise by Nikita Miropolskiy, nikat/2013
 * (https://www.shadertoy.com/view/XsX3zB)
 *
 * This work is licensed under a 
 * Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-nc-sa/3.0/
 *  - You must attribute the work in the source code 
 *  - You may not use this work for commercial purposes.
 *  - You may distribute a derivative work only under the same license.
 */

/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */

uniform float time;

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) {
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
	 
	 /* calculate s and x */
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));
	 
	 /* calculate i1 and i2 */
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	 /* x1, x2, x3 */
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;
	 
	 /* 2. find four surflets and store them in d */
	 vec4 w, d;
	 
	 /* calculate surflet weights */
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);
	 
	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	 w = max(0.6 - w, 0.0);
	 
	 /* calculate surflet components */
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);
	 
	 /* multiply d by w^4 */
	 w *= w;
	 w *= w;
	 d *= w;
	 
	 /* 3. return the sum of the four surflets */
	 return dot(d, vec4(52.0));
}

float fractal(vec3 p) {
	vec3 p2 = vec3(0.1 * p.xy, 0.1 * p.z);

	return	0.333333 * simplex3d(3.0 * p2) +
			0.111111 * simplex3d(9.0 * p2) +
			0.037037 * simplex3d(27.0 * p2);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	const vec3 color1 = vec3(0.9,0.2,0.2);
	const vec3 color2 = vec3(1.0,1.0,0.0);
	const vec3 crustColor = vec3(0.478431, 0.427451, 0.396078) * 0.5;
	
	iTime = time * 0.3;
	
	vec2 p = fragCoord.xy / 10;
	vec3 p3 = vec3(p, iTime*0.025);
	vec3 p3_2 = vec3(p, iTime*0.045 + 5.0);
	
	float value;
	

	value = simplex3d(p3*28.0);
	value = 0.5 + 0.5*value;
	float crustValue = fractal(p3*28.0);
	crustValue *= 5.0;
	crustValue = clamp(pow(crustValue, 4.0), 0.0, 1.0);

	fragColor = vec4(
		(color1 + (color2 - color1) * value) * (1.0-crustValue) + crustColor * crustValue,
		1.0);

	return;
}
