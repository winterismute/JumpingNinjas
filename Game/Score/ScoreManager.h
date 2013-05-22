#ifndef _SCOREMANAGER
#define _SCOREMANAGER
#include <map>
#include <OgreSingleton.h>

#include "../Entity/Player/RenderableChar.h"

#include "../../Sound/Engine/SoundEngine.h"

class ScoreManager : public Ogre::Singleton<ScoreManager>
{
public:
	static ScoreManager* getSingletonPtr(void);
	
	ScoreManager(void);
	~ScoreManager(void);
	
	//Method for adding a player to the score system
	void addPlayer(RenderableChar* character);
	//Method for removing a player from the score system
	void removePlayer(RenderableChar* character);
	
	//gets a score for a character
	int getScore(RenderableChar* character);
	//Add a oint for a player
	void addPoint(RenderableChar* character);

	//Add a player by name
	void addPlayer(std::string name);
	//Remove a player by name
	void removePlayer(std::string name);
	//Get a score by name
	int getScore(std::string name);
	
	//Add a point by name
	void addPoint(std::string name);
	//Set a score
	void setScore(std::string name, int points);

	//Methods for setting and resetting a killing streak
	void setStreak(std::string name, int points);
	void resetStreak(std::string name);
	
	//Check if sound fx need to be played: OWNAGE!!1
	void checkAndPlayEffect(std::string name);

private:
	std::map<std::string, int> scores;
	std::map<std::string, int> streak;
	int limit;
	std::map<std::string, irrklang::ISoundSource*> sounds;

	bool beforeFirstKill;
};

#endif
