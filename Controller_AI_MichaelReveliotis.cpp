// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Controller_AI_MichaelReveliotis.h"
#include "Vec2.h"
#include <ctime>


void Controller_AI_MichaelReveliotis::init()
{
    srand((int)time(NULL));
    actions.push_back(new WaitUtilityAction(*(this->m_pPlayer)));
    actions.push_back(new PlaceGiantLeftAction (* (this->m_pPlayer)));
    actions.push_back(new PlaceGiantRightAction(*(this->m_pPlayer)));
    actions.push_back(new PlaceArcherLeftAction(*(this->m_pPlayer)));
    actions.push_back(new PlaceArcherRightAction(*(this->m_pPlayer)));
    actions.push_back(new PlaceSwordsmanLeftAction(*(this->m_pPlayer)));
    actions.push_back(new PlaceSwordsmanRightAction(*(this->m_pPlayer)));

}


void Controller_AI_MichaelReveliotis::tick(float deltaTSec)
{
    assert(m_pPlayer);
    if (m_bNotInit) {
	  init();
      m_bNotInit = false;
	}
    /*
    // wait for elixir
    if (m_pPlayer->getElixir() >= 9)
    {
        // convert the positions from player space to game space
        bool isNorth = m_pPlayer->isNorth();
        Vec2 giantPos_Game = ksGiantPos.Player2Game(isNorth);
        Vec2 archerPos_Game = ksArcherPos.Player2Game(isNorth);

        // Create two archers and a giant
        m_pPlayer->placeMob(iEntityStats::Giant, giantPos_Game);
        m_pPlayer->placeMob(iEntityStats::Archer, archerPos_Game);
        m_pPlayer->placeMob(iEntityStats::Archer, archerPos_Game);
    }
	*/
    int maxRank = (int)DEFAULT_RANK - 1;
    //TODO: Calculate all UtilityAction ranks and weights
	for(UtilityAction* action : actions)
	{
        action->tick();
        //TODO: Get highest rank and eliminate weight <= 0
        int newRank = action->getRank();
        float newWeight = action->getWeight();
		if(maxRank < action->getRank() && newWeight > 0)
		{
            maxRank = newRank;
		}
	}
    std::vector<UtilityAction*> availableActions;
    float threshold = 0.1f;
    //TODO: eliminate options that do not have highest rank
    //TODO: eliminate very small weight
    for(UtilityAction* action : actions)
    {
	    if(action->getRank() == maxRank && action->getWeight() > threshold)
	    {
            availableActions.push_back(action);
	    }
    }

    //TODO: weighted random
    float totalWeight = 0.f;
    std::vector<std::pair<UtilityAction*, float>> weighted_probability;
	for(UtilityAction* action : availableActions)
	{
        weighted_probability.push_back(std::make_pair(action, action->getWeight()));
        totalWeight += action->getWeight();
	}
	for(size_t i = 0; i < weighted_probability.size(); ++i)
	{
        weighted_probability[i].second /= totalWeight;
	}

	//Get our random number for our decision making
    float randomNumber = (float) rand() / (float) RAND_MAX;
	
	//Sort by increasing weighted probabilities
    std::sort(weighted_probability.begin(), weighted_probability.end(),
        [](std::pair<UtilityAction*, float> a, std::pair<UtilityAction*, float> b) {return a.second < b.second;});
    float prob = 0.f;
	for(size_t i = 0; i < weighted_probability.size(); ++i)
	{
		//If we're at the end of the list, 
		if(i + 1 == weighted_probability.size())
		{
            weighted_probability[i].first->doAction();
            break;
		}
        else
        {
        	//Check each bucket of weighted probability to see if random number falls in
	        if(randomNumber >= prob && randomNumber < prob + weighted_probability[i].second)
	        {
	        	//If so, do the action corresponding to this weighted probability
                weighted_probability[i].first->doAction();
                break;
	        }
            else
            {
            	//If not, move onto next bucket
                prob += weighted_probability[i].second;
            }
        }
	}
}

