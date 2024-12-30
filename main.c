#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <stdbool.h>
#define table_size 100

typedef struct key_value{
    char * key;
    char * value;
    struct key_value * chain;
}key_value;



typedef struct Table{
key_value * table;
long long size;
}Table;


void extend_table(Table * table,bool flag){
   if(flag){
        table->size=100;
        table->table=(key_value *)malloc(100*sizeof(key_value));
   }else{
        table->size*=2; 
        table->table=(key_value *)realloc(table->table,table->size*sizeof(key_value));
   }
}









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


void insert(char * key,char * value,Table *table){  
   long long index=hash(key,table);
   table->table[index].key=key;
   table->table[index].value=value;
}


void del(char * key,Table * table){
   long long index=hash(key,table);
   free(table->table[index].key);
   table->table[index].key=NULL;
   free(table->table[index].value);
   table->table[index].value=NULL;
}
















void handle_cli(Table * hash_table) {
    char input[1000];
    char command[50];
    int kv_size=100;
    int a, b;

    while (1) {
        printf("redis:$ ");
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
        } else if (strcmp(command, "push") == 0) {
            char * key=malloc(kv_size*sizeof(char));
            char * value=malloc(kv_size*sizeof(char));
            // expect two integers after the "add" command
            if (sscanf(input, "push %s %s", key, value) == 2) {
                insert(key,value,hash_table);
            } else {
                printf("usage: insert <key> <value>\n");
            }
        } else if (strcmp(command, "exit") == 0) {
            break;  // exit the loop and the program
        } else {
            printf("unknown command: %s\n", command);
        }
    }
}


int main(){
Table * hash_table=malloc(sizeof(Table));
extend_table(hash_table,1);
handle_cli(hash_table);
for(int i=0;i<hash_table->size;i++){
        printf("key : %s value : %s \n",hash_table->table[i].key,hash_table->table[i].value);
}


return 0;
}
//setting null as initial address
//pop 
//chaining
//snapshot
