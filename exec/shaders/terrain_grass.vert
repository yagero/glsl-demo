
uniform float time;
uniform float lod_metric;
				
void main(void)
{
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec4 vertex = gl_Vertex;
	vec4 vertexMV = gl_ModelViewMatrix * gl_Vertex;
	vec3 normalMV = gl_NormalMatrix * gl_Normal;
	
	if(gl_Normal.y < 0.0) {
		normalMV = -normalMV;
		vertex.x += 0.5*cos(time) * cos(vertex.x) * sin(vertex.x);
		vertex.z += 0.5*sin(time) * cos(vertex.x) * sin(vertex.x);
		
	}
	
	vec4 vLightPosMV = -gl_LightSource[0].position;		// Lumière directionelle
	float intensity = dot(vLightPosMV.xyz, normalMV);
	gl_FrontColor = vec4(intensity, intensity, intensity, 1.0);
	gl_FrontColor.a = 1.0 - clamp(length(vertexMV)/lod_metric, 0.0, 1.0);
		
		
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
	
	// on multiplie par la matrice de la lumière : position du Vertex dans le repère de la lumière
	gl_TexCoord[1] = gl_TextureMatrix[0] * gl_Vertex;		
}





