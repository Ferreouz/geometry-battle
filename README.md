# Geometry Battle 🚀

This was my first ever game made in C++. It follows the [Entity Component System](https://austinmorlan.com/posts/entity_component_system/) (ECS) design. 

I learned some concepts of the ECS design, for example, how to separate logic from data, and the systems that when updated doesn't affect others. I also picked up some knowledge of C++.

It was a fun and exciting experience and yet really challenging (especially the collision Math part 😅).

## Gameplay and Demo

![](demo.gif)

Move your character using W-S-A-D keys, shoot enemies with the primary mouse button. With the secondary a special ability is activated (rain of fire).
Killing enemies gives you points based on the number of verticies that they have. You lose points and special ability cooldown on dying.

## How to Install and run

Linux:

1. Install make, the compiler and SFML
```bash 
sudo apt install g++ build-essential libsfml-dev
```

2. Clone the repository then copy config file
```bash
git clone https://github.com/Ferreouz/geometry-battle
cd geometry-battle
cp config.example.txt config.txt && mkdir bin
```

3. Download a font (the path is changed in the config.txt)
```bash
wget https://wisabo.com/system/uploads/font-files/5363/Mukta-Light.ttf && mv Mukta-Light.ttf mukta.ttf
```

4. Build and run
```bash
make
```

_OBS: I developed using Linux but can be runned in Windows as well_

## Configuration File:

This are the configurations that you can tweak in the config.txt file:

```text
Window Width Heigth FPS 0ForWindow1ForFullScreen
Font Path Size R G B
Player ShapeRadius CollisionRadius Speed FillColorR FillColorG FillColorB OutlineColorR OutlineColorG OutlineColorB OutlineThickness ShapeVertices
Enemy ShapeRadius CollisionRadius MinSpeed MaxSpeed OutlineColorR OutlineColorG OutlineColorB OutlineThickness MinVertices MaxVertices SmallLifespan SpawnIntervalSeconds
Bullet ShapeRadius CollisionRadius Speed FillColorR FillColorG FillColorB OutlineColorR OutlineColorG OutlineColorB OutlineThickness Vertices Lifespan
```
