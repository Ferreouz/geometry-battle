#include "Entity.h"
#include <iostream>

Entity::Entity(const size_t id, const std::string& type):
	m_id(id), m_type(type)
{
};

bool Entity::isAlive() const
{ 
	return m_isAlive;
};
const std::string& Entity::type() const 
{
	return m_type;
};
const size_t Entity::id() const 
{
	return m_id;
};
void Entity::destroy() 
{
	m_isAlive = false;
};

void Entity::print()
{
	std::cout << "*Entity printing*, type: " << m_type << ", id: " << m_id << ", active? " << (m_isAlive == true ? "true" : "false") << std::endl;
};
