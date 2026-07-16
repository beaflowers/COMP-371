//bea hoekstra / student id: 40285118 / Section CX

#include <iostream>
#include "Array.h"

int main() {

	int size;
	std::cout << "Enter the size of the array: ";
	std::cin >> size;

	int* array = createArray(size);

	initializeArray(array, size);
	printArray(array, size);
	deleteArray(array);

}