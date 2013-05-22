#ifndef _LEVEL_H
#define _LEVEL_H

#define OBJ_SIZE_X 10.0f
#define OBJ_SIZE_Y 10.0f
#define OBJ_SIZE_Z 10.0f

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cassert>

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include "LevelBlock.h"

#include "../GameState.h"

//class LevelFactory;

// The class represents a Level renderable in the game. Basically a Level is a collection of LevelBlocks in which the characters move

class Level
{
public:

	// Enum for distinguising the type of blocks
	enum LevelBlockMaterial{ ICE, BRICK };

	// The first constructor builds a level by parsing a text file
	Level(std::string& filepath, Ogre::SceneManager* smn, float dim);
	
	// This constructor builds a lvel from a GameState objects - Used to reastore a serialized game
	Level(GameState gs, Ogre::SceneManager* smn);
	
	// This constructor builds a "Random" Level
	// a random level is simply a prederined layout (depending on the int)
	//--> We had to no time to implement a real procedural lavel generator
	Level(int layout, Ogre::SceneManager* smn, float dim);
	
	virtual ~Level();

	// Methods are mainly the standard getters
	inline int getWidth()
	{
		return width;
	}

	inline int getHeight()
	{
		return height;
	}

	inline int getNumberOfSpawnPosition()
	{
		return player_spawn_positions.size();
	}

	inline Ogre::Vector3 getSpawnPosition(int no)
	{
		return player_spawn_positions[no];
	}

	inline std::vector<LevelBlock*> getLevelObjects()
	{
		return data;
	}

	inline float getObjectSide()
	{
		return obj_side;
	}

	/*
	inline LevelBlock* getObjAt(int row, int col)
	{
		return data[row][col];
	}
	*/

protected:
	void parseLevel(std::string& filepath);
	LevelBlock* createLevelBlock(float xPos, float yPos, LevelBlockMaterial mat);

	int width;
	int height;
	float obj_side;

	// Thi vector stores the blocks
	std::vector<LevelBlock*> data;

	// SPAWN_POSITION: we started to implement a mechanism such that each level could have some kind of pre-defined spawning position
	// for characters (max 4), which could be used. But, then we decided not use them: characters are spawned randomly in the level as in the 2D dos version
	std::vector<Ogre::Vector3> player_spawn_positions;

	Ogre::SceneManager* mSceneMgr;
};

#endif