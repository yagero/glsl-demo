
// Bounding Box du terrain
uniform vec3 bbox_min;
uniform vec3 bbox_max;

varying vec3 vPixToLight;		// Vecteur du pixel courant à la lumière
varying vec3 vPixToEye;			// Vecteur du pixel courant à l'oeil
varying vec4 vPosition;
		
void main(void)
{
	gl_Position = ftransform();
	
	vPosition = gl_Vertex;
	vec3 vPositionNormalized = (gl_Vertex.xyz - bbox_min.xyz) / (bbox_max.xyz - bbox_min.xyz);
	gl_TexCoord[0].st = vPositionNormalized.xz;
	
	vPixToLight = -(gl_LightSource[0].position.xyz);		// Position (ou direction) de la lumière dans la MV
	vPixToEye = -vec3(gl_ModelViewMatrix * gl_Vertex);		// Position du vertex dans la MV
	
	// on multiplie par la matrice de la lumière : position du Vertex dans le repère de la lumière
	gl_TexCoord[1] = gl_TextureMatrix[0] * gl_Vertex;		
}
