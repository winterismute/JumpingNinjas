#ifndef _RANDOM_LEVEL_H
#define _RANDOM_LEVEL_H

#include <OgreSceneManager.h>

#include "LevelFactory.h"

class RandomLevelFactory
	: public LevelFactory
{
public:
	//friend class Level;
	RandomLevelFactory(Ogre::SceneManager* smn);
	virtual ~RandomLevelFactory();

	Level* create() {
		return new Level(mSceneMgr);
	}

	inline Level* createLevelFromSavedGame(GameState gs)
	{
		return new Level(gs, mSceneMgr);
	}

protected:
	Ogre::SceneManager* mSceneMgr;

};

#endif