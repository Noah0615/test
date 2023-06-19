#include "phone.h"
#include <stdio.h>
#include <string.h>

static int count_service = 0;
extern int size;

void registerPhoneData()
{
	if(count_service >= MAX)
	{
		printf("PhoneBook is full. Cannot register more contacts.\n");
	}
	printf("New User: ");
	scanf("%s", PhoneBook[size].Name);

	printf("PhoneNumber: ");
	scanf("%s", PhoneBook[size].PhoneNumber);
	size++;
	printf("Registered...\n");
	count_service++;
}
