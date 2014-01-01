
uniform float time;

				
void main(void)
{
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec4 vertex = gl_Vertex;
	vec4 vertexMV = gl_ModelViewMatrix * gl_Vertex;
	vec3 normalMV = gl_NormalMatrix * gl_Normal;
	
	vec4 vertexM = gl_TextureMatrix[0] * gl_ModelViewMatrix * vec4(0.0, 0.0, 0.0, 1.0);
	
	// Animation des arbres
	float move_speed = (float(int(vertexM.y*vertexM.z) % 50)/50.0 + 0.5);
	float move_offset = vertexM.x;
	vertex.x += 0.01 * pow(vertex.y, 2.0) * cos(time * move_speed + move_offset);
	
	
	vec4 vLightPosMV = -gl_LightSource[0].position;		// Lumière directionelle
	float intensity = dot(vLightPosMV.xyz, normalMV);
	gl_FrontColor = vec4(intensity, intensity, intensity, 1.0);
	gl_FrontColor.a = 1.0 - clamp(length(vertexMV)/120.0, 0.0, 1.0);
		
		
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}





