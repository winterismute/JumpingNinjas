#include "RenderableChar.h"
#include "../../Score/ScoreManager.h"
#include "../../Level/LevelBlock.h"
#include "../../Game.h"
#include "../../../Network/Session.h"

#include <iostream>

RenderableChar::RenderableChar(Ogre::Vector3 pos, Ogre::Vector3 scale, Ogre::SceneManager* smptr, Ogre::String name) 
	//: Character(pos), mSceneMgr(smptr), mCharName(name)
	: RenderableMovingObject(pos, smptr), gravity(20.0f), friction(0.1f)
{
	mCharName = name;
	networkName = name;
	// Set the type of the OBJECT
	//mType = RenderableObject::OBJECT_PLAYER;

	myCollisionType = COLLISION_OBJECT_PLAYER;

	// Give this character a shape :)
	myEntity = mSceneMgr->createEntity(mCharName, "ninja.mesh");
	myEntity->setCastShadows(true);
	myEntity->setUserAny(Ogre::Any(this));

	//mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mCharName, position);
	mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mCharName);
	mMainNode->attachObject(myEntity);

#ifdef _DEBUG
	//mMainNode->showBoundingBox(true);
#endif

	// Set the object scale
	mMainNode->setScale(scale);
	
	// Set and scale the AABB accordingly
	mAABBox = myEntity->getWorldBoundingBox(true);
	//mAABBox = myEntity->getBoundingBox();
	//mAABBox.scale(scale);
	//mAABBox.scale(Ogre::Vector3(1.0f, 0.5, 1.0f));

	// Set the position
	position = pos;
	//+ mAABBox.getHalfSize();
	mMainNode->setPosition(position);
	//mMainNode->translate(0.0f, -mAABBox.getHalfSize().y, 0.0f);

	// Set the orientation
	Ogre::Quaternion q(Ogre::Degree(90), Ogre::Vector3::NEGATIVE_UNIT_Y);
	mMainNode->setOrientation(q);
	// Set Char Movement speed
	speedx = 10.0f;
	speedy = 20.0f;
	charstate = IDLE;
	//mDirection = Ogre::Vector3::ZERO;

	// Set the animation state - Idle
	mAnimationState = myEntity->getAnimationState("Idle1");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);

	jumpSFX = SoundEngine::getSingletonPtr()->loadSound("jump.wav");
}

RenderableChar::RenderableChar() : RenderableMovingObject()
{
	mCharName = "";
	myEntity = NULL;
	mMainNode = NULL;
	mAnimationState = NULL;

	charstate = IDLE;

	gravity = 2.0f;
	friction = 0.1f;
}

void RenderableChar::init(Ogre::Vector3 pos,  Ogre::Vector3 scale, Ogre::SceneManager* smptr, Ogre::String name)
{
	if (mSceneMgr == NULL) {
		mSceneMgr = smptr;
		mCharName = name;
		mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mCharName, position);
		//mMainNode->setPosition(pos);

		// Give tfhis character a shape :)
		myEntity = mSceneMgr->createEntity(mCharName, "ninja.mesh");
		myEntity->setCastShadows(true);
		mMainNode->attachObject(myEntity);

		mMainNode->scale(scale);
		
		// Set char direction
		Ogre::Quaternion q(Ogre::Degree(90), Ogre::Vector3::NEGATIVE_UNIT_Y); 
		mMainNode->setOrientation(q);

		//speedx = 70.0f;
		//speedy = 20.0f;
		//charstate = IDLE;
		//mDirection = Ogre::Vector3::ZERO;

		// Set the animation state - Idle
		mAnimationState = myEntity->getAnimationState("Idle1");
		mAnimationState->setLoop(true);
		mAnimationState->setEnabled(true);

		gravity = 1.0f;
		friction = 0.1f;
	}
}
         
RenderableChar::~RenderableChar()
{
	if (mMainNode != NULL) {
		mMainNode->detachAllObjects();
		mMainNode->removeAndDestroyAllChildren();

		if(mSceneMgr->hasSceneNode(mCharName) == true){
			mSceneMgr->destroySceneNode(mCharName);
		}
		if(mSceneMgr->hasEntity(mCharName) == true){
			mSceneMgr->destroyEntity(mCharName);
		}
		myEntity = NULL;
	}
	std::cout << "PLAYER DESTR called" << std::endl;
}

void RenderableChar::update(float dtime)
{
	if (mMainNode != NULL) {
		//std::cout << mEntity->getWorldBoundingBox().getSize();
		if (charstate == DEAD) {
			std::cout << " I will die" << std::endl;
			if (mAnimationState->getAnimationName() != "Death2" ) {
				// Play Death animation
				mAnimationState->setEnabled(false);
				mAnimationState->setLoop(false);
				//mAnimationState->setTimePosition(0.0f);
				mAnimationState = myEntity->getAnimationState("Death2");
				mAnimationState->setLoop(false);
				mAnimationState->setEnabled(true);
			}
			else if (mAnimationState->hasEnded()) {
				std::cout << "DEATH finished" << std::endl;
				mAnimationState->setEnabled(false);
				setVisible(false);
				charstate = SPAWNING;
			}
		}
		if (  (velocity.x <= 0.1f && velocity.x >= -0.1f) && (velocity.y == 0.0f)  && (velocity.z <= 0.1f && velocity.z >= -0.1f) ) {
			if (charstate != DEAD && charstate != SPAWNING) {
				mAnimationState->setEnabled(false);
				mAnimationState->setLoop(false);
				// Load idle animation
				mAnimationState = myEntity->getAnimationState("Idle1");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
				charstate = IDLE;
			}
		}
		else {
			if (velocity.y != 0.0f) {
				if( charstate == IDLE || charstate == WALKING) {
				charstate = JUMPING;
				// Unload the previous animation
				mAnimationState->setEnabled(false);
				mAnimationState->setLoop(false);
				//mAnimationState->setTimePosition(0.0f);
				// Load jumping animation
				mAnimationState = myEntity->getAnimationState("Jump");
				mAnimationState->setLoop(false);
				mAnimationState->setEnabled(true);
				//velocity.y -= gravity;
				}
			}
			else if(charstate == JUMPING && velocity.y == 0.0f){
				charstate = IDLE;
			}
			if (velocity.x > 0.1f || velocity.x < -0.1f) {
				// Change animation only if player is not jumping
				if (charstate == IDLE) {
					// Toggle previous animation
					mAnimationState->setEnabled(false);
					mAnimationState->setLoop(false);
					//mAnimationState->setTimePosition(0.0f);
					// Change to Walk animation
					mAnimationState = myEntity->getAnimationState("Walk");
					mAnimationState->setLoop(true);
					mAnimationState->setEnabled(true);
					charstate = WALKING;
				}
				// In any case rotate
				Ogre::Vector3 newdir = (mMainNode->getPosition() + Ogre::Vector3(velocity.x, 0.0f, velocity.z)) - mMainNode->getPosition();   
				Ogre::Vector3 src = mMainNode->getOrientation() * (Ogre::Vector3::UNIT_Z * -1);
				if (newdir[0] >= 0.1f || newdir[0] <= -0.1f || newdir[2] >= 0.1f || newdir[2] <= -0.1f) {
					if ((1.0f + src.dotProduct(newdir)) < 0.0001f) {
						mMainNode->yaw(Ogre::Degree(180));
					}
				}
			}
			// Rembember to translate
			mMainNode->translate(velocity.x * dtime, velocity.y * dtime, 0);
		}
		velocity.y -= gravity * dtime;
		velocity.x *= friction;

		mAnimationState->addTime(dtime);
	}
	
}

void RenderableChar::updateAnimation (float dtime)
{

	if (mMainNode != NULL) {
		//std::cout << mEntity->getWorldBoundingBox().getSize();
		if (velocity.x == 0.0f && velocity.y == 0.0f && velocity.z == 0.0f) {
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
			// Load idle animation
			mAnimationState = myEntity->getAnimationState("Idle1");
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
			charstate = IDLE;
		}
		else {
			if (velocity.y != 0.0f) {
				if( charstate != JUMPING){
					charstate = JUMPING;
				}
				// Unload the previous animation
				mAnimationState->setEnabled(false);
				mAnimationState->setLoop(false);
				//mAnimationState->setTimePosition(0.0f);
				// Load jumping animation
				mAnimationState = myEntity->getAnimationState("JumpNoHeight");
				mAnimationState->setLoop(false);
				mAnimationState->setEnabled(true);
				//velocity.y -= gravity;
			}
			else if(charstate == JUMPING && velocity.y == 0.0f){
				charstate = IDLE;
			}

			if (velocity.x != 0.0f) {
				// Change animation only if player is not jumping
				if (charstate == IDLE) {
					// Toggle previous animation
					mAnimationState->setEnabled(false);
					mAnimationState->setLoop(false);
					//mAnimationState->setTimePosition(0.0f);
					// Change to Walk animation
					mAnimationState = myEntity->getAnimationState("Walk");
					mAnimationState->setLoop(true);
					mAnimationState->setEnabled(true);
					charstate = WALKING;
				}

				// In any case, translate and rotate
				Ogre::Vector3 newdir = (mMainNode->getPosition() + Ogre::Vector3(velocity.x, 0.0f, velocity.z)) - mMainNode->getPosition();   
				Ogre::Vector3 src = mMainNode->getOrientation() * (Ogre::Vector3::UNIT_Z * -1);
				if (newdir[0] >= 0.1f || newdir[0] <= -0.1f || newdir[2] >= 0.1f || newdir[2] <= -0.1f) {
					if ((1.0f + src.dotProduct(newdir)) < 0.0001f) {
						mMainNode->yaw(Ogre::Degree(180));
					}
				}
			}

			//mMainNode->translate(velocity.x * dtime, velocity.y * dtime, 0);

		}

		velocity.y -= gravity * dtime;
		velocity.x *= friction;

		mAnimationState->addTime(dtime);
	}

	//reset friction
	friction = 0.1f;

}

void RenderableChar::onCollisionFromBelow(CollisionObject* other)
{
	
	if(other->getCollisionType() == COLLISION_OBJECT_BLOCK){
		LevelBlock* levelBlock = static_cast<LevelBlock*> (other);
		friction = levelBlock->getLevelMaterial().getFriction();
	}

	if (other->getCollisionType() == COLLISION_OBJECT_PLAYER) {
		if( !Game::getSingletonPtr()->GetIsNetworkGame() || (Game::getSingletonPtr()->GetIsNetworkGame() && Session::sessionType == HOST) ){
			ScoreManager::getSingletonPtr()->addPoint(mCharName);
			ScoreManager::getSingletonPtr()->resetStreak(static_cast<RenderableChar*>(other)->GetName());
		}
	}
}

void RenderableChar::onCollisionFromAbove(CollisionObject* other)
{
	if (other->getCollisionType() == COLLISION_OBJECT_PLAYER) {
		// I am getting stumped. Let's die and respawn
		charstate = DEAD;
		// And also, why don't spill some blood?
		Ogre::Vector3 po = getWorldPosition();
		EffectsFactory::getSingletonPtr()->requestSpillBlood(po.x, po.y, po.z);
	}
}

Ogre::String RenderableChar::GetName()
{
	return networkName;
}
