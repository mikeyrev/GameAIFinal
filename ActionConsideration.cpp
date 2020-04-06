#include "ActionConsideration.h"

std::tuple<float, float, float> ActionConsideration::getScore()
{
	//Applies the function to the given value being observed and returns 
	return std::tuple<float,float,float>(consideration.second(consideration.first));
}

