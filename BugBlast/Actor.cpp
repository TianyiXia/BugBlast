#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <ctime>
using namespace std;

/****************************Actor******************************************************************/
Actor::	Actor(StudentWorld* game, int id, int startX, int startY)
	: GraphObject(id, startX,startY), m_myWorld(game) 
{
	setVisible(true);
	m_isAlive=true;
}

Actor::~Actor() {}

StudentWorld* Actor::getWorld() const 
{
	return m_myWorld;
}

string Actor::identifyActor(int x,int y) const
{
	//notice that one DeBrick and spray/people could be at same point?????
	//should n't be a problem because Brick always come first, leave it as it is for now
	Actor* p=getWorld()->getActorAt(x,y);
	if(p!=NULL)
	{
		//there is an actor at least
		PermBrick* p1 = dynamic_cast<PermBrick*>(p);
		DeBrick* p2 = dynamic_cast<DeBrick*>(p);
		BugSprayer* p3=dynamic_cast<BugSprayer*>(p);//other constraint makes sure it cannot be overlapping except with Player/spray, in spray case sprayer will be picked
		SimpleZumi* p4=dynamic_cast<SimpleZumi*>(p);//when Zumi and spray on same brick, zumi should be detected first because it was added first
		ComplexZumi* p5=dynamic_cast<ComplexZumi*>(p);
		if (p1 != nullptr )
			return "PermBrick";
		if( p2 !=nullptr)
			return "DeBrick";
		if(p3 !=nullptr)
			return "BugSprayer";
		if(p4 !=nullptr)
			return "SimpleZumi";
		if(p5 !=nullptr)
			return "ComplexZumi";
	}
	//need to check if any other actors cannot be on same pos??????????????
	return "";
}

bool Actor::coLocationWithPlayer() const //used by zumis and spray,check if they are on same 
										//brick as Player, and kill player
{
	if( getWorld()->getPlayer()->getX()==getX() && getWorld()->getPlayer()->getY()==getY())
	{
		return true;
	}
	else
		return false;
}

bool Actor::isAlive() const
{
	return m_isAlive;
}
void Actor::setDead()
{
	m_isAlive=false;
}
/****************Brick*************************************************************************/
Brick::Brick(StudentWorld* game,int id,int startX, int startY) 
	: Actor(game,id, startX,startY) 
{
}

void Brick::doSomething() {}//brick do nothing


PermBrick::PermBrick(StudentWorld* game,int startX, int startY)
	: Brick(game,IID_PERMA_BRICK, startX,startY) 
{	
}



DeBrick::DeBrick(StudentWorld* game,int startX, int startY)
	: Brick(game,IID_DESTROYABLE_BRICK, startX,startY) 
{	
}



/****************Player***********************************************************************/
Player::Player(StudentWorld* game, int startX, int startY) 
	: Actor(game,IID_PLAYER, startX,startY) 
{
	allowedDropNum=2;
	canWalkThrough=false;
}

void Player::doSomething()
{
	if(!isAlive())
		return;
	if(identifyActor(getX(),getY())=="SimpleZumi" || identifyActor(getX(),getY())=="ComplexZumi"
		|| (!canWalkThrough && identifyActor(getX(),getY())=="DeBrick"))
	{//????????add complex zumi
		setDead();
		return;
	}
	 
	if(canWalkThrough)
	{
		if(walkThroughTime==0)
			canWalkThrough=false;
		walkThroughTime--;
	}
	if(allowedDropNum!=2)
	{
		if(moreSprayerTime==0)
		{
			allowedDropNum=2;
		}
		moreSprayerTime--;
	}

	int ch;
	if(getWorld()->getKey(ch))
	{
		//allowable case1) neither a PermBrick nor a Debrick
		// or
		//allowable case2) canWalkthrough and a not a PermBrick
		if(ch==KEY_PRESS_DOWN && 
			(((identifyActor(getX(), getY()-1)!="PermBrick" && identifyActor(getX(), getY()-1)!="DeBrick")) || (canWalkThrough && identifyActor(getX(), getY()-1)!="PermBrick")))
			moveTo(getX(), getY()-1);
		if(ch==KEY_PRESS_LEFT && 
			(((identifyActor(getX()-1, getY())!="PermBrick" && identifyActor(getX()-1, getY())!="DeBrick")) || (canWalkThrough && identifyActor(getX()-1, getY())!="PermBrick")))
			moveTo(getX()-1, getY());
		if(ch==KEY_PRESS_RIGHT &&
			(((identifyActor(getX()+1, getY())!="PermBrick" && identifyActor(getX()+1, getY())!="DeBrick")) || (canWalkThrough && identifyActor(getX()+1, getY())!="PermBrick")))
			moveTo(getX()+1, getY());
		if(ch==KEY_PRESS_UP &&
		   (((identifyActor(getX(), getY()+1)!="PermBrick"   && identifyActor(getX(), getY()+1)!="DeBrick")) || (canWalkThrough && identifyActor(getX(), getY()+1)!="PermBrick")))
			moveTo(getX(), getY()+1);
		if(ch==KEY_PRESS_SPACE && identifyActor(getX(), getY())=="")
			//need addtional check?????????????????? 
			//1)when player on destroyable brick,it cannot drop sprayer
			//2)when already a sprayer, cannot drop another at the same place
		{
			getWorld()->addSprayerIfPossible(getX(),getY(),allowedDropNum);//????max sprayer to be changed
		}
	}
}

void Player::setDead()
{
	Actor::setDead();
	getWorld()->playSound(SOUND_PLAYER_DIE);
}

void Player::activateWalkThrough()
{
	canWalkThrough=true;
	walkThroughTime=getWorld()->getwalkThruLifetimeTicks();
}

void Player::activateMoreSprayer()
{
	allowedDropNum=getWorld()->getmaxBoostedSprayers();
	moreSprayerTime=getWorld()->getboostedSprayerLifetimeTicks();
}

/*****************Zumi**********************************************************************/
Zumi::Zumi(StudentWorld* game,int id, int startX, int startY,unsigned int ticksPerMove)
	:Actor(game,id,startX,startY)
{
	m_ticksPerMove=ticksPerMove;
	m_currDir=getRandomDir();
}
	int Zumi::getRandomDir() 
	{
		return rand()%4;
	}

bool Zumi::moveOrWait()
{
	if(m_ticksPerMove==1)
		return true;
	else
	{
		m_ticksPerMove--;
		return false;
	}
}

void Zumi::applySimpleMove()
{
	if(m_currDir==0 && identifyActor(getX(),getY()+1)=="")
	{
		moveTo(getX(),getY()+1);
		m_ticksPerMove=getWorld()->getTicksPerSimpleZumiMove();
	}
	else if(m_currDir==1 && identifyActor(getX()+1,getY())=="")
	{
		moveTo(getX()+1,getY());
		m_ticksPerMove=getWorld()->getTicksPerSimpleZumiMove();
	}
	else if(m_currDir==2 && identifyActor(getX(),getY()-1)=="")
	{
		moveTo(getX(),getY()-1);
		m_ticksPerMove=getWorld()->getTicksPerSimpleZumiMove();
	}
	else if(m_currDir==3 && identifyActor(getX()-1,getY())=="")
	{
		moveTo(getX()-1,getY());
		m_ticksPerMove=getWorld()->getTicksPerSimpleZumiMove();
	}
	else
		m_currDir=getRandomDir();
}

void Zumi::setDead()
{
	Actor::setDead();
	getWorld()->playSound(SOUND_ENEMY_DIE);
	getWorld()->decZumi();
	unsigned int randNum = rand() % 100;
	if (randNum < getWorld()->getProbOfGoodieOverall())
	{
		if (randNum < getWorld()->getProbOfExtraLifeGoodie())
			getWorld()->addActor( new ExtraLifeGoodie(getWorld(), getX(), getY()) );
		else if (randNum >= getWorld()->getProbOfExtraLifeGoodie() && 
			randNum < (getWorld()->getProbOfExtraLifeGoodie() + getWorld()->getProbOfWalkThruGoodie()))
			getWorld()->addActor(new WalkThroughWallsGoodie(getWorld(), getX(), getY()));
		else if (randNum >= (getWorld()->getProbOfExtraLifeGoodie() + getWorld()->getProbOfWalkThruGoodie()) && 
			randNum < 100)
			getWorld()->addActor(new IncreaseSimultaneousSprayersGoodie(getWorld(), getX(), getY()));
	}
	//i will over riden setDead in Complex/Simple Zumi to change increased score
}

/*****************SimpleZumi**********************************************************************/
SimpleZumi::SimpleZumi(StudentWorld* world, int startX, int startY,unsigned int ticksPerMove)
	:Zumi(world,IID_SIMPLE_ZUMI,startX,startY,ticksPerMove)
{
	 
}

void SimpleZumi::doSomething()
{
	/*if(!isAlive())
		return;
	else
	{
		if(coLocationWithPlayer())
			getWorld()->getPlayer()->setDead();
		if(moveOrWait())//return true when time to move
		{
			applySimpleMove();
		}
	}*/
}


void SimpleZumi::setDead()
{
	Zumi::setDead();
	getWorld()->increaseScore(100);
}

/*************ComplexZumi*********************************************************************/
ComplexZumi::ComplexZumi(StudentWorld* world, int startX, int startY,unsigned int ticksPerMove)
	:Zumi(world,IID_COMPLEX_ZUMI,startX,startY,ticksPerMove)
{
	 
}

void ComplexZumi::doSomething()
{
	if(!isAlive())
		return;
	else
	{
		if(coLocationWithPlayer())
			getWorld()->getPlayer()->setDead();
		if(moveOrWait())//return true when time to move
		{
			applySimpleMove();
		}
	}
}


void ComplexZumi::setDead()
{
	Zumi::setDead();
	getWorld()->increaseScore(500);
}
/**************TimeLifeActor***************************************************************/
TimedLifetimeActor::TimedLifetimeActor(StudentWorld* world, int imageID, int startX, int startY, 
                                    unsigned int lifetime)
	:Actor(world,imageID,startX,startY)
{
	m_lifetime=lifetime;
}

bool TimedLifetimeActor::timesUp() const
{
	return m_lifetime==0;
}

void TimedLifetimeActor::decreaseLifetime() 
{
	m_lifetime--;
}

void TimedLifetimeActor::expireImmediately()
{
	m_lifetime=0;
}

/**************BugSprayer***************************************************************/
   BugSprayer::BugSprayer(StudentWorld* world, int startX, int startY)
	   : TimedLifetimeActor(world,IID_BUGSPRAYER,startX,startY,40)
   {}

   void BugSprayer::doSomething()
   {
	   if(!isAlive())
		   return;
	   else
	   {
		   if(timesUp())
		   {
			   //release bug sprayer
			   tryDropSpray();//look for 4 direction, and call function form studentWorld to drop spray
			   getWorld()->playSound(SOUND_SPRAY);
			   setDead();
		   }
		   decreaseLifetime();
	   }
   }


   void BugSprayer::tryDropSpray()
   {
	   //check four directions
	   int x=getX();
	   int y=getY();
	   for(int i=0;i<=2;i++)
	   {
		   if(identifyActor(x+i,y)!="PermBrick")//destroyable mist is allowed
			   getWorld()->dropSpray(x+i,y);
		   if(identifyActor(x+i,y)=="DeBrick" || identifyActor(x+i,y)=="PermBrick" )//any brick prevent mist from spreading
			   break;
	   }
	   for(int i=1;i<=2;i++)
	   {
		   if(identifyActor(x-i,y)!="PermBrick")
			   getWorld()->dropSpray(x-i,y);
		   if(identifyActor(x-i,y)=="DeBrick" || identifyActor(x-i,y)=="PermBrick")
			   break;
	   }
	   for(int i=1;i<=2;i++)
	   {
		   if(identifyActor(x,y+i)!="PermBrick")
			   getWorld()->dropSpray(x,y+i);
		   if(identifyActor(x,y+i)=="DeBrick" || identifyActor(x,y+i)=="PermBrick")
			   break;
	   }
	   for(int i=1;i<=2;i++)
	   {
		   if(identifyActor(x,y-i)!="PermBrick")
			   getWorld()->dropSpray(x,y-i);
		   if(identifyActor(x,y-i)=="DeBrick" || identifyActor(x,y-i)=="PermBrick")
			   break;
	   }
   }

/**************BugSpray***************************************************************/
   BugSpray::BugSpray(StudentWorld* world, int startX, int startY)
	   : TimedLifetimeActor(world,IID_BUGSPRAY,startX,startY,3)
   {
   }
   void BugSpray::doSomething()
   {
	   if(!isAlive())
		   return;
	   decreaseLifetime();
	   if(timesUp())
		   setDead();
	   else
	   {
		   int x=getX();
		   int y=getY();
		   if(identifyActor(x,y)=="DeBrick" || identifyActor(x,y)=="SimpleZumi" || identifyActor(x,y)=="ComplexZumi")
			   getWorld()->getActorAt(x,y)->setDead();
		   if(coLocationWithPlayer())
			   getWorld()->getPlayer()->setDead();
		   if(identifyActor(x,y)=="BugSprayer")
			   (dynamic_cast<BugSprayer*>(getWorld()->getActorAt(x,y)))->expireImmediately();
	   }
   }

/******************ExtraLifeGoodie********************************************************/
   ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* world, int x, int y)
	   :TimedLifetimeActor(world,IID_EXTRA_LIFE_GOODIE,x,y,world->getGoodieLifetimeInTicks())
   {}

   void ExtraLifeGoodie::doSomething()
   {
	   if(!isAlive())
		   return;
	   decreaseLifetime();
	   if(timesUp())
	   {
		   setDead();
	   }
	   else
	   {
		   if(coLocationWithPlayer())
		   {
			   getWorld()->incLives();
			   getWorld()->increaseScore(1000);
			   setDead();
			   getWorld()->playSound(SOUND_GOT_GOODIE);
		   }
	   }
   }


/***********Walk Through Walls Goodie********************************************************/
   WalkThroughWallsGoodie::WalkThroughWallsGoodie(StudentWorld* world, int x, int y)
	   :TimedLifetimeActor(world,IID_WALK_THRU_GOODIE,x,y,world->getGoodieLifetimeInTicks())
   {
   }

   void WalkThroughWallsGoodie::doSomething()
   {
	   if(!isAlive())
		   return;
	   decreaseLifetime();
	   if(timesUp())
	   {
		   setDead();
	   }
	   if(coLocationWithPlayer())
	   {
		   getWorld()->getPlayer()->activateWalkThrough();
		   getWorld()->increaseScore(1000);
		   setDead();
		   getWorld()->playSound(SOUND_GOT_GOODIE);
	   }
   }

/***********IncreaseSimultaneousSprayersGoodie********************************************************/
IncreaseSimultaneousSprayersGoodie::IncreaseSimultaneousSprayersGoodie(StudentWorld* world, int x, int y)
	   :TimedLifetimeActor(world,IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE,x,y,world->getGoodieLifetimeInTicks())
   {}

   void IncreaseSimultaneousSprayersGoodie::doSomething()
   {
	   if(!isAlive())
		   return;
	   decreaseLifetime();
	   if(timesUp())
	   {
		   setDead();
	   }
	   if(coLocationWithPlayer())
	   {
		   getWorld()->getPlayer()->activateMoreSprayer();
		   getWorld()->increaseScore(1000);
		   setDead();
		   getWorld()->playSound(SOUND_GOT_GOODIE);
	   }
   }

/**********************************************exit***********************/
Exit::Exit(StudentWorld* world, int startX, int startY)
	:Actor(world,IID_EXIT,startX,startY)
{
	setVisible(false);
	m_isActive=false;
}

void Exit::doSomething()
{
	if(getWorld()->allZumiDead())
	{
		m_isActive=true;
		setVisible(true);
	}
	if(!m_isActive)
		return;
	else
	{
		if(coLocationWithPlayer())
		{
			getWorld()->playSound(SOUND_FINISHED_LEVEL);
			getWorld()->setComplete();
		}
	}
}

void Exit::setActive()
{
	m_isActive=true;
}