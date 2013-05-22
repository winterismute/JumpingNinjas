#ifndef _LEVEL_FROM_FILE_FACTORY_H
#define _LEVEL_FROM_FILE_FACTORY_H

#include <OgreSceneManager.h>

#include "LevelFactory.h"

#define MAX_LEVELS 10

class LevelFromFileFactory : public LevelFactory
{
public:
	//friend class Level;
	LevelFromFileFactory(Ogre::SceneManager* smn);
	virtual ~LevelFromFileFactory();

	Level* create();

	inline Level* createLevelFromSavedGame(GameState gs)
	{
		return new Level(gs, mSceneMgr);
	}

protected:
	Ogre::SceneManager* mSceneMgr;
	int levelno;

};

#endif