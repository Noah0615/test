#include <stdio.h>
#include "phone.h"

extern struct Contact PhoneNumber[MAX];
extern int size;

void printAll(){
	printf("Print all contacts in the PhoneBook\n");
	if(size == 0){
		printf("PhoneBook is empty\n");
		return;
	}
	printf("<< Display all contacts in the PhoneBook >>\n");
	for(int i=0; i<size; i++){
		printf("%-10s     ", PhoneBook[i].Name);
		printf("%s\n", PhoneBook[i].PhoneNumber);
	}
}
