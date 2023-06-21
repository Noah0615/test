#include "sort.h"

void addStudent(Student* students, int* numStudents){
	if(*numStudents >= max) {
		printf("Maximum number of studetns reached!\n");
		return;
	}
	printf("Enter the name of the student: ");
	scanf("%s", students[*numStudents].name);
	printf("Enter the score of the student: ");
	scanf("%d", &(students[*numStudents].score));

	(*numStudents)++;
}
