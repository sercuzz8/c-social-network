#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#define NAME_LENGTH 34
#define RELATION_LENGTH 25
#define LINE_LENGTH 78
#define CHAR_LIST 64
#define NUMBER 27

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))


//TODO: definitely re-import the repeated optimization using a string instead of an array

/* -: 0th element 0:1 1:2 2:3 3:4 4:5 5:6 6:7 7:8 8:9 9:10  A:11 B:12 C:13 D:14 E:15 F:16 G:17 H:18 I:19 J:20 K:21
 * L:22 M:23 N:24 O:25 P:26 Q:27 R:28 S:29 T:30 U:31 V:32 W:33 X:34 Y:35 Z:36 _:37 a:38 b:39 c:40 d:41 e:42 f:43 g:44
 * h:45 i:46 j:47 k:48 l:49 m:50 n:51 o:52 p:53 q:54 r:55 s:56 t:57 u:58 v:59 w:60 x:61 y:62 z:63 */

int n_of_entities=0;
int n_of_relations=0;
int low=0;
int high=0;

typedef struct entities{
    char name[NAME_LENGTH];
    struct instances *instances;
    struct entities *next;
}entity;

typedef struct relations{
    entity *origin;
    entity *destination;
    struct relations *next;
}relation;


typedef struct types{
    char type_name[RELATION_LENGTH];
    int max_rel;
    relation *matrix[CHAR_LIST][CHAR_LIST] /*= {NULL}*/;
    char highest[NUMBER][NAME_LENGTH];
    struct types *next;
}type;

typedef struct instances{
    type* type;
    int enter_rel;
    struct instances *next;
}instance;

int ret_order(const char* choice){

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

entity* find_entity(entity **slot, char *name){

    if (n_of_entities==0)
        return NULL;

    entity* curr=slot[ret_order(name)];
    while (curr!=NULL && strcmp(curr->name, name)<=0){

        if (strcmp(curr->name, name)==0)
            return curr;

        curr=curr->next;
    }
    return NULL;
}

type* find_type(type* relation,char *type_searched){
    type* curr=relation;

    while (curr!=NULL && strcmp(curr->type_name, type_searched)<=0){

        if (strcmp(curr->type_name,type_searched)==0)

            return curr;

        curr=curr->next;
    }


    return NULL;
}

instance* find_instance(entity* ent, char *type_searched){

    instance* curr=ent->instances;

    while (curr!=NULL && strcmp(curr->type->type_name,type_searched)!=0){
        curr=curr->next;
    }

    return curr;

}

entity* make_entity(char* name, entity* next){
    entity* tmp = malloc(sizeof(entity));
    strcpy(tmp->name,name);
    tmp->next=next;
    return tmp;
}

void monitor_entity(entity** slot, char *name ){

    int order=ret_order(name);
    entity* curr=slot[order];
    entity* previous=curr;

    //Se il grafo non ha nomi nella lettera corrispondente, si crea l'entità
    if (slot[order]==NULL){
        slot[order]= make_entity(name, NULL);
        n_of_entities++;
    }
    else if (strcmp(curr->name,name)==0){
        return;
    }
    else if (strcmp(curr->name,name)>0){
        //Inserisci in testa
        slot[order]=make_entity(name, slot[order]);
        n_of_entities++;
    }
    else if (strcmp(curr->name,name)<0){

        //Se li ha, si usa l'insertion sort
        while (strcmp(curr->name,name)<0 && curr->next!=NULL){
            previous=curr;
            curr=curr->next;
        }

        if (strcmp(curr->name,name)==0){
            //Tranne se l'entità è già presente
            return;
        }
        else if (strcmp(curr->name,name)>0){
            previous->next=make_entity(name, curr);
            n_of_entities++;
        }
        else if (strcmp(curr->name,name)<0 && curr->next!=NULL){
            previous->next=make_entity(name, curr);
            n_of_entities++;
        }
        else{
            //Ok, nessuno ha un nome che precede il nuovo, in ordine alfabetico, lo si mette in coda;
            curr->next=make_entity(name, NULL);
            n_of_entities++;
        }

    }

    high=MAX(high,order);
    low=MIN(low,order);
}

type* make_type(char *type_searched, type* next, int max_rel){
    type* tmp = malloc(sizeof(type));
    strcpy(tmp->type_name,type_searched);
    tmp->next=next;
    tmp->max_rel=max_rel;
    return tmp;
}

type* insertion_sort(type **relation, char *type_searched){

    type* curr=*relation;
    type* prev=*relation;

    while (strcmp(curr->type_name,type_searched)<0 && curr->next!=NULL){
        prev=curr;
        curr=curr->next;
    }

    if (strcmp(curr->type_name,type_searched)==0){
        //No need to insert, the entity exists
        return curr;
    }
    else if (strcmp(curr->type_name,type_searched)>0){
        prev->next=make_type(type_searched, curr, 0);
        return prev->next;
    }
    else if (strcmp(curr->type_name,type_searched)<0 && curr->next!=NULL){
        prev->next=make_type(type_searched, curr, 0);
        return prev->next;
    }
    
    //Insert on tail;
    curr->next=make_type(type_searched, NULL, 0);
    return curr->next;
    
}

type* create_type(type **relation, char *type_searched){

    //Se ancora non c'è alcuna relazione che inizia per quella lettera, creo
    if (*relation==NULL){
        *relation=make_type(type_searched, NULL, 0);
        return *relation;
    }
    else if (strcmp((*relation)->type_name,type_searched)==0){
        return *relation;
    }
    else if (strcmp((*relation)->type_name,type_searched)>0){
        //Inserisci in testa
        *relation=make_type(type_searched, *relation, 0);
        return *relation;
    }
    
    insertion_sort(relation, type_searched);
}

instance* make_instance(instance* next, type *relation, int enter_rel){
    instance* tmp=malloc(sizeof(instance));
    tmp->next=next;
    tmp->type=relation;
    tmp->enter_rel=0;
    return tmp;
}

instance* instantiate(entity* destination, type *relation, char* type_searched){
    instance *ref=find_instance(destination,type_searched);

    if (ref!=NULL){
        return ref;
    }

    destination->instances = make_instance(destination->instances, relation, 0);
    return destination->instances;
}

relation* make_relation(entity* origin, entity* destination, relation* next){
    relation* tmp = malloc(sizeof(relation));
    tmp->origin=origin;
    tmp->destination=destination;
    tmp->next=next; 
    return tmp;
}

relation* find_relation(entity **slot, type* begin_type, char *origin, char* destination,char *type_searched){

    entity* orig=find_entity(slot,origin);
    entity* dest=find_entity(slot,destination);
    type* ty=find_type(begin_type, type_searched);


    if (n_of_relations==0 || orig==NULL || destination==NULL || ty==NULL) return NULL;

    relation *curr=ty->matrix[ret_order(origin)][ret_order(destination)];

    while (curr!=NULL && (curr->origin!=orig || curr->destination!=dest)){
        curr=curr->next;
    }

    return curr;
}

bool instaurate_relation(entity** slot,type **types, char *origin, char* destination,char *type_searched){

    instance *tmpInstance=NULL;
    entity *to_origin=find_entity(slot,origin);
    entity *to_destination=find_entity(slot,destination);
    relation *ref=NULL;

    if (to_origin==NULL || to_destination==NULL){
        return false;
    }

    type *created=create_type(types,type_searched);

    if (find_relation(slot, *types, origin, destination,type_searched)!=NULL){
        return false;
    }
    
    ref=created->matrix[ret_order(origin)][ret_order(destination)];
    relation* oth_relation = (ref == NULL) ? NULL : created->matrix[ret_order(origin)][ret_order(destination)];

    tmpInstance=instantiate(to_destination,created,type_searched);
    tmpInstance->enter_rel++;
    n_of_relations++;

    created->max_rel=MAX(created->max_rel,tmpInstance->enter_rel);
    created->matrix[ret_order(origin)][ret_order(destination)]=
                make_relation(to_origin, to_destination, created->matrix[ret_order(origin)][ret_order(destination)]);

    return true;
}

void deactivate_relation(entity** slot, type* types,  char *origin, char *destination,char *type_searched){

    entity* orig=find_entity(slot,origin);
    entity* dest=find_entity(slot,destination);
    type* ty=find_type(types, type_searched);

    if (n_of_relations==0 || orig==NULL || destination==NULL || ty==NULL) return;
    
    relation* curr=ty->matrix[ret_order(origin)][ret_order(destination)];

    while (curr!=NULL && (curr->origin!=orig || curr->destination!=dest)){
        curr=curr->next;
    }

    if (curr==NULL) return;
    
    if (curr->origin!=NULL) {
        instance *tmp=NULL;
        curr->origin=NULL;
        tmp=find_instance(curr->destination, type_searched);
        if (tmp->enter_rel==ty->max_rel){
            ty->max_rel=0;
        }
        tmp->enter_rel--;
        n_of_relations--;
    }
}

void deactivate_relations( entity** slot, type *begin_type, char *name){

    entity* ref=find_entity(slot, name);

    type* curr_type=begin_type;

    while (curr_type!=NULL) {

        relation* curr_relation=NULL;
        
        for (int col = low; col <=high; col++) {

            curr_relation = curr_type->matrix[ret_order(name)][col];

            while (curr_relation != NULL) {

                if (curr_relation->origin == ref) {
                    instance* tmp = find_instance(curr_relation->destination, curr_type->type_name);
                    if (tmp->enter_rel == curr_type->max_rel) {
                        curr_type->max_rel = 0;
                    }
                    tmp->enter_rel--;
                }

                curr_relation = curr_relation->next;

            }
        }

        for (int row = low; row <= high; row++) {

            curr_relation = curr_type->matrix[row][ret_order(name)];

            while (curr_relation != NULL) {

                if (curr_relation->destination == ref) {
                    curr_relation->origin = false;
                }
                curr_relation = curr_relation->next;

            }
        }

        curr_type=curr_type->next;
    }

}

void demonitor_entity(entity** slot,type *rel, char *name){

    entity *curr = slot[ret_order(name)];
    entity *prev=curr;

    if (curr != NULL && strcmp(curr->name,name)==0)
    {
        deactivate_relations(slot,rel,name);
        curr->instances=NULL;
        slot[ret_order(name)] = curr->next;
        curr->next=NULL;
        n_of_entities--;
        return;
    }

    while (curr != NULL && strcmp(curr->name,name)!=0)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return;

    deactivate_relations(slot, rel ,name);
    curr->instances=NULL;
    prev->next = curr->next;
    curr->next=NULL;
    n_of_entities--;
}


void print_all( type *types, entity** slot) {

    bool print = false;

    int max = 0;

    instance *inst = NULL;

    entity *curr = NULL;

    type *curr_type=types;

    while (curr_type!=NULL){

            max=0;

            for (int j = low; j <= high; j++) {

                curr = slot[j];

                while (curr != NULL) {

                    inst = find_instance(curr, curr_type->type_name);
                    if (inst != NULL )
                        max = MAX(inst->enter_rel, max);
                    curr = curr->next;

                }


            curr_type->max_rel=max;
        }

        if (max > 0) {

            print = true;
            fputs(curr_type->type_name,stdout);
            fputs(" ",stdout);

            for (int j = low; j <= high; j++) {

                curr = slot[j];

                while (curr != NULL) {

                    inst = find_instance(curr, curr_type->type_name);
                    if (inst != NULL && inst->enter_rel >= max) {
                        fputs(curr->name, stdout);
                        fputs(" ", stdout);
                    }
                    curr = curr->next;

                }

            }

            printf("%d", max);
            fputs("; ",stdout);

        }

        curr_type=curr_type->next;

    }

    if (print == false)
        fputs("none",stdout);

    fputs("\n",stdout);
}

void main() {

    bool end = false;
    bool addrel =false;
    bool addent =false;

    entity *pool[CHAR_LIST] = {NULL};

    //Even tough this is indeed the heart of the project, the matrix shall be checked few times, as it will be used
    // only to add or delete relations

    type *types=NULL;

    char line[LINE_LENGTH];
    char name1[NAME_LENGTH];
    char name2[NAME_LENGTH];
    char instance[RELATION_LENGTH];

    //Inizia a prendere le informazioni

    fgets(line, LINE_LENGTH, stdin);

    while (end == false) {
        if (strstr(line, "addent")) {
            addent=true;
            sscanf(&line[6], "%s", name1);
            monitor_entity(pool, name1);
            fgets(line, LINE_LENGTH, stdin);
        } else if (strstr(line, "delent")) {
            sscanf(&line[6], "%s", name1);
            demonitor_entity(pool,types, name1);
            fgets(line, LINE_LENGTH, stdin);
        } else if (strstr(line, "addrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);

            if (addent==true) {

                if (addrel == false) {
                    addrel = instaurate_relation(pool, &types, name1, name2, instance);
                } else {
                    instaurate_relation(pool, &types, name1, name2, instance);
                }

            }

            fgets(line, LINE_LENGTH, stdin);
        } else if (strstr(line, "delrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);
            if (addrel==true){
                deactivate_relation(pool, types,name1,name2,instance);
            }
            fgets(line, LINE_LENGTH, stdin);
        } else if (strstr(line, "report")) {
            if (addrel==false){
                fputs("none\n",stdout);
            }
            else{
                print_all(types,pool);
            }
            fgets(line, LINE_LENGTH, stdin);
        } else if (strstr(line, "end")) {
            end = true;
        }
    }
}