#pragma once
#ifndef BEHAVIOUR_HPP
#define BEHAVIOUR_HPP

#include <string>

struct Behaviour
{
	std::string m_sEntityID;

	virtual void Start() {};
	virtual void Update() {};
};

#endif