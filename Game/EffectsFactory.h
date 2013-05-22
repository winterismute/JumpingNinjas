#ifndef _EFFECTS_FACTORY_H
#define _EFFECTS_FACTORY_H

#include <OgreSingleton.h>
#include <OgreParticleSystem.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include "boost/lexical_cast.hpp"
#include <cassert>

#include "../Framework/OgreFW.h"

/*
	EffectsFactory is a SingletonED class that delivers effects, mainly the sun and blood.
*/

class EffectsFactory: public Ogre::Singleton<EffectsFactory>
{
public:
	EffectsFactory();
	virtual ~EffectsFactory();

    static EffectsFactory& getSingleton();
    static EffectsFactory* getSingletonPtr();

	inline void createSunEffect(float x, float y, float z)
	{
		std::string stringn = boost::lexical_cast<std::string>(my_sun_ps.size() + 1);
		Ogre::ParticleSystem* ps = OgreFW::getSingletonPtr()->m_pSceneMgr->createParticleSystem("sun" + stringn, "Mine/Sun");
		Ogre::SceneNode* sn  = OgreFW::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("sunParticle" + stringn);
		sn->attachObject(ps);
		sn->setPosition(Ogre::Vector3(x, y, z));

		my_sun_ps.push_back(ps);
		my_sn.push_back(sn);
	};

	// Blood generation is made in an efficient way: only one particle system entity is allocated by this class, then when requested, the particle system is moved to the right place and blood is emitted.
	void requestSpillBlood(float x, float y, float z);

protected:
	//EffectsFactory* ms_Singleton;
	std::list<Ogre::ParticleSystem*> my_blood_ps;
	std::list<Ogre::ParticleSystem*> my_sun_ps; 
	std::list<Ogre::SceneNode*> my_sn;
	//Ogre::SceneManager* mSceneManager;
};

#endif