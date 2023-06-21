#include <stdio.h>
#include <string.h>
#include "sort.h"

int main() {
	Student students[max] = {
		{"John", 85},
		{"Alice", 92},
		{"Michael", 78},
		{"Sarah", 95},
		{"David", 88}
	};
	int numStudents = max;

	printf("Before sorting:\n");
	for (int i=0; i<numStudents; i++){
		printf("Name: %s, Score: %d\n", students[i].name, students[i].score);
	}
	sortScores(students, max);

	printf("\nAfter Sorting:\n");
	for(int i=0; i<numStudents; i++){
		printf("Name: %s, Score: %d\n", students[i].name, students[i].score);
	}
	addStudent(students, &numStudents);
	printf("\nAfter adding a studetns:\n");
	for (int i=0; i<numStudents; i++){
		printf("Name: %s, Score: %d\n", students[i].name, students[i].score);
	}

	return 0;
}
