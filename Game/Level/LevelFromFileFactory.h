#ifndef _LEVEL_FROM_FILE_FACTORY_H
#define _LEVEL_FROM_FILE_FACTORY_H

#include <OgreSceneManager.h>

#include "LevelFactory.h"

/*
	LevelFromFileFactory creates level by parsing text files. It implements the abstract LevelFactory.
	Using this factory, users can create levels by saving textfiles called levelN.txt, where N is the number of the level.
	The first parsed level will be level1, then level2, 3 and so on until MAX_LEVELS. Afterwards, level1 will be parsed again.
*/

#define MAX_LEVELS 10

class LevelFromFileFactory : public LevelFactory
{
public:
	LevelFromFileFactory(Ogre::SceneManager* smn);
	virtual ~LevelFromFileFactory();

	// The 2 standard methods of LevelFactory
	Level* create(float dim = 10.0f);

	inline Level* createLevelFromSavedGame(GameState gs)
	{
		return new Level(gs, mSceneMgr);
	}

protected:
	Ogre::SceneManager* mSceneMgr;
	// levelno keeps track of the sequence of levels
	int levelno;
};

#endif