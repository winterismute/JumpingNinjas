
#include "LevelBlock.h"

LevelBlock::LevelBlock(float posx, float posy, float posz, Ogre::SceneManager* smn, float dim, LevelMaterial<> mat)
	: RenderableStaticObject(posx, posy, posz, smn), extension(dim), material(mat)
{
	// Set the type of the OBJECT
	//mType = RenderableObject::OBJECT_BLOCK;
	// Set collision type
	myCollisionType = COLLISION_OBJECT_BLOCK;
	myEntity = smn->createEntity("cube.mesh");
	myEntity->setCastShadows(false);
	myEntity->setMaterialName(material.getTexture());
//	myEntity->setMaterialName("Examples/Rockwall");
	myEntity->setUserAny(Ogre::Any(this));

	//std::cout << "BB1 L: " << myEntity->getBoundingBox() << std::endl;
	//std::cout << "BB1 W: " << myEntity->getWorldBoundingBox() << std::endl;	
	//std::cout << "POS PREV: " << Ogre::Vector3(posx, posy, posy) << std::endl;

	mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mMainNode->attachObject(myEntity);
#ifdef _DEBUG
	//mMainNode->showBoundingBox(true);
#endif

	Ogre::Quaternion q(Ogre::Degree(90), Ogre::Vector3::NEGATIVE_UNIT_Y);
	mMainNode->setOrientation(q);

	// dim is the side dimension I want
	//Ogre::Real sf = Ogre::Real(myEntity->getBoundingBox().getSize()[0] / dim);
	Ogre::Real sf = Ogre::Real(dim / myEntity->getBoundingBox().getSize()[0]);
	mMainNode->setScale(Ogre::Vector3(sf, sf, sf));

	// We scaled the mesh. Now we scale also the BoundingBox and store it
	mAABBox = myEntity->getBoundingBox();
	mAABBox.scale(Ogre::Vector3(sf, sf, sf));

	position = Ogre::Vector3(posx, posy, posz);
	//position = Ogre::Vector3(posx, posy, posz) + (mAABBox.getHalfSize());
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