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

	Level* create(float dim = 10.0f)
	{
		int nl;
		do {
			nl = (rand() % 4) +1;
		}
		while (nl == myLayout);
		myLayout = nl;
		return new Level(myLayout, mSceneMgr, dim);
	}

	inline Level* createLevelFromSavedGame(GameState gs)
	{
		return new Level(gs, mSceneMgr);
	}

protected:
	Ogre::SceneManager* mSceneMgr;
	int myLayout;
};

#endif