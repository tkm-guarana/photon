
#ifndef _RANDOM_H_
#define _RANDOM_H_


class Random
{
private:
	std::mt19937 random;
	Random() = default;

public:
	static Random& GetInstance();
	static float Value();
	static float Range(float min, float max);
};


#endif /* _RANDOM_H_ */
