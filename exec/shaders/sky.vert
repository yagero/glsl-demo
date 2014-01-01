
varying vec3 vertex;
varying vec3 vertexMV;

void main(void)
{
	vertex = normalize(gl_Vertex.xyz);
	vertexMV = normalize(gl_ModelViewMatrix * vec4(-gl_Vertex.x, gl_Vertex.y, -gl_Vertex.z, 1.0)).xyz;	
	
	gl_Position = ftransform();

	
}
