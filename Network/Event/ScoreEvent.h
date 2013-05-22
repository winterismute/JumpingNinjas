#pragma once
#include "NetworkEvent.h"
class ScoreEvent : public NetworkEvent
{
public:
	enum ScoreEventType{ ADDSCORE, SETSCORE, RESET_STREAK };
	ScoreEvent(std::string playerName, ScoreEventType type);
	std::string getPlayerName() const;
	ScoreEventType getScoreType() const;
	void setPoints(int points);
	int getPoints() const;
	~ScoreEvent(void);

private:
	std::string m_playerName;
	ScoreEventType m_scoreType;

	int m_points;
};

