precision mediump float;

varying vec2 TexCoords;

uniform sampler2D image;
uniform vec3 textColor;

void main(){
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(image, TexCoords).a);
	gl_FragColor = vec4(textColor, 1.0) * sampled;
}