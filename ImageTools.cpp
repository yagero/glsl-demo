#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include "ImageTools.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <assert.h>

#undef _UNICODE
#include "IL/il.h"


namespace ImageTools {

GLubyte* OpenImagePPM(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d)
{

	char head[70];
	int i,j;
	GLubyte * img = NULL;

	FILE * f = fopen(filename.c_str(), "rb");

	if(f==NULL){
		return 0;
	}
	fgets(head,70,f);

	if(!strncmp(head, "P6", 2)){
		i=0;
		j=0;
		while(i<3){
			fgets(head,70,f);

			if(head[0] == '#'){
				continue;
			}

			if(i==0)
				i += sscanf(head, "%d %d %d", &w, &h, &d);
			else if(i==1)
				i += sscanf(head, "%d %d", &h, &d);
			else if(i==2)
				i += sscanf(head, "%d", &d);
		}

		img = new GLubyte[(size_t)(w) * (size_t)(h) * 3];
		if(img==NULL) {
			fclose(f);
			return 0; 
		}

		fread(img, sizeof(GLubyte), (size_t)w*(size_t)h*3,f);
		fclose(f);
	}
	else{
		fclose(f);
	}
	
	return img;
}


GLubyte* OpenImageDevIL(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d)
{
	static bool first = true;
	if(first) {
		first = false;

		// Initalisation de DevIL
		ilInit();

		// On indique que l'origine des images se trouve sur le coin haut-gauche
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		ilEnable(IL_ORIGIN_SET);

		ilEnable(IL_TYPE_SET);
		ilTypeFunc(IL_UNSIGNED_BYTE);
		

	}

    // Génération d'une nouvelle texture
    ILuint ilTexture;
    ilGenImages(1, &ilTexture);
    ilBindImage(ilTexture);

    // Chargement de l'image
	if (!ilLoadImage(filename.c_str()))
		return false;


    // Récupération de la taille de l'image
	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	d = ilGetInteger(IL_IMAGE_BPP);


	if(d==4)
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    // Récupération des pixels de l'image
    const unsigned char* Pixels = ilGetData();

	GLubyte* img = new GLubyte[(size_t)(w) * (size_t)(h) * (size_t)(d)];
	memcpy(img, Pixels, (size_t)(w) * (size_t)(h) * (size_t)(d));

    // Suppression de la texture
    ilBindImage(0);
    ilDeleteImages(1, &ilTexture);

	return img;
}


GLubyte* OpenImage(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d)
{
	if(filename.find(".ppm") != std::string::npos){
		return ImageTools::OpenImagePPM("./textures/"+filename, w, h, d);
	}
	else {
		return ImageTools::OpenImageDevIL("./textures/"+filename, w, h, d);
	}
	std::cout << "Erreur de chargement de l'image " << filename << std::endl;
	return NULL;
}

void OpenImage(const std::string& filename, ImageData& img)
{
	img.data = OpenImage(filename, img.w, img.h, img.d);
}


void ImageData::Destroy()
{
	if(data) {
		delete [] data;
		data = NULL;
	}
}

ivec3 ImageData::getColor(unsigned int x, unsigned int y) const
{
	int idx = (y * w + x) * d;
	return ivec3( data[idx+0], data[idx+1], data[idx+2]);
}
/*
ivec3 ImageData::getColor(float x, float y)
{
	vec2  posF(	x, y );
	ivec2 posI(	(int)(posF.x), (int)(posF.y) );
	vec2  posD(	posF.x - posI.x, posF.y - posI.y );

	if(posI.x >= (int)w-1)	posI.x = w-2;
	if(posI.y >= (int)h-1)	posI.y = h-2;

	assert(posI.x>=0 && posI.x<(int)w-1 && posI.y>=0 && posI.y<(int)h-1);

	vec3 c00 = getColor((unsigned int)posI.x + 0, (unsigned int)posI.y + 0);
	vec3 c10 = getColor((unsigned int)posI.x + 1, (unsigned int)posI.y + 0);
	vec3 c01 = getColor((unsigned int)posI.x + 0, (unsigned int)posI.y + 1);
	vec3 c11 = getColor((unsigned int)posI.x + 1, (unsigned int)posI.y + 1);

	vec3 c =  vec3(c00  * (1.0f-posD.x) * (1.0f-posD.y))
			+ vec3(c10  *       posD.x  * (1.0f-posD.y))
			+ vec3(c01  * (1.0f-posD.x) *       posD.y)
			+ vec3(c11  *       posD.x  *       posD.y);
}
*/

}

