#pragma once

#include "ActionConsideration.h"
#include "../Game/src/Player.h"
#include <vector>
#include <tuple>
//#include "src/Controller_AI_MichaelReveliotis.h"
#include "Constants.h"
#include "EntityStats.h"
#include "../Game/src/Mob.h"

#define DEFAULT_RANK 1.f
#define DEFAULT_WEIGHT 1.f
#define DEFAULT_BONUS 1.f
#define DEFAULT_MULT 1.f

static const Vec2 ksGiantPosL(LEFT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);
static const Vec2 ksGiantPosR(RIGHT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);
static const Vec2 ksArcherPosL(LEFT_BRIDGE_CENTER_X, 0.f);
static const Vec2 ksArcherPosR(RIGHT_BRIDGE_CENTER_X, 0.f);
static const Vec2 ksSwordsmanPosL(LEFT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);
static const Vec2 ksSwordsmanPosR(RIGHT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);



class UtilityAction
{
public:
	
	UtilityAction(iPlayer& player)
	{
		m_player = &player;
		m_pElixir = m_player->getElixir();
		m_eElixir = Game::get().getPlayer(!m_player->isNorth()).getElixir();
		//Get left building's health (corresponds to 1st building in building array)
		leftBuildingHealth = (float)m_player->getOpponentBuilding(1).m_Health;
		//Get right building's health (corresponds to 2nd building in building array)
		rightBuildingHealth = (float)m_player->getOpponentBuilding(2).m_Health;
		//Get king's health (corresponds to 0th building in building array)
		kingBuildingHealth = (float)m_player->getOpponentBuilding(0).m_Health;

		std::vector<Entity*> opponentMobs = Game::get().getPlayer(!m_player->isNorth()).getMobs();
		int leftArcherCount = 0;
		int rightArcherCount = 0;
		int leftGiantCount = 0;
		int rightGiantCount = 0;
		int leftSwordsmanCount = 0;
		int rightSwordsmanCount = 0;
		for (Entity* mob : opponentMobs)
		{
			if (mob->getStats().getMobType() == iEntityStats::Archer)
			{
				if (mob->getPosition().x < GAME_GRID_WIDTH / 2) {
					leftArcherCount += 1;
				}
				else
				{
					rightArcherCount += 1;
				}
			}
			else if (mob->getStats().getMobType() == iEntityStats::Swordsman)
			{
				if (mob->getPosition().x < GAME_GRID_WIDTH / 2) {
					leftSwordsmanCount += 1;
				}
				else
				{
					rightSwordsmanCount += 1;
				}
			}
			else if (mob->getStats().getMobType() == iEntityStats::Giant)
			{
				if (mob->getPosition().x < GAME_GRID_WIDTH / 2) {
					leftGiantCount += 1;
				}
				else
				{
					rightGiantCount += 1;
				}
			}
		}
		numGiantsLeft = (float)leftGiantCount;
		numGiantsRight = (float)rightGiantCount;
		numSwordsmenLeft = (float)leftSwordsmanCount;
		numSwordsmenRight = (float)rightSwordsmanCount;
		numArchersLeft = (float)leftArcherCount;
		numArchersRight = (float)rightArcherCount;
	}
	
	~UtilityAction()
	{
		for(size_t i = 0; i < considerations.size(); ++i)
		{
			delete considerations[i];
		}
		//DO NOT TOUCH PLAYER
	}
	//Each frame, update the rank and weight of this action
	void tick()
	{
		update();
		//Go through each consideration and get a rank, a bonus, and a multiplier
		std::vector<std::tuple <float, float, float>> rank_bonus_multipliers;
		for (ActionConsideration* c : considerations)
		{
			rank_bonus_multipliers.push_back(c->getScore());
		}

		//TODO: Update rank and weight
		//Sorted based on first value in tuple (rank) in descending order
		std::sort(rank_bonus_multipliers.begin(), rank_bonus_multipliers.end());
		std::reverse(rank_bonus_multipliers.begin(), rank_bonus_multipliers.end());
		m_rank = (int)std::get<0>(rank_bonus_multipliers[0]);
		float total_bonus = 1.f;
		float total_mult = 1.f;
		//Sum all bonuses and multiply all multipliers
		for(std::tuple<float, float, float> rbm : rank_bonus_multipliers)
		{
			total_bonus *= std::get<1>(rbm);
			std::cout << "Total bonus: " << total_bonus << std::endl;
			total_mult *= std::get<2>(rbm);
		}
		//Multiplying all normalized bonuses together, we need to account for having multiple considerations that will
		//lower our 
		float modificationFactor = 1.f - (1 / considerations.size());
		float makeupValue = (1.f - total_bonus) * modificationFactor;
		total_bonus = total_bonus + (makeupValue * total_bonus);
		m_weight = total_bonus * total_mult;
		std::cout << "Weight: " << m_weight << std::endl;
	}

	void update()
	{
		m_pElixir = m_player->getElixir();
		m_eElixir = Game::get().getPlayer(!m_player->isNorth()).getElixir();
		//Get left building's health (corresponds to 1st building in building array)
		leftBuildingHealth = (float)m_player->getOpponentBuilding(1).m_Health;
		//Get right building's health (corresponds to 2nd building in building array)
		rightBuildingHealth = (float)m_player->getOpponentBuilding(2).m_Health;
		//Get king's health (corresponds to 0th building in building array)
		kingBuildingHealth = (float)m_player->getOpponentBuilding(0).m_Health;
		
		std::vector<Entity*> opponentMobs = Game::get().getPlayer(!m_player->isNorth()).getMobs();
		int leftArcherCount = 0;
		int rightArcherCount = 0;
		int leftGiantCount = 0;
		int rightGiantCount = 0;
		int leftSwordsmanCount = 0;
		int rightSwordsmanCount = 0;
		for (Entity* mob : opponentMobs)
		{
			if (mob->getStats().getMobType() == iEntityStats::Archer)
			{
				if (mob->getPosition().x < GAME_GRID_WIDTH / 2) {
					leftArcherCount += 1;
				}
				else
				{
					rightArcherCount += 1;
				}
			}
			else if(mob->getStats().getMobType() == iEntityStats::Swordsman)
			{
				if (mob->getPosition().x < GAME_GRID_WIDTH / 2) {
					leftSwordsmanCount += 1;
				}
				else
				{
					rightSwordsmanCount += 1;
				}
			}
			else if (mob->getStats().getMobType() == iEntityStats::Giant)
			{
				if (mob->getPosition().x < GAME_GRID_WIDTH / 2) {
					leftGiantCount += 1;
				}
				else
				{
					rightGiantCount += 1;
				}
			}
		}
		numGiantsLeft = (float)leftGiantCount;
		numGiantsRight = (float)rightGiantCount;
		numSwordsmenLeft = (float)leftSwordsmanCount;
		numSwordsmenRight = (float)rightSwordsmanCount;
		numArchersLeft = (float)leftArcherCount;
		numArchersRight = (float)rightArcherCount;

		
		
	}

	//Actually do the action
	virtual void doAction() = 0;

	//Returns the rank of this action
	int getRank() { return m_rank; }

	//Returns the weight of this action
	float getWeight() { return m_weight; }

	//Adds considerations to the list of actions
	void addConsideration(ActionConsideration* consideration) { considerations.push_back(consideration); }


protected:
	//The rank of the action. The highest rank will be considered first
	int m_rank = 0;
	//The weight of this action. With a tie in rank, the score determines the best action to take
	float m_weight = 1;

	//Player's elixir
	float m_pElixir;
	//Enemy's elixir
	float m_eElixir;
	//Left opponent building health
	float leftBuildingHealth;
	//Right opponent building health
	float rightBuildingHealth;
	//King opponent building health
	float kingBuildingHealth;
	//Number of giants on the left and right side
	float numGiantsLeft;
	float numGiantsRight;
	//Number of arechers on the left and right side
	float numArchersLeft;
	float numArchersRight;
	//Number of swordsmen on the left and right side
	float numSwordsmenLeft;
	float numSwordsmenRight;
	//Vector of considerations for this action. These determine the rank and weight
	std::vector<ActionConsideration*> considerations;
	//The player we are deciding for
	iPlayer* m_player;
};

//Wait (gain elixir)
class WaitUtilityAction : public UtilityAction
{
public:
	WaitUtilityAction(iPlayer& player) : UtilityAction(player)
	{
		m_rank = (int)DEFAULT_RANK;
		m_weight = DEFAULT_WEIGHT;
		//How much elixir do we have?
		std::function<std::tuple<float,float,float>(float*)> myElixirFunc = [](float *elixir)
		{
			//Minimum amount needed for a mob
			if (*elixir < 2.5f)
			{
				return std::tuple<float, float, float>(FLT_MAX, DEFAULT_BONUS, DEFAULT_MULT);
			}
			else
			{
				//Min and maxes for our function so we can get a weight normalized/as a percent
				//We're shifting the graph of 1/x^2 over by the minimum elixir needed for a mob spawn (archer)

				
				//Normalizing input
				float maxVal = MAX_ELIXIR;
				float minVal = 2.5f;
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / pow(input, 2);

				//Normalizing output
				//Avoiding divide by zero error
				float maxResult = 1.f/pow(0.1f, 2);
				float minResult = 1.f;

				//Normalizing the result
				result = (result - minResult) / (maxResult - minResult);
				
				//Multiplier set to 1.1 because we want to encourage waiting with limited consideration
				return std::tuple<float, float, float>(DEFAULT_RANK, result, 1.1f);
			}
		};
		//Auto because it's a pair of a *float and a tuple of 3 floats... becomes a lot to type
		//This is clearly defined in ActionConsideration's constructor though
		auto myElixirConsideration = std::make_pair(&m_pElixir, myElixirFunc);
		ActionConsideration* myElixir = new ActionConsideration(myElixirConsideration);
		considerations.push_back(myElixir);
	}

	virtual void doAction() override
	{
		//We wait and replenish our elixir, so nothing happens here
		return;
	}

};

//Place the giant to move in the left lane
//Giant will be our offensive unit, will look for less action and more vulnerable spots
class PlaceGiantLeftAction : public UtilityAction
{
public:
	PlaceGiantLeftAction(iPlayer& player) : UtilityAction(player)
	{
		m_rank = (int)DEFAULT_RANK;
		m_weight = DEFAULT_WEIGHT;
		
		//Can we even spawn a giant? Check our elixir count to verify against the elixir cost.
		std::function<std::tuple<float, float, float>(float*)> myElixirFun = [](float* elixir)
		{
			if (*elixir < iEntityStats::getStats(iEntityStats::Giant).getElixirCost())
			{
				//0 multiplier will ensure the final weight is 0 if we cannot spawn one
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 0.f);
			}
			else
			{
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 1.f);
			}
		};

		auto enoughElixirConsiderationPair = std::make_pair(&m_pElixir, myElixirFun);
		ActionConsideration* enoughElixer = new ActionConsideration(enoughElixirConsiderationPair);
		considerations.push_back(enoughElixer);

		
		//Archers can kill giants. Lessen the bonus for each archer on the left side
		std::function<std::tuple<float, float, float>(float*)> enemyArcherFunc = [](float* numArchers)
		{
			/*
			//Min archers is obviously zero
			int minArchers = 0;
			//This is the maximum amount of archers a player could possible put onto the field at once
			int maxArchers = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Archer).getElixirCost();
			*/
			
			//y = 1 - 0.1x, x is number of archers on left side
			float archerPenalty = 0.1f;
			float bonus = 1 - (*numArchers * archerPenalty);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto leftArchersConsiderationPair = std::make_pair(&numArchersLeft, enemyArcherFunc);
		ActionConsideration* leftArchers = new ActionConsideration (leftArchersConsiderationPair);
		considerations.push_back(leftArchers);

		
		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> leftBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = 0;
			float multiplier = DEFAULT_MULT;
			if (*buildingHealth > 0) {
				maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::Princess).getMaxHealth();
			}
			//If the right tower is down, include a multiplier to go for the win and end the query
			else
			{
				//maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();
				multiplier = 1.5f;
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, multiplier);
			}

			//y = x^3
			//More weight towards getting giants on damaged buildings vs other units
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = 1 - pow(input, 3);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = std::min(0.5f, (bonus - minResult) / (maxResult - minResult));
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, multiplier);
		};

		auto leftBuildingConsideration = std::make_pair(&leftBuildingHealth, leftBuildingCheckup);
		ActionConsideration* leftBuildings = new ActionConsideration(leftBuildingConsideration);
		considerations.push_back(leftBuildings);

		
		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> kingBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();

			//y = 1 - x^3
			//More weight towards getting giants on damaged buildings vs other units
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = 1 - pow(input, 3);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = std::min(0.5f, (bonus - minResult) / (maxResult - minResult));
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto kingBuildingConsideration = std::make_pair(&kingBuildingHealth, kingBuildingCheckup);
		ActionConsideration* kingBuilding = new ActionConsideration(kingBuildingConsideration);
		considerations.push_back(kingBuilding);

		
		//Don't want to be killed by swordsmen either
		std::function<std::tuple<float, float, float>(float*)> enemySwordsmanFunc = [](float* numSwordsmen)
		{
			/*
			//Min swordsmen is obviously zero
			int minSwordsmen = 0;
			//This is the maximum amount of swordsmen a player could possible put onto the field at once
			int maxSwordsmen = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost();
			*/

			//y = 1 - 0.15x, x is number of swordsmen on left side
			float swordsmanPenalty = 0.15f;
			float bonus = 1 - (*numSwordsmen * swordsmanPenalty);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto leftSwordsmenConsiderationPair = std::make_pair(&numSwordsmenLeft, enemySwordsmanFunc);
		ActionConsideration* leftSwordsmen = new ActionConsideration(leftSwordsmenConsiderationPair);
		considerations.push_back(leftSwordsmen);
		
		//Can opponent counter quickly?
		//considerations.push_back(new OpponentElixirConsideration);
		std::function<std::tuple<float, float, float>(float*)> opponentCounter = [](float* elixir)
		{
			float mult = DEFAULT_MULT;
			float bonus = DEFAULT_BONUS;
			//If the opponent doesn't have enough elixir to spawn a mob, use the difference
			//between their elixir and the min amount needed to spawn a mob to add a multiplier
			if (*elixir < 2.5f)
			{
				//mult = 1 + (1/x)
				//Want to maximize multiplier when the opponent is nowhere near spawning and minimize when close


				float minVal = 0.f;
				float maxVal = 2.5f;
				//Avoid divide by zero
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / input;

				//Normalize result
				//No divide by zeroes
				float minResult = 1.f / 0.1f;
				float maxResult = 1.f;
				result = (result - minResult) / (maxResult - minResult);
				mult = 1 + result;
			}
			else
			{
				//bonus = 1/x^2
				//Enemy can make some type of move, so we won't go extreme with a multiplier
				//Normalizing input
				float maxVal = MAX_ELIXIR;
				float minVal = 2.5f;
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / pow(input, 2);

				//Normalizing output
				//Avoiding divide by zero error
				float maxResult = 1.f / pow(0.1f, 2);
				float minResult = 1.f / pow(maxVal - 2.5f, 2);

				//Normalizing the result
				result = (result - minResult) / (maxResult - minResult);
				//bonus = std::max(0.5f, result);
				bonus = result;
			}

			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, mult);
		};

		auto opponentElixirConsideration = std::make_pair(&m_eElixir, opponentCounter);
		ActionConsideration* opponentCounterConsideration = new ActionConsideration(opponentElixirConsideration);
		considerations.push_back(opponentCounterConsideration);
		//How many more mobs do they have than me?
		//considerations.push_back(new NumberEnemyMobsConsideration)
	}

	virtual void doAction() override
	{
		bool isNorth = m_player->isNorth();
		Vec2 giantPos_Game = ksGiantPosL.Player2Game(isNorth);
		m_player->placeMob(iEntityStats::Giant, giantPos_Game);
	}
};

//Place a giant to move in the right lane
class PlaceGiantRightAction : public UtilityAction
{
public:
	PlaceGiantRightAction(iPlayer& player) : UtilityAction(player)
	{
		m_rank = (int)DEFAULT_RANK;
		m_weight = DEFAULT_WEIGHT;
		
		std::function<std::tuple<float, float, float>(float*)> myElixirFun = [](float* elixir)
		{
			if (*elixir < iEntityStats::getStats(iEntityStats::Giant).getElixirCost())
			{
				//0 multiplier will ensure the final weight is 0 if we cannot spawn one
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 0.f);
			}
			else
			{
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 1.f);
			}
		};
		
		auto enoughElixirConsiderationPair = std::make_pair(&m_pElixir, myElixirFun);
		ActionConsideration* enoughElixer = new ActionConsideration(enoughElixirConsiderationPair);
		considerations.push_back(enoughElixer);

		//Archers can kill giants. Lessen the bonus for each archer on the right side
		std::function<std::tuple<float, float, float>(float*)> enemyArcherFunc = [](float* numArchers)
		{
			/*
			//Min archers is obviously zero
			int minArchers = 0;
			//This is the maximum amount of archers a player could possible put onto the field at once
			int maxArchers = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Archer).getElixirCost();
			*/

			//y = 1 - 0.1x, x is number of archers
			float archerPenalty = 0.1f;
			float bonus = 1 - (*numArchers * archerPenalty);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto rightArchersConsiderationPair = std::make_pair(&numArchersLeft, enemyArcherFunc);
		ActionConsideration* rightArchers = new ActionConsideration(rightArchersConsiderationPair);
		considerations.push_back(rightArchers);


		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> rightBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = 0;
			float multiplier = DEFAULT_MULT;
			if (*buildingHealth > 0) {
				maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::Princess).getMaxHealth();
			}
			//If the right tower is down, include a multiplier to go for the win and end the query
			else
			{
				//maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();
				multiplier = 1.5f;
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, multiplier);
			}

			//y = 1 - x^3
			//More weight towards getting giants on damaged buildings vs other units
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = 1 - pow(input, 3);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = std::min(0.5f, (bonus - minResult) / (maxResult - minResult));
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, multiplier);
		};

		auto rightBuildingConsideration = std::make_pair(&rightBuildingHealth, rightBuildingCheckup);
		ActionConsideration* rightBuildings = new ActionConsideration(rightBuildingConsideration);
		considerations.push_back(rightBuildings);

		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> kingBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();

			//y = 1 - x^3
			//More weight towards getting giants on damaged buildings vs other units
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = 1 - pow(input, 3);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = std::min(0.5f, (bonus - minResult) / (maxResult - minResult));
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto kingBuildingConsideration = std::make_pair(&kingBuildingHealth, kingBuildingCheckup);
		ActionConsideration* kingBuilding = new ActionConsideration(kingBuildingConsideration);
		considerations.push_back(kingBuilding);

		//Don't want to be killed by swordsmen either
		std::function<std::tuple<float, float, float>(float*)> enemySwordsmanFunc = [](float* numSwordsmen)
		{
			/*
			//Min swordsmen is obviously zero
			int minSwordsmen = 0;
			//This is the maximum amount of swordsmen a player could possible put onto the field at once
			int maxSwordsmen = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost();
			*/

			//y = 1 - 0.15x, x is number of swordsmen
			float swordsmanPenalty = 0.15f;
			float bonus = 1 - (*numSwordsmen * swordsmanPenalty);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};


		auto rightSwordsmenConsiderationPair = std::make_pair(&numSwordsmenRight, enemySwordsmanFunc);
		ActionConsideration* rightSwordsmen = new ActionConsideration(rightSwordsmenConsiderationPair);
		considerations.push_back(rightSwordsmen);

		//Can opponent counter quickly?
		//considerations.push_back(new OpponentElixirConsideration);
		std::function<std::tuple<float, float, float>(float*)> opponentCounter = [](float* elixir)
		{
			float mult = DEFAULT_MULT;
			float bonus = DEFAULT_BONUS;
			//If the opponent doesn't have enough elixir to spawn a mob, use the difference
			//between their elixir and the min amount needed to spawn a mob to add a multiplier
			if (*elixir < 2.5f)
			{
				//mult = 1 + (1/x)
				//Want to maximize multiplier when the opponent is nowhere near spawning and minimize when close


				float minVal = 0.f;
				float maxVal = 2.5f;
				//Avoid divide by zero
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / input;

				//Normalize result
				//No divide by zeroes
				float minResult = 1.f / 0.1f;
				float maxResult = 1.f;
				result = (result - minResult) / (maxResult - minResult);
				mult = 1 + result;
			}
			else
			{
				//bonus = 1/x^2
				//Enemy can make some type of move, so we won't go extreme with a multiplier
				//Normalizing input
				float maxVal = MAX_ELIXIR;
				float minVal = 2.5f;
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / pow(input, 2);

				//Normalizing output
				//Avoiding divide by zero error
				float maxResult = 1.f / pow(0.1f, 2);
				float minResult = 1.f / pow(maxVal - 2.5f, 2);

				//Normalizing the result
				result = (result - minResult) / (maxResult - minResult);
				//bonus = std::max(0.5f, result);
				bonus = result;
			}

			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, mult);
		};

		auto opponentElixirConsideration = std::make_pair(&m_eElixir, opponentCounter);
		ActionConsideration* opponentCounterConsideration = new ActionConsideration(opponentElixirConsideration);
		considerations.push_back(opponentCounterConsideration);
		//How many more mobs do they have than me?
		//considerations.push_back(new NumberEnemyMobsConsideration)
	}

	virtual void doAction() override
	{
		bool isNorth = m_player->isNorth();
		Vec2 giantPos_Game = ksGiantPosR.Player2Game(isNorth);
		m_player->placeMob(iEntityStats::Giant, giantPos_Game);
	}
};

//Place an archer to move in the left lane
class PlaceArcherLeftAction : public UtilityAction
{
public:
	PlaceArcherLeftAction(iPlayer& player) : UtilityAction(player)
	{
		m_rank = (int)DEFAULT_RANK;
		m_weight = DEFAULT_WEIGHT;

		std::function<std::tuple<float, float, float>(float*)> myElixirFun = [](float* elixir)
		{
			if (*elixir < iEntityStats::getStats(iEntityStats::Archer).getElixirCost())
			{
				//0 multiplier will ensure the final weight is 0 if we cannot spawn one
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 0.f);
			}
			else
			{
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 1.f);
			}
		};

		auto enoughElixirConsiderationPair = std::make_pair(&m_pElixir, myElixirFun);
		ActionConsideration* enoughElixer = new ActionConsideration(enoughElixirConsiderationPair);
		considerations.push_back(enoughElixer);

		//Archers can counter archers. Increase the bonus for each archer on the left side (encourage defense)
		std::function<std::tuple<float, float, float>(float*)> enemyArcherFunc = [](float* numArchers)
		{
			/*
			//Min archers is obviously zero
			int minArchers = 0;
			//This is the maximum amount of archers a player could possible put onto the field at once
			int maxArchers = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Archer).getElixirCost();
			*/

			//y = 0.15x + 0.5, x is number of archers
			float archerBonus = 0.15f;
			float bonus = std::min(0.5f, (*numArchers * archerBonus)) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto leftArchersConsiderationPair = std::make_pair(&numArchersLeft, enemyArcherFunc);
		ActionConsideration* leftArchers = new ActionConsideration(leftArchersConsiderationPair);
		considerations.push_back(leftArchers);


		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> leftBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = 0;
			float multiplier = DEFAULT_MULT;
			if (*buildingHealth > 0) {
				maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::Princess).getMaxHealth();
			}
			//If the right tower is down, include a multiplier to go for the win and end the query
			else
			{
				//maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();
				multiplier = 1.5f;
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, multiplier);
			}

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, multiplier);
		};


		auto leftBuildingConsideration = std::make_pair(&leftBuildingHealth, leftBuildingCheckup);
		ActionConsideration* leftBuildings = new ActionConsideration(leftBuildingConsideration);
		considerations.push_back(leftBuildings);

		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> kingBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto kingBuildingConsideration = std::make_pair(&kingBuildingHealth, kingBuildingCheckup);
		ActionConsideration* kingBuilding = new ActionConsideration(kingBuildingConsideration);
		considerations.push_back(kingBuilding);


		//Don't want to be killed by swordsmen either, so let's defend against them
		std::function<std::tuple<float, float, float>(float*)> enemySwordsmanFunc = [](float* numSwordsmen)
		{
			/*
			//Min swordsmen is obviously zero
			int minSwordsmen = 0;
			//This is the maximum amount of swordsmen a player could possible put onto the field at once
			int maxSwordsmen = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost();
			*/

			//y = 0.3x + 0.5, x is number of swordsmen on right side
			float swordsmanBonus = 0.3f;
			float bonus = std::min(0.5f, *numSwordsmen * swordsmanBonus) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto leftSwordsmenConsiderationPair = std::make_pair(&numSwordsmenLeft, enemySwordsmanFunc);
		ActionConsideration* leftSwordsmen = new ActionConsideration(leftSwordsmenConsiderationPair);
		considerations.push_back(leftSwordsmen);

		//Can opponent counter quickly?
		//considerations.push_back(new OpponentElixirConsideration);
		std::function<std::tuple<float, float, float>(float*)> opponentCounter = [](float* elixir)
		{
			float mult = DEFAULT_MULT;
			float bonus = DEFAULT_BONUS;
			//If the opponent doesn't have enough elixir to spawn a mob, use the difference
			//between their elixir and the min amount needed to spawn a mob to add a multiplier
			if (*elixir < 2.5f)
			{
				//mult = 1 + (1/x)
				//Want to maximize multiplier when the opponent is nowhere near spawning and minimize when close


				float minVal = 0.f;
				float maxVal = 2.5f;
				//Avoid divide by zero
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / input;

				//Normalize result
				//No divide by zeroes
				float minResult = 1.f / 0.1f;
				float maxResult = 1.f;
				result = (result - minResult) / (maxResult - minResult);
				mult = 1 + result;
			}
			else
			{
				//bonus = 1/x^2
				//Enemy can make some type of move, so we won't go extreme with a multiplier
				//Normalizing input
				float maxVal = MAX_ELIXIR;
				float minVal = 2.5f;
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / pow(input, 2);

				//Normalizing output
				//Avoiding divide by zero error
				float maxResult = 1.f / pow(0.1f, 2);
				float minResult = 1.f / pow(maxVal - 2.5f, 2);

				//Normalizing the result
				result = (result - minResult) / (maxResult - minResult);
				//bonus = std::max(0.5f, result);
				bonus = result;
			}

			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, mult);
		};

		auto opponentElixirConsideration = std::make_pair(&m_eElixir, opponentCounter);
		ActionConsideration* opponentCounterConsideration = new ActionConsideration(opponentElixirConsideration);
		considerations.push_back(opponentCounterConsideration);


		std::function<std::tuple<float, float, float>(float*)> enemyGiantFunc = [](float* numGiants)
		{
			/*
			//Min giants is obviously zero
			int minGiants = 0;
			//This is the maximum amount of giants a player could possible put onto the field at once
			int maxGiants = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Giant).getElixirCost();
			*/

			//y = 0.35x + 0.5, x is number of swordsmen on left side
			//We will start at minimum of 0.5f
			float giantBonus = 0.35f;
			float bonus = std::min(0.5f, (*numGiants * giantBonus)) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};
		auto leftGiantsConsiderationPair = std::make_pair(&numGiantsLeft, enemyGiantFunc);
		ActionConsideration* leftGiants = new ActionConsideration(leftGiantsConsiderationPair);
		considerations.push_back(leftGiants);
		
		
	}

	virtual void doAction() override
	{
		bool isNorth = m_player->isNorth();
		Vec2 archerPos_Game = ksArcherPosL.Player2Game(isNorth);
		m_player->placeMob(iEntityStats::Archer,archerPos_Game);
	}
};

//Place an archer to move in the right lane
class PlaceArcherRightAction : public UtilityAction
{
public:
	PlaceArcherRightAction(iPlayer& player) : UtilityAction(player)
	{
		m_rank = (int)DEFAULT_RANK;
		m_weight = DEFAULT_WEIGHT;

		std::function<std::tuple<float, float, float>(float*)> myElixirFun = [](float* elixir)
		{
			if (*elixir < iEntityStats::getStats(iEntityStats::Archer).getElixirCost())
			{
				//0 multiplier will ensure the final weight is 0 if we cannot spawn one
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 0.f);
			}
			else
			{
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 1.f);
			}
		};
		
		auto enoughElixirConsiderationPair = std::make_pair(&m_pElixir, myElixirFun);
		ActionConsideration* enoughElixer = new ActionConsideration(enoughElixirConsiderationPair);
		considerations.push_back(enoughElixer);

		//Archers can counter archers. Increase the bonus for each archer on the left side (encourage defense)
		std::function<std::tuple<float, float, float>(float*)> enemyArcherFunc = [](float* numArchers)
		{
			/*
			//Min archers is obviously zero
			int minArchers = 0;
			//This is the maximum amount of archers a player could possible put onto the field at once
			int maxArchers = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Archer).getElixirCost();
			*/

			//y = 0.2x, x is number of archers on left side
			float archerBonus = 0.2f;
			float bonus = std::min(0.5f, (*numArchers * archerBonus)) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto rightArchersConsiderationPair = std::make_pair(&numArchersRight, enemyArcherFunc);
		ActionConsideration* rightArchers = new ActionConsideration(rightArchersConsiderationPair);
		considerations.push_back(rightArchers);


		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> rightBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = 0;
			float multiplier = DEFAULT_MULT;
			if (*buildingHealth > 0) {
				maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::Princess).getMaxHealth();
			}
			//If the right tower is down, include a multiplier to go for the win and end the query
			else
			{
				//maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();
				multiplier = 1.5f;
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, multiplier);
			}

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, multiplier);
		};
		
		auto rightBuildingConsideration = std::make_pair(&rightBuildingHealth, rightBuildingCheckup);
		ActionConsideration* rightBuilding = new ActionConsideration(rightBuildingConsideration);
		considerations.push_back(rightBuilding);

		
		//King check-in
		std::function<std::tuple<float, float, float>(float*)> kingBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto kingBuildingConsideration = std::make_pair(&kingBuildingHealth, kingBuildingCheckup);
		ActionConsideration* kingBuilding = new ActionConsideration(kingBuildingConsideration);
		considerations.push_back(kingBuilding);

		

		//Don't want to be killed by swordsmen either, so let's defend against them
		std::function<std::tuple<float, float, float>(float*)> enemySwordsmanFunc = [](float* numSwordsmen)
		{
			/*
			//Min swordsmen is obviously zero
			int minSwordsmen = 0;
			//This is the maximum amount of swordsmen a player could possible put onto the field at once
			int maxSwordsmen = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost();
			*/

			//y = 0.3x + 0.5, x is number of swordsmen on right side
			float swordsmanBonus = 0.3f;
			float bonus = std::min(0.5f, *numSwordsmen * swordsmanBonus) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto rightSwordsmenConsiderationPair = std::make_pair(&numSwordsmenRight, enemySwordsmanFunc);
		ActionConsideration* rightSwordsmen = new ActionConsideration(rightSwordsmenConsiderationPair);
		considerations.push_back(rightSwordsmen);

		//Can opponent counter quickly?
		std::function<std::tuple<float, float, float>(float*)> opponentCounter = [](float* elixir)
		{
			float mult = DEFAULT_MULT;
			float bonus = DEFAULT_BONUS;
			//If the opponent doesn't have enough elixir to spawn a mob, use the difference
			//between their elixir and the min amount needed to spawn a mob to add a multiplier
			if (*elixir < 2.5f)
			{
				//mult = 1 + (1/x)
				//Want to maximize multiplier when the opponent is nowhere near spawning and minimize when close


				float minVal = 0.f;
				float maxVal = 2.5f;
				//Avoid divide by zero
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / input;

				//Normalize result
				//No divide by zeroes
				float minResult = 1.f / 0.1f;
				float maxResult = 1.f;
				result = (result - minResult) / (maxResult - minResult);
				mult = 1 + result;
			}
			else
			{
				//bonus = 1/x^2
				//Enemy can make some type of move, so we won't go extreme with a multiplier
				//Normalizing input
				float maxVal = MAX_ELIXIR;
				float minVal = 2.5f;
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / pow(input, 2);

				//Normalizing output
				//Avoiding divide by zero error
				float maxResult = 1.f / pow(0.1f, 2);
				float minResult = 1.f / pow(maxVal - 2.5f, 2);

				//Normalizing the result
				result = (result - minResult) / (maxResult - minResult);
				//bonus = std::max(0.5f, result);
				bonus = result;
			}

			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, mult);
		};

		auto opponentElixirConsideration = std::make_pair(&m_eElixir, opponentCounter);
		ActionConsideration* opponentCounterConsideration = new ActionConsideration(opponentElixirConsideration);
		considerations.push_back(opponentCounterConsideration);

		//Attack giants

		std::function<std::tuple<float, float, float>(float*)> enemyGiantFunc = [](float* numGiants)
		{
			/*
			//Min giants is obviously zero
			int minGiants = 0;
			//This is the maximum amount of giants a player could possible put onto the field at once
			int maxGiants = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Giant).getElixirCost();
			*/

			//y = 0.35x + 0.5, x is number of swordsmen on left side
			//We will start at minimum of 0.5f
			float giantBonus = 0.35f;
			float bonus = std::min(0.5f, (*numGiants * giantBonus)) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};
		auto rightGiantsConsiderationPair = std::make_pair(&numGiantsRight, enemyGiantFunc);
		ActionConsideration* rightGiants = new ActionConsideration(rightGiantsConsiderationPair);
		considerations.push_back(rightGiants);
	}
	
	virtual void doAction() override
	{
		bool isNorth = m_player->isNorth();
		Vec2 archerPos_Game = ksArcherPosR.Player2Game(isNorth);
		m_player->placeMob(iEntityStats::Archer, archerPos_Game);
	}
};

//Place a swordsmen to move in the left lane
class PlaceSwordsmanLeftAction : public UtilityAction
{
public:
	PlaceSwordsmanLeftAction(iPlayer& player) : UtilityAction(player)
	{
		m_rank = (int)DEFAULT_RANK;
		m_weight = DEFAULT_WEIGHT;

		std::function<std::tuple<float, float, float>(float*)> myElixirFun = [](float* elixir)
		{
			if (*elixir < iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost())
			{
				//0 multiplier will ensure the final weight is 0 if we cannot spawn one
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 0.f);
			}
			else
			{
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 1.f);
			}
		};

		auto enoughElixirConsiderationPair = std::make_pair(&m_pElixir, myElixirFun);
		ActionConsideration* enoughElixer = new ActionConsideration(enoughElixirConsiderationPair);
		considerations.push_back(enoughElixer);

		//Assault the archers with swordsmen
		std::function<std::tuple<float, float, float>(float*)> enemyArcherFunc = [](float* numArchers)
		{
			/*
			//Min archers is obviously zero
			int minArchers = 0;
			//This is the maximum amount of archers a player could possible put onto the field at once
			int maxArchers = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Archer).getElixirCost();
			*/

			//y = 0.15x + 0.5, x is number of archers
			float archerBonus = 0.15f;
			float bonus = std::min(0.5f, (*numArchers * archerBonus)) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto leftArchersConsiderationPair = std::make_pair(&numArchersLeft, enemyArcherFunc);
		ActionConsideration* leftArchers = new ActionConsideration(leftArchersConsiderationPair);
		considerations.push_back(leftArchers);


		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> leftBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = 0;
			float multiplier = DEFAULT_MULT;
			if (*buildingHealth > 0) {
				maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::Princess).getMaxHealth();
			}
			//If the right tower is down, include a multiplier to go for the win and end the query
			else
			{
				//maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();
				multiplier = 1.5f;
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, multiplier);
			}

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, multiplier);
		};

		auto leftBuildingConsideration = std::make_pair(&leftBuildingHealth, leftBuildingCheckup);
		ActionConsideration* leftBuildings = new ActionConsideration(leftBuildingConsideration);
		considerations.push_back(leftBuildings);

		//Check in with the king
		std::function<std::tuple<float, float, float>(float*)> kingBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto kingBuildingConsideration = std::make_pair(&kingBuildingHealth, kingBuildingCheckup);
		ActionConsideration* kingBuilding = new ActionConsideration(kingBuildingConsideration);
		considerations.push_back(kingBuilding);


		//Don't want to be killed by swordsmen either, so let's defend against them
		std::function<std::tuple<float, float, float>(float*)> enemySwordsmanFunc = [](float* numSwordsmen)
		{
			/*
			//Min swordsmen is obviously zero
			int minSwordsmen = 0;
			//This is the maximum amount of swordsmen a player could possible put onto the field at once
			int maxSwordsmen = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost();
			*/

			//y = 0.3x + 0.5, x is number of swordsmen on right side
			float swordsmanBonus = 0.3f;
			float bonus = std::min(0.5f, *numSwordsmen * swordsmanBonus) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto leftSwordsmenConsiderationPair = std::make_pair(&numSwordsmenLeft, enemySwordsmanFunc);
		ActionConsideration* leftSwordsmen = new ActionConsideration(leftSwordsmenConsiderationPair);
		considerations.push_back(leftSwordsmen);

		//Can opponent counter quickly?
		//considerations.push_back(new OpponentElixirConsideration);
		std::function<std::tuple<float, float, float>(float*)> opponentCounter = [](float* elixir)
		{
			float mult = DEFAULT_MULT;
			float bonus = DEFAULT_BONUS;
			//If the opponent doesn't have enough elixir to spawn a mob, use the difference
			//between their elixir and the min amount needed to spawn a mob to add a multiplier
			if (*elixir < 2.5f)
			{
				//mult = 1 + (1/x)
				//Want to maximize multiplier when the opponent is nowhere near spawning and minimize when close


				float minVal = 0.f;
				float maxVal = 2.5f;
				//Avoid divide by zero
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / input;

				//Normalize result
				//No divide by zeroes
				float minResult = 1.f / 0.1f;
				float maxResult = 1.f;
				result = (result - minResult) / (maxResult - minResult);
				mult = 1 + result;
			}
			else
			{
				//bonus = 1/x^2
				//Enemy can make some type of move, so we won't go extreme with a multiplier
				//Normalizing input
				float maxVal = MAX_ELIXIR;
				float minVal = 2.5f;
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / pow(input, 2);

				//Normalizing output
				//Avoiding divide by zero error
				float maxResult = 1.f / pow(0.1f, 2);
				float minResult = 1.f / pow(maxVal - 2.5f, 2);

				//Normalizing the result
				result = (result - minResult) / (maxResult - minResult);
				//bonus = std::max(0.5f, result);
				bonus = result;
			}

			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, mult);
		};

		auto opponentElixirConsideration = std::make_pair(&m_eElixir, opponentCounter);
		ActionConsideration* opponentCounterConsideration = new ActionConsideration(opponentElixirConsideration);
		considerations.push_back(opponentCounterConsideration);

		//Attack giants
		std::function<std::tuple<float, float, float>(float*)> enemyGiantFunc = [](float* numGiants)
		{
			/*
			//Min giants is obviously zero
			int minGiants = 0;
			//This is the maximum amount of giants a player could possible put onto the field at once
			int maxGiants = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Giant).getElixirCost();
			*/

			//y = 0.35x + 0.5, x is number of swordsmen on left side
			//We will start at minimum of 0.5f
			float giantBonus = 0.35f;
			float bonus = std::min(0.5f, (*numGiants * giantBonus)) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};
		auto leftGiantsConsiderationPair = std::make_pair(&numGiantsLeft, enemyGiantFunc);
		ActionConsideration* leftGiants = new ActionConsideration(leftGiantsConsiderationPair);
		considerations.push_back(leftGiants);
	}

	virtual void doAction() override
	{
		bool isNorth = m_player->isNorth();
		Vec2 swordsmanPos_Game = ksSwordsmanPosL.Player2Game(isNorth);
		m_player->placeMob(iEntityStats::Swordsman, swordsmanPos_Game);
	}
};

//Place a swordsmen to move in the right lane
class PlaceSwordsmanRightAction : public UtilityAction
{
public:
	PlaceSwordsmanRightAction(iPlayer& player) : UtilityAction(player)
	{
		m_rank = (int)DEFAULT_RANK;
		m_weight = DEFAULT_WEIGHT;

		std::function<std::tuple<float, float, float>(float*)> myElixirFun = [](float* elixir)
		{
			if (*elixir < iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost())
			{
				//0 multiplier will ensure the final weight is 0 if we cannot spawn one
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 0.f);
			}
			else
			{
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, 1.f);
			}
		};

		auto enoughElixirConsiderationPair = std::make_pair(&m_pElixir, myElixirFun);
		ActionConsideration* enoughElixer = new ActionConsideration(enoughElixirConsiderationPair);
		considerations.push_back(enoughElixer);

		//Assult the archers with swordsmen.
		std::function<std::tuple<float, float, float>(float*)> enemyArcherFunc = [](float* numArchers)
		{
			/*
			//Min archers is obviously zero
			int minArchers = 0;
			//This is the maximum amount of archers a player could possible put onto the field at once
			int maxArchers = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Archer).getElixirCost();
			*/

			//y = 0.15x + 0.5, x is number of archers
			float archerBonus = 0.15f;
			float bonus = std::min(0.5f, (*numArchers * archerBonus)) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};
		
		auto rightArchersConsiderationPair = std::make_pair(&numArchersRight, enemyArcherFunc);
		ActionConsideration* rightArchers = new ActionConsideration(rightArchersConsiderationPair);
		considerations.push_back(rightArchers);


		//Weigh more towards building with more damage taken
		std::function<std::tuple<float, float, float>(float*)> rightBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = 0;
			float multiplier = DEFAULT_MULT;
			if (*buildingHealth > 0) {
				maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::Princess).getMaxHealth();
			}
			//If the right tower is down, include a multiplier to go for the win and end the query
			else
			{
				//maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();
				multiplier = 1.5f;
				return std::tuple<float, float, float>(DEFAULT_RANK, DEFAULT_BONUS, multiplier);
			}

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, multiplier);
		};

		auto rightBuildingConsideration = std::make_pair(&rightBuildingHealth, rightBuildingCheckup);
		ActionConsideration* rightBuilding = new ActionConsideration(rightBuildingConsideration);
		considerations.push_back(rightBuilding);

		//Check in with the king
		std::function<std::tuple<float, float, float>(float*)> kingBuildingCheckup = [](float* buildingHealth)
		{
			int maxBuildingHealth = iEntityStats::getBuildingStats(iEntityStats::King).getMaxHealth();

			//y = x^2
			//Normalized input
			float minVal = 0.f;
			float maxVal = (float)maxBuildingHealth;
			float input = (*buildingHealth - minVal) / (maxVal - minVal);
			float bonus = pow(input, 2);

			//Normalized output
			//min and max in normalized form are 0^2 and 1^2, respectively
			//Writing it out since thinking of normalization makes my head hurt
			float minResult = 0.f;
			float maxResult = 1.f;
			bonus = (bonus - minResult) / (maxResult - minResult);
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto kingBuildingConsideration = std::make_pair(&kingBuildingHealth, kingBuildingCheckup);
		ActionConsideration* kingBuilding = new ActionConsideration(kingBuildingConsideration);
		considerations.push_back(kingBuilding);



		//Don't want to be killed by swordsmen either, so let's defend against them
		std::function<std::tuple<float, float, float>(float*)> enemySwordsmanFunc = [](float* numSwordsmen)
		{
			/*
			//Min swordsmen is obviously zero
			int minSwordsmen = 0;
			//This is the maximum amount of swordsmen a player could possible put onto the field at once
			int maxSwordsmen = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Swordsman).getElixirCost();
			*/

			//y = 0.3x + 0.5, x is number of swordsmen on right side
			float swordsmanBonus = 0.3f;
			float bonus = std::min(0.5f, *numSwordsmen * swordsmanBonus) + 0.5f;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};

		auto rightSwordsmenConsiderationPair = std::make_pair(&numSwordsmenRight, enemySwordsmanFunc);
		ActionConsideration* rightSwordsmen = new ActionConsideration(rightSwordsmenConsiderationPair);
		considerations.push_back(rightSwordsmen);

		//Can opponent counter quickly?
		std::function<std::tuple<float, float, float>(float*)> opponentCounter = [](float* elixir)
		{
			float mult = DEFAULT_MULT;
			float bonus = DEFAULT_BONUS;
			//If the opponent doesn't have enough elixir to spawn a mob, use the difference
			//between their elixir and the min amount needed to spawn a mob to add a multiplier
			if (*elixir < 2.5f)
			{
				//mult = 1 + (1/x)
				//Want to maximize multiplier when the opponent is nowhere near spawning and minimize when close


				float minVal = 0.f;
				float maxVal = 2.5f;
				//Avoid divide by zero
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / input;

				//Normalize result
				//No divide by zeroes
				float minResult = 1.f / 0.1f;
				float maxResult = 1.f;
				result = (result - minResult) / (maxResult - minResult);
				mult = 1 + result;
			}
			else
			{
				//bonus = 1/x^2
				//Enemy can make some type of move, so we won't go extreme with a multiplier
				//Normalizing input
				float maxVal = MAX_ELIXIR;
				float minVal = 2.5f;
				float input = std::max((*elixir - minVal) / (maxVal - minVal), 0.1f);
				float result = 1.f / pow(input, 2);

				//Normalizing output
				//Avoiding divide by zero error
				float maxResult = 1.f / pow(0.1f, 2);
				float minResult = 1.f / pow(maxVal - 2.5f, 2);

				//Normalizing the result
				result = (result - minResult) / (maxResult - minResult);
				//bonus = std::max(0.5f, result);
				bonus = result;
			}

			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, mult);
		};

		auto opponentElixirConsideration = std::make_pair(&m_eElixir, opponentCounter);
		ActionConsideration* opponentCounterConsideration = new ActionConsideration(opponentElixirConsideration);
		considerations.push_back(opponentCounterConsideration);

		//Attack giants
		std::function<std::tuple<float, float, float>(float*)> enemyGiantFunc = [](float* numGiants)
		{
			/*
			//Min giants is obviously zero
			int minGiants = 0;
			//This is the maximum amount of giants a player could possible put onto the field at once
			int maxGiants = MAX_ELIXIR / iEntityStats::getStats(iEntityStats::Giant).getElixirCost();
			*/

			//y = 0.35x + 0.5, x is number of swordsmen on left side
			//We will start at minimum of 0.5f
			float giantBonus = 0.35f;
			float bonus = std::min(0.5f, (*numGiants * giantBonus)) + 0.5f;
			std::cout << "Giant bonus: " << bonus << std::endl;
			return std::tuple<float, float, float>(DEFAULT_RANK, bonus, DEFAULT_MULT);
		};
		auto rightGiantsConsiderationPair = std::make_pair(&numGiantsRight, enemyGiantFunc);
		ActionConsideration* rightGiants = new ActionConsideration(rightGiantsConsiderationPair);
		considerations.push_back(rightGiants);
	}

	virtual void doAction() override
	{
		bool isNorth = m_player->isNorth();
		Vec2 swordsmanPos_Game = ksSwordsmanPosR.Player2Game(isNorth);
		m_player->placeMob(iEntityStats::Swordsman, swordsmanPos_Game);
	}
};

