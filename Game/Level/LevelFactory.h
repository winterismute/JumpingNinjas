#ifndef _LEVEL_FACTORY_H
#define _LEVEL_FACTORY_H

#include "Level.h"
#include "Factory.h"
#include "../GameState.h"

class LevelFactory : public Factory<Level>
{
public:
	//friend class Level;

	virtual Level* create() = 0;

	virtual Level* createLevelFromSavedGame(GameState gs) = 0;

};

#endif