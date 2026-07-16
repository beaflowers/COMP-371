#include <iostream>
#include "Array.h"

int* createArray(int size) {
	int* newArr = new int[size];
	return newArr;
}

void initializeArray(int* arr, int size) {
	for (int i = 0; i < size; i++) {
		arr[i] = i;
	}
}

void printArray(int* arr, int size) {
	for (int i = 0; i < size; i++) {
		std::cout << arr[i] << " ";
	}
	std::cout << std::endl;
}

void deleteArray(int* arr) {
	std::cout << "Deleting array..." << std::endl;
	delete[] arr;
}