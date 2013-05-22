#include "CollisionHandler.h"


CollisionHandler::CollisionHandler(Ogre::SceneManager* smn) : mSceneMgr(smn)
{
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	mRaySceneQuery->setSortByDistance(true, 3);
}
	
CollisionHandler::CollisionHandler(Ogre::SceneManager* smn, std::vector<RenderableStaticObject*> inputstatic) : mSceneMgr(smn)
{
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	mRaySceneQuery->setSortByDistance(true, 3);
	//std::vector<RenderableStaticObject*>::iterator ite;
	for (std::vector<RenderableStaticObject*>::iterator ite = inputstatic.begin(); ite != inputstatic.end(); ++ite) {
		static_collision_objects.push_back(*ite);
	}
}

CollisionHandler::CollisionHandler(Ogre::SceneManager* smn, std::vector<RenderableMovingObject*> inputmoving) : mSceneMgr(smn)
{
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	mRaySceneQuery->setSortByDistance(true, 3);
	for (std::vector<RenderableMovingObject*>::iterator ite = inputmoving.begin(); ite != inputmoving.end(); ++ite) {
		moving_collision_objects.push_back(*ite);
	}
}

CollisionHandler::CollisionHandler(Ogre::SceneManager* smn, std::vector<RenderableMovingObject*> inputmoving, std::vector<RenderableStaticObject*> inputstatic) : mSceneMgr(smn)
{
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	mRaySceneQuery->setSortByDistance(true, 3);
	for (std::vector<RenderableStaticObject*>::iterator ite = inputstatic.begin(); ite != inputstatic.end(); ++ite) {
		static_collision_objects.push_back(*ite);
	}
	for (std::vector<RenderableMovingObject*>::iterator ite = inputmoving.begin(); ite != inputmoving.end(); ++ite) {
		moving_collision_objects.push_back(*ite);
	}

}

CollisionHandler::~CollisionHandler()
{
	static_collision_objects.clear();
	moving_collision_objects.clear();
	mRaySceneQuery->clearResults();
	mSceneMgr->destroyQuery(mRaySceneQuery);

}

void CollisionHandler::updateCollisions()
{
	checkCollisions();
}


/*
void CollisionHandler::updateNewObjects()
{
	// Add the objects in the new lists
	for (std::vector<RenderableStaticObject*>::iterator ite = static_objects_to_add.begin(); ite != static_objects_to_add.end(); ++ite) {
		static_collision_objects.push_back(*ite);
	}
	for (std::vector<RenderableMovingObject*>::iterator ite = moving_objects_to_add.begin(); ite != moving_objects_to_remove.end(); ++ite) {
		moving_collision_objects.push_back(*ite);
	}

	static_objects_to_add.clear();
	moving_objects_to_add.clear();
}
*/

/*
void CollisionHandler::updateRemovedObjects()
{
	for (std::vector<RenderableStaticObject*>::iterator ite = static_objects_to_remove.begin(); ite != static_objects_to_remove.end(); ++ite) {
		static_collision_objects.erase(std::remove(static_collision_objects.begin(), static_collision_objects.end(), *ite), static_collision_objects.end());
		//static_collision_objects.push_back(*ite);
	}

	for (std::vector<RenderableMovingObject*>::iterator ite = moving_objects_to_remove.begin(); ite != moving_objects_to_remove.end(); ++ite) {
		moving_collision_objects.erase(std::remove(moving_collision_objects.begin(), moving_collision_objects.end(), *ite), moving_collision_objects.end());
		//static_collision_objects.push_back(*ite);
	}

	static_objects_to_remove.clear();
	moving_objects_to_remove.clear();

}
*/

void CollisionHandler::checkCollisions()
{
	RenderableChar* my_char  = NULL;
	LevelBlock* my_block = NULL;
	
	for (std::vector<RenderableMovingObject*>::iterator ite = moving_collision_objects.begin(); ite != moving_collision_objects.end(); ++ite) {
		bool found = false;
		if ( (*ite)->getVelocity().x > 0.0f ) {
			// I set a query ray RIGHTward
			mRaySceneQuery->setRay(Ogre::Ray( ((*ite)->getWorldPosition() + Ogre::Vector3((*ite)->getAABB().getHalfSize().x, 0.0f, 0.0f)), Ogre::Vector3::UNIT_X ));
			Ogre::RaySceneQueryResult& result_left = mRaySceneQuery->execute();
			Ogre::RaySceneQueryResult::iterator itr_left = result_left.begin();

			while (itr_left != result_left.end() && !found) {
				if ( itr_left->movable->getMovableType() == "Entity" ) {
					// Add cast to ENTITY HERE OR NOT?
					Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(itr_left->movable);

					// Now we know we are dealing with an entity. We want to know the object, if any
					// What we do is trying to cast as an ObjectCollision. Then we will call onCollision between two ObjectCollision which is fine
					// Every object extending ObjectCollision has its own version of OnCollision
					
					if ( Ogre::any_cast<LevelBlock*>(&ent->getUserAny()) ) {
						LevelBlock* mycobj = Ogre::any_cast<LevelBlock*>(ent->getUserAny());

						float dx = itr_left->distance;
						if ( dx < 0.1f ) {
							(*ite)->resetVelX();
							(*ite)->setPositionX(mycobj->getWorldPosition().x - mycobj->getAABB().getHalfSize().x - (*ite)->getAABB().getHalfSize().x - 0.1);
							(*ite)->onCollisionFromRight(mycobj);
							found = true;
						}
					}
					else if(Ogre::any_cast<RenderableChar*>(&ent->getUserAny())){
						RenderableChar* otherChar = Ogre::any_cast<RenderableChar*>(ent->getUserAny());
						if(otherChar != (RenderableChar*) *ite){
							float dx = itr_left->distance;
							if ( dx < 0.1f) {
								(*ite)->resetVelX();
								(*ite)->setPositionX(otherChar->getWorldPosition().x - otherChar->getAABB().getHalfSize().x - (*ite)->getAABB().getHalfSize().x - 0.1f);
								(*ite)->onCollisionFromRight(otherChar);
								found = true;
							}
						}

					}
				}
				itr_left++;
			}
		//result_left.clear();
		}

		if ( (*ite)->getVelocity().x < 0.0f ) {
			found = false;
			// I set a query RAY LEFTward
			mRaySceneQuery->setRay(Ogre::Ray( ((*ite)->getWorldPosition() - Ogre::Vector3((*ite)->getAABB().getHalfSize().x, 0.0f, 0.0f)), Ogre::Vector3::NEGATIVE_UNIT_X ));
			Ogre::RaySceneQueryResult& result_right = mRaySceneQuery->execute();
			Ogre::RaySceneQueryResult::iterator itr_right = result_right.begin();

			// If it is a movable object
			while (itr_right != result_right.end() && !found) {
				if ( itr_right->movable && itr_right->movable->getMovableType() == "Entity" ) {
					// Add cast to ENTITY HERE OR NOT?
					Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(itr_right->movable);

					// Now we know we are dealing with an entity. We want to know the object, if any
					// What we do is trying to cast as an ObjectCollision. Then we will call onCollision between two ObjectCollision which is fine
					// Every object extending ObjectCollision has its own version of OnCollision
					if ( Ogre::any_cast<LevelBlock*>(&ent->getUserAny()) ) {
						LevelBlock* mycobj = Ogre::any_cast<LevelBlock*>(ent->getUserAny());

						float dx = itr_right->distance;
						if ( dx < 0.4f) {
							// Roll back
							std::cout << "Box: " << (*ite)->getAABB().getSize().x << std::endl; 
							(*ite)->resetVelX();
							(*ite)->setPositionX(mycobj->getWorldPosition().x + mycobj->getAABB().getHalfSize().x + (*ite)->getAABB().getHalfSize().x + 0.4f);
							(*ite)->onCollisionFromLeft(mycobj);
							found = true;
						}
					}
					else if(Ogre::any_cast<RenderableChar*>(&ent->getUserAny()) && (int) Ogre::any_cast<RenderableChar*>(&ent->getUserAny()) != (int) (*ite)){
						RenderableChar* otherChar = Ogre::any_cast<RenderableChar*>(ent->getUserAny());
						if(otherChar != (RenderableChar*) *ite){
							float dx = itr_right->distance;
							if ( dx < 0.4f) {
								(*ite)->resetVelX();
								(*ite)->setPositionX(otherChar->getWorldPosition().x + otherChar->getAABB().getHalfSize().x + (*ite)->getAABB().getHalfSize().x + 0.4f);
								(*ite)->onCollisionFromLeft(otherChar);
								found = true;
							}
						}

					}

				}
				++itr_right;
			}
		}
	
		if ( (*ite)->getVelocity().y < 0.0f ) {
			found = false;
			// I set a query ray downward
			mRaySceneQuery->setRay(Ogre::Ray( ( (*ite)->getWorldPosition() - Ogre::Vector3(0.0f, (*ite)->getAABB().getHalfSize().y, 0.0f) ), Ogre::Vector3::NEGATIVE_UNIT_Y ));
			Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
			Ogre::RaySceneQueryResult::iterator itr = result.begin();
			//++itr;
			// If it is a movable object
			while (itr != result.end() && !found) {
				//std::cout << itr->movable->getMovableType() << std::endl;
				if ( itr->movable && itr->movable->getMovableType() == "Entity" ) {
					// Add cast to ENTITY HERE OR NOT?
					Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(itr->movable);

					// Now we know we are dealing with an entity. We want to know the object, if any
					// What we do is trying to cast as an ObjectCollision. Then we will call onCollision between two ObjectCollision which is fine
					// Every object extending ObjectCollision has its own version of OnCollision
					if ( Ogre::any_cast<LevelBlock*>(&ent->getUserAny()) ) {
						LevelBlock* mycobj = Ogre::any_cast<LevelBlock*>(ent->getUserAny());
						float dx = itr->distance;
						if ( dx < 0.1f) {
							(*ite)->resetVelY();
							(*ite)->setPositionY(mycobj->getWorldPosition().y + mycobj->getAABB().getHalfSize().y + 0.2f);
							(*ite)->onCollisionFromBelow(mycobj);
							found = true;
						}
					}
					else if(Ogre::any_cast<RenderableChar*>(&ent->getUserAny())){
						float dx = itr->distance;
						RenderableChar* otherChar = Ogre::any_cast<RenderableChar*>(ent->getUserAny());
						if(otherChar != (RenderableChar*) *ite){
							if ( dx < 0.1f) {
								// FILTER OUT COLLISIONS WITH DYING/RESP CHARS
								if (otherChar->getCharState() != RenderableChar::DEAD && otherChar->getCharState() != RenderableChar::SPAWNING) {
									(*ite)->resetVelY();
									(*ite)->setPositionY(otherChar->getWorldPosition().y + otherChar->getAABB().getHalfSize().y + (*ite)->getAABB().getHalfSize().y + 0.2f);
									(*ite)->onCollisionFromBelow(otherChar);
									otherChar->onCollisionFromAbove((*ite));
									found = true;
								}
							}
						}
					}
				}
				++itr;
			}
		}
		
		else if ( (*ite)->getVelocity().y > 0.0f ) {
			found = false;
			// I set a query upward
			mRaySceneQuery->setRay(Ogre::Ray( ( (*ite)->getWorldPosition() + Ogre::Vector3(0.0f, (*ite)->getAABB().getHalfSize().y, 0.0f) ), Ogre::Vector3::UNIT_Y ));
			Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
			Ogre::RaySceneQueryResult::iterator itr = result.begin();
			//++itr;
			// If it is a movable object
			while (itr != result.end() && !found) {
				//std::cout << itr->movable->getMovableType() << std::endl;
				if ( itr->movable && itr->movable->getMovableType() == "Entity" ) {
					//compare("Entity")) {
					// Add cast to ENTITY HERE OR NOT?
					Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(itr->movable);

					// Now we know we are dealing with an entity. We want to know the object, if any
					// What we do is trying to cast as an ObjectCollision. Then we will call onCollision between two ObjectCollision which is fine
					// Every object extending ObjectCollision has its own version of OnCollision
					if ( Ogre::any_cast<LevelBlock*>(&ent->getUserAny()) ) {
						LevelBlock* mycobj = Ogre::any_cast<LevelBlock*>(ent->getUserAny());
						float dx = itr->distance;
						std::cout << "Dist: " << dx << std::endl;
						if ( dx < 0.1f) {
							// Roll back
							std::cout << "Box: " << (*ite)->getAABB().getSize().y << std::endl; 
							(*ite)->resetVelY();
							(*ite)->setPositionY(mycobj->getWorldPosition().y - mycobj->getAABB().getHalfSize().y - (*ite)->getAABB().getSize().y - 0.2f);
							(*ite)->onCollisionFromAbove(mycobj);
							found = true;
						}
					}
					else if(Ogre::any_cast<RenderableChar*>(&ent->getUserAny()) && (int) Ogre::any_cast<RenderableChar*>(&ent->getUserAny()) != (int) (*ite)){
						RenderableChar* otherChar = Ogre::any_cast<RenderableChar*>(ent->getUserAny());
						if(otherChar != (RenderableChar*) *ite){
							float dx = itr->distance;
							if ( dx < 0.1f) {
								// FILTER OUT COLLISIONS WITH DYING/RESP CHARS
								if (otherChar->getCharState() != RenderableChar::DEAD && otherChar->getCharState() != RenderableChar::SPAWNING) {
									(*ite)->resetVelY();
									//(*ite)->setPositionY(otherChar->getWorldPosition().y - otherChar->getAABB().getHalfSize().y - 0.2f);
									(*ite)->setPositionY(otherChar->getWorldPosition().y - otherChar->getAABB().getHalfSize().y - (*ite)->getAABB().getSize().y - 0.2f);
									(*ite)->onCollisionFromAbove(otherChar);
									otherChar->onCollisionFromBelow((*ite));
									found = true;
								}
							}
						}
					}
					
				}
			++itr;
			}
		}
	}
}