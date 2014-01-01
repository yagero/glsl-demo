
#include "Timer.h"
#include <GL/glut.h>

#define GETTIME() ((float)glutGet(GLUT_ELAPSED_TIME)/1000)

Timer::Timer() {
	Start();
}

void Timer::Start() {
	fStartTime = GETTIME();
	fCurrentTime = 0.0f;
	fElapsedTime = 0.0f;
	nFPS = 0;
}


void Timer::Idle() {

	static GLint TempFPS = 0;	// FPS temporaires						
    static GLfloat fPreviousFPSTime = 0.0f;	


	GLfloat fNewCurrentTime;

//	do
	{
		fNewCurrentTime = GETTIME() - fStartTime;		
		fElapsedTime = fNewCurrentTime - fCurrentTime;
	}
//	while(fElapsedTime < (1.0f/60));	// blocage du fps

    ++TempFPS;	// Incrémentation du nombre de FPS

	// Si il s'est écoulé plus d'une seconde, on affiche les FPS :
    if( fNewCurrentTime - fPreviousFPSTime > 1.0f ) {
	    fPreviousFPSTime = fNewCurrentTime;
		nFPS = TempFPS;
        TempFPS = 0;
    }

	fCurrentTime = fNewCurrentTime;	// On sauvegarde le temps actuel
}