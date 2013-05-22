#include "Character.h"

Character::Character(Ogre::Vector3 pos) : position(pos), velocity(Ogre::Vector3::ZERO)
{

}

Character::Character() : position(Ogre::Vector3::ZERO), velocity(Ogre::Vector3::ZERO) 
{

}

Character::~Character()
{

}

Ogre::Vector3 Character::getVelocity() const
{
	return velocity;
}

