#pragma once

#include "Components.h"
#include <memory>
#include <string>

class Entity
{
	friend class EntityManager;

	bool m_isAlive = true;
	size_t m_id = 0;
	std::string m_type = "none";

	Entity(const size_t id, const std::string& type);

public:

	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CLifespan> cLifespan;
	std::shared_ptr<CSpecialWeapon> cSpecialWeapon;
	std::shared_ptr<CFlamming> cFlamming;
	std::shared_ptr<CGrowable> cGrowable;

	bool isAlive() const;
	const std::string& type() const;
	const size_t id() const;
	void destroy();
	void print();
};