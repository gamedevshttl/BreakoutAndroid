//#version 330 core
precision mediump float;

//in vec2 TexCoords;
//out vec4 color;

varying vec2 TexCoords;

uniform sampler2D scene;

uniform vec2 offsets[9];
uniform float edge_kernel[9];
uniform float blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
	//color = vec4(0.0f);
	vec3 sample[9];

	if(chaos || shake)
		for(int i = 0; i < 9; i++ )
			sample[i] = vec3(texture2D(scene, TexCoords.st + offsets[i]));

	if(chaos){
		for(int i = 0; i<9; i++)
			gl_FragColor += vec4(sample[i] * edge_kernel[i], 0.0f);
		gl_FragColor.a = 1.0;
	}
	else if(confuse){
		gl_FragColor = vec4(1.0 - texture2D(scene, TexCoords).rgb, 1.0);
	}
	else if(shake){	
		for(int i = 0; i<9; i++)
			gl_FragColor += vec4(sample[i] * blur_kernel[i], 0.0f);
		gl_FragColor.a = 1.0;
	}
	else{
		gl_FragColor = texture2D(scene, TexCoords);
	}


	//gl_FragColor = texture2D(scene, TexCoords.st);
}