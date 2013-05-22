
#include "LevelBlock.h"

LevelBlock::LevelBlock(float posx, float posy, float posz, Ogre::SceneManager* smn, float dim, LevelMaterial<> mat)
	: RenderableStaticObject(posx, posy, posz, smn), extension(dim), material(mat)
{
	// Set collision type
	myCollisionType = COLLISION_OBJECT_BLOCK;
	// We create the associated entity
	myEntity = smn->createEntity("cube.mesh");
	// LO do not cast shadows: it would be a waste of processing
	myEntity->setCastShadows(false);
	// Ogre materials specs are taken from the Material template
	myEntity->setMaterialName(material.getTexture());
	// We make the entity point to this associated object -> used in collision detection
	myEntity->setUserAny(Ogre::Any(this));

	// Childnode and creation
	mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mMainNode->attachObject(myEntity);
#ifdef _DEBUG
	//mMainNode->showBoundingBox(true);
#endif

	// Orientation is set
	Ogre::Quaternion q(Ogre::Degree(90), Ogre::Vector3::NEGATIVE_UNIT_Y);
	mMainNode->setOrientation(q);

	// dim is the side dimension I want
	Ogre::Real sf = Ogre::Real(dim / myEntity->getBoundingBox().getSize()[0]);
	// We do not scale the mesh but we correctly set a scale factor in the node
	mMainNode->setScale(Ogre::Vector3(sf, sf, sf));

	// Set position
	position = Ogre::Vector3(posx, posy, posz);
	mMainNode->setPosition(position);

}

LevelBlock::~LevelBlock()
{
	mMainNode->detachAllObjects();
	//delete myEntity;
	mMainNode->removeAndDestroyAllChildren();
	mSceneMgr->destroySceneNode(mMainNode);
	mSceneMgr->destroyEntity(myEntity);

}