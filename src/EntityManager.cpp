#include "EntityManager.h"
#include <iostream>

//create and update, print to screen
EntityManager::EntityManager()
{
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& type)
{
	return m_entityMap[type];
}

void EntityManager::update()
{
	for (auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->type()].push_back(e); // already creates record if not exists
	}
	m_entitiesToAdd.clear();


	int length = m_entities.size();
	for (int i = length - 1; i >= 0; i--)
	{
		auto entity = m_entities[i];
		if (m_entities[i]->isAlive())
		{
			continue;
		}
		for (int j = m_entityMap[entity->type()].size() - 1; j >= 0; j--)
		{
			if (m_entityMap[entity->type()][j]->id() == entity->id())
			{
				m_entityMap[entity->type()].erase(m_entityMap[entity->type()].begin() + j);
				break;
			}
		}
		m_entities.erase(m_entities.begin() + i);
		//std::cout << "Entity dying: " << entity->id() << " Unique? " << (entity.unique() ? "true": "false") << std::endl;
	}

}
