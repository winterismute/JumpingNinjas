#ifndef _EFFECTS_FACTORY_H
#define _EFFECTS_FACTORY_H

#include <OgreSingleton.h>
#include <OgreParticleSystem.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include "boost/lexical_cast.hpp"
#include <cassert>

#include "../Framework/OgreFW.h"

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

	void requestSpillBlood(float x, float y, float z);
	/*
	{
		Ogre::ParticleSystem* ps = OgreFW::getSingletonPtr()->m_pSceneMgr->createParticleSystem("blood" + boost::lexical_cast<std::string>(my_blood_ps.size() + 1), "Mine/Blood");  // create fountain 1
		ps->setEmitting(false);
		// attach the fountain to a child node of the pivot at a distance and angle
		Ogre::SceneNode* mFountainPivot = OgreFW::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("fountainParticle", Ogre::Vector3(40, 10, -20), Ogre::Quaternion(Ogre::Degree(20), Ogre::Vector3::UNIT_Z));
		mFountainPivot->attachObject(ps);
	}
	*/

protected:
	//EffectsFactory* ms_Singleton;
	std::list<Ogre::ParticleSystem*> my_blood_ps;
	std::list<Ogre::ParticleSystem*> my_sun_ps; 
	std::list<Ogre::SceneNode*> my_sn;
	//Ogre::SceneManager* mSceneManager;
};

#endif