#include "SoundEngine.h"
#define SOUNDS_MAP "../../media/sounds"

using namespace irrklang;

template<> SoundEngine* Ogre::Singleton<SoundEngine>::ms_Singleton = 0;

SoundEngine::SoundEngine(void)
{
	irrSoundEngine = createIrrKlangDevice();
}


SoundEngine::~SoundEngine(void)
{
	irrSoundEngine->drop();
}

ISoundSource* SoundEngine::loadSound(std::string name)
{
	std::stringstream ss;
	ss << SOUNDS_MAP;
	ss << "/";
	ss << name;

	std::string fileName = ss.str();

	if(sources.find(fileName) != sources.end()){
		return sources[fileName];
	}
	
	ISoundSource* src = irrSoundEngine->addSoundSourceFromFile(fileName.c_str(), ESM_AUTO_DETECT, true);
	
	if(src != NULL){
		sources[fileName] = src;
	}

	//Returns null if sound does not exist
	return src;
}

void SoundEngine::playSound( ISoundSource* sound )
{
	if(sound != NULL && !irrSoundEngine->isCurrentlyPlaying(sound)){
		irrSoundEngine->play2D(sound);
	}
}

SoundEngine* SoundEngine::getSingletonPtr( void )
{
	return ms_Singleton;
}
