#ifndef RENDERABLELEVEL_H
#define RENDERABLELEVEL_H

#include "Level.h"

#include <vector>
#include <OgreSceneNode.h>
//#include <Ogre.h>


class RenderableLevel: public Level
{
public:
	RenderableLevel(std::string& filepath);
	virtual ~RenderableLevel();

	inline Ogre::SceneNode* getMainSceneNode()
	{
		return LevelSN;
	}

protected:
	Ogre::SceneNode* LevelSN;

};

#endif