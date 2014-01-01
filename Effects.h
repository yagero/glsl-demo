#ifndef EFFECTS_H
#define EFFECTS_H

#include "Singleton.h"
#include "Texture2D.h"
#include "FrameBufferObject.h"

#define FBO_BLOOM_SAMPLES 1

SINGLETON_BEGIN( Effects )
public:
	// Rendusur l'ecran
	FrameBufferObject fboScreen;
	FrameBufferObject fboDepth;

	// Profondeur de champ
	FrameBufferObject fboDepthScreen;
	FrameBufferObject fboDepthScreenTemp;

	// Anaglyph
	FrameBufferObject fboAnaglyph[2];

	// Bloom
	FrameBufferObject fboBloomFinal;
	FrameBufferObject fboBloomFinalTemp;

	// Effets Vignette
	Texture2D*	m_pVignette;

	// Bruit et pellicule
	Texture2D*	m_pBruit;
	//Texture2D*	m_pBruit2;
	float randomCoeffNoise;
	float randomCoeffFlash;
	float m_fT;
	float m_fPeriod;

private:
	void displaySceneWithoutAnaglyph(void);
	void displaySceneWithAnaglyph(void);
	void GenerateBloomTexture();
	void GenerateDepthBlurTexture();

public:
	void init();
	void idle();
	void Render();
	void reshapeFBO(int newwidth , int newheight);
	
	void DrawQuadAtScreen();
	

SINGLETON_END()

#endif