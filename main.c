#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <stdbool.h>
#define table_size 100
int kv_size=100;
typedef struct key_value{
    char * key;
    char * value;
    struct key_value * chain;
}key_value;



typedef struct Table{
key_value * table;
long long size;
}Table;









long long hash(char * key,Table * table){
    int i=0;
    long long hash=0; 
    while(key[i]!='\0'){
        hash+=key[i];
        i++;
    }
    hash=hash%table->size;
    return hash;

}

void free_table(Table * hash_table,bool flag){
for(int i=0;i<hash_table->size;i++){
        key_value * head=&(hash_table->table[i]);
        key_value * temp;
        while(head!=NULL){
           temp=head;
           head=head->chain;
           free(temp->key);
           free(temp->value);
           free(temp);
        }
}
free(hash_table->table);
if(flag){
free(hash_table);
}

}

void insert(char * key,char * value,Table * hash_table){  
   long long index=hash(key,hash_table);
   if(hash_table->table[index].key==NULL){
   hash_table->table[index].key=key;
   hash_table->table[index].value=value;
   }else{
    key_value * next=malloc(sizeof(key_value));
    next->key=key;
    next->value=value;
    next->chain=NULL;
    key_value * head=&(hash_table->table[index]);
    while(head->chain!=NULL){
        head=head->chain;
    }
    head->chain=next;
   }

}


void del(char * key,Table * table){
   long long index=hash(key,table);
   key_value * head=&(table->table[index]);
   if(strcmp(head->key,key)==0){
      if(head->chain==NULL){
          free(head->key);
          free(head->value);
          head->key=NULL;
          head->value=NULL;
      }else{
         key_value * next=head->chain;
         head->key=next->key;
         head->value=next->value;
         head->chain=next->chain;
         free(next);
      }

   }else{
    key_value * prev=head;
    head=head->chain;
    while(head!=NULL) {
        if(strcmp(head->key,key)==0){
            prev->chain=head->chain;
            free(head);
            break;
        }
        prev=head;
        head=head->chain;
    }


   }
}


char * get(char * key,Table * table){
   long long index=hash(key,table);
   key_value * head=&(table->table[index]);
   while(head!=NULL){
    if(strcmp(head->key,key)==0) {
        printf("value : %s",head->value);
        return head->value;
    }
    head=head->chain;
   }
   printf("Key is not present");
   return NULL;
}





Table * extend_table(Table * table,bool flag){
   if(flag){
        table->size=100;
        table->table=(key_value *)malloc(table->size*sizeof(key_value));
        for(int i=0;i<table->size;i++){
            table->table[i].key=NULL;
            table->table[i].value=NULL; 
        }
        return table;
   }else{
        Table * new_table=malloc(sizeof(Table));
        new_table->size=table->size*2;
        new_table->table=(key_value *)malloc(new_table->size*sizeof(key_value));
        for(int i=0;i<table->size;i++){
            if(table->table[i].key!=NULL){
                 
                     char * new_key=malloc(kv_size*sizeof(char));
                     char * new_value=malloc(kv_size*sizeof(char));
                     strcpy(new_key,table->table[i].key);
                     strcpy(new_value,table->table[i].value);
                     insert(new_key,new_value,new_table);
            }
        }
        free_table(table,0);
        return new_table;
   }
}


void handle_cli(Table * hash_table) {
    
    int kv_size=100;

    while (1) {
        printf("redis:$ ");
        char input[1000];
        char command[50];
        fgets(input, sizeof(input), stdin);

        // remove trailing newline character from fgets
        input[strcspn(input, "\n")] = 0;

        // parse the command (the first word of the input)
        sscanf(input, "%s", command);

        // handle different commands
        if (strcmp(command, "del") == 0) {
            char key[kv_size];
            // expect two integers after the "add" command
            if (sscanf(input, "del %s",key) == 1) {
                del(key,hash_table);
            } else {
                printf("usage: del <key>\n");
            }
        } else if (strcmp(command, "set") == 0) {
            char * key=malloc(kv_size*sizeof(char));
            char * value=malloc(kv_size*sizeof(char));
            // expect two integers after the "add" command
            if (sscanf(input, "set %s %s", key, value) == 2) {
                insert(key,value,hash_table);
            } else {
                printf("usage: set <key> <value>\n");
            } 
        } else if(strcmp(command,"get")==0){
             char key[kv_size];
            // expect two integers after the "add" command
            if (sscanf(input, "get %s",key) == 1) {
                get(key,hash_table);
            } else {
                printf("usage: get <key>\n");
            }

        }else if (strcmp(command, "exit") == 0) {
            break;  // exit the loop and the program
        } else {
            printf("unknown command: %s\n", command);
        }
    }
}


int main(){
Table * hash_table=malloc(sizeof(Table));
hash_table=extend_table(hash_table,1);
handle_cli(hash_table);

for(int i=0;i<hash_table->size;i++){
      
        key_value * head=&(hash_table->table[i]);
        while(head!=NULL){
           printf("key : %s value : %s",head->key,head->value);
           printf(" -> ");
           head=head->chain;

        }
        printf("NULL \n");
}
hash_table=extend_table(hash_table,0);
for(int i=0;i<hash_table->size;i++){
      
        key_value * head=&(hash_table->table[i]);
        while(head!=NULL){
           printf("key : %s value : %s",head->key,head->value);
           printf(" -> ");
           head=head->chain;

        }
        printf("NULL \n");
}
free_table(hash_table,1);

return 0;
}
//setting null as initial address
//pop 
//chaining
//snapshot
