#ifndef _LEVEL_FACTORY_H
#define _LEVEL_FACTORY_H

#include "Level.h"
#include "Factory.h"
#include "../GameState.h"

/*
	LevelFactory is the level abstract factory.
	it extends the Factory class in order to acquire the power of templates!
	The game only includes to implementation: LevelFromFileFactory and RandomLevelFactory.
*/

class LevelFactory : public Factory<Level>
{
public:
	// Create a new level
	virtual Level* create(float dim = 10.0f) = 0;

	// Restores a level from a GameState
	virtual Level* createLevelFromSavedGame(GameState gs) = 0;

};

#endif