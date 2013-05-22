#ifndef RENDERABLE_STATIC_OBJECT_H
#define RENDERABLE_STATIC_OBJECT_H

#include "RenderableObject.h"
#include "CollisionObject.h"

/*
	Static object in a level, e.g. a levelblock.
	Distinguish can be useful when we have to update, or in the collision detection phase
*/
class RenderableStaticObject : public RenderableObject, public CollisionObject
{
public:
	RenderableStaticObject(float posx, float posy, float posz, Ogre::SceneManager* smn);
	RenderableStaticObject();
	virtual ~RenderableStaticObject();

	inline bool isStatic()
	{
		// always returns true
		return static_flag;
	}

	// Get this object correct boundingBox
	inline virtual Ogre::AxisAlignedBox getAABB()
	{
		if (myEntity != NULL)
			return myEntity->getWorldBoundingBox(true);
	}

protected:
	bool static_flag; 

};

#endif