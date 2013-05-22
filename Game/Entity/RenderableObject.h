#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H

#include <OgreEntity.h>
#include <OgreSceneNode.h>

/*
	Base Class for Objects of the game. Incapsulates all the ogre things.
*/

class RenderableObject
{
public:

	RenderableObject(float posx, float posy, float posz, Ogre::SceneManager* smn);
	RenderableObject(Ogre::Vector3 pos, Ogre::SceneManager* smn);
	RenderableObject();
	virtual ~RenderableObject();

	// Getters
	inline Ogre::SceneNode* getMainNode()
	{
		return mMainNode;
	}

	inline Ogre::Vector3 getWorldPosition()
	{
		if (mMainNode != NULL) {
			return mMainNode->_getDerivedPosition();
		}
	}

	// Change visibility - Useful for 1st person view?
	inline void setVisible (bool visible)
	{
		if (mMainNode != NULL)
			mMainNode->setVisible(visible);
	}

	inline void translate(Ogre::Vector3 npos)
	{
		if (mMainNode != NULL)
			mMainNode->translate(npos);
	}

	inline void translateX(float tx)
	{
		if (mMainNode != NULL)
			mMainNode->translate(tx, 0.0f, 0.0f);
	}

	inline void translateY(float ty)
	{
		if (mMainNode != NULL)
			mMainNode->translate(0.0f, ty, 0.0f);
	}

	inline void translateZ(float tz)
	{
		if (mMainNode != NULL)
			mMainNode->translate(0.0f, 0.0f, tz);
	}

	inline void setPosition(Ogre::Vector3 pos)
	{
		position = pos;
		if (mMainNode != NULL) {
			mMainNode->setPosition(pos);
		}
	}

	inline void setPositionX(float posx)
	{
		position.x = posx;
		if (mMainNode != NULL)
		{
			Ogre::Vector3 mp = mMainNode->getPosition();
			mMainNode->setPosition(posx, mp.y, mp.z);
		}
	}

	inline void setPositionY(float posy)
	{
		position.y = posy;
		if (mMainNode != NULL)
		{
			Ogre::Vector3 mp = mMainNode->getPosition();
			mMainNode->setPosition(mp.x, posy, mp.z);
		}
	}

	inline void setPositionZ(float posz)
	{
		position.z = posz;
		if (mMainNode != NULL)
		{
			Ogre::Vector3 mp = mMainNode->getPosition();
			mMainNode->setPosition(mp.x, mp.y, posz);
		}
	}

protected:
	Ogre::Vector3 position;

	Ogre::Entity* myEntity;
	Ogre::SceneNode* mMainNode;
	Ogre::SceneManager* mSceneMgr;
	//Ogre::String mCharName;

	//object_type mType;

};

#endif

/*

protected:
	float px;
	float py;
	float pz;

	Ogre::Entity* myEntity;
	Ogre::SceneNode* mainSceneNode;

protected:
	Ogre::SceneNode* mMainNode; // Main character node
    //SceneNode* mSightNode; // "Sight" node - The character is supposed to be looking here
    //SceneNode* mCameraNode; // Node for the chase camera
    Ogre::Entity* mEntity; // Character entity
    Ogre::SceneManager* mSceneMgr;
	Ogre::String mCharName;
	
	Ogre::AnimationState *mAnimationState;
	//Ogre::Vector3 mDirection;
	STATE_TYPE charstate;
	float speedx;
	float speedy;

	*/