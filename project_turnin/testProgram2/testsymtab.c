#include "symtable.h"
#include<stdio.h>
#include<time.h>
#include <string.h>
#define RAND_KEYS 2000

/*
Prints all key - value pairs, if used as a pfApply argument in pfMap.
*/
void printAll(const char *k, void *v, void *e);


/*
Tests the symtable. Allows the user to perform simple insert/delete/lookup
operations
*/
void userTest(SymTable_T table);


/*
Generates RAND_KEYS random keys and values. It then inserts these in table
and finally searches for the first key, in order to make sure that keys were
properly stored (for large RAND_KEYS, the efficiency of hash tables vs lists is
made apparent)
*/
void massiveTest(SymTable_T table);

int main()
{
	SymTable_T table = SymTable_new();
	int choice;
	printf("Input:\n 1 for user-input testing\n 2 for "
				"a massive random key (%d keys) generation test.\n", RAND_KEYS);
	scanf("%d", &choice);
	srand(time(NULL));
	while(choice != 1 && choice != 2)
	{
		printf("Invalid choice. Input again.\n");
		scanf("%d", &choice);
	}
	switch(choice)
	{
		case(1):
			userTest(table);
			break;
		case(2):
			massiveTest(table);
	}
	SymTable_free(table);
	return 0;
}

void printAll(const char *k, void *v, void *e)
{
	printf("<%s, %d>\n", (char*)k, *(int *)v);
}

void userTest(SymTable_T table)
{
	int choice2, values[400], i;
	printf("\n\n----INITIATING USER TEST----\n\n");
	printf("Input:\n1 for putting a new binding in the symtable\n"
			"2 to execute a get action for a key\n3 to delete a"
			"binding\n4 to print all elements in symtable\n5 to "
			"check whether a key is contained\nAny other int to end."
			"\n");
	scanf("%d", &choice2);
	for(i = 0; i<400; i++)
		values[i] = i;/*initialize value array*/
	printf("-------------------\n");
	while(choice2 >0 && choice2<6)
	{
		char newKey[50];
		int newVal, *retVal, isDeleted, isContained;	
		switch(choice2)
		{
			case(1):
				printf("Input a string up to 50 characters.\n");
				scanf("%50s", newKey);
				printf("Input an int in [0, 399] to "
					"bind to given string.\n");
				scanf("%d", &newVal);
				while(newVal> 399 || newVal < 0)
				{
					printf("Int must be in range [0, 399]."
						" Input again\n");
					scanf("%d", &newVal);
				}
				SymTable_put(table, newKey, &values[newVal]);
				break;
			case(2):
				printf("Input a string up to 50 characters.\n");
				scanf("%50s", newKey);
				retVal = SymTable_get(table, newKey);
				if(retVal == NULL)
					printf("No binding found for %s\n", newKey);
				else
					printf("Key %s is bound to %d\n", newKey, *retVal);
				break;
			case(3):
				printf("Input a string up to 50 characters.\n");
				scanf("%50s", newKey);
				isDeleted = SymTable_remove(table, newKey);
				if(isDeleted == 0)
					printf("Key %s does not exist. "
							"Could not delete.\n", newKey);
				else
					printf("Key %s deleted successfully.\n", newKey);
				break;
			case(4):
				printf("Mappings are:\n\n");
				SymTable_map(table, printAll, NULL);
				printf("\n");
				break;
			case(5):
				printf("Input a string up to 50 characters.\n");
				scanf("%50s", newKey);
				isContained = SymTable_contains(table, newKey);
				if(isContained == 0)
					printf("Key %s is not contained in"
							" symtable.\n", newKey);
				else
					printf("Key %s is contained in"
							" symtable.\n", newKey);							
		}
		printf("-------------------\n");
		printf("\nInput:\n1 for putting a new binding in the symtable\n"
			"2 to execute a get action for a key\n3 to delete a"
			"binding\n4 to print all elements in symtable\n5 to"
			"check whether a key is contained\nAny other int to end."
			"\n");
		scanf("%d", &choice2);				
	}
}


void massiveTest(SymTable_T table)
{
	char key[60], firstKey[60];
	int values[RAND_KEYS], firstVal, *retVal;
	int i, j;
	printf("\n\n----INITIATING MASSIVE TEST----\n\n");
	printf("Will generate %d random keys...\n", RAND_KEYS);
	for(i = 0; i<RAND_KEYS; i++)/*generate the random keys and values*/
	{
		short size = rand()%59+1;/*size ranges from 1 to 59*/
		for(j = 0; j<size; j++)
		{
			short type = rand()%3;/*for current letter, decide whether it will
			be capital letter, small letter or digit*/
			if(type == 0)/*0 --> Capital*/
				key[j] = rand()%26 + 65;/*random capital ascii code*/
			else if(type == 1)/*1 --> Small*/
				key[j] = rand()%26 + 97;/*random small letter ascii code*/
			else/*2 --> digit*/
				key[j] = rand()%10 + 48;/*random ascii digit code*/
		}
		key[size] = '\0'; /*terminate string*/
		/*we've made a new key*/
		values[i] = rand() % 3123;/*generate a random value 
		(3123 chosen arbitrarily)*/
		SymTable_put(table, key, &values[i]);
		if(i == 0)
		{
			strcpy(firstKey, key);
			firstVal = values[0];
		}
	}
	/*validate existence of first key*/
	printf("Keys generated & inserted in symtable.\n");
	printf("Let's search for the first key, %s\n", firstKey);
	printf("It should be bound to %d\n", firstVal);
	retVal = SymTable_get(table, firstKey);
	printf("Value bound to key: %d \n", *retVal);
}