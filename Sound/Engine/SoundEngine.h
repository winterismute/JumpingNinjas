#pragma once
#include <string>
#include <map>
#include <irrKlang.h>
#include <sstream>
#include <OgreSingleton.h>


class SoundEngine : public Ogre::Singleton<SoundEngine>
{
public:
	SoundEngine(void);
	~SoundEngine(void);

	//Returns sound engine singleton pointer
	static SoundEngine* getSingletonPtr(void);

	//Load sound from file
	irrklang::ISoundSource* loadSound(std::string);
	
	//Play a sound file
	void playSound(irrklang::ISoundSource*);

private:
	irrklang::ISoundEngine* irrSoundEngine;
	std::map<std::string, irrklang::ISoundSource*> sources;
};


