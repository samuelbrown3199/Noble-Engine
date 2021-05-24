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

	template<typename T>
	T* GetComponent()
	{
		T* temp = T::GetComponent(entityID);
		return temp;
	}
};

#endif