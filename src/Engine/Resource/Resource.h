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
public:

	std::string m_sLocalPath;
	/**
	*Stores the path of the resource.
	*/
	std::string m_sResourcePath;

	/**
	*Loads the resource.
	*/
	virtual void OnLoad() = 0;

	/**
	*Saves the resource.
	*/
	virtual void OnSave() {};
};
#endif