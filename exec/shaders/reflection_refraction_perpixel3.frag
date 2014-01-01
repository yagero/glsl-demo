
// Paramètres
uniform bool enable_diffuse;
uniform bool enable_glossmap;
uniform bool enable_reflection;
uniform bool enable_refraction;
uniform bool enable_chromatic_aberration;
uniform bool enable_bumpmapping;
uniform float bumpmapping_factor;
uniform float refraction_eta;
uniform float fresnel_bias;
uniform float diffuse_intensity;

// Textures
uniform samplerCube texCubeMap;
uniform sampler2D texDiffuse;
uniform sampler2D texGlossMap;
uniform sampler2D texNormalMap;


varying vec3 vertex_view;
varying vec3 normal_view;




void main(void)
{

	// Rayon incident
	vec3 incident_view = normalize(vertex_view);
	vec3 incident_global = normalize(vec3(gl_TextureMatrix[0]*vec4(incident_view, 0.0)));
	
	// Diffuse
	vec4 diffuse = texture2D(texDiffuse, gl_TexCoord[0].st);

	// Normal
	vec3 normal_final_view = normalize(normal_view);


	if(enable_bumpmapping) {
		normal_final_view += bumpmapping_factor*(2.0 * texture2D(texNormalMap, gl_TexCoord[0].st).rgb - 1.0);	// DOT3 bump mapping
//		vec3 bump = bumpmapping_factor*(2.0 * texture2D(texNormalMap, gl_TexCoord[0].st) - 1.0);
	
	}	

	normal_final_view = normalize(normal_final_view);
	vec3 normal_final_global = (gl_TextureMatrix[0]*vec4(normal_final_view, 0.0)).xyz; 
	normal_final_global = normalize(normal_final_global);
	

	// Reflection
	vec4 reflection = vec4(1.0, 1.0, 1.0, 1.0);
	if(enable_reflection) {
		vec3 reflect_global = reflect(incident_global.xyz, normal_final_global);
		reflect_global = normalize(reflect_global);
		reflection = textureCube(texCubeMap, reflect_global);
	}
	


	// Refraction
	vec4 refraction = vec4(1.0, 1.0, 1.0, 1.0);
	if(enable_refraction) {	
		vec3 u_eta_ratio = vec3(refraction_eta+0.015, refraction_eta+0.0, refraction_eta-0.015);
		if(!enable_chromatic_aberration) u_eta_ratio = vec3(refraction_eta, refraction_eta, refraction_eta);
		refraction.r = textureCube(texCubeMap, normalize(refract(incident_global, normal_final_global, u_eta_ratio.r))).r;
		refraction.g = textureCube(texCubeMap, normalize(refract(incident_global, normal_final_global, u_eta_ratio.g))).g;
		refraction.b = textureCube(texCubeMap, normalize(refract(incident_global, normal_final_global, u_eta_ratio.b))).b;
	}
	gl_FragColor = refraction;


	bool bAddDiffuse = enable_diffuse;
	
	if(enable_reflection && enable_refraction) {
		// Fresnel terms :
		float power = 2.0;
		float scale = 1.0 - fresnel_bias;
		
		float fresnel = fresnel_bias + pow(1.0 - dot(-incident_view, normal_final_view), power) * scale;	
	//	float fresnel = 1.0 - dot(-incident_view, normal_final_view);
	
		gl_FragColor = mix(refraction, reflection, fresnel);
	}
	else if(enable_reflection)
		gl_FragColor = reflection;
	else if(enable_refraction)
		gl_FragColor = refraction;
	else if(enable_diffuse) {
		gl_FragColor = diffuse;
		bAddDiffuse = false;
	}
	else {
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		bAddDiffuse = false;
	}
	

	if(bAddDiffuse) {
		float reflectivity = diffuse_intensity;
		/*
		if(enable_glossmap)	{
			vec3 gloss_color = texture2D(texGlossMap, gl_TexCoord[0].st).rgb;
			reflectivity *= (gloss_color.r + gloss_color.g + gloss_color.b)/3.0;
		}*/
		
		gl_FragColor = mix(gl_FragColor, diffuse, reflectivity);
	}
		
	
	gl_FragColor.a = 1.0;

}
