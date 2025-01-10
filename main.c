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

void free_table(Table * hash_table){
for(int i=0;i<hash_table->size;i++){
        key_value * head=&(hash_table->table[i]);
        key_value * temp;
        temp=head;
        head=head->chain;
        free(temp->key);
        free(temp->value);
        while(head!=NULL){
           temp=head;
           head=head->chain;
           free(temp->key);
           free(temp->value);
           free(temp);

        }
}
free(hash_table->table);
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

void del(char * key, Table * table) {
   long long index = hash(key, table);
   key_value * head = &(table->table[index]);
   
   // First check if the key exists at this position
   if(head->key == NULL) {
       printf("Key not found\n");
       return;
   }
   
   if(strcmp(head->key, key)==0) {
      if(head->chain == NULL) {
          free(head->key);
          free(head->value);
          head->key = NULL;
          head->value = NULL;
      } else {
         key_value * next = head->chain;
         head->key = next->key;
         head->value = next->value;
         head->chain = next->chain;
         free(next);
      }
   } else {
      key_value * prev = head;
      head = head->chain;
      while(head != NULL) {
          if(strcmp(head->key, key)==0) {
              prev->chain = head->chain;
              free(head->key);
              free(head->value);
              free(head);
              break;
          }
          prev = head;
          head = head->chain;
      }
   }
}

char * get(char * key,Table * table){
   long long index=hash(key,table);
   key_value * head=&(table->table[index]);
   while(head!=NULL && head->key!=NULL){
    if(strcmp(head->key,key)==0) {
        printf("value : %s\n",head->value);
        return head->value;
    }
    head=head->chain;
   }
   printf("Key is not present \n");
   return NULL;
}


void extend_table(Table **table, bool flag) {
    if (flag) {
        // Allocate memory for a new table
        Table *new_table = malloc(sizeof(Table));
        if (!new_table) {
            perror("Failed to allocate memory for new_table");
            return;
        }

        // Initialize new_table properties
        new_table->size = 100;
        new_table->table = malloc(new_table->size * sizeof(key_value));
        if (!new_table->table) {
            perror("Failed to allocate memory for new_table->table");
            free(new_table);
            return;
        }

        // Initialize key-value pairs
        for (int i = 0; i < new_table->size; i++) {
            new_table->table[i].key = NULL;
            new_table->table[i].value = NULL;
        }

        // Update the original table pointer
        *table = new_table;

    } else {
        // Allocate memory for a new table with double the size
        Table *new_table = malloc(sizeof(Table));
        if (!new_table) {
            perror("Failed to allocate memory for new_table");
            return;
        }

        new_table->size = (*table)->size * 2;
        new_table->table = malloc(new_table->size * sizeof(key_value));
        if (!new_table->table) {
            perror("Failed to allocate memory for new_table->table");
            free(new_table);
            return;
        }

        // Copy existing key-value pairs to the new table
        for (int i = 0; i < (*table)->size; i++) {
            if ((*table)->table[i].key != NULL) {
                key_value * temp=&(*table)->table[i];
                while(temp!=NULL){
                
                // Allocate memory for new key and value
                char *new_key = malloc(strlen(temp->key) + 1);
                char *new_value = malloc(strlen(temp->value) + 1);

                if (!new_key || !new_value) {
                    perror("Failed to allocate memory for key or value");
                    free(new_table->table);
                    free(new_table);
                    return;
                }

                // Copy key and value
                strcpy(new_key, temp->key);
                strcpy(new_value, temp->value);

                // Insert into the new table
                insert(new_key, new_value, new_table);
                temp=temp->chain;
                }
            }
        }

        // Free the old table
        free_table(*table);

        // Update the original table pointer
        *table = new_table;
    }
}

void handle_cli(Table * hash_table) {
    
    int kv_size = 100;

    while (1) {
        printf("redis:$ ");
        char input[1000];
        char command[50];
        fgets(input, sizeof(input), stdin);

        // Remove trailing newline character from fgets
        input[strcspn(input, "\n")] = 0;

        // Parse the command (the first word of the input)
        sscanf(input, "%s", command);
        // Handle different commands
        if (strcmp(command,"del") == 0) {
            char key[kv_size];
            if (sscanf(input, "del %s", key) == 1) {
                del(key, hash_table);
            } else {
                printf("usage: del <key>\n");
            }
        } 
        else if (strcmp(command,"set") == 0) {
            char *key = malloc(kv_size * sizeof(char));
            char *value = malloc(kv_size * sizeof(char));

            // Check if memory allocation succeeded
            if (!key || !value) {
                printf("Memory allocation failed for key or value\n");
                return;
            }

            if (sscanf(input, "set %s %s", key, value) == 2) {
                insert(key, value, hash_table);
            } else {
                printf("usage: set <key> <value>\n");
            }
        } 
        else if (strcmp(command, "get") == 0) {
            char key[kv_size];
            if (sscanf(input, "get %s", key) == 1) {
                get(key, hash_table);
            } else {
                printf("usage: get <key>\n");
            }
        } 
        else if (strcmp(command, "exit") == 0) {
            break;  // Exit the loop and the program
        } 
        else {
            printf("unknown command: %s\n", command);
        }
    }
}



int main(){
Table * hash_table=malloc(sizeof(Table));
extend_table(&hash_table,1);
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

extend_table(&hash_table,0);
printf("extended table %lld\n",hash_table->size);
for(int i=0;i<hash_table->size;i++){
      
        key_value * head=&(hash_table->table[i]);
        while(head!=NULL){
           printf("key : %s value : %s",head->key,head->value);
           printf(" -> ");
           head=head->chain;

        }
        printf("NULL \n");
}
free_table(hash_table);
free(hash_table);
return 0;
}
//setting null as initial address
//pop 
//chaining
//snapshot
