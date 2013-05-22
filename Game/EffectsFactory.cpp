#include "EffectsFactory.h"

template<> EffectsFactory* Ogre::Singleton<EffectsFactory>::ms_Singleton = 0;

EffectsFactory::EffectsFactory()
{
	Ogre::ParticleSystem* ps = OgreFW::getSingletonPtr()->m_pSceneMgr->createParticleSystem("blood1", "Mine/Blood");
	Ogre::SceneNode* sn =  OgreFW::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("blood1", Ogre::Vector3(-50.0f, -50.0f, -50.0f));
	sn->attachObject(ps);
	//ps->setEmitting(false);

	my_blood_ps.push_back(ps);
	my_sn.push_back(sn);
	
	//Ogre::ParticleSystem* ps = OgreFW::getSingletonPtr()->m_pSceneMgr->createParticleSystem("Fountain1", "Mine/Blood");  // create fountain 1
	//ps->setEmitting(false);
	// attach the fountain to a child node of the pivot at a distance and angle
	//Ogre::SceneNode* mFountainPivot = OgreFW::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("fountainParticle", Ogre::Vector3(40, 10, -20), Ogre::Quaternion(Ogre::Degree(20), Ogre::Vector3::UNIT_Z));
	//mFountainPivot->attachObject(ps);
}

EffectsFactory::~EffectsFactory()
{
	for ( std::list<Ogre::ParticleSystem*>::iterator ite = my_sun_ps.begin(); ite != my_sun_ps.begin(); ++ite) {
		(*ite)->detachFromParent();
		(*ite)->clear();
	}
	for ( std::list<Ogre::ParticleSystem*>::iterator ite = my_blood_ps.begin(); ite != my_blood_ps.begin(); ++ite) {
		(*ite)->detachFromParent();
		(*ite)->clear();
	}
	for (std::list<Ogre::SceneNode*>::iterator ite = my_sn.begin(); ite != my_sn.end(); ++ite) {
		(*ite)->detachAllObjects();
		(*ite)->removeAndDestroyAllChildren();
		OgreFW::getSingletonPtr()->m_pSceneMgr->destroySceneNode(*ite);
	}
	my_sun_ps.clear();
	my_blood_ps.clear();
	my_sn.clear();
}

EffectsFactory* EffectsFactory::getSingletonPtr(void)
{
    return ms_Singleton;
}

EffectsFactory& EffectsFactory::getSingleton(void)
{  
    assert( ms_Singleton );
	return ( *ms_Singleton );
}

void EffectsFactory::requestSpillBlood(float x, float y, float z)
{
	assert(my_blood_ps.size() > 0);
	assert(my_sn.size());

	my_blood_ps.front()->getParentSceneNode()->setPosition(x, y, z);

	my_blood_ps.front()->setEmitting(true);
	int numEmitters = my_blood_ps.front()->getNumEmitters();
	std::cout << "Num emitters:" << numEmitters << std::endl;
	for(int x = 0; x < numEmitters; x++) {
		Ogre::ParticleEmitter* emitter = my_blood_ps.front()->getEmitter(x);
		emitter->setEnabled(true);
		std::cout << "EMITTER ENABLED" << std::endl;
	}

}