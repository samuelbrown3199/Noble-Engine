#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <exception>

/**
*A struct that other types of resources inherit from, e.g textures and models.
*/
struct Resource
{
	/**
	*Stores the path of the resource. This is used to checkForPhysicsBody if the resource is already loaded.
	*/
	std::string m_sResourcePath;

	/**
	*Loads the resource.
	*/
	virtual void OnLoad() = 0;
};
#endif