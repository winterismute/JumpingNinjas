#ifndef LEVELBLOCK_H
#define LEVELBLOCK_H

#include "../Entity/RenderableStaticObject.h"
#include "../Entity/CollisionObject.h"
#include "Material/LevelMaterial.h"
#include <OgreSceneManager.h>
#include <OgreAny.h>
#include <OgreMovableObject.h>

/*
	LevelBlock class represents a renderable static level object. Currently, level objects are created only as cubes.
*/

class LevelBlock : public RenderableStaticObject
{
public:
	LevelBlock(float posx, float posy, float posz, Ogre::SceneManager* smn, float dim, LevelMaterial<> mat);
	virtual ~LevelBlock();
	
	// Get this object correct boundingBox
	inline Ogre::AxisAlignedBox getAABB()
	{
		return myEntity->getWorldBoundingBox(true);
	}

	// Extension is the dimension of the edge
	inline float getExtension()
	{
		return extension;
	}

	// Returns the level material of the current block
	inline LevelMaterial<> getLevelMaterial()
	{
		return material;
	}
	
	// COLLISION METHODS - they have to be defined
	inline virtual void onCollisionFromAbove(CollisionObject* other)
	{

	}

	inline virtual void onCollisionFromBelow(CollisionObject* other)
	{

	}

	inline virtual void onCollisionFromLeft(CollisionObject* other)
	{

	}

	inline virtual void onCollisionFromRight(CollisionObject* other)
	{

	}

protected:
	float extension; // Dimension
	LevelMaterial<CollisionMaterial> material; // Block Material


};

#endif