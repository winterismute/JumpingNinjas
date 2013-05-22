#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include "Ogre.h"

// 
// Interface class that defines the collision callbacks
//
class CollisionObject
{
public:
	enum collision_type {COLLISION_OBJECT_BASE_MOVING, COLLISION_OBJECT_BASE_STATIC, COLLISION_OBJECT_BLOCK, COLLISION_OBJECT_PLAYER};

	//Top collision
	virtual void onCollisionFromAbove(CollisionObject* other) = 0;
	//Bottom collision
	virtual void onCollisionFromBelow(CollisionObject* other) = 0;
	//Left collision
	virtual void onCollisionFromLeft(CollisionObject* other) = 0;
	//Right collision
	virtual void onCollisionFromRight(CollisionObject* other) = 0;

	inline collision_type getCollisionType()
	{
		return myCollisionType;
	}

protected:
	collision_type myCollisionType;

};

#endif 


