#include "GameState.h"

GameState::GameState()
{
	//myInt = 1;
	//myFloat = 5.72f;
	//myPosVector.push_back(PositionState(1.0f, 1.0f, 1.0f));
	//myPosVector.push_back(PositionState(1.0f, 1.0f, 1.0f));
	//myPos = new PositionState(1.0f, 2.0f, 3.0f);
	//myString = std::string("Base");
	//myStringPointer = new std::string("Hello");
	std::cout << "GameState CONSTR called" << std::endl;
}

void GameState::addPlayerState(float playerx, float playery, float playerz, std::string playername)
{
	playerStateVector.push_back(PlayerState(playerx, playery, playerz, playername));
}
	
void GameState::addLevelBlockState(float levelblockx, float levelblocky, float levelblockz, float levelblockdim)
{
	levelBlockStateVector.push_back(LevelBlockState(levelblockx, levelblocky, levelblockz, levelblockdim));
}

void GameState::addScore(int newscore)
{
	scoreVector.push_back(newscore);
}

GameState::~GameState()
{
	// Maybe free
	//delete myStringPointer;
	scoreVector.clear();
	levelBlockStateVector.clear();
	playerStateVector.clear();
	std::cout << "GameState DESTR called" << std::endl;
}