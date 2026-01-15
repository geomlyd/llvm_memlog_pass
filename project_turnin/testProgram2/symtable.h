#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct symTable *SymTable_T;

/*
Returns a new, empty symtable.
*/
SymTable_T SymTable_new(void);

/*
Frees all memory occupied by oSymTable. If it is NULL,
functions does nothing.
*/
void SymTable_free(SymTable_T oSymTable);

/*
Returns number of bindings in oSymTable. It is a checked runtime error for
oSymTable to be NULL.
*/
unsigned int SymTable_getLength(SymTable_T oSymTable);

/*
If the key pcKey is not contained in oSymTable, function creates a new binding
in oSymTable containing key pcKey and value  pvValue, and returns 1. 
If a binding already exists, function makes no changes and returns 0.
It is a checked runtime error for oSymTable or pcKey to be NULL.
*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/*
If oSymTable contains a binding of pcKey, function removes the binding and
returns 1. Otherwise, function returns 0 and doesn't change oSymTable.
It is a checked runtime error for oSymTable or pcKey to be NULL.
*/
int SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*
Returns 1 if oSymTable contains a binding of pcKey, 0 otherwise.
It is a checked runtime error for oSymTable or pcKey to be NULL.
*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*
Returns value bound to pcKey if such a binding exists in oSymTable. If such a 
binding does not exist, function returns NULL. It is a checked runtime error for
oSymTable or pcKey to be NULL.
*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*
Applies function pointed to by pfApply to all bindings in oSymTable. 
Argument pvExtra is the third argument to be passed to pfApply. It is a checked 
runtime error for oSymTable or pfApply to be NULL.
*/
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey,
			void *pvValue, void *pvExtra), const void *pvExtra);
			
#endif