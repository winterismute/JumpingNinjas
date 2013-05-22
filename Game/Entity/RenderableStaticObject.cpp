#include "RenderableStaticObject.h"

RenderableStaticObject::RenderableStaticObject(float posx, float posy, float posz, Ogre::SceneManager* smn)
	: RenderableObject(posx, posy, posz, smn), static_flag(false)
{
	myCollisionType = COLLISION_OBJECT_BASE_STATIC;
}

RenderableStaticObject::RenderableStaticObject() : RenderableObject(), static_flag(false)
{
	myCollisionType = COLLISION_OBJECT_BASE_STATIC;
}

RenderableStaticObject::~RenderableStaticObject()
{

}