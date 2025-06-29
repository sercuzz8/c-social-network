#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define NAME_LENGTH 34
#define TYPE_LENGTH 25
#define LINE_LENGTH 78
#define CHAR_LIST 64
#define NUMBER 27

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

// Entities will be an ordered hashset with co-domain 0-53 as the hash function has to be ordered
typedef struct entities{
    char name[NAME_LENGTH];
    struct instances* instances;
    struct entities* next;
}entity;

typedef struct relationships{
    entity* origin;
    entity* destination;
    struct relationships* next;
}relationship;

typedef struct types{
    char type_name[TYPE_LENGTH];
    relationship* matrix[CHAR_LIST][CHAR_LIST];
    struct types* next;
}type;

typedef struct instances{
    type* type;
    int n_occurrences;
    struct instances* next;
}instance;

int hash(const char* choice){
    if (choice[1] == '-')
        return 0;
    if (choice[1] == '_')
        return 37;
    if (choice[1]>='0' && choice[1] <='9')
        return 1+choice[1]-'0';
    if (choice[1]>='A' && choice[1] <='Z')
        return 11+choice[1]-'A';
    if (choice[1]>='a' && choice[1] <='z')
        return 38+choice[1]-'a';

}

entity* make_entity(char* name, entity* next){
    entity* tmp = malloc(sizeof(entity));
    strcpy(tmp->name,name);
    tmp->next = next;
    return tmp;
}

entity* find_entity(entity** entities, char* name){

    if (entities==NULL) return NULL;

    entity* curr = entities[hash(name)];
    while (curr && strcmp(curr->name, name)<=0){
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

type* make_type(char* type_searched, type* next){
    type* tmp = malloc(sizeof(type));
    strcpy(tmp->type_name,type_searched);
    tmp->next = next;
    return tmp;
}

type* find_type(type* types ,char* type_searched){
    type* curr = types;
    while (curr && strcmp(curr->type_name, type_searched)<=0){
        if (strcmp(curr->type_name,type_searched) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

instance* make_instance(instance* next, type* type){
    instance* tmp = malloc(sizeof(instance));
    tmp->next = next;
    tmp->type = type;
    tmp->n_occurrences = 1;
    return tmp;
}

instance* find_instance(entity* ent, type* type){
    instance* curr = ent->instances;
    while (curr && strcmp(curr->type->type_name,type->type_name) != 0){
        curr = curr->next;
    }
    return curr;
}

// The alphabetical order has to be kept for printing purposes, hence insertion sort is used
void addent(entity** entities, char* name) {
    int order = hash(name);

    // No entities, insert on head
    if (!entities[order] || strcmp(name, entities[order]->name) < 0) {
        entities[order] = make_entity(name, entities[order]);
        return;
    }

    entity* curr = entities[order];
    entity* prev = NULL;

    while (curr && strcmp(name, curr->name) > 0) {
        prev = curr;
        curr = curr->next;
    }

    // The entity already exists
    if (curr && strcmp(name, curr->name) == 0)
        return;

    prev->next = make_entity(name, curr);   
}

// The alphabetical order has to be kept for printing purposes, hence insertion sort is used
type* add_type(type** types, char* type_searched){
    
    if (!*types || strcmp(type_searched, (*types)->type_name) < 0) {
        // The list is null or the element has to be inserted on the head
        *types = make_type(type_searched, *types);
        return *types;
    }

    // The head is a match
    if (strcmp((*types)->type_name, type_searched) == 0)
        return *types;

    type* prev = *types;
    type* curr = prev->next;

    while (curr && strcmp(curr->type_name, type_searched) < 0) {
        prev = curr;
        curr = curr->next;
    }

    if (curr && strcmp(curr->type_name, type_searched) == 0) {
        return curr;  // It already exists
    }

    // Insert between prev and curr
    prev->next = make_type(type_searched, curr);
    return prev->next;
}

void instantiate(entity* destination, type* type){

    instance* inst = find_instance(destination,type);

    if (inst){
        inst->n_occurrences++;
        return;
    }

    destination->instances = make_instance(destination->instances, type);
}

relationship* make_relationship(entity* origin, entity* destination, relationship* next){
    relationship* tmp = malloc(sizeof(relationship));
    tmp->origin = origin;
    tmp->destination = destination;
    tmp->next = next; 
    return tmp;
}

relationship* find_relationship(entity** entities, type* begin_type, entity* origin, entity* destination,type* type_searched){

    if (!type_searched || type_searched->matrix[hash(origin->name)][hash(destination->name)]==NULL) return NULL;

    relationship* curr = type_searched->matrix[hash(origin->name)][hash(destination->name)];

    while (curr && (curr->origin != origin || curr->destination != destination)){
        curr = curr->next;
    }

    return curr;
}

bool addrel(entity** entities,type** types, char* origin, char* destination,char* type_searched){

    entity* to_origin = find_entity(entities,origin);
    entity* to_destination = find_entity(entities,destination);

    if (!(to_origin && to_destination)){
        return false;
    }

    type* created = add_type(types,type_searched);

    if (find_relationship(entities, *types, to_origin, to_destination, created)){
        return false;
    }
    
    instantiate(to_destination,created);

    created->matrix[hash(origin)][hash(destination)]=
                make_relationship(to_origin, to_destination, created->matrix[hash(origin)][hash(destination)]);

    return true;
}

void delrel(entity** entities, type* types,  char* origin, char* destination,char* type_searched){

    entity* orig = find_entity(entities,origin);
    entity* dest = find_entity(entities,destination);
    type* type = find_type(types, type_searched ) ;

    if (!(orig && destination && type && type->matrix[hash(origin)][hash(destination)])) return;
    
    relationship* curr = type->matrix[hash(origin)][hash(destination)];
    relationship* prev = NULL;

    while (curr && (curr->origin != orig || curr->destination != dest)) {
        prev = curr;
        curr = curr->next;
    }

    if (!curr) return;

    // Unlink and free the current node
    if (!prev) {
        // Removing the head of the list
        type->matrix[hash(origin)][hash(destination)] = curr->next;
    } else {
        prev->next = curr->next;
    }
    
    instance* tmp = find_instance(dest, type);
    if (tmp) {
        tmp->n_occurrences--;
    }

    free(curr);
}

void delete_relationships(entity** entities, type* begin_type, char* name){

    entity* ref = find_entity(entities, name);

    type* curr_type = begin_type;

    while (curr_type) {

        relationship* curr_relationship = NULL;
        
        for (int col = 0; col < CHAR_LIST; col++) {

            curr_relationship = curr_type->matrix[hash(name)][col];

            while (curr_relationship) {

                if (curr_relationship->origin == ref) {
                    instance* tmp = find_instance(curr_relationship->destination, curr_type);
                    tmp->n_occurrences--;
                }

                curr_relationship = curr_relationship->next;

            }
        }

        for (int row = 0; row < CHAR_LIST; row++) {

            relationship* prev_relationship = NULL;
            curr_relationship = curr_type->matrix[row][hash(name)];

            while (curr_relationship) {

                if (curr_relationship->destination == ref) {
                    if (!prev_relationship){
                        curr_type->matrix[row][hash(name)] = curr_relationship->next;
                    }
                    else{
                        prev_relationship->next = curr_relationship->next;
                    }
                    curr_relationship = curr_relationship->next;
                }
                else{
                    prev_relationship = curr_relationship;
                    curr_relationship = curr_relationship->next;
                }
            }
        }

        curr_type = curr_type->next;
    }

}

void delent(entity** entities,type* rel, char* name){

    entity* curr = entities[hash(name)];
    entity* prev = curr;

    if (curr && strcmp(curr->name,name) == 0)
    {
        delete_relationships(entities,rel,name);
        curr->instances = NULL;
        entities[hash(name)] = curr->next;
        curr->next = NULL;
        return;
    }

    while (curr && strcmp(curr->name,name) != 0)
    {
        prev = curr;
        curr = curr->next;
    }

    if (!curr) return;

    delete_relationships(entities, rel ,name);
    curr->instances = NULL;
    prev->next = curr->next;
    curr->next = NULL;
}

int max_instances(entity** entities, type *curr_type){
    instance* inst = NULL;
    entity* curr_entity = NULL;

    int max = 0;

    for (int j = 0; j < CHAR_LIST; j++) {
        curr_entity = entities[j];
        while (curr_entity) {
            inst = find_instance(curr_entity, curr_type);
            if (inst )
                max = MAX(inst->n_occurrences, max);
            curr_entity = curr_entity->next;
        }
    }

    return max;
}

void report( type* types, entity** entities) {

    bool print = false;

    type* curr_type = types;

    while (curr_type){

        int max = max_instances(entities, curr_type);

        entity* curr_entity = NULL;
        instance* inst = NULL;

        if (max > 0) {

            print = true;
            fputs(curr_type->type_name,stdout);
            fputs(" ",stdout);

            for (int j = 0; j < CHAR_LIST; j++) {
                curr_entity = entities[j];
                while (curr_entity) {
                    inst = find_instance(curr_entity, curr_type);
                    if (inst && inst->n_occurrences == max) {
                        fputs(curr_entity->name, stdout);
                        fputs(" ", stdout);
                    }
                    curr_entity = curr_entity->next;
                }
            }

            printf("%d", max);
            fputs("; ",stdout);
        }

        curr_type = curr_type->next;

    }

    if (print == false) fputs("none",stdout);

    fputs("\n",stdout);
}

void main() {
    entity* entities[CHAR_LIST] = {NULL};
    //Even tough this is indeed the heart of the project, the matrix shall be checked few times, as it will be used
    // only to add or delete relationships
    type* types = NULL;

    char line[LINE_LENGTH];
    char name1[NAME_LENGTH];
    char name2[NAME_LENGTH];
    char instance[TYPE_LENGTH];

    //Begin parsing
    while (!strstr(line, "end")) {
        if (strstr(line, "addent")) {
            sscanf(&line[6], "%s", name1);
            addent(entities, name1);
        } else if (strstr(line, "delent")) {
            sscanf(&line[6], "%s", name1);
            delent(entities,types, name1);
        } else if (strstr(line, "addrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);
            addrel(entities, &types, name1, name2, instance);
        } else if (strstr(line, "delrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);
            delrel(entities, types,name1,name2,instance);
        } else if (strstr(line, "report")) {
            report(types,entities);
        }
        fgets(line, LINE_LENGTH, stdin);
    }
}