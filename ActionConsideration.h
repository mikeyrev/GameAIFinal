#pragma once
#include <tuple>
#include <functional>
#include "../Game/src/Game.h"
//#include "UtilityAction.h"


class ActionConsideration
{
public:
	//Pass in a function with an initial value
	ActionConsideration(std::pair<float*, std::function<std::tuple<float,float,float>(float*)>> c) { this->consideration = c; }

	//Get the rank, weight, and multiplier of this consideration
	std::tuple<float, float, float> getScore();
	
private:
	//A pair with the value we want to check and the function to use on it
	//Function returns a tuple with the rank, weight, and multiplier (in that order)
	std::pair<float*, std::function<std::tuple<float,float,float>(float*)>> consideration;
};

/*
//Want to know how offensive opponent currently is
class NumberEnemyMobsConsideration : public ActionConsideration
{
public:
	NumberEnemyMobsConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}
	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		//Defaults
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};

		//Get difference in number of mobs between player and opponent
		//float rank = 
	}
};

//Helpful for countering giants
class NumberOfGiantsConsideration : public ActionConsideration
{
public:
	NumberOfGiantsConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}

	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};
	}
};

//Helpful for countering archers
class NumberOfArchersConsideration : public ActionConsideration
{
public:
	NumberOfArchersConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}

	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};

		//return 0.f;
	}
};

//Helpful for countering swordsmen
class NumberOfSwordsmenConsideration : public ActionConsideration
{
public:
	NumberOfSwordsmenConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}

	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};
		//return 0.f;
	}
};

//How healthy are the enemy towers?
class OpponentTowersConsideration : public ActionConsideration
{
public:
	OpponentTowersConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}
	
	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};

		//return 0.f;
	}
};

//How healthy are my towers?
class HomeTowersConsideration : public ActionConsideration
{
public:
	HomeTowersConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}

	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};

		//return 0.f;
	}
};

//How much elixir does the enemy have? 
class OpponentElixirConsideration : public ActionConsideration
{
public:
	OpponentElixirConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}
	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};

		//return 0.f;
	}
};

//How much elixir do I have?
class HomeElixirConsideration : public ActionConsideration
{
public:
	HomeElixirConsideration(std::pair<float, std::function<float(float)>> c) : ActionConsideration(c) {}

	virtual std::tuple<float, float, float> getScore(iPlayer* player) override
	{
		float multiplier = DEFAULT_MULT;
		float rank = DEFAULT_RANK;
		float weight = DEFAULT_WEIGHT;
		return std::tuple<float, float, float>{0, 0, 0};

		//return 0.f;
	}
};
*/

