/*
 * hashtable.h
 * header file for really simple hash table implementation
 *
 * Copyright (c) 2011 Nikias Bassen, All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdlib.h>
#include "common.h"

typedef struct hashentry_t {
	void *key;
	void *value;
	void *next;
} hashentry_t;

typedef unsigned int(*hash_func_t)(const void* key);
typedef int (*compare_func_t)(const void *a, const void *b);

typedef struct hashtable_t {
	hashentry_t *entries[256];
	size_t count;
	hash_func_t hash_func;
	compare_func_t compare_func;
} hashtable_t;

_PLIST_INTERNAL hashtable_t* hash_table_new(hash_func_t hash_func, compare_func_t compare_func);
_PLIST_INTERNAL void hash_table_destroy(hashtable_t *ht);

_PLIST_INTERNAL void hash_table_insert(hashtable_t* ht, void *key, void *value);
_PLIST_INTERNAL void* hash_table_lookup(hashtable_t* ht, void *key);

#endif
