
#include <random>
#include "Random.h"


Random& Random::GetInstance()
{
	static Random instance;
	return instance;
}

float Random::Value()
{
	static std::uniform_real_distribution<> urd(0, 1);	
	Random &random = Random::GetInstance();
	return urd(random.random);
}

float Random::Range(float min, float max)
{
	std::uniform_real_distribution<> urd(min, max);	
	Random &random = Random::GetInstance();
	return urd(random.random);
}
