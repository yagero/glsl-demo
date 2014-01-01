
uniform int win_width;
uniform int win_height;
uniform float noise_tile;
uniform float noise_factor;
uniform float time;
uniform float water_shininess;
uniform vec3 fog_color;

uniform sampler2D texWaterReflection;
uniform sampler2D texWaterNoiseNM;

varying vec3 vPixToLight;		// Vecteur du pixel courant à la lumière
varying vec3 vPixToEye;			// Vecteur du pixel courant à l'oeil
varying vec4 vPosition;

// SHADOW MAPPING //
uniform int depth_map_size;
uniform sampler2DShadow texDepthMapFromLight0;
uniform sampler2DShadow texDepthMapFromLight1;
#define Z_TEST_SIGMA 0.0001
////////////////////

float ShadowMapping(vec4 vVertexFromLightView);

float Fresnel(vec3 incident, vec3 normal, float bias, float power);

void main (void)
{
	vec2 uvNormal0 = gl_TexCoord[0].st*noise_tile;
	uvNormal0.s += time*0.01;
	uvNormal0.t += time*0.01;
	vec2 uvNormal1 = gl_TexCoord[0].st*noise_tile;
	uvNormal1.s -= time*0.01;
	uvNormal1.t += time*0.01;
		
	vec3 normal0 = texture2D(texWaterNoiseNM, uvNormal0).rgb * 2.0 - 1.0;
	vec3 normal1 = texture2D(texWaterNoiseNM, uvNormal1).rgb * 2.0 - 1.0;
	vec3 normal = normalize(normal0+normal1);
	
	vec2 uvReflection = vec2(gl_FragCoord.x/win_width, gl_FragCoord.y/win_height);
	
	vec2 uvFinal = uvReflection.xy + noise_factor*normal.xy;
	gl_FragColor = texture2D(texWaterReflection, uvFinal);
	
	vec3 N = normalize(vec3(gl_ModelViewMatrix * vec4(normal.x, normal.z, normal.y, 0.0)));
	vec3 L = normalize(vPixToLight);
	vec3 V = normalize(vPixToEye);
	float iSpecular = pow(clamp(dot(reflect(-L, N), V), 0.0, 1.0), water_shininess);
	
	gl_FragColor += gl_LightSource[0].specular * iSpecular;
	gl_FragColor = clamp(gl_FragColor, vec4(0.0, 0.0, 0.0, 0.0),  vec4(1.0, 1.0, 1.0, 1.0));

	// SHADOW MAPPING
/*
	vec4 posInLightView = vPosition;
	posInLightView.x += noise_factor*normal.x;
	posInLightView.z += noise_factor*normal.y;
	float shadow = ShadowMapping( gl_TextureMatrix[0] * posInLightView );
*/
	float shadow = ShadowMapping( gl_TexCoord[1] );
	shadow = shadow * 0.5 + 0.5;
	gl_FragColor *= shadow;

	// FRESNEL ALPHA
	gl_FragColor.a	= Fresnel(V, N, 0.5, 2.0);

}


float Fresnel(vec3 incident, vec3 normal, float bias, float power)
{
	float scale = 1.0 - bias;
	return bias + pow(1.0 - dot(incident, normal), power) * scale;
}


float ShadowMapping(vec4 vVertexFromLightView)
{
	float fShadow = 0.0;
						
	float tOrtho[2];
	tOrtho[0] = 20.0;
	tOrtho[1] = 100.0;

	if( length(vPixToEye) <= 140.0)
	{
		bool ok = false;
		int id = 0;
		vec3 vPixPosInDepthMap;

		for(int i=0; i<2; i++) 
		{
			if(!ok)
			{
				vPixPosInDepthMap = vec3(vVertexFromLightView.xy/tOrtho[i], vVertexFromLightView.z) / (vVertexFromLightView.w);
				vPixPosInDepthMap = (vPixPosInDepthMap + 1.0) * 0.5;					// de l'intervale [-1 1] à [0 1]
				
				if(vPixPosInDepthMap.x >= 0.0 && vPixPosInDepthMap.y >= 0.0 && vPixPosInDepthMap.x <= 1.0 && vPixPosInDepthMap.y <= 1.0)
				{
					id = i;
					ok = true;
				}
			}
		}
		
		if(ok)
		{
			vec4 vDepthMapColor = vec4(0.0, 0.0, 0.0, 1.0);
			if(id == 0)	vDepthMapColor = shadow2D(texDepthMapFromLight0, vPixPosInDepthMap);
			else		vDepthMapColor = shadow2D(texDepthMapFromLight1, vPixPosInDepthMap);


			if((vDepthMapColor.z+Z_TEST_SIGMA) < vPixPosInDepthMap.z)
			{
				fShadow = clamp((vPixPosInDepthMap.z - vDepthMapColor.z)*10.0, 0.0, 1.0);
			}
			else
			{
				fShadow = 1.0;
			}

			fShadow = clamp(fShadow, 0.0, 1.0);
		}
		else fShadow = 1.0;
	}
	else fShadow = 1.0;
	
	return fShadow;
}


