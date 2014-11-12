#include "StudentWorld.h"
#include "Actor.h"
#include "Level.h"
#include "GameConstants.h"
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator set
using namespace std;
GameWorld* createStudentWorld()
{
	return new StudentWorld();
}

StudentWorld::StudentWorld()
{
	vector<Actor*> m_actorsList;
	m_player=NULL;
	m_isComplete=false;
	m_ZumiNum=0;
}

StudentWorld::~StudentWorld()//???????????????????????????????
{
	for(unsigned int i=0;i<m_actorsList.size();i++)
		delete m_actorsList[i];
	delete m_player;
}



/***********helper function************************************/
void StudentWorld::updateDisplayText()
{
	int score=getScore();
	int level=getLevel();
	int playerLives=getLives();
	ostringstream oss;
	oss<<"Score: ";//for now assume : and num has one blank
	oss.fill('0');
	oss<<setw(8)<<score;
	oss<<"  Level: "<<setw(2)<<level<<"  Lives: "<<setw(3)<<getLives()
		<<"  Bonus:";
	oss.fill(' ');
	oss<<setw(6)<<m_levelBonus;
	string s=oss.str();
	setGameStatText(s);
}

bool StudentWorld::playerDiedDuringThisTick()
{
	return !getPlayer()->isAlive();
}

Player* StudentWorld::getPlayer() const
{
	return m_player;
}

Actor* StudentWorld::getActorAt(int x, int y)//return null if there is no match in container
{
	for(unsigned int i=0;i<m_actorsList.size();i++)
	{
		if(m_actorsList[i]!=NULL)
		{
			if(m_actorsList[i]->getX()==x && m_actorsList[i]->getY()==y )
				return m_actorsList[i];
		}
	}
	return NULL;
}

string StudentWorld::getFileName(int level)
{
	ostringstream oss;
	oss.fill('0');
	oss<<setw(2)<<level;
	return "level"+oss.str()+".dat";
}

void StudentWorld::addSprayerIfPossible(int x, int y, unsigned int maxSprayers)
{
	//went through list to find # of active/alive bugSprayer
	//notDead==>not exploaded yet
	unsigned int num=0;
	for(unsigned int i=0;i<m_actorsList.size();i++)
	{
		if(m_actorsList[i]!=NULL)
		{
			Actor* p=dynamic_cast<BugSprayer*>(m_actorsList[i]);
			if(p!=NULL && p->isAlive() )
				num++;
		}
	}
	if(num<maxSprayers)
		m_actorsList.push_back(new BugSprayer(this,x,y));
}

 // Drop bug spray here if allowed and return true if bug spray can be
 // sprayed beyond the actors at this position (e.g., false for a brick).
void StudentWorld::dropSpray(int x, int y)
{
	m_actorsList.push_back(new BugSpray(this,x,y));
}

void StudentWorld::setComplete()
{
	m_isComplete=true;
}

bool StudentWorld::isComplete () const
{
	return m_isComplete;
}

bool StudentWorld::allZumiDead() const
{
	return m_ZumiNum==0;
}

void StudentWorld::decZumi()
{
	m_ZumiNum--;
}

void StudentWorld::addActor(Actor* a)
{
	m_actorsList.push_back(a);
}

unsigned int StudentWorld::getProbOfGoodieOverall() const{return m_ProbOfGoodieOverall;}
unsigned int StudentWorld::getProbOfExtraLifeGoodie() const{return m_ProbOfExtraLifeGoodie;}
unsigned int StudentWorld::getProbOfWalkThruGoodie() const{return m_ProbOfWalkThruGoodie;}
unsigned int StudentWorld::getProbOfMoreSprayersGoodie() const{return m_ProbOfMoreSprayersGoodie;}
unsigned int StudentWorld::getTicksPerSimpleZumiMove() const{return m_TicksPerSimpleZumiMove;}
unsigned int StudentWorld::getTicksPerComplexZumiMove() const{return m_TicksPerComplexZumiMove;}

unsigned int StudentWorld::getlevelBonus() const{return m_levelBonus;}

unsigned int StudentWorld::getwalkThruLifetimeTicks() const {return m_walkThruLifetimeTicks;}
unsigned int StudentWorld::getboostedSprayerLifetimeTicks() const {return m_boostedSprayerLifetimeTicks;}
unsigned int StudentWorld::getmaxBoostedSprayers() const {return m_maxBoostedSprayers;}
unsigned int StudentWorld::getGoodieLifetimeInTicks() const {return m_GoodieLifetimeInTicks;}
unsigned int StudentWorld::getComplexZumiSearchDistance() const{return m_ComplexZumiSearchDistance;}
/************helper function*******************************/


int StudentWorld::init()
{
	Level lev;
	int currentLev=getLevel();
	Level::LoadResult result = lev.loadLevel(getFileName(currentLev));
	if (currentLev==0 && result == Level::load_fail_file_not_found)
		return GWSTATUS_NO_FIRST_LEVEL;
	else if(result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;
	else if (result == Level::load_success)
	{
		//initilize maze parameter
		m_ProbOfGoodieOverall=lev.getOptionValue(optionProbOfGoodieOverall);
		m_ProbOfExtraLifeGoodie=lev.getOptionValue(optionProbOfExtraLifeGoodie);
	    m_ProbOfWalkThruGoodie=lev.getOptionValue(optionProbOfWalkThruGoodie);
		m_ProbOfMoreSprayersGoodie=lev.getOptionValue(optionProbOfMoreSprayersGoodie);
		m_TicksPerSimpleZumiMove=lev.getOptionValue(optionTicksPerSimpleZumiMove);
		m_TicksPerComplexZumiMove=lev.getOptionValue(optionTicksPerComplexZumiMove);
		m_levelBonus=lev.getOptionValue(optionLevelBonus);
		m_walkThruLifetimeTicks=lev.getOptionValue(optionWalkThruLifetimeTicks);
		m_boostedSprayerLifetimeTicks=lev.getOptionValue(optionBoostedSprayerLifetimeTicks);
		m_maxBoostedSprayers=lev.getOptionValue(optionMaxBoostedSprayers);
		m_GoodieLifetimeInTicks=lev.getOptionValue(optionGoodieLifetimeInTicks);
		m_ComplexZumiSearchDistance=lev.getOptionValue(optionComplexZumiSearchDistance);

		int x,y,i=0;
		for(x=0;x<VIEW_WIDTH;x++)
		{
			for(y=0;y<VIEW_HEIGHT;y++)
			{
				Level::MazeEntry ge = lev.getContentsOf(x,y);
				switch (ge)
				{
				case Level::player:
					m_player= new Player(this,x,y);
					break;
				case Level::perma_brick:
					m_actorsList.push_back(new PermBrick(this,x,y));
					break;
				case Level::destroyable_brick:
					m_actorsList.push_back(new DeBrick(this,x,y));
					break;
				case Level::exit:
					m_actorsList.push_back(new Exit(this,x,y));
					break;
				case Level::simple_zumi:
					m_actorsList.push_back(new SimpleZumi(this,x,y,m_TicksPerSimpleZumiMove));
					m_ZumiNum++;
					break;
				case Level::complex_zumi:
					m_actorsList.push_back(new ComplexZumi(this,x,y,m_TicksPerSimpleZumiMove));
					m_ZumiNum++;
				}
			}
		}
		return GWSTATUS_CONTINUE_GAME;
	}
	else
		return GWSTATUS_LEVEL_ERROR;
}

int StudentWorld::move()
{
	updateDisplayText();
	//question: check result during round or after or both????????????
	if(m_player!=NULL)
		m_player->doSomething();
	for(unsigned int i=0;i<m_actorsList.size();i++)
	{
		if(m_actorsList[i]!=NULL)
		{
			m_actorsList[i]->doSomething();
			if(playerDiedDuringThisTick())
			{
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
			if(isComplete())
			{
				increaseScore(m_levelBonus);
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
	}
	//check player died
	if(!m_player->isAlive())
	{
		delete m_player;
		m_player=NULL;
		return GWSTATUS_PLAYER_DIED;
	}
	//check player win (Player at exit)

	//check other dead actor and delete
	for(unsigned int i=0;i<m_actorsList.size();i++)
	{
		if(m_actorsList[i]!=NULL)
		{
			if(!m_actorsList[i]->isAlive())
			{
				delete m_actorsList[i];
				m_actorsList[i]=NULL;
			}
		}
	}
	if(m_levelBonus>=0)
		m_levelBonus--;

	//check if player kill all zombie->activate exit
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	delete m_player;
	for(unsigned int i=0;i<m_actorsList.size();i++)
		delete m_actorsList[i];
	m_actorsList.clear();//renew the vector
	m_player=NULL;
	m_isComplete=false;
	m_ZumiNum=0;
}