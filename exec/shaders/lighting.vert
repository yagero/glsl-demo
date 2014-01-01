

varying vec4 vPixToLightTBN[1];	// Vecteur du pixel courant à la lumière
varying vec3 vPixToEyeTBN;					// Vecteur du pixel courant à l'oeil
varying vec3 vVertexMV;
varying vec3 vNormalMV;
varying vec3 vPixToLightMV;
varying vec3 vLightDirMV;

// SHADOW MAPPING //
uniform int enable_shadow_mapping;
////////////////////


#define MODE_PHONG		0
#define MODE_BUMP		1
#define MODE_PARALLAX	2
#define MODE_RELIEF		3
uniform int mode;

#define LIGHT_DIRECTIONAL		0.0
#define LIGHT_OMNIDIRECTIONAL	1.0
#define LIGHT_SPOT				2.0
				
void main(void)
{

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec3 vTangent = gl_MultiTexCoord1.xyz;
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * vTangent);
	vec3 b = cross(n, t);
	
	vNormalMV = n;
	
	vec4 vLightPosMV = gl_LightSource[0].position;		// Position (ou direction) de la lumière dans la MV
	vVertexMV = vec3(gl_ModelViewMatrix * gl_Vertex);	// Position du vertex dans la MV
	
	vec3 tmpVec;



	if(vLightPosMV.w == LIGHT_DIRECTIONAL)
		tmpVec = -vLightPosMV.xyz;					// Lumière directionelle
	else
		tmpVec = vLightPosMV.xyz - vVertexMV.xyz;	// Lumière ponctuelle

	vPixToLightMV = tmpVec;
/*
	if(mode == MODE_PHONG)
	{
		vPixToLightTBN[0].xyz = tmpVec.xyz;
		vPixToLightTBN[0].w = vLightPosMV.w;	// ponctuelle ou directionnelle
		
		vPixToEyeTBN = -vVertexMV;
	}
	else*/
	{
		// Position ou direction de la lumière
		vPixToLightTBN[0].x = dot(tmpVec, t);
		vPixToLightTBN[0].y = dot(tmpVec, b);
		vPixToLightTBN[0].z = dot(tmpVec, n);
		vPixToLightTBN[0].w = vLightPosMV.w;	// ponctuelle ou directionnelle
			
		// Vecteur vue
		tmpVec = -vVertexMV;
		vPixToEyeTBN.x = dot(tmpVec, t);
		vPixToEyeTBN.y = dot(tmpVec, b);
		vPixToEyeTBN.z = dot(tmpVec, n);
	}
	
	
	
	if(length(gl_LightSource[0].spotDirection) > 0.001)
	{
		// Lumière spot
		vLightDirMV = normalize(gl_LightSource[0].spotDirection);
		vPixToLightTBN[0].w = LIGHT_SPOT;
	}
	else
	{
		// Lumière non spot
		vLightDirMV = gl_LightSource[0].spotDirection;
	}
	
	if(enable_shadow_mapping != 0) {
		// pos a subit les transformations + la caméra
		vec4 pos = gl_ModelViewMatrix * gl_Vertex;
		// on multiplie par la matrice inverse de la caméra : pos a seulement subit les transformations
		pos = gl_TextureMatrix[0] * pos;
		// on multiplie par la matrice de la lumière : position du Vertex dans le repère de la lumière
		gl_TexCoord[1] = gl_TextureMatrix[1] * pos;
	}
}
