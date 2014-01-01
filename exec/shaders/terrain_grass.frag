

uniform sampler2D texDiffuse;


// SHADOW MAPPING //
uniform int depth_map_size;
uniform sampler2DShadow texDepthMapFromLight0;
uniform sampler2DShadow texDepthMapFromLight1;
#define Z_TEST_SIGMA 0.0001
////////////////////

float ShadowMapping(vec4 vVertexFromLightView);

void main (void)
{
	vec4 cBase = texture2D(texDiffuse, gl_TexCoord[0].st);
	if(cBase.a < 0.4) discard;
	
	vec4 cAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 cDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * gl_Color;
	
//	float shadow = ShadowMapping(gl_TexCoord[1]);
	
	gl_FragColor = cAmbient * cBase + cDiffuse * cBase;
	gl_FragColor.a = gl_Color.a;
}


float ShadowMapping(vec4 vVertexFromLightView)
{
	float fShadow = 0.0;
						
	float tOrtho[2];
	tOrtho[0] = 20.0;
	tOrtho[1] = 100.0;
	

	bool ok = false;
	int id = 0;
	vec3 vPixPosInDepthMap;
	
	for(int i=0; i<2; i++) 
	{
		vPixPosInDepthMap = vec3(vVertexFromLightView.xy/tOrtho[i], vVertexFromLightView.z) / (vVertexFromLightView.w);
		vPixPosInDepthMap = (vPixPosInDepthMap + 1.0) * 0.5;					// de l'intervale [-1 1] à [0 1]
		
		if(vPixPosInDepthMap.x >= 0.0 && vPixPosInDepthMap.y >= 0.0 && vPixPosInDepthMap.x <= 1.0 && vPixPosInDepthMap.y <= 1.0)
		{
			id = i;
			i = 2;
			ok = true;
		}
	}
	
	if(!ok) {
		return 1.0;
	}
	
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
	return fShadow;
}














