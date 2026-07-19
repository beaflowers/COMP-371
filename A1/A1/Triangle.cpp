#include <cmath>
#include <iostream>
#include "Point.h"
#include "Triangle.h"

Triangle::Triangle() {
	vertex_1 = nullptr;
	vertex_2 = nullptr;
	vertex_3 = nullptr;
}

Triangle::Triangle(Point x, Point y, Point z) {
	vertex_1 = new Point(x);
	vertex_2 = new Point(y);
	vertex_3 = new Point(z);
}

Triangle::~Triangle() {
	delete vertex_1;
	delete vertex_2;
	delete vertex_3;
}

int Triangle::translate(int d, char axis) {
	if (vertex_1 == nullptr || vertex_2 == nullptr || vertex_3 == nullptr) return -1;

	return (vertex_1->translate(d, axis) + vertex_2->translate(d, axis) + vertex_3->translate(d, axis)) < 0 ? -1 : 0;
}

double Triangle::calcArea() const {
	if (vertex_1 == nullptr || vertex_2 == nullptr || vertex_3 == nullptr) return 0.0;

	int ab[3];
	int ac[3];

	ab[0] = vertex_2->getX() - vertex_1->getX();
	ab[1] = vertex_2->getY() - vertex_1->getY();
	ab[2] = vertex_2->getZ() - vertex_1->getZ();
	ac[0] = vertex_3->getX() - vertex_1->getX();
	ac[1] = vertex_3->getY() - vertex_1->getY();
	ac[2] = vertex_3->getZ() - vertex_1->getZ();

	int cross[3];

	cross[0] = ab[1] * ac[2] - ab[2] * ac[1];
	cross[1] = ab[2] * ac[0] - ab[0] * ac[2];
	cross[2] = ab[0] * ac[1] - ab[1] * ac[0];

	return 0.5 * sqrt(cross[0] * cross[0] + cross[1] * cross[1] + cross[2] * cross[2]);
}

void Triangle::display() {
	if (vertex_1 == nullptr || vertex_2 == nullptr || vertex_3 == nullptr) {
		std::cout << "Invalid triangle." << std::endl;
		return;
	}
	std::cout << "Point 1: (" << vertex_1->getX() << ", " << vertex_1->getY() << ", " << vertex_1->getZ() << ")" << std::endl;
	std::cout << "Point 2: (" << vertex_2->getX() << ", " << vertex_2->getY() << ", " << vertex_2->getZ() << ")" << std::endl;
	std::cout << "Point 3: (" << vertex_3->getX() << ", " << vertex_3->getY() << ", " << vertex_3->getZ() << ")" << std::endl;
}