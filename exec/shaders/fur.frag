
uniform sampler2D texDiffuse;
uniform sampler2D texFur;
uniform float current_layer;

varying vec3 vNormalMV;
varying vec4 vLightMV;
varying vec3 vEyeMV;

void main(void)
{
	float att = 1.0;/*
	if(vLightMV.w != 0.0) {
		float dist = length(vLightMV.xyz);
		att = 1.0/(gl_LightSource[0].constantAttenuation + gl_LightSource[0].linearAttenuation*dist + gl_LightSource[0].quadraticAttenuation*dist*dist);
		att = max(att, 0.0);
	}*/

	vec3 L = normalize(vLightMV.xyz);
	vec3 N = normalize(vNormalMV.xyz);
	vec3 V = normalize(vEyeMV.xyz);
	
	vec4 base = texture2D(texDiffuse, gl_TexCoord[0].st);	// Couleur diffuse
	
	float iDiffuse = max(dot(L, N), 0.0);	// Intensité diffuse
	float iSpecular = pow(clamp(dot(reflect(-L, N), V), 0.0, 1.0), gl_FrontMaterial.shininess/10.0 ) /3.0;

	vec4 cAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 cDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * iDiffuse;	
	vec4 cSpecular = gl_LightSource[0].specular * gl_FrontMaterial.specular * iSpecular;

	
	gl_FragColor = cAmbient * base + (cDiffuse * base + cSpecular) * att;
	
	// Poil ou pas poil ?
	vec4 furColor = texture2D(texFur,  gl_TexCoord[0].st);
	gl_FragColor.x *= furColor.x;
	gl_FragColor.y *= furColor.y;
	gl_FragColor.z *= furColor.z;
	gl_FragColor.a = (1.0-current_layer) * furColor.a;
}
