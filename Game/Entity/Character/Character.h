#ifndef CHARACTER_H
#define CHARACTER_H

#include <OgreEntity.h>

//Class that represents a character in the game (Playable or otherwise)
class Character
{
public:
	Character(Ogre::Vector3 pos);
	
	Character();

	virtual ~Character();

	inline virtual void init(Ogre::Vector3 pos, Ogre::Vector3 vel)
	{
		position = pos;
		vel = velocity;
	}


	virtual void update(float dtime) = 0;

	Ogre::Vector3 getVelocity() const;
protected:
	Ogre::Vector3 position;
	Ogre::Vector3 velocity;

	void moveToPos(float px, float py, float pz);
	void move();

};

#endif


