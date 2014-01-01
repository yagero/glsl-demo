// Texture to blur
uniform sampler2D texScreen;

// Texture size
uniform vec2 size;

// blur horizontal ou vertical
uniform bool horizontal;

// Taille du blurage
uniform int kernel_size;



void main()
{
	vec2 pas = 1.0/size;
	vec2 uv = gl_TexCoord[0].st;
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	
	// HORIZONTAL BLUR
	if(horizontal) {
		int j = 0;
		int sum = 0;
		for(int i=-kernel_size; i<=kernel_size; i++) {
			vec4 value = texture2D(texScreen, uv + vec2(pas.x*i, 0.0));
			int factor = kernel_size+1 - (int)abs((float)i);
			color += value * factor;
			sum += factor;
		}
		color /= sum;
	}
	
	// VERTICAL BLUR
	else {
		int j = 0;
		int sum = 0;
		for(int i=-kernel_size; i<=kernel_size; i++) {
			vec4 value = texture2D(texScreen, uv + vec2(0.0, pas.y*i));
			int factor = kernel_size+1 - (int)abs((float)i);
			color += value * factor;
			sum += factor;
		}
		color /= sum;
	}
	
	gl_FragColor = color;
	gl_FragColor.a = 1.0;
}
