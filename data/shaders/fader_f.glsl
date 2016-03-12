


//uniform float hue;
uniform float saturation = 1.0f;
uniform float value = 1.0f;

uniform sampler2D texture;

void main()
{
	// lookup the pixel in the texture
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

	float avg = 0.21 * pixel.r + 0.71 * pixel.g + 0.08 * pixel.b;
	vec4 grey = vec4(avg, avg, avg, 1.0);

	gl_FragColor = gl_Color * mix( grey, pixel, saturation) * value;
}
