#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#define NAMELENGTH 34
#define RELATIONLENGTH 25
#define LINELENGTH 78
#define CHARLIST 64
#define NUMBER 27
#define ITSOK "|"

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
    char name[NAMELENGTH];
    struct instances *instances;
    struct entities *next;
}entity;

typedef struct relations{
    bool active;
    entity *origin;
    entity *destination;
    struct relations *next;
}relation;


typedef struct types{
    bool calculateNewMax;
    char typeName[RELATIONLENGTH];
    int maxRel;
    relation *matrix[CHARLIST][CHARLIST] /*= {NULL}*/;
    char highest[NUMBER][NAMELENGTH];
    struct types *next;
}type;

typedef struct instances{
    type* type;
    int enterRel;
    struct instances *next;
}instance;

int retOrder(const char* choice){

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

void wipeHighest(char highest[NUMBER][NAMELENGTH]){
    for (int k = 0; k < NUMBER; k++) {
        strcpy(highest[k], ITSOK);
        highest[k][6]='\0';
    }
}

void insertHighest(char highest[NUMBER][NAMELENGTH], char* name){
    for (int k=0; k<NUMBER;k++){
        if (strcmp(highest[k], ITSOK)==0){
            strcpy(highest[k],name);
            return;
        }
    }
}

void printHighest(char highest[NUMBER][NAMELENGTH]){
    for (int l = 0; l < NUMBER != 0; l++) {

        if (strcmp(highest[l], ITSOK) != 0)
            printf("%s ", highest[l]);

    }
}

entity* findEntity(entity **slot, char *name){

    if (n_of_entities==0)
        return NULL;

    entity* curr=slot[retOrder(name)];
    while (curr!=NULL && strcmp(curr->name, name)<=0){

        if (strcmp(curr->name, name)==0)
            return curr;

        curr=curr->next;
    }
    return NULL;
}

type* findType(type* relation,char *typeSearched){
    type* curr=relation;

    while (curr!=NULL && strcmp(curr->typeName, typeSearched)<=0){

        if (strcmp(curr->typeName,typeSearched)==0)

            return curr;

        curr=curr->next;
    }


    return NULL;
}

relation* findRelation(entity **slot, type* beginType, char *origin, char* destination,char *typeSearched){

    if (n_of_relations==0)
        return NULL;

    entity* orig=findEntity(slot,origin);
    entity* dest=findEntity(slot,destination);
    type* ty=findType(beginType, typeSearched);


    if (orig==NULL || destination==NULL || ty==NULL) return NULL;

    relation *curr=ty->matrix[retOrder(origin)][retOrder(destination)];

    while (curr!=NULL){

        if (curr->active==true && curr->origin==orig && curr->destination==dest) return curr;

        curr=curr->next;
    }

    return curr;
}

instance* findInstance(entity* ent, char *typeSearched){

    instance* curr=ent->instances;

    while (curr!=NULL && strcmp(curr->type->typeName,typeSearched)!=0){
        curr=curr->next;
    }

    return curr;

}

entity* make_entity(char* name){
    entity* tmp = malloc(sizeof(entity));
    strcpy(tmp->name,name);
    tmp->next=NULL;
    return tmp;
}

entity* createEntity(entity **listOfUnused, char* name){

    entity* tmp;

    if (*listOfUnused==NULL){
        tmp = make_entity(name);
    }
    else{
        tmp=*listOfUnused;
        (*listOfUnused)=(*listOfUnused)->next;
        strcpy(tmp->name,name);
        tmp->next=NULL;
    }

    return tmp;

}

void monitorEntity(entity** slot, entity **listOfUnused,char *name ){

    int order=retOrder(name);
    entity* tmp=NULL;
    entity* curr=slot[order];
    entity* previous=curr;

    //Se il grafo non ha nomi nella lettera corrispondente, si crea l'entità
    if (slot[order]==NULL){
        slot[order]= make_entity(name);
        n_of_entities++;
    }
    else if (strcmp(curr->name,name)==0){
        return;
    }
    else if (strcmp(curr->name,name)>0){
        //Inserisci in testa
        tmp=createEntity(listOfUnused,name);
        tmp->next=slot[order];
        slot[order]=tmp;
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
            tmp=createEntity(listOfUnused,name);
            previous->next=tmp;
            tmp->next=curr;
            n_of_entities++;
        }
        else if (strcmp(curr->name,name)<0 && curr->next!=NULL){
            tmp=createEntity(listOfUnused,name);
            previous->next=tmp;
            tmp->next=curr;
            n_of_entities++;
        }
        else{
            //Ok, nessuno ha un nome che precede il nuovo, in ordine alfabetico, lo si mette in coda;
            tmp=createEntity(listOfUnused,name);
            curr->next=tmp;
            n_of_entities++;
        }

    }

    high=MAX(high,order);
    low=MIN(low,order);
}

type* make_rel(char *typeSearched, type* next, int maxRel, bool calculateNewMax){
    type* tmp = malloc(sizeof(type));
    strcpy(tmp->typeName,typeSearched);
    tmp->next=next;
    tmp->maxRel=maxRel;
    tmp->calculateNewMax=calculateNewMax;
    return tmp;
}

type* insertion_sort(type **relation, char *typeSearched){

    type* curr=*relation;
    type* prev=*relation;

    while (strcmp(curr->typeName,typeSearched)<0 && curr->next!=NULL){
        prev=curr;
        curr=curr->next;
    }

    if (strcmp(curr->typeName,typeSearched)==0){
        //No need to insert, the entity exists
        return curr;
    }
    else if (strcmp(curr->typeName,typeSearched)>0){
        prev->next=make_rel(typeSearched, curr, 0, true);
        return prev->next;
    }
    else if (strcmp(curr->typeName,typeSearched)<0 && curr->next!=NULL){
        prev->next=make_rel(typeSearched, curr, 0, true);
        return prev->next;
    }
    
    //Insert on tail;
    curr->next=make_rel(typeSearched, NULL, 0, true);;
    return curr->next;
    
}

type* createRel(type **relation, char *typeSearched){

    //Se ancora non c'è alcuna relazione che inizia per quella lettera, creo
    if (*relation==NULL){
        *relation=make_rel(typeSearched, NULL, 0, true);
        return *relation;
    }
    else if (strcmp((*relation)->typeName,typeSearched)==0){
        return *relation;
    }
    else if (strcmp((*relation)->typeName,typeSearched)>0){
        //Inserisci in testa
        *relation=make_rel(typeSearched, *relation, 0, true);
        return *relation;
    }
    
    insertion_sort(relation, typeSearched);
}

instance* make_instance(instance* next, type *relation, int enterRel){
    instance* tmp=malloc(sizeof(instance));
    tmp->next=next;
    tmp->type=relation;
    tmp->enterRel=0;
    return tmp;
}

instance* instantiate(entity* destination, type *relation, char* typeSearched){
    instance *ref=findInstance(destination,typeSearched);

    if (ref!=NULL){
        return ref;
    }

    destination->instances = make_instance(destination->instances, relation, 0);
    return destination->instances;
}

relation* make_relation(bool active, entity* origin, entity* destination, relation* next){
    relation* tmp = malloc(sizeof(relation));
    tmp->active=active;
    tmp->origin=origin;
    tmp->destination=destination;
    tmp->next=next; 
    return tmp;
}

bool instaurateRelation(entity** slot,type **types, char *origin, char* destination,char *typeSearched){


    instance *tmpInstance=NULL;
    entity *toOrigin=findEntity(slot,origin);
    entity *toDestination=findEntity(slot,destination);
    relation *ref=NULL;

    if (toOrigin==NULL || toDestination==NULL){
        return false;
    }

    type *created=createRel(types,typeSearched);

    if (findRelation(slot, *types, origin, destination,typeSearched)!=NULL){
        return false;
    }
    
    ref=created->matrix[retOrder(origin)][retOrder(destination)];
    relation* oth_relation = (ref == NULL) ? NULL : created->matrix[retOrder(origin)][retOrder(destination)];

    tmpInstance=instantiate(toDestination,created,typeSearched);
    tmpInstance->enterRel++;
    n_of_relations++;

    created->maxRel=MAX(created->maxRel,tmpInstance->enterRel);
    created->matrix[retOrder(origin)][retOrder(destination)]=
                make_relation(true, toOrigin, toDestination, created->matrix[retOrder(origin)][retOrder(destination)]);

    return true;
}

void deactivateRelation(entity** slot, type* rel,  char *origin, char *destination,char *typeSearched){

    type* ty=findType(rel, typeSearched);

    if (ty==NULL) return;

    relation *found=findRelation(slot, rel, origin,destination, typeSearched);

    if (found==NULL) return;
    
    if (found->active==true) {
        instance *tmp=NULL;
        found->active=false;
        tmp=findInstance(found->destination, typeSearched);
        if (tmp->enterRel==ty->maxRel){
            ty->maxRel=0;
            ty->calculateNewMax=true;
        }
        tmp->enterRel--;
        n_of_relations--;
    }
}

void deactivateRelations( entity** slot, type *beginType, char *name){

    entity* ref=findEntity(slot, name);

    type* currType=beginType;

    while (currType!=NULL) {

        relation* currRelation=NULL;
        
        for (int col = low; col <=high; col++) {

            currRelation = currType->matrix[retOrder(name)][col];

            while (currRelation != NULL) {

                if (currRelation->active == true && currRelation->origin == ref) {
                    //currRelation->active = false;
                    instance* tmp = findInstance(currRelation->destination, currType->typeName);
                    if (tmp->enterRel == currType->maxRel) {
                        currType->maxRel = 0;
                        currType->calculateNewMax = true;
                    }
                    tmp->enterRel--;
                }

                currRelation = currRelation->next;

            }
        }

        for (int row = low; row <= high; row++) {

            currRelation = currType->matrix[row][retOrder(name)];

            while (currRelation != NULL) {

                if (currRelation->active == true && currRelation->destination == ref) {
                    currRelation->active = false;
                }
                currType->calculateNewMax = true;
                currRelation = currRelation->next;

            }
        }

        currType=currType->next;
    }

}

void demonitorEntity(entity** slot,type *rel,entity **listOfUnused,char *name){

    entity *curr = slot[retOrder(name)];
    entity *prev=curr;

    if (curr != NULL && strcmp(curr->name,name)==0)
    {
        deactivateRelations(slot,rel,name);
        curr->instances=NULL;
        slot[retOrder(name)] = curr->next;
        curr->next=*listOfUnused;
        *listOfUnused=curr;
        n_of_entities--;
        return;
    }

    while (curr != NULL && strcmp(curr->name,name)!=0)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return;

    deactivateRelations(slot, rel ,name);
    curr->instances=NULL;
    prev->next = curr->next;
    curr->next=*listOfUnused;
    *listOfUnused=curr;
    n_of_entities--;
}


void printAll( type *types, entity** slot) {

    bool print = false;

    int max = 0;

    instance *inst = NULL;

    entity *curr = NULL;

    type *currType=types;

    while (currType!=NULL){

        if (currType->calculateNewMax==true){

            max=0;

            for (int j = low; j <= high; j++) {

                curr = slot[j];

                while (curr != NULL) {

                    inst = findInstance(curr, currType->typeName);

                    if (inst != NULL )
                        max = MAX(inst->enterRel, max);

                    curr = curr->next;

                }

            }

            currType->maxRel=max;
            currType->calculateNewMax=false;
        }
        else{
            max=currType->maxRel;
        }

        if (max > 0) {

            print = true;

            //printf("%s ", types[i]);
            fputs(currType->typeName,stdout);
            fputs(" ",stdout);

            for (int j = low; j <= high; j++) {

                curr = slot[j];

                while (curr != NULL) {

                    inst = findInstance(curr, currType->typeName);

                    if (inst != NULL && inst->enterRel >= max) {
                        fputs(curr->name, stdout);
                        fputs(" ", stdout);
                    }

                    curr = curr->next;

                }


            }

            printf("%d", max);
            fputs("; ",stdout);

        }

        currType=currType->next;

    }

    if (print == false)
        fputs("none",stdout);

    fputs("\n",stdout);
}

int main() {

    bool end = false;
    bool addrel =false;
    bool addent =false;

    entity *pool[CHARLIST] = {NULL};

    entity *unusedEntity=NULL;

    instance *unusedInstances=NULL;

    relation *unusedRelations=NULL;

    //Even tough this is indeed the heart of the project, the matrix shall be checked few times, as it will be used
    // only to add or delete relations

    type *types=NULL;

    char line[LINELENGTH];
    char name1[NAMELENGTH];
    char name2[NAMELENGTH];
    char instance[RELATIONLENGTH];

    //Inizia a prendere le informazioni

    fgets(line, LINELENGTH, stdin);

    while (end == false) {
        if (strstr(line, "addent")) {
            addent=true;
            sscanf(&line[6], "%s", name1);
            monitorEntity( pool, &unusedEntity, name1);
            fgets(line, LINELENGTH, stdin);
        } else if (strstr(line, "delent")) {
            sscanf(&line[6], "%s", name1);
            demonitorEntity(pool,types,&unusedEntity,name1);
            fgets(line, LINELENGTH, stdin);
        } else if (strstr(line, "addrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);

            if (addent==true) {

                if (addrel == false) {
                    addrel = instaurateRelation(pool, &types, name1, name2, instance);
                } else {
                    instaurateRelation(pool, &types, name1, name2, instance);
                }

            }

            fgets(line, LINELENGTH, stdin);
        } else if (strstr(line, "delrel")) {
            sscanf(&line[6], "%s %s %s", name1, name2, instance);
            if (addrel==true){
                deactivateRelation(pool, types,name1,name2,instance);
            }
            fgets(line, LINELENGTH, stdin);
        } else if (strstr(line, "report")) {
            if (addrel==false){
                fputs("none\n",stdout);
            }
            else{
                printAll(types,pool);
            }
            fgets(line, LINELENGTH, stdin);
        } else if (strstr(line, "end")) {
            end = true;
        }
    }
    return 0;
}
