#pragma once
#include "Entity.h"
#include "EntityManager.h"
#include "Vec2.h"

#include <SFML/Graphics.hpp>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; }; 
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L; float SMIN, SMAX, SP	; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Engine
{
	sf::RenderWindow m_window;
	int m_frameRateLimit;
	EntityManager m_manager;
	sf::Font m_font;
	sf::Text m_text;
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	bool m_paused = false;
	bool m_running = true;

	std::shared_ptr<Entity> m_player;

	void init(const std::string& path);
	void setPaused();
	void screenCollision(std::shared_ptr<Entity> entity) const;

	void sMovement(); // System: entity pos / movemnt update
	void sUserInput(); //user input
	void sLifespan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon (std::shared_ptr<Entity> entity);

public:
	Engine(const std::string& config);

	void run();
};