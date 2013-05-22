#ifndef COLLISION_HANDLER_H
#define COLLISION_HANDLER_H

#include <vector>
#include <OgreSceneManager.h>
#include <OgreSceneQuery.h>
#include <OgreAny.h>

#include "RenderableMovingObject.h"
#include "RenderableStaticObject.h"

#include "../Level/LevelBlock.h"
#include "../Entity/Player/RenderableChar.h"

//Collision handler class, handles in game collisions
class CollisionHandler
{
public:
	CollisionHandler(Ogre::SceneManager* smn);
	CollisionHandler(Ogre::SceneManager* smn, std::vector<RenderableStaticObject*> inputstatic);
	CollisionHandler(Ogre::SceneManager* smn, std::vector<RenderableMovingObject*> inputmoving);
	CollisionHandler(Ogre::SceneManager* smn, std::vector<RenderableMovingObject*> inputmoving, std::vector<RenderableStaticObject*> inputstatic);

	virtual ~CollisionHandler();
	
	//Add a new object to the collision system
	inline void addNewObject(RenderableStaticObject* instatic)
	{
		static_collision_objects.push_back(instatic);
		//static_objects_to_add.push_back(instatic);
	}

	//Add a whole vector the the collision system
	inline void addNewObjectsVector(std::vector<RenderableStaticObject*> instatic_vec)
	{
		for (std::vector<RenderableStaticObject*>::iterator i = instatic_vec.begin(); i != instatic_vec.end(); ++i) {
			addNewObject(*i);
		}
	}

	//Add a new moving object to the system
	inline void addNewObject(RenderableMovingObject* inmoving)
	{
		moving_collision_objects.push_back(inmoving);
		//moving_objects_to_add.push_back(inmoving);
	}

	//Add a new vector of movable objects to the system
	inline void addNewObjectsVector(std::vector<RenderableMovingObject*> inmoving_vec)
	{
		for (std::vector<RenderableMovingObject*>::iterator i = inmoving_vec.begin(); i != inmoving_vec.end(); ++i) {
			addNewObject(*i);
		}
	}

	//Remove an object from the system
	inline void removeObject(RenderableStaticObject* instatic)
	{
		// Remove object from our static objects vector
		for (std::vector<RenderableStaticObject*>::iterator ite = static_collision_objects.begin(); ite != static_collision_objects.end(); ) {
			if ( *ite == instatic) {
				ite = static_collision_objects.erase(ite);
			}
			else ++ite;
			//static_collision_objects.push_back(*ite);
		}
		//static_objects_to_remove.push_back(instatic);
	}

	//Remove a moving object
	void removeObject(RenderableMovingObject* inmoving)
	{
		// Remove object from our moving objects vector
		for (std::vector<RenderableMovingObject*>::iterator ite = moving_collision_objects.begin(); ite != moving_collision_objects.end(); ) {
			if ( *ite == inmoving) {
				ite = moving_collision_objects.erase(ite);
			}
			else ++ite;
		}
		//static_objects_to_remove.push_back(inmoving);
	}

	inline void clearAllMovingObjects()
	{
		moving_collision_objects.clear();
	}

	inline void clearAllStaticObjects()
	{
		static_collision_objects.clear();
	}	

	//Update method for the collision handler
	void updateCollisions();

protected:

	Ogre::SceneManager* mSceneMgr;
	Ogre::RaySceneQuery* mRaySceneQuery;

	//Check for collisions between objects
	void checkCollisions();

	std::vector<RenderableStaticObject*> static_collision_objects;
	std::vector<RenderableMovingObject*> moving_collision_objects;

};


#endif