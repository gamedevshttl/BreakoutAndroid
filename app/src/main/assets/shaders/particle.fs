precision mediump float;

varying vec2 TexCoord;
varying vec4 ParticleColor;

uniform sampler2D image;

void main()
{
	gl_FragColor = ParticleColor * texture2D(image, TexCoord);
}
