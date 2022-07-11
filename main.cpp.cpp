#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "View.h"
#include "Level.h"

using namespace sf;

class Entity // Main class of all entities
{
public:
	int currentMap = 2;
	float x, y, speed = 0, dx = 0, dy = 0, w, h;
	enum State { left, right, up, jump, stay };
	enum PreState { pleft, pright, pstay };
	bool onGround, isAlive;
	PreState prestate;
	State state;
	String file;
	Image image;
	Texture texture;
	Sprite sprite;

	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}
};

class Hero : public Entity // inheritor
{
public:
	Hero(String f, int posX, int posY, float sX, float sY, int curMap) // constructor creates icon, sets initial attributes
	{
		currentMap = curMap;
		w = 77; h = 75;
		onGround = false; state = stay; isAlive = true;
		prestate = pright;
		file = f;
		image.loadFromFile(file);
		image.createMaskFromColor(Color::White);
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.scale(sX, sY);
		sprite.setTextureRect(IntRect(79, 165, w, h)); //loading the icon of the Hero
		this->x = posX;
		this->y = posY;
		sprite.setTexture(texture); // setting the icon into sprite
	}
	void changePosition(float newPosX, float newPosY) {
		sprite.setPosition(newPosX, newPosY);
	} 
	void Movement(float time, Keyboard k, float& currentFrame)
	{
		if (isAlive) {
			getView(x, y, currentMap); // capturing the x position of the Hero, so to move the camera according to X position 
			control(k, currentFrame, time); // controlling and playing an animations
			switch (state)
			{
			case right:
				dx = speed;
				break;
			case left:
				dx = -speed;
				break;
			case jump:
				dy = -speed;
				break;
			case stay:
				dx = 0;
				break;
			}
			y += dy * time;
			checkCollisionMap(0, dy); // checking wheather current Tile is passable in Y direction
			x += dx * time;
			checkCollisionMap(dx, 0); // the same but in X direction

			if (!onGround) { dy += 0.0015 * time; dx = 0; } // gravity

			if (state == stay and prestate == pright) {
				dx = 0;
				sprite.setTextureRect(IntRect(79, 165, w, h));
				currentFrame = 3;
			} // setting an animation in initial pose(looking to the right)
			if (state == stay and prestate == pleft) {
				dx = 0;
				sprite.setTextureRect(IntRect(1758, 165, w, h));
				currentFrame = 3;
			} // looking to the left

			sprite.setPosition(x, y); // after movement, it sets the hero in current position

			if (state == right) prestate = pright; // previous state is for stopping animation in initial pose
			if (state == left) prestate = pleft;
			if (state == stay) prestate = pstay;

			state = stay;
		}
	}
	void control(Keyboard k, float& currentFrame, float& timer)
	{
		if (k.isKeyPressed(Keyboard::Left) and (k.isKeyPressed(Keyboard::Up) || k.isKeyPressed(Keyboard::Space)) and isAlive) {
			state = left;
			speed = 0.22;
			currentFrame += 0.005 * timer;
			if (currentFrame >= 8) currentFrame -= 8;
			sprite.setTextureRect(IntRect(1758 - (w * int(currentFrame)), 165, w, h));
		}
		else if (k.isKeyPressed(Keyboard::Left) and isAlive) {
			state = left;
			speed = 0.22;
			onGround = false;
			currentFrame += 0.005 * timer; // animation is changing accoding time the 'Left' button is being pressed
			if (currentFrame >= 8) currentFrame -= 8; // if animation reaches its end, it starts again
			sprite.setTextureRect(IntRect(1758 - (w * int(currentFrame)), 165, w, h)); // animation
		}
		if (k.isKeyPressed(Keyboard::Right) and (k.isKeyPressed(Keyboard::Up) || k.isKeyPressed(Keyboard::Space)) and isAlive) {
			state = right;
			speed = 0.22;
			currentFrame += 0.005 * timer;
			if (currentFrame >= 8) currentFrame -= 8;
			sprite.setTextureRect(IntRect(79 + (w * int(currentFrame)), 165, w, h));
		}
		else if (k.isKeyPressed(Keyboard::Right) and isAlive) {
			state = right;
			speed = 0.22;
			onGround = false;
			currentFrame += 0.005 * timer;
			if (currentFrame >= 8) currentFrame -= 8;
			sprite.setTextureRect(IntRect(79 + (w * int(currentFrame)), 165, w, h));
		}

		if ((k.isKeyPressed(Keyboard::Up) || k.isKeyPressed(Keyboard::Space)) and onGround and isAlive) {
			state = jump;
			speed = 0.55; onGround = false;
			sprite.setTexture(texture);
		}
	}
	void checkCollisionMap(float Dx, float Dy)
	{
		if (x >= 1850 and currentMap == 1) { x = 3; y = 180; currentMap = 2; }
		if (x >= 1550 and currentMap == 2) { x = 3; y = 180; currentMap = 3; }
		if (y < 1) y = 1;
		if (y > 440 and currentMap == 1) { isAlive = false; }// if Hero falls, he dies
		if (y > 760 and currentMap == 2) { isAlive = false; }
		if (y > 960 - 35 and currentMap == 3) { x = 64; y = 5; currentMap = 1; }
		if (x < 2) x = 2; // Hero cannot leave the range of the map
		if (x > 1888 and currentMap == 1) x = 1888;
		if (x > 1561 and currentMap == 2) x = 1561;
		if (isAlive) {
			if (currentMap == 1) {
				for (int i = y / 32; i < (y + h * 0.456) / 32; i++) {
					for (int j = x / 32; j < (x + w * 0.456) / 32; j++) {
						if (map1[i][j] == 'a' || map1[i][j] == 'b' || map1[i][j] == 'c' || map1[i][j] == 'd')
						{
							if (Dy > 0) { y = i * 32 - (h * 0.462); dy = 0; onGround = true; }
							if (Dy < 0) { y = i * 32 + 32; dy = 0;  }
							if (Dx > 0) { x = j * 32 - (w * 0.462); dx = 0; }
							if (Dx < 0) { x = j * 32 + 32; dx = 0; }
						}
					}
				}
			}
			if (currentMap == 2){
				for (int i = y / 32; i < (y + h * 0.46) / 32; i++) {
					for (int j = x / 32; j < (x + w * 0.46) / 32; j++) {
						if (map2[i][j] == 'a' || map2[i][j] == 'b' || map2[i][j] == 'c' || map2[i][j] == 'd' || 
							map2[i][j] == 'e' || map2[i][j] == 'f' || map2[i][j] == 'h' || map2[i][j] == 'p')
						{
							if (Dy > 0) { y = i * 32 - (h * 0.462); dy = 0; onGround = true; }
							if (Dy < 0) { y = i * 32 + 32; dy = 0;  }
							if (Dx > 0) { x = j * 32 - (w * 0.462); dx = 0; }
							if (Dx < 0) { x = j * 32 + 32; dx = 0; }
						}
						if (map2[i][j] == 'k' || map2[i][j] == 'l' || map2[i][j] == 'j')
						{
							if (Dy > 0 || Dy < 0 || Dx > 0 || Dx < 0) isAlive = false;
						}
					}
				}
			}
			if (currentMap == 3) {
				for (int i = y / 32; i < (y + h * 0.46) / 32; i++) {
					for (int j = x / 32; j < (x + w * 0.46) / 32; j++) {
						if (map3[i][j] == 'a' || map3[i][j] == 'b')
						{
							if (Dy > 0) { y = i * 32 - (h * 0.462); dy = 0; onGround = true; }
							if (Dy < 0) { y = i * 32 + 32; dy = 0; }
							if (Dx > 0) { x = j * 32 - (w * 0.462); dx = 0; }
							if (Dx < 0) { x = j * 32 + 32; dx = 0; }
						}
					}
				}
			}
		}
	}
};

int main()
{
	RenderWindow window(VideoMode(800, 480), "Caveman", Style::Close | Style::Resize); // 30 * 15 tiles
	view.reset(FloatRect(0, 0, 800 , 480)); // initial camera position

	float currentFrame = 0; // for animation
	
							//LEVEL 1
	Texture text_bg;
	text_bg.loadFromFile("img/level_1.png"); // background
	Sprite spr_bg(text_bg);
							//LEVEL 2
	Texture text_bg2;
	text_bg2.loadFromFile("img/level_2.png");
	Sprite spr_bg2(text_bg2);
							//LEVEL 3
	Texture text_bg3;
	text_bg3.loadFromFile("img/level_3.png");
	Sprite spr_bg3(text_bg3);

	Hero hero("img/ddd.png", 64, 224, 0.462, 0.462, 3);// creating our hero
	
	Image map_image;
	map_image.loadFromFile("img/solid_4.png");
	map_image.createMaskFromColor(Color::White);
	Texture map_texture;
	map_texture.loadFromImage(map_image);
	Sprite map_sprite(map_texture); // platforms

	Image map_image2;
	map_image2.loadFromFile("img/black_solid.png");
	map_image2.createMaskFromColor(Color::White);
	Texture map_texture2;
	map_texture2.loadFromImage(map_image2);
	Sprite map_sprite2(map_texture2); // platforms

	Clock clock; // time flow
	Keyboard k; // is a parameter for our function 'Movement'
	
	float delay = 1;
	bool LevelClear = false;
	while (window.isOpen())
	{
		float timer = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		timer = timer / 800;

		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == e.Closed)
			{
				window.close();
			}
		} // to close the window
		hero.Movement(timer, k, currentFrame);
		window.setView(view); // setting the camera on Hero
		window.clear();
		if (hero.currentMap == 1) {
			window.draw(spr_bg); // drawing our background
			for (int i = 0; i < HEIGHT1; i++) {
				for (int j = 0; j < WIDTH1; j++) {
					if (map1[i][j] == 'a') { map_sprite.setTextureRect(IntRect(64, 0, 32, 32)); }
					if (map1[i][j] == 'b') { map_sprite.setTextureRect(IntRect(96, 0, 32, 32)); }
					if (map1[i][j] == 'c') { map_sprite.setTextureRect(IntRect(0, 0, 32, 32)); }
					if (map1[i][j] == 'd') { map_sprite.setTextureRect(IntRect(32, 0, 32, 32)); }
					if (map1[i][j] == ' ') { continue; }

					map_sprite.setPosition(32 * j, 32 * i);
					window.draw(map_sprite);
				}
			}
		}
		if (hero.currentMap == 2)
		{
			window.draw(spr_bg2);
			for (int i = 0; i < HEIGHT2; i++) {
				for (int j = 0; j < WIDTH2; j++) {
					if (map2[i][j] == 'a') { map_sprite2.setTextureRect(IntRect(0, 0, 32, 32)); }
					if (map2[i][j] == 'b') { map_sprite2.setTextureRect(IntRect(32, 0, 32, 32)); }
					if (map2[i][j] == 'c') { map_sprite2.setTextureRect(IntRect(64, 0, 32, 32)); }
					if (map2[i][j] == 'd') { map_sprite2.setTextureRect(IntRect(96, 0, 32, 32)); }

					if (map2[i][j] == 'e') { map_sprite2.setTextureRect(IntRect(128, 0, 32, 32)); }
					if (map2[i][j] == 'f') { map_sprite2.setTextureRect(IntRect(160, 0, 32, 32)); }
					if (map2[i][j] == 'g') { map_sprite2.setTextureRect(IntRect(192, 0, 32, 32)); }
					if (map2[i][j] == 'h') { map_sprite2.setTextureRect(IntRect(224, 0, 32, 32)); }

					if (map2[i][j] == 'k') { map_sprite2.setTextureRect(IntRect(256, 0, 32, 32)); }
					if (map2[i][j] == 'l') { map_sprite2.setTextureRect(IntRect(288, 0, 32, 32)); }
					if (map2[i][j] == ' ' || map2[i][j] == 'p' || map2[i][j] == 'j') { continue; }

					map_sprite2.setPosition(32 * j, 32 * i);
					window.draw(map_sprite2);
				}
			}
		}
		if (hero.currentMap == 3)
		{
			window.draw(spr_bg3);
			for (int i = 0; i < HEIGHT3; i++) {
				for (int j = 0; j < WIDTH3; j++) {
					if (map3[i][j] == 'b') { map_sprite2.setTextureRect(IntRect(32, 0, 32, 32)); }
					if (map3[i][j] == 'a' || map3[i][j] == 'j') { map_sprite2.setTextureRect(IntRect(64, 0, 32, 32)); }
					if (map3[i][j] == ' ') { continue; }

					map_sprite2.setPosition(32 * j, 32 * i);
					window.draw(map_sprite2);
				}
			}
		}
		if (!hero.isAlive){
			hero.currentMap = 1;
			hero.isAlive = true;
			hero.changePosition(64, 220);
			hero.x = 64; hero.y = 220;
		}
		window.draw(hero.sprite);
		window.display(); // displays all happening around
	}
	return 0;
}
