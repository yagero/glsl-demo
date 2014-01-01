// Texture to blur
uniform sampler2D texScreen;

// Seuil
uniform float threshold;

void main()
{	
	vec4 value = texture2D(texScreen, gl_TexCoord[0].st);
		
	if( (value.r + value.g + value.b)/3.0 > threshold )
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	else
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

}
