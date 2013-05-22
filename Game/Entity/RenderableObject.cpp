#include "RenderableObject.h"

RenderableObject::RenderableObject(float posx, float posy, float posz, Ogre::SceneManager* smn)
	: mSceneMgr(smn), position(Ogre::Vector3(posx, posy, posz)), mMainNode(NULL), myEntity(NULL) 
{
	//mType = OBJECT_BASE;
}

RenderableObject::RenderableObject(Ogre::Vector3 pos, Ogre::SceneManager* smn)
	: mSceneMgr(smn), position(pos)
{
	//mType = OBJECT_BASE;
}

RenderableObject::RenderableObject()
	: mSceneMgr(NULL), position(Ogre::Vector3(0.0f, 0.0f, 0.0f)), mMainNode(NULL), myEntity(NULL) 
{
	//mType = OBJECT_BASE;
}

RenderableObject::~RenderableObject()
{

}