//bea hoekstra / student id: 40285118 / Section CX
//omar ghazaly / student id: 40280795 / Section CY

#include <iostream>
#include "Array.h"
#include "Point.h"
#include "Triangle.h"

int main() {

	int size;
	std::cout << "Enter the size of the array: ";
	std::cin >> size;

	int* array = createArray(size);

	initializeArray(array, size);
	printArray(array, size);
	deleteArray(array);

	int x;
	int y;
	int z;
	char axis;
	int d;
	Point p1;
	Point p2;
	Point p3;
	Triangle* t = nullptr;

	int choice;

	do {
		std::cout << "\nMenu\n";
		std::cout << "1. Create triangle\n";
		std::cout << "2. Translate triangle\n";
		std::cout << "3. Display triangle coordinates\n";
		std::cout << "4. Display triangle area\n";
		std::cout << "0. Exit program\n";
		std::cout << "\nEnter choice: ";
		std::cin >> choice;

		switch (choice) {
			case 1:
				std::cout << "Enter the x coordinate of the first point: ";
				std::cin >> x;
				std::cout << "Enter the y coordinate of the first point: ";
				std::cin >> y;
				std::cout << "Enter the z coordinate of the first point: ";
				std::cin >> z;
				p1 = Point(x, y, z);
				std::cout << "Enter the x coordinate of the second point: ";
				std::cin >> x;
				std::cout << "Enter the y coordinate of the second point: ";
				std::cin >> y;
				std::cout << "Enter the z coordinate of the second point: ";
				std::cin >> z;
				p2 = Point(x, y, z);
				std::cout << "Enter the x coordinate of the third point: ";
				std::cin >> x;
				std::cout << "Enter the y coordinate of the third point: ";
				std::cin >> y;
				std::cout << "Enter the z coordinate of the third point: ";
				std::cin >> z;
				p3 = Point(x, y, z);
				delete t;
				t = new Triangle(p1, p2, p3);
				break;
			case 2:
				if (t == nullptr) {
					std::cout << "Invalid triangle.";
					break;
				}
				std::cout << "Enter the axis to translate the triangle x/y/z: ";
				std::cin >> axis;
				std::cout << "Enter the the distance to translate the triangle: ";
				std::cin >> d;
				t->translate(d, axis);
				break;
			case 3:
				if (t == nullptr) {
					std::cout << "Invalid triangle.";
					break;
				}
				t->display();
				break;
			case 4:
				if (t == nullptr) {
					std::cout << "Invalid triangle.";
					break;
				}
				std::cout << t->calcArea() << std::endl;
				break;
			case 0:
				delete t;
				return 0;
			default:
				std::cout << "Invalid choice\n";
				break;
		}
	} while (choice != 0);


	delete t;
	return 0;
}