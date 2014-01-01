
uniform sampler2D texLeftEye;
uniform sampler2D texRightEye;

void main(void)
{
	vec4 colorLeftEye = texture2D(texLeftEye, gl_TexCoord[0].st);
	vec4 colorRightEye = texture2D(texRightEye, gl_TexCoord[0].st);
	gl_FragColor = vec4(colorLeftEye.r, colorRightEye.g, colorRightEye.b, 1.0);
}
