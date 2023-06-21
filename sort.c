#include "sort.h"

void sortScores(Student* students, int numStudents){
	int i, j;
	Student temp;

	for(i=0; i< numStudents-1; i++) {
		for(j=0; j<numStudents-i-1; j++){
			if(students[j].score < students[j+1].score) {
				temp = students[j];
				students[j] = students[j+1];
				students[j+1] = temp;
			}
		}
	}
}


