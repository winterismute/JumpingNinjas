#include "ScoreManager.h"
#include "../../Network/Event/NetworkEventManager.h"
#include "../../Network/Event/ScoreEvent.h"

#define DEFAULT_LIMIT 10

template<> ScoreManager* Ogre::Singleton<ScoreManager>::ms_Singleton = 0;

ScoreManager::ScoreManager(void) : limit(DEFAULT_LIMIT), beforeFirstKill(true)
{
	irrklang::ISoundSource* ns;
	ns = SoundEngine::getSingletonPtr()->loadSound("ef/firstblood.wav");
	sounds["FirstBlood"] = ns;

	ns = SoundEngine::getSingletonPtr()->loadSound("ef/triple_kill.wav");
	sounds["TripleKill"] = ns;

	ns = SoundEngine::getSingletonPtr()->loadSound("ef/monster_kill.wav");
	sounds["MonsterKill"] = ns;

	ns = SoundEngine::getSingletonPtr()->loadSound("ef/Ownage.wav");
	sounds["Ownage"] = ns;

	ns = SoundEngine::getSingletonPtr()->loadSound("ef/Killing_spree.wav");
	sounds["KillingSpree"] = ns;

	assert(sounds["FirstBlood"] != NULL);
	assert(sounds["TripleKill"] != NULL);
	assert(sounds["MonsterKill"] != NULL);
	assert(sounds["Ownage"] != NULL);
	assert(sounds["KillingSpree"] != NULL);

}

ScoreManager::~ScoreManager(void)
{
	sounds.clear();
}

void ScoreManager::addPlayer(RenderableChar* character){
	addPlayer(character->GetName());
}

void ScoreManager::addPlayer( std::string name )
{
	scores[name] = 0;
	streak[name] = 0;
}

void ScoreManager::removePlayer(RenderableChar* character){
	removePlayer(character->GetName());
}

void ScoreManager::removePlayer( std::string name )
{
	scores.erase(scores.find(name));
	streak.erase(streak.find(name));
}

ScoreManager* ScoreManager::getSingletonPtr(void){
	return ms_Singleton;
}

int ScoreManager::getScore(RenderableChar* charachter){
	return getScore(charachter->GetName());
}

int ScoreManager::getScore( std::string name )
{
	return scores[name];
}

void ScoreManager::addPoint(RenderableChar* charachter){
	addPoint(charachter->GetName());	
}

void ScoreManager::checkAndPlayEffect(std::string name)
{
	if(beforeFirstKill){
		SoundEngine::getSingletonPtr()->playSound(sounds["FirstBlood"]);
		beforeFirstKill = false;
	}
	if(streak[name] == 3)
		SoundEngine::getSingletonPtr()->playSound(sounds["TripleKill"]);
	else if(streak[name] == 5)
		SoundEngine::getSingletonPtr()->playSound(sounds["KillingSpree"]);
	else if(streak[name] == 7)
		SoundEngine::getSingletonPtr()->playSound(sounds["Ownage"]);
	else if(streak[name] == 10)
		SoundEngine::getSingletonPtr()->playSound(sounds["MonsterKill"]);
}

void ScoreManager::addPoint( std::string name )
{
	scores[name] = getScore(name) + 1;
	std::cout << "Added score: " << scores[name] << std::endl;
	NetworkEventManager::getSingletonPtr()->fireEvent(new ScoreEvent(name, ScoreEvent::ADDSCORE));
	streak[name] = streak[name] + 1;
	checkAndPlayEffect(name);
}

void ScoreManager::setScore(std::string name, int points)
{
	scores[name] = points;
	ScoreEvent* sEvent = new ScoreEvent(name, ScoreEvent::SETSCORE);
	sEvent->setPoints(points);
	NetworkEventManager::getSingletonPtr()->fireEvent(sEvent);
}

void ScoreManager::resetStreak( std::string name )
{
	streak[name] = 0;
	NetworkEventManager::getSingletonPtr()->fireEvent(new ScoreEvent(name, ScoreEvent::RESET_STREAK));
}

void ScoreManager::setStreak( std::string name, int points )
{
	streak[name] = points;
}
