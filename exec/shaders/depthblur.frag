

uniform sampler2D texScreen, texDepth;

uniform int screenWidth;
uniform int screenHeight;

// static array initialisation doesn't compile on ATI gpu
#define MASK11 (1.0/11.0)
#define MASK5  (1.0/5.0)
#define MASK3  (1.0/3.0)

uniform bool bHorizontal;


vec4 convolH(float value, int size)
{
	float stepX = 1.0/screenWidth;
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	int k = (size/2);

	for(int i=-k; i<=k; i++)
		color += value * texture2D(texScreen, gl_TexCoord[0].st + vec2(i*stepX, 0));
				
	return color;
}

vec4 convolV(float value, int size)
{
	float stepY = 1.0/screenHeight;
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	int k = (size/2);

	for(int i=-k; i<=k; i++)
		color += value * texture2D(texScreen, gl_TexCoord[0].st + vec2(0, i*stepY));
				
	return color;
}

vec4 Pdc(vec4 color)
{
	float depth = texture2D(texDepth, gl_TexCoord[0].st).r;
	if(depth > 0.997f)
	{
		vec4 color;
		if(bHorizontal)
		{
			color = convolH(MASK11, 11);
		}
		else
		{
			color = convolV(MASK11, 11);
		}
		
		return color;
	}
	else if(depth > 0.995f)
	{
		vec4 color;
		if(bHorizontal)
		{
			color = convolH(MASK5, 5);
		}
		else
		{
			color = convolV(MASK5, 5);
		}
		
		return color;
	}
	else if(depth > 0.994f)
	{
		vec4 color;
		if(bHorizontal)
		{
			color = convolH(MASK3, 3);
		}
		else
		{
			color = convolV(MASK3, 3);
		}
		
		return color;
	}
	else
	{
		return texture2D(texScreen, gl_TexCoord[0].st);
	}
}

void main()
{
	gl_FragColor = Pdc(gl_FragColor);
}