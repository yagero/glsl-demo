varying vec3 normal, viewVec, lightVec;

void main(void)
{
gl_TexCoord[0] = gl_MultiTexCoord0;
	vec3 vertex = (gl_ModelViewMatrix*gl_Vertex).xyz;
	viewVec = -vertex;
	lightVec = gl_LightSource[0].position.xyz-vertex;
	gl_FrontColor = gl_Color;
	normal = gl_NormalMatrix*gl_Normal;
	gl_Position = ftransform();
}
