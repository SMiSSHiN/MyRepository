#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct Item{
    char *key;
    char *data;
}Item;

typedef struct Table{
    int size;
    Item *hashtable[TABLE_SIZE];
}Table;

const char *messages[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);
const char *error_message[] = {"OK", "Duplicate key", "Table overflow"};

int getInt(int *input);
char *getStr(void);
int dialog(const char *messages, int N);
int hash(char *key);
void createTable(Table *table);
void showTable(Table *table);
int addItem(Table *table);
int insert(Table *table, char *key, char *data);

int (*functions[])(Table *) = {NULL, addItem, NULL, NULL, NULL};


int main(){
    Table table;
    int rc;

    createTable(&table);
    showTable(&table);

    addItem(&table);
    showTable(&table);
    

    while(rc = dialog(messages, size_messages))
        if(!functions[rc](&table))
            break;
    
    printf("Thats's all. Bye!\n");

    // delTable(&table);

    return 0;
}

// вернет 0, если введено что-то плохое
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

// вернет указатель на новую строку
char *getStr(void){
    char *ptr = (char *)malloc(1);
    char buf[81];
    int check, length = 0;
    *ptr = '\0';

    do{
        check = scanf("%80[^\n]", buf);
        
        if(check < 0){
            free(ptr);
            ptr = NULL;
            continue;
        }
        if(check == 0){
            scanf("%*c");
        }else{
            length += strlen(buf);
            ptr = (char *) realloc(ptr, length + 1);
            strcat(ptr, buf);
        }
    }while(check > 0);

    return ptr;
}

// выводит массив сообщений и получает выбор пользователя
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

// вернет число - hash
int hash(char *key){
    int slot = 0;
    int i = 0;
    int key_length = strlen(key);

    for( ; i < key_length; i++){
        slot = slot * 37 + key[i];
    }

    slot = slot % TABLE_SIZE;

    return slot;
}

void createTable(Table *table){
    for(int i = 0; i < TABLE_SIZE; i++){
        table -> hashtable[i] = NULL;
    }
    table -> size = TABLE_SIZE;
    
    /*
    while(puts("Enter KEY and DATA"), key = getStr(), data = getStr()){
        slot = hash(key);
        item = (Item *)malloc(sizeof(Item) * 1);
        table -> hashtable[slot] = item;
        item -> key = key;
        item -> data = data;
    }
    puts("Your table has been created.");
    */
}

void showTable(Table *table){
    for(int i = 0; i < table -> size; i++){
        Item *point = table -> hashtable[i];

        if(!point){
            printf("\t%i\t---\n", i);
        }else{
            printf("\t%i\t%s\n", i, point -> data);
        }
    }
}

int addItem(Table *table){
    int rc;
    char *key = NULL;
    char *data = NULL;

    printf("Enter key: --> ");
    key = getStr();
    if(key == NULL)
        return 0;

    printf("Enter data: --> ");
    data = getStr();
    if(data == NULL)
        return 0;

    rc = insert(table, key, data);
    free(key);
    free(data);

    printf("%s: %s\n", error_message[rc], key);

    return 1;
}

int insert(Table *table, char *key, char *data){
    int slot = hash(key);

    Item *point = table -> hashtable[slot];

    if(point == NULL){
        point = (Item *)malloc(sizeof(Item) * 1);
        table -> hashtable[slot] = point;
        
        point -> key = (char *)malloc(strlen(key) + 1);
        point -> data = (char *)malloc(strlen(key) + 1);

        strcpy(point -> key, key);
        strcpy(point -> data, data);

        return 0;
    }

    return 0;
}