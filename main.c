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

int low = 0;
int high = 0;

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
    int enter_rel;
    struct instances* next;
}instance;

int hash(const char* choice){

    if (choice[1] == '-')
        return 0;
    else if (choice[1] == '_')
        return 37;
    else if (choice[1]>='0' && choice[1] <='9')
        return 1+choice[1]-'0';
    else if (choice[1]>='A' && choice[1] <='Z')
        return 11+choice[1]-'A';
    else if (choice[1]>='a' && choice[1] <='z')
        return 38+choice[1]-'a';

}

entity* make_entity(char* name, entity* next){
    entity* tmp = malloc(sizeof(entity));
    strcpy(tmp->name,name);
    tmp->next = next;
    return tmp;
}

entity* find_entity(entity** slot, char* name){

    if (slot==NULL) return NULL;

    entity* curr = slot[hash(name)];
    while (curr != NULL && strcmp(curr->name, name)<=0){
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

type* find_type(type* type_,char* type_searched){
    type* curr = type_;
    while (curr != NULL && strcmp(curr->type_name, type_searched)<=0){
        if (strcmp(curr->type_name,type_searched) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

instance* make_instance(instance* next, type* type_, int enter_rel){
    instance* tmp = malloc(sizeof(instance));
    tmp->next = next;
    tmp->type = type_;
    tmp->enter_rel = 0;
    return tmp;
}

instance* find_instance(entity* ent, char* type_searched){
    instance* curr = ent->instances;
    while (curr != NULL && strcmp(curr->type->type_name,type_searched) != 0){
        curr = curr->next;
    }
    return curr;
}

void addent(entity** slot, char* name ){

    int order = hash(name);
    entity* curr = slot[order];
    entity* previous = curr;

    // If the graph does not have names on the letter, create the entity
    if (slot[order] == NULL){
        slot[order] = make_entity(name, NULL);
    }
    else if (strcmp(curr->name,name) == 0){
        return;
    }
    else if (strcmp(curr->name,name)>0){
        //Insert on head
        slot[order]=make_entity(name, slot[order]);
    }
    else if (strcmp(curr->name,name)<0){

        //If you have it, use insertion sort
        while (strcmp(curr->name,name)<0 && curr->next != NULL){
            previous = curr;
            curr = curr->next;
        }

        if (strcmp(curr->name,name) == 0){
            //If the entity is not already present
            return;
        }
        else if (strcmp(curr->name,name)>0){
            previous->next = make_entity(name, curr);
        }
        else if (strcmp(curr->name,name)<0 && curr->next != NULL){
            previous->next = make_entity(name, curr);
        }
        else{
            //None has the name that precedes the new one alphabetically, put it in tail;
            curr->next = make_entity(name, NULL);
        }

    }

    high = MAX(high,order);
    low = MIN(low,order);
}

type* instaurate_relationship(type** relationship, char* type_searched){

    type* curr=*relationship;
    type* prev = NULL;

    while (strcmp(curr->type_name,type_searched)<0 && curr->next != NULL){
        prev = curr;
        curr = curr->next;
    }

    if (strcmp(curr->type_name,type_searched) == 0){
        //No need to insert, the entity exists
        return curr;
    }
    else if (strcmp(curr->type_name,type_searched)>0){
        prev->next = make_type(type_searched, curr);
        return prev->next;
    }
    else if (strcmp(curr->type_name,type_searched)<0 && curr->next != NULL){
        prev->next = make_type(type_searched, curr);
        return prev->next;
    }
    
    //Insert on tail;
    curr->next = make_type(type_searched, NULL);
    return curr->next;
    
}

type* create_type(type** relationship, char* type_searched){

    //If no instance exists already, create it
    if (*relationship == NULL){
        *relationship = make_type(type_searched, NULL);
        return *relationship;
    }
    else if (strcmp((*relationship)->type_name,type_searched) == 0){
        return *relationship;
    }
    else if (strcmp((*relationship)->type_name,type_searched)>0){
        //Insert in head
        *relationship = make_type(type_searched, *relationship);
        return *relationship;
    }
    
    instaurate_relationship(relationship, type_searched);
}

instance* instantiate(entity* destination, type* type_, char* type_searched){
    instance* ref = find_instance(destination,type_searched);

    if (ref != NULL){
        return ref;
    }

    destination->instances = make_instance(destination->instances, type_, 0);
    return destination->instances;
}

relationship* make_relationship(entity* origin, entity* destination, relationship* next){
    relationship* tmp = malloc(sizeof(relationship));
    tmp->origin = origin;
    tmp->destination = destination;
    tmp->next = next; 
    return tmp;
}

relationship* find_relationship(entity** slot, type* begin_type, entity* origin, entity* destination,char* type_searched){

    type* type = find_type(begin_type, type_searched);

    if (type == NULL || type->matrix[hash(origin->name)][hash(destination->name)]==NULL) return NULL;

    relationship* curr = type->matrix[hash(origin->name)][hash(destination->name)];

    while (curr != NULL && (curr->origin != origin || curr->destination != destination)){
        curr = curr->next;
    }

    return curr;
}

bool addrel(entity** slot,type** types, char* origin, char* destination,char* type_searched){

    entity* to_origin = find_entity(slot,origin);
    entity* to_destination = find_entity(slot,destination);

    if (to_origin == NULL || to_destination == NULL){
        return false;
    }

    type* created = create_type(types,type_searched);

    if (find_relationship(slot, *types, to_origin, to_destination,type_searched) != NULL){
        return false;
    }
    
    instance* tmp_instance = instantiate(to_destination,created,type_searched);
    tmp_instance->enter_rel++;

    created->matrix[hash(origin)][hash(destination)]=
                make_relationship(to_origin, to_destination, created->matrix[hash(origin)][hash(destination)]);

    return true;
}

void delrel(entity** slot, type* types,  char* origin, char* destination,char* type_searched){

    entity* orig = find_entity(slot,origin);
    entity* dest = find_entity(slot,destination);
    type* type = find_type(types, type_searched ) ;

    if (orig == NULL || destination == NULL || type == NULL || type->matrix[hash(origin)][hash(destination)] == NULL ) return;
    
    relationship* curr = type->matrix[hash(origin)][hash(destination)];
    relationship* prev = NULL;

    while (curr != NULL && (curr->origin != orig || curr->destination != dest)) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return;

    // Unlink and free the current node
    if (prev == NULL) {
        // Removing the head of the list
        type->matrix[hash(origin)][hash(destination)] = curr->next;
    } else {
        prev->next = curr->next;
    }
    
    instance* tmp = find_instance(dest, type_searched);
    if (tmp != NULL) {
        tmp->enter_rel--;
    }

    free(curr);
}

void delete_relationshipships(entity** slot, type* begin_type, char* name){

    entity* ref = find_entity(slot, name);

    type* curr_type = begin_type;

    while (curr_type != NULL) {

        relationship* curr_relationship = NULL;
        
        for (int col = low; col <=high; col++) {

            curr_relationship = curr_type->matrix[hash(name)][col];

            while (curr_relationship != NULL) {

                if (curr_relationship->origin == ref) {
                    instance* tmp = find_instance(curr_relationship->destination, curr_type->type_name);
                    tmp->enter_rel--;
                }

                curr_relationship = curr_relationship->next;

            }
        }

        for (int row = low; row <= high; row++) {

            relationship* prev_relationship = NULL;
            curr_relationship = curr_type->matrix[row][hash(name)];

            while (curr_relationship != NULL) {

                if (curr_relationship->destination == ref) {
                    if (prev_relationship == NULL){
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

void delent(entity** slot,type* rel, char* name){

    entity* curr = slot[hash(name)];
    entity* prev = curr;

    if (curr != NULL && strcmp(curr->name,name) == 0)
    {
        delete_relationshipships(slot,rel,name);
        curr->instances = NULL;
        slot[hash(name)] = curr->next;
        curr->next = NULL;
        return;
    }

    while (curr != NULL && strcmp(curr->name,name) != 0)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return;

    delete_relationshipships(slot, rel ,name);
    curr->instances = NULL;
    prev->next = curr->next;
    curr->next = NULL;
}


void report( type* types, entity** slot) {

    if (types == NULL){
        fputs("none\n",stdout);
        return;
    }

    bool print = false;

    type* curr_type = types;

    while (curr_type != NULL){

        instance* inst = NULL;
        entity* curr_entity = NULL;

        int max = 0;

        for (int j = low; j <= high; j++) {
            curr_entity = slot[j];
            while (curr_entity != NULL) {
                inst = find_instance(curr_entity, curr_type->type_name);
                if (inst != NULL )
                    max = MAX(inst->enter_rel, max);
                curr_entity = curr_entity->next;
            }
        }

        if (max > 0) {

            print = true;
            fputs(curr_type->type_name,stdout);
            fputs(" ",stdout);

            for (int j = low; j <= high; j++) {
                curr_entity = slot[j];
                while (curr_entity != NULL) {
                    inst = find_instance(curr_entity, curr_type->type_name);
                    if (inst != NULL && inst->enter_rel >= max) {
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
    entity* pool[CHAR_LIST] = {NULL};
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
            addent(pool, name1);
        } else if (strstr(line, "delent")) {
            sscanf(&line[6], "%s", name1);
            delent(pool,types, name1);
        } else if (strstr(line, "addrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);
            addrel(pool, &types, name1, name2, instance);
        } else if (strstr(line, "delrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);
            delrel(pool, types,name1,name2,instance);
        } else if (strstr(line, "report")) {
            report(types,pool);
        }
        fgets(line, LINE_LENGTH, stdin);
    }
}