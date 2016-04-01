//#version 130
#version 120

uniform float saturation = 1.0;
uniform float value = 1.0;

uniform float time;

// random start

// Bob Jenkins One-At-A-Time hashing algorithm.
/*uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }*/

// GLSL 1.2 compatable implementation
float random(vec3 n)
{
  return 0.5 + 0.5 * fract(sin(dot(vec3(n.xy, sin(2.465*n.z)), vec3(12.9898, 78.233, -4.233)))*43758.5453);
}

// random end

uniform sampler2D texture;

void main()
{
	// lookup the pixel in the texture
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

	float avg = 0.21 * pixel.r + 0.71 * pixel.g + 0.08 * pixel.b;
	vec4 grey = vec4(avg, avg, avg, pixel.a);

	float randsat = 1.0-(random(vec3( gl_FragCoord.xy, time))*0.5);
	float randval = 1.0-(random(vec3( gl_FragCoord.xy, time))*0.05);
	gl_FragColor = gl_Color * mix( grey, pixel, saturation * randsat) * (value * randval);
}
