

varying vec4 vPixToLightTBN;		// Vecteur du pixel courant à la lumière
varying vec3 vPixToEyeTBN;			// Vecteur du pixel courant à l'oeil
varying vec3 vPosition;
varying vec3 vPositionNormalized;

// Bounding Box du terrain
uniform vec3 bbox_min;
uniform vec3 bbox_max;

				
void main(void)
{
	gl_Position = ftransform();
	
	// Position du vertex
	vPosition = gl_Vertex.xyz;
	
	// Position du vertex si le terrain est compris entre 0.0 et 1.0
	vPositionNormalized = (gl_Vertex.xyz - bbox_min.xyz) / (bbox_max.xyz - bbox_min.xyz);
	
	// Coordonnées de texture
	gl_TexCoord[0].st = vPositionNormalized.xz;
	
	
	// Calcul de l'espace TBN
	vec3 vTangent = gl_MultiTexCoord0.xyz;
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * vTangent);
	vec3 b = cross(t, n);
	
	vec4 vLightPosMV = gl_LightSource[0].position;			// Position (ou direction) de la lumière dans la MV
	vec3 vVertexMV = vec3(gl_ModelViewMatrix * gl_Vertex);	// Position du vertex dans la MV
	
	vec3 tmpVec = -vLightPosMV.xyz;					// Lumière directionelle

	// Vecteur lumière dans l'espace TBN
	vPixToLightTBN.x = dot(tmpVec, t);
	vPixToLightTBN.y = dot(tmpVec, b);
	vPixToLightTBN.z = dot(tmpVec, n);
	vPixToLightTBN.w = vLightPosMV.w;	// ponctuelle ou directionnelle
	
	// Vecteur vue dans l'espace TBN
	tmpVec = -vVertexMV;
	vPixToEyeTBN.x = dot(tmpVec, t);
	vPixToEyeTBN.y = dot(tmpVec, b);
	vPixToEyeTBN.z = dot(tmpVec, n);
	
	
	// on multiplie par la matrice de la lumière : position du Vertex dans le repère de la lumière
	gl_TexCoord[1] = gl_TextureMatrix[0] * gl_Vertex;	
	
}





