

uniform float fur_length;		// taille maxi des poils
uniform float current_layer;	// entre 0.0 et 1.0
uniform float tile_factor;		// facteur de répétition  de la texture
uniform float fur_weakness;		// force des poils (0 = très fort, 1 = faible (tombera sous la gravité)
uniform float time;

varying vec3 vNormalMV;
varying vec4 vLightMV;
varying vec3 vEyeMV;


void main(void)
{
	
	vec3 vVertexMV = (gl_ModelViewMatrix * gl_Vertex).xyz;
	vEyeMV = -vVertexMV;
	vNormalMV = normalize(gl_NormalMatrix * gl_Normal);
	gl_TexCoord[0] = gl_MultiTexCoord0 * tile_factor;


	// Longueur de poil de la couche courante
	float L = current_layer*fur_length;

	// Position du vertex
	vec3 P = vVertexMV.xyz + (vNormalMV.xyz * L) ;

	// Apply gravity //
	vec3 vGravity = (gl_TextureMatrix[0]*vec4(0.0, -1.0*L*fur_weakness, 0.0, 0.0)).xyz;
	P += (vGravity.xyz * pow(L, 3.0));
	///////////////////

//	float wind = cos(time) + mod((gl_TexCoord[0].s)*10, 1.0);
//	P.x += cos(time) + mod((gl_TexCoord[0].s)*10, 1.0);
//	P.y += sin(time) + mod((gl_TexCoord[0].t)*10, 1.0);

	
//	gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * vec4(P, 1.0);
	
	
	// Lumière
	if(gl_LightSource[0].position.w == 0.0)
		vLightMV.xyz = -gl_LightSource[0].position.xyz;					// Lumière directionelle
	else
		vLightMV.xyz = gl_LightSource[0].position.xyz - vVertexMV.xyz;	// Lumière ponctuelle
	vLightMV.w = gl_LightSource[0].position.w;	
}
