attribute vec4 vertex;

varying vec2 TexCoord;
varying vec4 ParticleColor;


uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;
uniform float scale;

void main()
{
	TexCoord = vertex.zw;
	ParticleColor = color;

	gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
}
