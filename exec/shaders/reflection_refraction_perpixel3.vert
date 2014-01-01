

varying vec3 vertex_view;
varying vec3 normal_view;

void main(void)
{
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
	

// PER PIXEL
	vertex_view = vec3(gl_ModelViewMatrix*gl_Vertex);
	normal_view = normalize(gl_NormalMatrix * gl_Normal);

}
