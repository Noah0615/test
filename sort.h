#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define max  10

typedef struct {
	char name[20];
	int score;
} Student;


void sortScores(Student* students, int numStudents);
void addStudent(Student* students, int* numStudents);
