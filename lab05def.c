#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define TABLE_SIZE 1000

typedef struct Item{
    char *key;
    char *data;
    struct Item *next;
}Item;

typedef struct Table{
    int size;
    Item **vector;
}Table;

const char *messages[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);
const char *error_message[] = {"OK", "Duplicate key", "Table overflow"};

int getInt(int *input);
int dialog(const char *messages, int N);
int hash(const char *);
Item *pairItem(const char *key, const char *data);
Table *createTable(void);
void inputTable(Table *hashtable, const char *key, const char *data);
char *getTable(Table *hashtable, const char *key);
void showTable(Table *hashtable);

/*
int addItem(Table *),
    findItem(Table *),
    deleteItem(Table *),
    showTable(Table *);
*/


//int (*functions[])(Table *) = {NULL, addItem, findItem, deleteItem, showTable};

/*
int main(){
    Table table;
    int rc;

    while(rc = dialog(messages, size_messages))
        if(!functions[rc](&table))

            break;
    
    printf("Thats's all. Bye!\n");
    delTable(&table);

    return 0;
}
*/

int main(){
    Table *hashtable = createTable();

    inputTable(hashtable, "fuck1", "shit");
    inputTable(hashtable, "fuck2", "shit");
    inputTable(hashtable, "fuck3", "shit");
    inputTable(hashtable, "fuck4", "shit");
    inputTable(hashtable, "fuck5", "shit");
    showTable(hashtable);

    return 0;
}

int getInt(int *input){
    int check;
    do{
        check = scanf("%d", input);
        if(check == 0){
            printf("Error!\n");
            scanf("%*[^\n]");
        }
    }while(check == 0);

    return (check < 0) ? 0:1;
}

int dialog(const char *messages, int N){
    char *error_message = "";
    int rc;
    int i, n;
    
    do{
        puts(error_message);
        error_message = "You are wrong. Repeat, please.";
        for( ; i < N; i++) puts(messages[i]);
        puts("Make your choice");

        n = getInt(&rc);
        if(n == 0) rc = 0;
    }while (rc < 0 || rc >= N);

    return rc;
}

int hash(const char *key){
    int slot = 0;
    int i = 0;
    int key_length = strlen(key);

    for( ; i < key_length; i++){
        slot = slot * 37 + key[i];
    }

    slot = slot % TABLE_SIZE;

    return slot;
}

Table *createTable(void){
    // указатель на таблицу
    Table *hashtable = (Table *)malloc(sizeof(Table) * 1);
    // зачем?
    if(hashtable == NULL)
        return NULL;
    
    
    // выделение памяти для TABLE_SIZE указателей на указатели на Item
    // ??? как появляется массив?
    hashtable -> vector = (Item **)malloc(sizeof(Item *) * TABLE_SIZE);
    if(hashtable -> vector == NULL)
        return NULL;
    
    // каждый из созданных указателей на указатели сделать равным NULL
    for(int i = 0; i < TABLE_SIZE; i++){
        hashtable -> vector[i] = NULL;
    }

    // можно сделать так:
    // ???
    // memset(hashtable -> vector, NULL, TABLE_SIZE * sizeof(Item));

    hashtable -> size = TABLE_SIZE;

    return hashtable;
}

// вернет указатель на новый Item
Item *pairItem(const char *key, const char *data){
    // приведение типа?
    Item *point = (Item *)malloc(sizeof(point) * 1);
    point -> key = malloc(strlen(key) + 1);
    point -> data = malloc(strlen(data) + 1);

    strcpy(point -> key, key);
    strcpy(point -> data, data);

    point -> next = NULL;

    return point;
}

void inputTable(Table *hashtable, const char *key, const char *data){
    int slot = hash(key);

    // ???
    Item *point = hashtable -> vector[slot];

    if(point == NULL){
        // выделяется память для Item и в него записываются значения key и data
        hashtable -> vector[slot] = pairItem(key, data);
        return ;
    }

    Item *previouse;
    
    while(point != NULL){
        if(strcmp(point -> key, key) == 0){
            free(point -> data);
            point -> data = malloc(strlen(data) + 1);
            strcpy(point -> data, data);
            return;
        }

        previouse = point;
        point = point -> next; 
    }

    previouse -> next = pairItem(key, data);
}

char *getTable(Table *hashtable, const char *key){
    int slot = hash(key);

    Item *point = hashtable -> vector[slot];

    if(point == NULL){
        return NULL;
    }

    while(point != NULL){
        if(strcmp(point -> key, key) == 0){
            return point -> data;
        }

        point = point -> next;
    }

    return NULL;
}

void showTable(Table *hashtable){
    for(int i = 0; i < TABLE_SIZE; i++){
        Item *point = hashtable -> vector[i];

        if(point == NULL){
            continue;
        }

        printf("slot[%d]: ", i);

        for(;;){
            printf("%s = %s ", point -> key, point -> data);

            if(point -> next == NULL){
                break;
            }

            point = point -> next;
        }

        printf("\n");
    }
}

/*
int addItem(Table *table){
    int key, rc, n;
    char *data = NULL;
    printf("Enter key: -->");
    n = getInt(&key);
    if(n == 0) return 0;

    //rc = insert(table, key, data);
    free(data);

    printf("%s: %d\n", error_message[rc], key);

    return 1;
}
*/
