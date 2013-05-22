#ifndef _FACTORY_H_
#define _FACTORY_H_

/*
	Factory class is used as an interface for creating templated objects
*/

template <class T>
class Factory
{
public:
	Factory(void){}
	~Factory(void){}
	virtual T* create(float dim = 10.0f) = 0;
};

#endif