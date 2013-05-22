#ifndef _LEVEL_H
#define _LEVEL_H

#define OBJ_SIZE_X 10.0f
#define OBJ_SIZE_Y 10.0f
#define OBJ_SIZE_Z 10.0f

#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include "LevelBlock.h"
#include "../GameState.h"

//class LevelFactory;

class Level
{
public:

	enum LevelBlockMaterial{ ICE, BRICK };
	Level(std::string& filepath, Ogre::SceneManager* smn);
	Level(GameState gs, Ogre::SceneManager* smn);
	Level(Ogre::SceneManager* smn);
	virtual ~Level();

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
	//array_type data;
	//array_type* data;
	//std::vector <std::vector<int>> data;
	//std::vector<std::vector<LevelBlock*>> data;
	std::vector<LevelBlock*> data;

	std::vector<Ogre::Vector3> player_spawn_positions;

	//Ogre::SceneNode* mLevelNode;
	Ogre::SceneManager* mSceneMgr;

};


#endif