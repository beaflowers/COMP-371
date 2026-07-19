#pragma once

#include "Point.h"

class Triangle {
	private:
		Point* vertex_1;
		Point* vertex_2;
		Point* vertex_3;

	public:
		Triangle();
		Triangle(Point p1, Point p2, Point p3);
		~Triangle();

		int translate(int d, char axis);
		double calcArea() const;
		void display();
};