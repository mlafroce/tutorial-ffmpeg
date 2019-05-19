#ifndef __AREA_H__
#define __AREA_H_

class Area {
public:
	Area(int x, int y, int width, int height);
	int getX() const;
	int getY() const;
	int getWidth() const;
	int getHeight() const;
private:
	int x, y;
	int width, height;
};

#endif
