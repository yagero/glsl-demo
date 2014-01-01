#ifndef TIMER_H
#define TIMER_H

// -------------------------------
// Gestion du temps
// -------------------------------

#include "Singleton.h"

SINGLETON_BEGIN( Timer )
public:
	void Start();
	void Idle();

	inline float getCurrentTime()	const {return fCurrentTime;}	// Temps courant
	inline float getElapsedTime()	const {return fElapsedTime;}	// Temps entre 2 frames
	inline int getFPSCounter()		const {return nFPS;}			// FPS

protected:
	Timer();

private:
	float	fStartTime;		// date du système lors de l'initialisation de la base de temps
	float	fCurrentTime;	// Temps courant remis à jour à chaque frame
	float	fElapsedTime;
	int		nFPS;

SINGLETON_END()



#endif

