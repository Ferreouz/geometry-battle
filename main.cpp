#include <SFML/Graphics.hpp>
#include "Engine.h"

#include <iostream>
//#include "Vec2.h"

#include "EntityManager.h"

int main()
{
    Engine e("config.txt");
    e.run();

    //Vec2 vec1(2, 2);
    //Vec2 vec2(1, 1);
    //auto manager = EntityManager();
    //auto entity = manager.addEntity("Player");
    //manager.update();

    //auto es = manager.getEntities("Player");
    //auto ex = manager.getEntities();

    //for (auto e : ex)
    //{
    //    std::cout << e->id() << " " << e->tag();
    //}

    //for (auto e : es)
    //{
    //    std::cout << e->id() << " " << e->tag();
    //}


}