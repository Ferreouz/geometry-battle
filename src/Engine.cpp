#include "Engine.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "Vec2.h"
#define CIRCLE_ROTATION 0.6f
#define PI 3.14f


Engine::Engine(const std::string& config)
{
	init(config);
}

void Engine::init(const std::string& path)
{
	srand(time(0));
	std::ifstream fin(path);
	std::string word;

	while (fin >> word)
	{
		if (word == "Window")
		{
			int w, h, frameRate, fullscreen;
			fin >> w >> h >> frameRate >> fullscreen;
			auto style = (fullscreen == 1 ? sf::Style::Fullscreen : sf::Style::Default);
			m_window.create(sf::VideoMode(w, h), "Assignment 2", style);
			m_window.setFramerateLimit(frameRate);
			m_frameRateLimit = frameRate;
			continue;
		}
		if (word == "Font")
		{
			std::string path;
			int size, r, g, b;
			fin >> path >> size >> r >> g >> b;
			//m_font = sf::Font();
			if (!m_font.loadFromFile(path))
			{
				std::cout << "Font: " << path << " Could not be loaded!" << std::endl;
				exit(-1);
			}
			m_text.setFillColor(sf::Color(r, g, b));
			m_text.setFont(m_font);
			m_text.setCharacterSize(size);
			continue;
		}
		if (word == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR
				>> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG 
				>> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
			continue;
		}
		if (word == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
				>> m_enemyConfig.OR >> m_enemyConfig.OG 
				>> m_enemyConfig.OB 
				>> m_enemyConfig.OT 
				>> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SP;
			continue;
		}
		if (word == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR
				>> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG
				>> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
			continue;
		}
	}
	spawnPlayer();
};

void Engine::run()
{
	while (m_running)
	{
		m_manager.update();

		if (!m_player)
		{
			spawnPlayer();
		}

		if (!m_paused)
		{
			sLifespan();
			sEnemySpawner();
			sMovement();
			sCollision();
			sSpecialWeaponCooldown();
		}
		
		sUserInput();
		sRender();

		m_currentFrame++;
	}
};

void Engine::setPaused()
{
	m_paused = !m_paused;
}

void Engine::checkScreenCollision(std::shared_ptr<Entity> entity) const
{
	auto radius = entity->cCollision->radius;
	if ((entity->cTransform->pos.x - radius) <= 0)
	{
		entity->cTransform->velocity.x = entity->cTransform->velocity.x * -1;
		entity->cTransform->pos.x = radius + 50;
	}
	if ((entity->cTransform->pos.y - radius) <= 0)
	{
		entity->cTransform->velocity.y = entity->cTransform->velocity.y * -1;
		entity->cTransform->pos.y = radius + 40;
	}
	if ((entity->cTransform->pos.x + radius) >= m_window.getSize().x)
	{
		entity->cTransform->velocity.x = entity->cTransform->velocity.x * -1;
		entity->cTransform->pos.x = m_window.getSize().x - (radius + 50);
	}
	if ((entity->cTransform->pos.y + radius) >= m_window.getSize().y)
	{
		entity->cTransform->velocity.y = entity->cTransform->velocity.y * -1;
		entity->cTransform->pos.y = m_window.getSize().y - (radius + 40);
	}
}

void Engine::spawnPlayer()
{
	m_player = m_manager.addEntity("Player");

	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	Vec2 vel = Vec2(0.0f, 0.0f);
	m_player->cTransform = std::make_shared<CTransform>(Vec2(mx, my), vel, CIRCLE_ROTATION + 0.1f);

	m_player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
	m_player->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), 
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

	m_player->cInput = std::make_shared<CInput>();
	m_player->cSpecialWeapon = std::make_shared<CSpecialWeapon>(2400);
}

void Engine::spawnBullet(std::shared_ptr<Entity> creator, const Vec2& mousePos)
{
	auto entity = m_manager.addEntity("Bullet");

	Vec2 distanceVec = creator->cTransform->pos.dist(mousePos);
	Vec2 n = distanceVec.normalize();
	Vec2 vel = Vec2(n.x * m_bulletConfig.S, n.y * m_bulletConfig.S);

	entity->cTransform = std::make_shared<CTransform>(Vec2(creator->cTransform->pos.x, creator->cTransform->pos.y), vel, CIRCLE_ROTATION);
	entity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	entity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
	entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Engine::spawnEnemy()
{
	auto entity = m_manager.addEntity("Enemy");

	float mx = (float) genRandomInt(0 + m_enemyConfig.SR, m_window.getSize().x - m_enemyConfig.SR);
	float my = (float) genRandomInt(0 + m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR);

	Vec2 vel = Vec2(genRandomInt(m_enemyConfig.SMIN, m_enemyConfig.SMAX) * cos(45), genRandomInt(m_enemyConfig.SMIN, m_enemyConfig.SMAX) * sin(45));
	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
	entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), vel, CIRCLE_ROTATION);

	entity->cTransform->velocity = 	vel;  

	float vertices = genRandomInt(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices,
		sf::Color(genRandomInt(0 + vertices, 255 - vertices), genRandomInt(0 + vertices, 255 - vertices), genRandomInt(0 + vertices, 255 - vertices)),
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

	m_lastEnemySpawnTime = m_currentFrame;
}

void Engine::spawnSpecialWeapon(std::shared_ptr<Entity> creator) 
{
	if(!creator->cSpecialWeapon || creator->cSpecialWeapon->remainingCooldown > 0) 
	{
		return;
	}
	creator->cSpecialWeapon->remainingCooldown = creator->cSpecialWeapon->cooldown;

	Vec2 vel = Vec2(12*cos(72*PI/180),12*sin(72*PI/180));
	int numberOfMeteors = genRandomInt(100, 400);
	for (int i = 0; i < numberOfMeteors; i++)
	{
		auto entity = m_manager.addEntity("Bullet");

		Vec2 location = {
			genRandomInt(0, m_window.getSize().x), 
			genRandomInt(0, m_window.getSize().y * 8) * -1
		};

		entity->cTransform = std::make_shared<CTransform>(location, vel, 0);
		entity->cCollision = std::make_shared<CCollision>(4);
		entity->cShape = std::make_shared<CShape>(8, 120,
			sf::Color(255, 0, 0),
			sf::Color(255, 0, 0), 5);

		entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L * 2);
		entity->cFlamming = std::make_shared<CFlamming>();
	}
}

void Engine::sEnemySpawner()
{
	int spawnPeriod = (int) (m_enemyConfig.SP * m_frameRateLimit);
	if (m_lastEnemySpawnTime + spawnPeriod <= m_currentFrame)
	{
		spawnEnemy();
	}
}

void Engine::sMovement()
{
	if (m_player && m_player->cTransform)
	{
		Vec2 vel = { (float) (m_playerConfig.S*cos(45*PI/180)), (float)(m_playerConfig.S*sin(45*PI/180)) };
		m_player->cTransform->velocity = { 0, 0 };

		if (m_player->cInput->up)
		{
			m_player->cTransform->velocity.y += vel.y * -1;
		}

		if (m_player->cInput->down)
		{
			m_player->cTransform->velocity.y += vel.y;
		}

		if (m_player->cInput->left)
		{
			m_player->cTransform->velocity.x += vel.x * -1;
		}

		if (m_player->cInput->right)
		{
			m_player->cTransform->velocity.x += vel.x;
		}
	}

	for (auto e : m_manager.getEntities())
	{
		if (!e->cTransform)
		{
			continue;
		}
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
	}

}

void Engine::sRender()
{
	m_window.clear();

	for (auto e : m_manager.getEntities())
	{
		if (!e->cShape || !e->cTransform)
		{
			continue;
		}
		
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		e->cTransform->angle += e->cTransform->rotationAngle;
		e->cShape->circle.setRotation(e->cTransform->angle);

		if(e->cFlamming) 
		{
			if(e->cShape->circle.getFillColor().g == 0) 
			{
				
				e->cShape->circle.setFillColor(sf::Color(255,99,71,255));
				e->cShape->circle.setOutlineColor(sf::Color(255,99,71,255));

			}
			else 
			{
				e->cShape->circle.setFillColor(sf::Color(255,0,0,255));
				e->cShape->circle.setOutlineColor(sf::Color(255,0,0,255));
			}
		}

		if(e->cGrowable) 
		{
			float newThickness = e->cShape->circle.getOutlineThickness() * e->cGrowable->growthRate;
			e->cShape->circle.setOutlineThickness(newThickness);
			e->cCollision->radius = newThickness;
			e->cShape->circle.setRadius(e->cShape->circle.getRadius() * (e->cGrowable->growthRate - 0.007f));
		}

		m_window.draw(e->cShape->circle);
	}

	m_window.display();
}

void Engine::sUserInput()
{
	if (!m_player)
	{
		return;
	}

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::P:
				setPaused();
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			default:
				break;
			}
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				spawnBullet(m_player, Vec2(sf::Mouse::getPosition(m_window).x, sf::Mouse::getPosition(m_window).y));
			} 
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				spawnSpecialWeapon(m_player);
			} 
		}
	}
}

void Engine::sCollision() 
{
	EntityVec bullets = m_manager.getEntities("Bullet");
	EntityVec enemies = m_manager.getEntities("Enemy");

	for (auto e : enemies)
	{
		checkScreenCollision(e);
		for (auto b : bullets)
		{
			float touchingDistance = e->cCollision->radius + b->cCollision->radius;
			Vec2 disVec = b->cTransform->pos - e->cTransform->pos;
			if ( (disVec.x * disVec.x + disVec.y * disVec.y) <= (touchingDistance * touchingDistance) )
			{
				e->destroy();
				b->destroy();
				if(b->cFlamming) 
				{
					spawnGrowableEntity(b);
				}
				spawnSmallEnemies(e);
			}
		}

		if (!m_player)
		{
			continue;
		}
		float touchingDistance = e->cCollision->radius + m_player->cCollision->radius;
		Vec2 disVec = m_player->cTransform->pos - e->cTransform->pos;
		if ( (disVec.x * disVec.x + disVec.y * disVec.y) <= (touchingDistance * touchingDistance) )
		{
			m_player->destroy();
			m_player.reset();
			e->destroy();
		}
	}
	if (m_player)
	{
		checkScreenCollision(m_player);
	}
};

void Engine::sLifespan()
{
	for (auto e : m_manager.getEntities())
	{
		if (!e->cLifespan)
		{
			continue;
		}
		if (e->cLifespan->remaining == 0)
		{
			e->destroy();
			continue;
		}
		float newAlpha = 255 * ((float)e->cLifespan->remaining / (float)e->cLifespan->total);
		
		sf::Color color = e->cShape->circle.getFillColor();
		e->cShape->circle.setFillColor(sf::Color(color.r, color.g, color.b, newAlpha));

		color = e->cShape->circle.getOutlineColor();
		e->cShape->circle.setOutlineColor(sf::Color(color.r, color.g, color.b, newAlpha));

		e->cLifespan->remaining--;
	}
};

void Engine::sSpecialWeaponCooldown()
{
	for (auto e : m_manager.getEntities())
	{
		if (e->cSpecialWeapon && e->cSpecialWeapon > 0)
		{
			e->cSpecialWeapon->remainingCooldown--;
		}
	}
};

void Engine::spawnGrowableEntity(std::shared_ptr<Entity> creator)
{
	auto entity = m_manager.addEntity("Bullet");
	entity->cTransform = std::make_shared<CTransform>(creator->cTransform->pos, Vec2(0, 0), 0);
	entity->cCollision = std::make_shared<CCollision>(4);
	entity->cShape = std::make_shared<CShape>(4, 120,
	sf::Color(0, 0, 0, 0),
	sf::Color(255, 255, 255), 20);
	entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L * 2);
	entity->cGrowable = std::make_shared<CGrowable>(1.01f);
}

void Engine::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	int eVertices = entity->cShape->circle.getPointCount();
	int angle = 360 / eVertices;

	for (int i = 0; i < 360; i+=angle)
	{
		auto e = m_manager.addEntity("SmallEnemy");
		e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
		e->cShape = std::make_shared<CShape>(
			m_enemyConfig.SR/2,
			eVertices,
			entity->cShape->circle.getFillColor(),
			entity->cShape->circle.getOutlineColor(), 
			m_enemyConfig.OT
		);
		Vec2 vel = { (float)cos(i * PI / 180), (float)sin(i * PI / 180) };
		e->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, vel, CIRCLE_ROTATION);
	}
	std::cout << std::endl;

};

int Engine::genRandomInt(int min, int max) const
{
	int len = max - min + 1;
	int random = rand() % len;// [0, len - 1]

	return (random + min);
};