#include <SFML/Graphics.hpp>

using namespace sf;

View view;

View getView(float x, float y, int currentMap)
{
	if (currentMap == 1) {
		if (x < 400) { x = 400; }// so that camera would not go off the map
		if (x > 1520) { x = 1520; } // so that camera would not go off the map
		view.setCenter(x, 240); // camera is on Hero X position, and Y is fixed
		return view;
	}
	else if (currentMap == 2){
		if (x < 400){
			x = 400;
		}
		if (x > 1200){
			x = 1200;
		}
		if (y < 240){
			y = 240;
		}
		if (y > 560){
			y = 560;
		}
		view.setCenter(x, y);
		return view;
	}
	else {
		if (x < 400) {
			x = 400;
		}
		if (x > 2240 - 400 - 32) {
			x = 2240 - 400 - 32;
		}
		if (y < 240) {
			y = 240;
		}
		if (y > 560) {
			y = 560;
		}
		view.setCenter(x, y);
		return view;
	}
}