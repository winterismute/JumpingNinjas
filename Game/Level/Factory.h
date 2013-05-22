#pragma once
template <class T>
class Factory
{
public:
	Factory(void){}
	~Factory(void){}
	virtual T* create() = 0;
};

