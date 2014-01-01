

varying vec4 vPixToLightTBN;	// Vecteur du pixel courant à la lumière
varying vec3 vPixToEyeTBN;		// Vecteur du pixel courant à l'oeil
varying vec3 vPosition;
varying vec3 vPositionNormalized;

uniform sampler2D texDiffuseMap;
uniform sampler2D texNormalHeightMap;
uniform sampler2D texDiffuse0;
uniform sampler2D texDiffuse1;
uniform sampler2D texDiffuse2;
uniform sampler2D texWaterCaustics;

uniform float parallax_factor;
uniform float detail_scale;
uniform float diffuse_scale;

uniform bool water_reflection_rendering;
uniform float water_height;
uniform vec3 fog_color;
uniform float time;

// Bounding Box du terrain
uniform vec3 bbox_min;
uniform vec3 bbox_max;

// SHADOW MAPPING //
uniform int depth_map_size;
uniform sampler2D texDepthMapFromLight0;
uniform sampler2D texDepthMapFromLight1;
#define Z_TEST_SIGMA 0.0001
////////////////////


float ShadowMapping(vec4 vVertexFromLightView);
vec4 NormalMapping(vec2 uv, vec3 vPixToEyeTBN, vec4 vPixToLightTBN, bool bParallax);
vec4 ReliefMapping(vec2 uv);
vec4 CausticsColor();
float CausticsAlpha();
bool isUnderWater();

void main (void)
{
	// Clip plane dans le cas du rendu de la réflexion
	if(water_reflection_rendering)
		if(isUnderWater())
			discard;
	
	vec4 vPixToLightTBNcurrent = vPixToLightTBN;
	
	gl_FragColor = NormalMapping(gl_TexCoord[0].st, vPixToEyeTBN, vPixToLightTBNcurrent, false);
	
	
	if(isUnderWater()) {
		float alpha = CausticsAlpha();
		gl_FragColor = (1-alpha) * gl_FragColor + alpha * CausticsColor();
	}

}

bool isUnderWater()
{
	return (vPosition.y < water_height);
}

float CausticsAlpha()
{
	return (water_height - vPosition.y) / (2*(water_height - bbox_min.y));
}

vec4 CausticsColor()
{
	vec2 uv0 = gl_TexCoord[0].st*100.0;
	uv0.s -= time*0.1;
	uv0.t += time*0.1;
	vec4 color0 = texture2D(texWaterCaustics, uv0);
	
	vec2 uv1 = gl_TexCoord[0].st*100.0;
	uv1.s += time*0.1;
	uv1.t += time*0.1;	
	vec4 color1 = texture2D(texWaterCaustics, uv1);
	
	return (color0 + color1) /2;	
}



vec4 NormalMapping(vec2 uv, vec3 vPixToEyeTBN, vec4 vPixToLightTBN, bool bParallax)
{	
	vec3 lightVecTBN = normalize(vPixToLightTBN.xyz);
	vec3 viewVecTBN = normalize(vPixToEyeTBN);

	vec2 uv_detail = uv * detail_scale;
	vec2 uv_diffuse = uv * diffuse_scale;

	
	// on trouve la normale pertubée dans l'espace TBN
	vec3 normalTBN = texture2D(texNormalHeightMap, uv_detail).rgb * 2.0 - 1.0;
	normalTBN = normalize(normalTBN);
//	vec3 normalTBN = vec3(0.0, 0.0, 1.0);
	
//// ECLAIRAGE :
	// Couleur diffuse
	vec4 tBase[3];
	tBase[0] = texture2D(texDiffuse0, uv_diffuse);
	tBase[1] = texture2D(texDiffuse1, uv_diffuse);	
	tBase[2] = texture2D(texDiffuse2, uv_diffuse);
	vec4 DiffuseMap = texture2D(texDiffuseMap, uv);
	
	vec4 cBase;
	// Calcul du la couleur :
	if(vPosition.y < water_height)
		cBase = tBase[0];
	else {
		cBase = mix(mix(tBase[1], tBase[0], DiffuseMap.r), tBase[2], DiffuseMap.g);
	}


	float iDiffuse = max(dot(lightVecTBN.xyz, normalTBN), 0.0);	// Intensité diffuse
	float iSpecular = 0;
	
	if(isUnderWater())
		iSpecular = pow(clamp(dot(reflect(-lightVecTBN.xyz, normalTBN), viewVecTBN), 0.0, 1.0), gl_FrontMaterial.shininess )/2.0;
	
	
	/////////////////////////
	// SHADOW MAPS
	float distance_max = 200.0;
	float shadow = 1.0;
	float distance = length(vPixToEyeTBN);
	if(distance < distance_max) {
		shadow = ShadowMapping(gl_TexCoord[1]);
		shadow = 1.0 - (1.0-shadow) * (distance_max-distance) / distance_max;
	}
	/////////////////////////
	
	
	vec4 cAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 cDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * iDiffuse * shadow;	
	vec4 cSpecular = gl_LightSource[0].specular * gl_FrontMaterial.specular * iSpecular * shadow;	

	return cAmbient * cBase + cDiffuse * cBase + cSpecular;
}



float ShadowMapping(vec4 vVertexFromLightView)
{
	float fShadow = 0.0;
/*			
	vec2 tOffset[3*3];
	tOffset[0] = vec2(-1.0, -1.0); tOffset[1] = vec2(0.0, -1.0); tOffset[2] = vec2(1.0, -1.0);
	tOffset[3] = vec2(-1.0,  0.0); tOffset[4] = vec2(0.0,  0.0); tOffset[5] = vec2(1.0,  0.0);
	tOffset[6] = vec2(-1.0,  1.0); tOffset[7] = vec2(0.0,  1.0); tOffset[8] = vec2(1.0,  1.0);
*/
	float tOrtho[2];
	tOrtho[0] = 20.0;
	tOrtho[1] = 100.0;
	

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
		if(id == 0)	vDepthMapColor = texture2D(texDepthMapFromLight0, vPixPosInDepthMap.st);
		else		vDepthMapColor = texture2D(texDepthMapFromLight1, vPixPosInDepthMap.st);
		

		if((vDepthMapColor.z+Z_TEST_SIGMA) < vPixPosInDepthMap.z)
		{
			fShadow = clamp((vPixPosInDepthMap.z - vDepthMapColor.z)*10.0, 0.0, 1.0);
			
			/*
			// Soft Shadow pour les fragments proches
			if( id == 0 )
			{
				float fMaxShadow = fShadow;
				fShadow = 0.0;

				for(int i=0; i<9; i++)
				{
					vec2 offset = tOffset[i] / (float(depth_map_size));
					// Couleur du pixel sur la depth map
					vec4 vDepthMapColor = texture2D(texDepthMapFromLight0, vPixPosInDepthMap + vec3(offset.s, offset.t, 0.0));
			
					if((vDepthMapColor.z+Z_TEST_SIGMA) < vPixPosInDepthMap.z) {
						fShadow += 0.0;
					}
					else {
						fShadow += fMaxShadow / 9.0;
					}
				}
			}*/
		}
		else
		{
			fShadow = 1.0;
		}


		fShadow = clamp(fShadow, 0.0, 1.0);
	}
	else fShadow = 1.0;
	
	return fShadow;
}



















