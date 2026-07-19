#include "Point.h"

Point::Point() {
	x = 0;
	y = 0;
	z = 0;
}

Point::Point(int x, int y, int z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Point::~Point(){}

int Point::getX() const {
	return x;
}

int Point::getY() const {
	return y;
}

int Point::getZ() const {
	return z;
}

int Point::translate(int d, char axis) {
	switch (axis) {
	case 'x':
		x += d;
		return 0;
	case 'y':
		y += d;
		return 0;
	case 'z':
		z += d;
		return 0;
	default:
		return -1;
	}
}