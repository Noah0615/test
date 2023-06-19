#include "phone.h"

struct Contact PhoneBook[MAX];
int size = 0;

int main() {
	int service;
	do
	{
		printf("============ Telephone Book Management ============\n");
                printf(" <<<1. Register\t 2. Print All \t 3. Search by Name \t 4. Delete \t 5. Exit >>>\n");
                printf("Please enter your service number (1-5): ");
                scanf("%d", &service);

		switch (service)
		{
		case 1: registerPhoneData(); break;
		case 2: printAll(); break;
		case 3: searchByName(); break;
		case 4: deleteByName(); break;
		}
	} while (service != 5);
	return 0;
}	


