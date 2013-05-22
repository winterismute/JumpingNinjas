#include <fstream>
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>

#include "LevelFromFileFactory.h"

LevelFromFileFactory::LevelFromFileFactory(Ogre::SceneManager* smn) : mSceneMgr(smn), levelno(1)
{

}

LevelFromFileFactory::~LevelFromFileFactory()
{

}

Level* LevelFromFileFactory::create()
{
	std::string levelname = "level";
	bool found = false;
	std::string name;
	do {
		name = levelname + boost::lexical_cast<std::string>(levelno) + ".txt";
		std::ifstream my_file(name);
		found = my_file.good();
		levelno++;
		if (levelno > MAX_LEVELS) levelno = 1;
		my_file.close();
	} while (!found && levelno < MAX_LEVELS + 1);
	
	if (levelno - 1 > MAX_LEVELS) {
		std::cerr << "ERROR: Not being able to load a level from file" << std::endl;
		exit(-1);
	}
	else std::cout << "Opening level file: " << name << std::endl;

	return (new Level(name, mSceneMgr));
}