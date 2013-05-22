#ifndef _RENDERABLE_CHAR
#define _RENDERABLE_CHAR

#include <OgreEntity.h>
#include <OgreSceneManager.h>

//#include "../Character/Character.h"
#include "../RenderableMovingObject.h"
#include "../CollisionObject.h"
#include "../../../Sound/Engine/SoundEngine.h"
#include "../../EffectsFactory.h"

#include <OgreMovableObject.h>
#include <OgreAny.h>

//Class that represents the player in the game
class RenderableChar 
	: public RenderableMovingObject
{
public:
	enum STATE_TYPE {WALKING, JUMPING, IDLE, DEAD, SPAWNING};

	RenderableChar(Ogre::Vector3 pos, Ogre::Vector3 scale, Ogre::SceneManager* smptr, Ogre::String name);

	RenderableChar();

	virtual ~RenderableChar();

	virtual void init(Ogre::Vector3 pos,  Ogre::Vector3 scale, Ogre::SceneManager* smptr, Ogre::String name);

	//Update method for player
	void update (float dtime);
	void updateAnimation (float dtime);
	Ogre::String GetName();

	inline Ogre::String GetNetworkName(){
		return networkName;
	}

	//Set current charachter state
	inline void SetState(STATE_TYPE state){
		charstate = state;
	}

	//Set ogre world position
	inline void SetPosition(Ogre::Vector3 newPos){
		mMainNode->setPosition(newPos);
	}

	//Set name
	inline void SetName(Ogre::String newName){
		mCharName = newName;
	}

	//Set the network name
	inline void SetNetworkName(Ogre::String newName){
		networkName = newName;
	}

	// Change visibility - Useful for 1st person view?
	inline void setVisible(bool visible)
	{
		mMainNode->setVisible(visible);
	}

	//Sets the friction cooficient
	inline void setFriction(float nf)
	{
		friction = nf;
	}

	//Get the character state
	inline STATE_TYPE getCharState()
	{
		return charstate;
	}

	//Sets the character state
	inline void setCharState(STATE_TYPE newtype)
	{
		charstate = newtype;
	}

	//Performs the actual jump
	inline void performJump(float k)
	{
		if (charstate != JUMPING) {
			setVelY(k);
			SoundEngine::getSingletonPtr()->playSound(jumpSFX);
		}
	}

	// Get this object correct boundingBox
	inline Ogre::AxisAlignedBox getAABB()
	{
		return myEntity->getWorldBoundingBox(true);
	}

	// Collision methods
	inline Ogre::Sphere GetBoundingSphere()
	{
		return myEntity->getWorldBoundingSphere();
	}
	
	//Collision callbacks
	inline virtual void onCollisionFromAbove(CollisionObject* other);

	inline virtual void onCollisionFromBelow(CollisionObject* other);

	inline virtual void onCollisionFromLeft(CollisionObject* other)
	{
		if (other->getCollisionType() == COLLISION_OBJECT_PLAYER )
			std::cout << "Colliding Left" << std::endl; 

	}

	inline virtual void onCollisionFromRight(CollisionObject* other)
	{
		if(other->getCollisionType() == COLLISION_OBJECT_PLAYER )
			std::cout << "Colliding Right" << std::endl; 
	}

	inline float GetDTime(){
		return mAnimationState->getTimePosition();
	}

protected:
	
	Ogre::String mCharName;
	Ogre::String networkName;
	Ogre::AnimationState *mAnimationState;
	//Ogre::Vector3 mDirection;
	
	float gravity;
	float friction;

	float maxSpeedx;
	float maxSpeedy;
	float maxSpeedz;

	STATE_TYPE charstate;

	Ogre::AxisAlignedBox mAABBox;
	irrklang::ISoundSource* jumpSFX;
};

#endif