#include "Engine.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "Vec2.h"

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
				>> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V >> m_playerConfig.S;
			continue;
		}
		if (word == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
				>> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OT >> m_enemyConfig.VMIN
				>> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SP;
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
		sLifespan();

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sCollision();
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

void Engine::screenCollision(std::shared_ptr<Entity> entity) const
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
		entity->cTransform->pos.y = radius + 20;
	}
	if ((entity->cTransform->pos.x + radius) >= m_window.getSize().x)
	{
		entity->cTransform->velocity.x = entity->cTransform->velocity.x * -1;
		entity->cTransform->pos.x = m_window.getSize().x - (radius + 50);
	}
	if ((entity->cTransform->pos.y + radius) >= m_window.getSize().y)
	{
		entity->cTransform->velocity.y = entity->cTransform->velocity.y * -1;
		entity->cTransform->pos.y = m_window.getSize().y - (radius + 20);

	}
}

void Engine::spawnPlayer()
{
	m_player = m_manager.addEntity("Player");

	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	Vec2 vel = Vec2(0.0f, 0.0f);//TODO:
	m_player->cTransform = std::make_shared<CTransform>(Vec2(mx, my), vel, 1.5f);

	m_player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
	m_player->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), 
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

	m_player->cInput = std::make_shared<CInput>();
}

void Engine::spawnBullet(std::shared_ptr<Entity> creator, const Vec2& mousePos)
{
	auto entity = m_manager.addEntity("Bullet");
	Vec2 vel = Vec2(0.0f, 0.0f);//TODO:
	entity->cTransform = std::make_shared<CTransform>(Vec2(mousePos.x, mousePos.y), vel, 1.5f);
	entity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	entity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
	entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
	std::cout << m_bulletConfig.L;
}

void Engine::spawnEnemy()
{
	auto entity = m_manager.addEntity("Enemy");

	//TODO: generate random location within the screen not touching the border
	float mx = 1 + rand() % m_window.getSize().x;
	float my = 1 + rand() % m_window.getSize().y;

	Vec2 vel = Vec2(0.0f, 0.0f);//TODO: generate random velocity based on SMIN and SMAX
	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
	entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), vel, 1.5f);

	int range[] = { 1.9f , 3 };
	float velocity =  rand() % (range[1] - range[0]);
	velocity = velocity + range[0];

	entity->cTransform->velocity = 
	{ 
		(rand() % 2 == 0 ? velocity * -1.0f : velocity), 
		(rand() % 2 == 0 ? velocity * -1.0f : velocity) 
	}; 

	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, m_playerConfig.V,//TODO: generate random vertices using VMIN VMAX
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),//TODO: generate random color
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

	m_lastEnemySpawnTime = m_currentFrame;
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
		float velocity = m_playerConfig.S;
		m_player->cTransform->velocity = { 0, 0 };

		if (m_player->cInput->up)
		{
			m_player->cTransform->velocity.y += velocity * -1;
		}

		if (m_player->cInput->down)
		{
			m_player->cTransform->velocity.y += velocity;
		}

		if (m_player->cInput->left)
		{
			m_player->cTransform->velocity.x += velocity * -1;
		}

		if (m_player->cInput->right)
		{
			m_player->cTransform->velocity.x += velocity;
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

		e->cTransform->angle += e->cTransform->rotationAngle;//config
		e->cShape->circle.setRotation(e->cTransform->angle);
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
			spawnBullet(m_player, Vec2(sf::Mouse::getPosition(m_window).x, sf::Mouse::getPosition(m_window).y));
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				spawnBullet(m_player, Vec2(sf::Mouse::getPosition(m_window).x, sf::Mouse::getPosition(m_window).y));
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
		screenCollision(e);
		for (auto b : bullets)
		{
			float touchingDistance = e->cCollision->radius + b->cCollision->radius;
			Vec2 disVec = b->cTransform->pos - e->cTransform->pos;
			if ( (disVec.x * disVec.x + disVec.y * disVec.y) <= (touchingDistance * touchingDistance) )
			{
					b->destroy();
					e->destroy();
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
		screenCollision(m_player);
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
		e->cLifespan->remaining--;
	}
};