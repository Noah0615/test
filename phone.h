#ifndef PHONE_H
#define PHONE_H

#include <stdio.h>
#include <string.h>

#define MAX 50

struct Contact
{
	char Name[50];
	char PhoneNumber[20];
};

extern struct Contact PhoneBook[MAX];

extern int size;

void registerPhoneData();
void printAll();
void searchByName();
void deleteByName();

#endif
