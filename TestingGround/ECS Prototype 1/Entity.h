#pragma once
#ifndef ENTITY_H_
#define ENTITY_H_

class Entity
{
public:

	Entity(int _ID);

	int entityID = 0;

	template<typename T>
	void AddComponent()
	{
		T::AddToEntity(entityID); 
	}

	template<typename T, typename U>
	T* GetComponent()
	{
		T* temp = nullptr;
		U::GetComponent(&temp, entityID);
		return temp;
	}
};

#endif

