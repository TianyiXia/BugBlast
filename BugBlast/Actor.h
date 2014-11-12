#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <string>
class StudentWorld;
/****************************Actor******************************************************************/
class Actor: public GraphObject
{
public:
	Actor(StudentWorld* game, int id, int startX, int startY);//actor constructor will set visible
	virtual ~Actor();
	virtual void doSomething()=0;
	StudentWorld* getWorld() const;
	std::string identifyActor(int x, int y) const;//
	bool isAlive() const;
	virtual bool coLocationWithPlayer() const;
	virtual void setDead();//display differnt sound when dead
private:
	StudentWorld* m_myWorld;
	bool m_isAlive;//every object except permBrick could be dead
};






/****************Brick*************************************************************************/
class Brick: public Actor
{
public:
	//constructor
	Brick(StudentWorld* game,int id,int startX, int startY);
	virtual void doSomething();

};

class PermBrick:public Brick
{
public:
	PermBrick(StudentWorld* game,int startX, int startY);
};

class DeBrick: public Brick
{
public:
	DeBrick(StudentWorld* game,int startX, int startY);
};



/****************Player***********************************************************************/
class Player: public Actor
{
public:
	//constructor
	Player(StudentWorld* game, int startX, int startY);
	virtual void doSomething();//overriding doSomething
	virtual void setDead();
	void activateWalkThrough();
	void activateMoreSprayer();
	//setDead 
private:
	int allowedDropNum;
	bool canWalkThrough;
	int walkThroughTime;
	int moreSprayerTime;
};

/*****************Zumi**********************************************************************/
class Zumi: public Actor
{
public:
	//constructor
    Zumi(StudentWorld* world, int startX, int startY, int imageID, unsigned int ticksPerMove);
	bool moveOrWait(); //return true if m_ticksPerMove is 1, else decrease m_ticksPerMove and return false 
	void applySimpleMove();//try move in curr direction, if meet bricks or Bugsprayer, pick new direction and return
	int getRandomDir() ;
	virtual void setDead();
private:
	int m_ticksPerMove;
	int m_currDir;//0 means up, 1 means right, 2 means down, 3 means left
};

/*****************SimpleZumi**********************************************************************/
class SimpleZumi :public Zumi
{
public:
	SimpleZumi(StudentWorld* world, int startX, int startY,unsigned int ticksPerMove);
    virtual void doSomething();
	virtual void setDead();//specify increased score
private:
};


/*************ComplexZumi*********************************************************************/

class ComplexZumi :public Zumi
{
public:
	ComplexZumi(StudentWorld* world, int startX, int startY,unsigned int ticksPerMove);
    virtual void doSomething();
	virtual void setDead();//specify increased score
private:
};









/**************TimeLifeActor***************************************************************/
//for this type of object, m_alive is ture when they first came up,
//when m_lifetime is 0, set it to false to let game know to despose it
class TimedLifetimeActor : public Actor
{
public:
    TimedLifetimeActor(StudentWorld* world, int startX, int startY, int imageID,unsigned int lifetime);
	void decreaseLifetime() ;
	bool timesUp() const;
	// Set remining lifetime to 0
    void expireImmediately();
private:
	int m_lifetime;
};

/**************BugSprayer***************************************************************/
class BugSprayer : public TimedLifetimeActor
{
public:
    BugSprayer(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
	void tryDropSpray();
};

/**************BugSpray***************************************************************/
class BugSpray : public TimedLifetimeActor
{
public:
    BugSpray(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

/***********ExtraLifeGoodie********************************************************/
class ExtraLifeGoodie : public TimedLifetimeActor
{
public:
	ExtraLifeGoodie(StudentWorld* world, int x, int y);
	virtual void doSomething();
};

/***********Walk Through Walls Goodie********************************************************/
class WalkThroughWallsGoodie : public TimedLifetimeActor
{
public:
	WalkThroughWallsGoodie(StudentWorld* world, int x, int y);
	virtual void doSomething();
};

/***********IncreaseSimultaneousSprayersGoodie********************************************************/
class IncreaseSimultaneousSprayersGoodie : public TimedLifetimeActor
{
public:
	IncreaseSimultaneousSprayersGoodie(StudentWorld* world, int x, int y);
	virtual void doSomething();
};

/**************Exit************************************************************************/

class Exit : public Actor
{
public:
    Exit(StudentWorld* world, int startX, int startY);
	void setActive();
    virtual void doSomething();
private:
	bool m_isActive;
};

#endif // ACTOR_H_
