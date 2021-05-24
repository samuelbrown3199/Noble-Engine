#pragma once
#ifndef ENTITY_H_
#define ENTITY_H_

namespace NobleCore
{
	/**
	*Entities act as containers of component data.
	*/
	struct Entity
	{
		/**
		*The unique ID for the entity.
		*/
		unsigned int entityID;

		/**
		*Constructor for the Entity.
		*/
		Entity(unsigned int _ID);

		/**
		*Adds a component of the type to the Entity.
		*/
		template<typename T>
		void AddComponent()
		{
			T::AddToEntity(entityID);
		}

		/**
		*Gets a component of the type from the Entity.
		*/
		template<typename T>
		T* GetComponent()
		{
			T* temp = T::GetComponent(entityID);
			return temp;
		}
	};
}

#endif