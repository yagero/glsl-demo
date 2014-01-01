

uniform sampler2D texDiffuse;

void main (void)
{
	vec4 cBase = texture2D(texDiffuse, gl_TexCoord[0].st);
	if(cBase.a < 0.4) discard;
	
	vec4 cAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 cDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * gl_Color;	
	
	gl_FragColor = cAmbient * cBase + cDiffuse * cBase;
	gl_FragColor.a = gl_Color.a;
	
}














