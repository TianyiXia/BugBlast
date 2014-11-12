#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <vector>
#include <string>

class Actor;
class Player;
class StudentWorld : public GameWorld
{
public:
	StudentWorld();
	
	~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	Actor* getActorAt(int x, int y);//exclude player

	Player* getPlayer() const;
	
	std::string getFileName(int level);

	void updateDisplayText();

	// Drop a sprayer here if allowed
    void addSprayerIfPossible(int x, int y, unsigned int maxSprayers);

    void dropSpray(int x, int y);

	bool playerDiedDuringThisTick();

	void setComplete();

	bool isComplete() const;

	bool allZumiDead() const;

	void decZumi();

	// Add an actor to the world
    void addActor(Actor* a);

	//maze parameter
	unsigned int getProbOfGoodieOverall() const;
	unsigned int getProbOfExtraLifeGoodie() const;
	unsigned int getProbOfWalkThruGoodie() const;
    unsigned int getProbOfMoreSprayersGoodie() const;
    unsigned int getTicksPerSimpleZumiMove() const;
    unsigned int getTicksPerComplexZumiMove() const;
	unsigned int getlevelBonus ()const;
	unsigned int getwalkThruLifetimeTicks () const;
	unsigned int getboostedSprayerLifetimeTicks() const;
	unsigned int getmaxBoostedSprayers() const;
    unsigned int getGoodieLifetimeInTicks() const;
    unsigned int getComplexZumiSearchDistance() const;

private:
	std::vector<Actor*> m_actorsList;
	Player* m_player;
	bool m_isComplete;
	int m_ZumiNum;

	unsigned int m_ProbOfGoodieOverall;
    unsigned int m_ProbOfExtraLifeGoodie;
    unsigned int m_ProbOfWalkThruGoodie;
    unsigned int m_ProbOfMoreSprayersGoodie;
    unsigned int m_TicksPerSimpleZumiMove;
    unsigned int m_TicksPerComplexZumiMove;
	unsigned int m_levelBonus;
	unsigned int m_walkThruLifetimeTicks;
	unsigned int m_boostedSprayerLifetimeTicks;
	unsigned int m_maxBoostedSprayers;
    unsigned int m_GoodieLifetimeInTicks;
    unsigned int m_ComplexZumiSearchDistance;
};

#endif // STUDENTWORLD_H_
