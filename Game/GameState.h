#ifndef GAME_STATE_H
#define GAME_STATE_H

// include headers for serialization
#include <fstream>
#include <string>
#include <iostream>
#include <assert.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

/*
	This file is the heart of the serialization: it includes actually more than one class.
	Each state class seizes the attributes needed to save and restore the corresponding object.
	boost archives are used for actual serialization
*/

class PlayerState
{
public:
	PlayerState() : px(0.0f), py(0.0f), pz(0.0f), name(std::string(""))
	{

	};

	PlayerState(float x, float y, float z, std::string nm) : px(x), py(y), pz(z), name(nm)
	{

	};

	virtual ~PlayerState()
	{
		// DO NOTHING
	};

	float getPosX()
	{
		return px;
	};

	float getPosY()
	{
		return py;
	};

	float getPosZ()
	{
		return pz;
	};

	std::string getName()
	{
		return name;
	};

protected:
	float px;
	float py;
	float pz;
	std::string name;

private:
	friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		std::cout << "GameState TRYING TO SERIALIZE" << std::endl;
		ar & px;
        ar & py;
		ar & pz;
		ar & name;
    }

};

class LevelBlockState
{
public:
	LevelBlockState() : px(0.0f), py(0.0f), pz(0.0f), dim(1.0f)
	{

	};
	
	LevelBlockState(float x, float y, float z, float d) : px(x), py(y), pz(z), dim(d)
	{

	};
	
	~LevelBlockState()
	{
		// DO NOTHING
	};

	float getPosX()
	{
		return px;
	};

	float getPosY()
	{
		return py;
	};

	float getPosZ()
	{
		return pz;
	};

	float getDimension()
	{
		return dim;
	};

protected:
	float px;
	float py;
	float pz;
	float dim;

private:
	friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		std::cout << "LEVELBLOCKSTATE TRYING TO SERIALIZE" << std::endl;
		ar & px;
        ar & py;
		ar & pz;
		ar & dim;
		//ar & myStringPointer;
    }

};

class GameState
{

public:
	GameState();
	virtual ~GameState();

	void addPlayerState(float playerx, float playery, float playerz, std::string playername);
	void addLevelBlockState(float levelblockx, float levelblocky, float levelblockz, float levelblockdim);
	void addScore(int newscore);

	inline PlayerState getPlayerState(int n)
	{
		assert(n < playerStateVector.size());
		return playerStateVector[n];
	}

	inline int getPlayerStateNumber()
	{
		return playerStateVector.size();
	}

	inline LevelBlockState getLevelBlockState(int n)
	{
		assert (n < levelBlockStateVector.size());
		return levelBlockStateVector[n];
	}

	inline int getLevelBlockStateNumber()
	{
		return levelBlockStateVector.size();
	}

	inline int getScore(int n)
	{
		assert (n < scoreVector.size());
		return scoreVector[n];
	}

	inline int getScoreNumber()
	{
		return scoreVector.size();
	}

	inline int getLevelWidth()
	{
		return levelwidth;
	}

	inline int getLevelHeight()
	{
		return levelheight;
	}

protected:
	std::vector<PlayerState> playerStateVector;
	std::vector<LevelBlockState> levelBlockStateVector;
	std::vector<int> scoreVector;
	int levelwidth;
	int levelheight;

private:
	friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		assert(playerStateVector.size() == scoreVector.size());
		std::cout << "GAMESTATE TRYING TO SERIALIZE" << std::endl;
		ar & playerStateVector;
		ar & levelBlockStateVector;
		ar & scoreVector;
		ar & levelwidth;
		ar & levelheight;
    }

};

#endif