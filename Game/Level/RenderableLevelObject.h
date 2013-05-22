#ifndef RENDERABLELEVELOBJECT_H
#define RENDERABLELEVELOBJECT_H

#include <OgreEntity.h>
#include <OgreSceneNode.h>

// This class should be abastract

class RenderableLevelObject
{

public:
	RenderableLevelObject(float posx, float posy, float posz);
	virtual ~RenderableLevelObject();

	inline Ogre::SceneNode* getMainSceneNode()
	{
		return mainSceneNode;
	}

	inline Ogre::Entity* getEntity()
	{
		return myEntity;
	}

protected:
	float px;
	float py;
	float pz;

	Ogre::Entity* myEntity;
	Ogre::SceneNode* mainSceneNode;

};

#endif