#include "phone.h"
#include <stdio.h>
#include <string.h>

extern struct Contact PhoneBook[MAX];
extern int size;

void deleteByName(){
	char deleteName[50];
	printf(">> Enter a name to delete: ");
	scanf("%s", deleteName);

	int found = 0;
	for (int i=0; i<size; i++){
		if(strcmp(PhoneBook[i].Name, deleteName) == 0)
		{
			printf("%s is delete...\n", deleteName);
			for (int j=i; j<size -1; j++)
			{
				strcpy(PhoneBook[j].Name, PhoneBook[j+1].Name);
				strcpy(PhoneBook[j].PhoneNumber, PhoneBook[j+1].PhoneNumber);
			}
			size--;
			found = 1;
			break;
		}
	}
	if(!found)
	{
		printf("Oops! %s is not in the PhoneBook\n", deleteName);
	}
}
