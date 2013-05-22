#ifndef RENDERABLE_MOVING_OBJECT_H
#define RENDERABLE_MOVING_OBJECT_H

#include "RenderableObject.h"
#include "CollisionObject.h"

/*
	A moving object has attributes like velocities etc.
*/

class RenderableMovingObject 
	: public RenderableObject, public CollisionObject
{
public:
	RenderableMovingObject(float posx, float posy, float posz, Ogre::SceneManager* smn);
	RenderableMovingObject(Ogre::Vector3 pos, Ogre::SceneManager* smn);
	RenderableMovingObject();
	virtual ~RenderableMovingObject();

	inline bool isStatic()
	{
		// Always returns false
		return static_flag;
	}

	// Get this object correct boundingBox
	inline virtual Ogre::AxisAlignedBox getAABB()
	{
		if (myEntity != NULL)
			return myEntity->getWorldBoundingBox(true);
	}

	inline Ogre::Vector3 getVelocity() const
	{
		return velocity;
	}

	//
	inline void setVelX(int kx)
	{
		velocity.x = kx * speedx;
	}

	inline void setVelY(int ky)
	{
		velocity.y = ky * speedy;
	}

	inline void setVelZ(int kz)
	{

		velocity.z = kz * speedz;
	}

	inline void addToVelY(int ky)
	{
		velocity.y += ky * speedy;
	}

	inline void resetVelX()
	{
		velocity.x = 0.0f;
	}

	inline void resetVelY()
	{
		velocity.y = 0.0f;
	}

	inline void resetVelZ()
	{
		velocity.z = 0.0f;
	}

protected:
	Ogre::Vector3 velocity;
	bool static_flag;

	float speedx;
	float speedy;
	float speedz;

};

#endif