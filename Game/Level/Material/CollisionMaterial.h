#pragma once
#include <string>

//Struct that defines the general template for collision material
struct CollisionMaterial{
	CollisionMaterial(){}
	
	CollisionMaterial(std::string name, float fric, std::string text){
		this->name = name;
		friction = fric;
		texture = text;

	}
	std::string name;
	float friction;
	std::string texture;
};

//Ice material
struct Ice : CollisionMaterial{
	Ice() : CollisionMaterial("Ice", 0.95f, "Examples/Frost"){
	}

};

//Brick like material
struct Brick : CollisionMaterial{
	Brick() : CollisionMaterial("Brick", 0.1f, "Examples/Rockwall"){}
};