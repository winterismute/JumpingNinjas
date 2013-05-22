#pragma once
#include <string>
#include "CollisionMaterial.h"

//Template class for defining LevelMaterials, can be defined as LevelMaterial<Ice> for example
template <class T = CollisionMaterial>
class LevelMaterial
{
public:

	LevelMaterial(){
	}

	LevelMaterial(T t){
		this->t = t;
	}

	//Template method for ensuring that a LevelMaterial<Ice> can be held in a variable like:
	//LevelMaterial<CollisionMaterial> which is more general
	 template < typename U >
	operator LevelMaterial<U> () const{
		return LevelMaterial<U>(t);
	}

	//Returns the name of the collision material
	std::string getName() const{
		return t.name;
	}
	
	//Returns the texture of the material
	std::string getTexture() const{
		return t.texture;
	}

	//Returns the friction cooficient
	float getFriction() const{
		return t.friction;
	}
	

private:
	T t;

};