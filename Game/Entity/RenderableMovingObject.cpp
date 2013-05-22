#include "RenderableMovingObject.h"

RenderableMovingObject::RenderableMovingObject(float posx, float posy, float posz, Ogre::SceneManager* smn)
	: RenderableObject(posx, posy, posz, smn), static_flag(false), velocity(Ogre::Vector3(0.0f, 0.0f, 0.0f)), speedx(10.0f), speedy(10.0f), speedz(0.0f)
{
	myCollisionType = COLLISION_OBJECT_BASE_MOVING;
	// DEFAULT MAX_SPEED IS 10/10
}

RenderableMovingObject::RenderableMovingObject(Ogre::Vector3 pos, Ogre::SceneManager* smn)
	: RenderableObject(pos, smn), static_flag(false), velocity(Ogre::Vector3(0.0f, 0.0f, 0.0f)), speedx(10.0f), speedy(10.0f), speedz(0.0f)
{
	 myCollisionType = COLLISION_OBJECT_BASE_MOVING;
	// DEFAULT MAX_SPEED IS 10/10
}

RenderableMovingObject::RenderableMovingObject()
	: RenderableObject(), static_flag(false), velocity(Ogre::Vector3(0.0f, 0.0f, 0.0f)), speedx(10.0f), speedy(10.0f), speedz(0.0f)
{
	myCollisionType = COLLISION_OBJECT_BASE_MOVING;
	// DEFAULT MAX_SPEED 10/10
}

RenderableMovingObject::~RenderableMovingObject()
{

}