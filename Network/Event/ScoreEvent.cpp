#include "ScoreEvent.h"


ScoreEvent::ScoreEvent(std::string playerName, ScoreEventType type) : m_playerName(playerName)
{
	m_type = NetworkEventType::SCORE;
	this->m_playerName = playerName;

	if(type == ADDSCORE)
		m_methodToCall = "AddScore";
	if(type == SETSCORE)
		m_methodToCall = "SetScore";
	if(type == RESET_STREAK)
		m_methodToCall = "ResetStreak";

	m_scoreType = type;
}


ScoreEvent::~ScoreEvent(void)
{
}

std::string ScoreEvent::getPlayerName() const
{
	return m_playerName;
}

ScoreEvent::ScoreEventType ScoreEvent::getScoreType() const
{
	return m_scoreType;
}

void ScoreEvent::setPoints( int points )
{
	m_points = points;
}

int ScoreEvent::getPoints() const
{
	return m_points;
}
