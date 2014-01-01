varying vec3 normal, viewVec, lightVec;
uniform sampler2D myText;
uniform int alphaTex;
uniform bool enable_texture;

void main()
{
	vec4 colorTex = vec4(1.0, 1.0, 1.0, 1.0);
	
	if(enable_texture)
		colorTex = texture2D(myText, gl_TexCoord[0].st);
		
	vec4 color;	
	float facteur;
	
	vec3 N=normalize(normal);
	vec3 L=normalize(lightVec);		
	vec3 V=normalize(viewVec);	
	vec3 R=normalize(2.0*dot(N,V)*N-V);
	// Intensité diffuse
	float LdotN=dot(L,N);
	// Intensité spéc
	float LdotR=dot(L,R);

	// diffus 3 niveaux
	if(LdotN<0.2)
		facteur=0.5;
	else if(LdotN<0.5)
		facteur=0.75;
	else
		facteur=0.9;
	
	color = gl_FrontMaterial.ambient + (gl_FrontMaterial.diffuse * facteur);

	// speculaire
	if(LdotR>0.95)
		color += gl_FrontMaterial.specular * pow(clamp(LdotR,0.0,1.0), 20.0);
		
	if(color.r>1.0) color.r=1.0;
	if(color.g>1.0) color.g=1.0;
	if(color.b>1.0) color.b=1.0;
	
	gl_FragColor = color * colorTex;
	gl_FragColor.a = 1.0;
} 

