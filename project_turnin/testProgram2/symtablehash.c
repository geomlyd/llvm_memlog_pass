#include "symtable.h"
#define HASH_MULTIPLIER 65599
/*initial number of buckets in hash tables*/
#define INIT_BUCKETS 509
/*how many resizes are allowed*/
#define NUM_OF_RSZ 7

struct listNode{
	const char *key;
	void *value;
	struct listNode *next;
};

typedef struct listNode listNode;/*just for the .c file, no need to include in
header*/

struct listHead{
	listNode *first;
	unsigned int len;
};

typedef struct listHead listHead;/*only for this .c file*/

struct symTable{
	listHead **listStarts;/*a hash table contains an array of pointers*/
	unsigned int numOfBuckets;
	unsigned int len;
	unsigned int resizeIndex;/*stores index of next resize to be performed*/
	unsigned int resizes[NUM_OF_RSZ];
};

typedef struct symTable hashTable;/*only for this .c file*/

/*
Attempts to allocate as many bytes as the argument for the user. If malloc
fails, prints an error message to stdout and terminates the program.
*/
static void *mySafeMalloc(size_t bytes)
{
	void *ret = malloc(bytes);
	if(ret == NULL)
	{
		printf("ERROR: Malloc returned NULL, program will exit.\n");
		exit(-1);
	}
	return ret;
}
/*
Returns the hash code of string pcKey in a hash table of hashSize buckets.
It is a checked runtime error for hashSize to be 0 or pcKey to be NULL.
*/
static unsigned int SymTable_hash(unsigned int hashSize, const char *pcKey)
{
	size_t ui;
	unsigned long uiHash = 0U;
	assert(pcKey != NULL && hashSize != 0);
	for(ui = 0U; pcKey[ui] != '\0'; ui++)
	{
		uiHash = uiHash*HASH_MULTIPLIER + pcKey[ui];
	}
	return uiHash%hashSize;
}


/*
If oSymTable has fewer buckets than those specified by its last resize bound,
function rehashes all elements in oSymTable to a new one with size specified by
oSymTable. Otherwise, function does nothing. It is a checked runtime error for
oSymTable to be NULL.
*/
static void SymTable_resize(SymTable_T oSymTable)
{
	assert(oSymTable != NULL);
	if(oSymTable->resizeIndex != NUM_OF_RSZ)/*resizing is allowed*/
	{
		listHead **oldLists = oSymTable->listStarts;/*address of old array*/
		unsigned int oldBuckets = oSymTable->numOfBuckets, i, newBuckets;
		newBuckets = oSymTable->resizes[oSymTable->resizeIndex];
		oSymTable->listStarts = (listHead**) mySafeMalloc(sizeof(listHead *)
						*newBuckets);
		oSymTable->numOfBuckets = newBuckets;
		for(i = 0; i < newBuckets; i++)/*allocate the new lists*/
		{
			oSymTable->listStarts[i] = 
						(listHead*)mySafeMalloc(sizeof(listHead));
			oSymTable->listStarts[i]->first = NULL;/*all the lists are empty*/	
			oSymTable->listStarts[i]->len = 0;/*no bindings contained*/
		}
		for(i = 0; i < oldBuckets; i++)
		{
			listNode *traverse = oldLists[i]->first;
			while(traverse != NULL)
			{
				listNode *tmp;
				unsigned int newHash = SymTable_hash(newBuckets, traverse->key);
				listHead *newList = oSymTable->listStarts[newHash];
				tmp = traverse->next;
				traverse->next = newList->first;/*connect current node to its
				new list*/
				newList->first = traverse;
				newList->len++;/*increase length of list*/
				traverse = tmp;/*advance traverse*/
			}
			free(oldLists[i]);/*free old list head*/
		}
		free(oldLists);/*free the old list array*/
		oSymTable->resizeIndex++;
	}
}

/*
Returns a new, empty symtable.
*/
SymTable_T SymTable_new(void)
{
	SymTable_T newSymTable = (hashTable *) mySafeMalloc(sizeof(hashTable));
	unsigned int i = 0;
	newSymTable->numOfBuckets = INIT_BUCKETS;
	newSymTable->len = 0;/*no bindings contained*/
	newSymTable->resizeIndex = 0;/*initially, no resizes have been done*/
	newSymTable->resizes[0] = 1021;
	newSymTable->resizes[1] = 2053;
	newSymTable->resizes[2] = 4093;
	newSymTable->resizes[3] = 8191;
	newSymTable->resizes[4] = 16381;
	newSymTable->resizes[5] = 32771;
	newSymTable->resizes[6] = 65521;
	newSymTable->listStarts = (listHead **) 
								mySafeMalloc(sizeof(listHead *)*INIT_BUCKETS);
	for(i = 0; i < INIT_BUCKETS; i++)
	{
		newSymTable->listStarts[i] = (listHead*)mySafeMalloc(sizeof(listHead));
		newSymTable->listStarts[i]->first = NULL;/*all the lists are empty*/	
		newSymTable->listStarts[i]->len = 0;/*no bindings contained*/
	}
	return newSymTable;
}


/*
Frees all memory occupied by oSymTable. If it is NULL,
function does nothing.
*/
void SymTable_free(SymTable_T oSymTable)
{
	listNode *traverse;
	unsigned int i, buckets;
	if(oSymTable == NULL) 
		return;
	buckets = oSymTable->numOfBuckets;
	for(i = 0; i < buckets; i++)
	{
		traverse = (oSymTable->listStarts[i])->first;
		while(traverse != NULL)
		{
			listNode *tmp = traverse;
			free((void *)traverse->key);/*free current node's string key*/
			traverse->key = NULL;/*for safety, set its fields to NULL*/
			traverse->value = NULL;
			traverse = traverse->next;/*advance the pointer*/
			free((void *)tmp);/*free the current node*/
		}
		(oSymTable->listStarts[i])->first = NULL;
		/*for safety, set header's fields to NULL and 0*/
		oSymTable->listStarts[i]->len = 0;	
		free(oSymTable->listStarts[i]);/*free list head*/
	}
	free(oSymTable->listStarts);
	free(oSymTable);
}


/*
Returns number of bindings in oSymTable. It is a checked runtime error for
oSymTable to be NULL.
*/
unsigned int SymTable_getLength(SymTable_T oSymTable)
{
	assert(oSymTable != NULL);
	return oSymTable->len;/*since we keep a field for the length, this
	function is quite simple*/
}


/*
If the key pcKey is not contained in oSymTable, function creates a new binding
in oSymTable containing key pcKey and value  pvValue, and returns 1. 
If a binding already exists, function makes no changes and returns 0.
It is a checked runtime error for oSymTable or pcKey to be NULL.
*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
	listHead *insertionList;/*the list where the key will be inserted*/
	listNode *newNode;
	assert(oSymTable != NULL && pcKey != NULL);
	if(SymTable_contains(oSymTable, pcKey))
			return 0;/*binding for pcKey exists*/
	/*find the list where pcKey should be inserted via hashing*/
	if( oSymTable->len + 1 > oSymTable->numOfBuckets )
	{
		/*at each put, check if the insertion will create more than numOfBuckets
		bindings*/
		SymTable_resize(oSymTable);/*attempt to resize*/
	}
	insertionList = 
		oSymTable->listStarts[ SymTable_hash(oSymTable->numOfBuckets, pcKey) ];
	/*if we got here, key does not exist*/
	newNode = (listNode *) mySafeMalloc(sizeof(listNode));
	newNode->key = (char *) mySafeMalloc( strlen(pcKey)+1 );/*allocate mem for key	
	(+1 needed for \0 char)*/
	strcpy((char *)(newNode->key), pcKey);/*copy key*/	
	newNode->value = pvValue;
	newNode->next = insertionList->first;
	insertionList->first = newNode;/*insertion at beginning*/
	insertionList->len++;
	oSymTable->len++;
	return 1;
}


/*
If oSymTable contains a binding of pcKey, function removes the binding and
returns 1. Otherwise, function returns 0 and doesn't change oSymTable.
It is a checked runtime error for oSymTable or pcKey to be NULL.
*/
int SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
	listHead *deletionList;
	listNode *traverse, *previous = NULL;
	unsigned int i, length;
	assert(oSymTable != NULL && pcKey != NULL);
	/*find the list from which pcKey should be deleted via hash*/
	deletionList = 
		oSymTable->listStarts[ SymTable_hash(oSymTable->numOfBuckets, pcKey) ];
	length = deletionList->len;
	for(i = 0, traverse = deletionList->first; i<length; i++)
	{
		if(strcmp(traverse->key, pcKey) == 0)
			break;
		previous = traverse;/*advance pointers*/
		traverse = traverse->next;	
	}
	if(i == length)
		return 0;/*key not found, change nothing*/
	/*if we got here, key exists*/
	free((void *)traverse->key);
	if(previous == NULL)/*deletion from beginning*/
		deletionList->first = traverse->next;
	else
		previous->next = traverse->next;
	/*node is now disconnected*/
	traverse->next = NULL;
	traverse->value = NULL;
	free((void *)traverse);
	deletionList->len--;
	oSymTable->len--;/*decrease length of list and hash table*/
	return 1;	
}


/*
Returns 1 if oSymTable contains a binding of pcKey, 0 otherwise.
It is a checked runtime error for oSymTable or pcKey to be NULL.
*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
	listNode *traverse;
	listHead *searchList;/*searchList: list in which pcKey should be*/
	unsigned int i, length;
	assert(pcKey != NULL && oSymTable != NULL);
	/*find the list to search via hash*/
	searchList = 
		oSymTable->listStarts[ SymTable_hash(oSymTable->numOfBuckets, pcKey) ];
	length = searchList->len;
	for(i = 0, traverse = searchList->first; i < length; 
						i++, traverse = traverse->next)/*search for key*/
	{
		if( strcmp(traverse->key, pcKey) == 0 )
			return 1;
	}
	return 0;/*not found*/
}


/*
Returns value bound to pcKey if such a binding exists in oSymTable. If such a 
binding does not exist, function returns NULL. It is a checked runtime error for
oSymTable or pcKey to be NULL.
*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
	listNode *traverse;
	listHead *searchList;/*searchList: list where pcKey should be*/
	unsigned int i = 0, length;
	assert(oSymTable != NULL && pcKey != NULL);
	searchList = 
		oSymTable->listStarts[ SymTable_hash(oSymTable->numOfBuckets, pcKey) ];
	length = searchList->len;
	traverse = searchList->first;
	for(i = 0; i< length; i++, traverse = traverse->next)/*search for key*/
	{
		if(strcmp(traverse->key, pcKey) == 0)
			return traverse->value;/*key found*/
	}
	return NULL;/*key not found*/
}


/*
Applies function pointed to by pfApply to all bindings in oSymTable. 
Argument pvExtra is the third argument to be passed to pfApply. It is a checked 
runtime error for oSymTable or pfApply to be NULL.
*/
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey,
			void *pvValue, void *pvExtra), const void *pvExtra)
{
	listNode *traverse;
	unsigned int i, buckets = oSymTable->numOfBuckets;
	assert(oSymTable != NULL && pfApply != NULL);
	for(i = 0; i < buckets; i++)/*for all buckets/lists*/
	{
		traverse = (oSymTable->listStarts[i])->first;
		while(traverse != NULL)/*for all nodes in current list*/
		{
			(*pfApply)(traverse->key, traverse->value, pvExtra);
			traverse = traverse->next;/*apply function pfApply to all nodes*/
		}			
	}
}