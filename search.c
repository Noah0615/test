#include "phone.h"
#include <stdio.h>
#include <string.h>

extern struct Contact PhoneBook[MAX];
extern int size;

void searchByName(){
	char searchName[50];
	printf("Search by Name\n");
	printf(">> Enter a name to search: ");
	scanf("%s", searchName);

	int found =0;
	for(int i=0; i<size; i++){
		if(strcmp(PhoneBook[i].Name, searchName) == 0)
		{
			printf("%-10s %s\n", PhoneBook[i].Name, PhoneBook[i].PhoneNumber);
			found = 1;
			break;
			}
	}
	if(!found)
		printf("Oops! %s is not in the PhoneBook.\n", searchName);
}
