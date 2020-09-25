#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct Item{
    int key1, key2;
    char *data;
    int version1, version2; 
    int slot1, slot2;
    struct Item *next;
}Item;

typedef struct Table{
    int size;
    Item *deleted;
    Item **space1;
    Item **space2;
}Table;

const char *messages[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);
const char *error_message[] = {"OK", "Key not found", "Table overflow", "The table is empty", "Error"};

char *getStr(void);
int getInt(int *);
int dialog(const char *[], int);
int hash(int );
int insert(Table *, int, int, char *);
int find(Table *, int, int, Item **);
int delete(Table *, int , int, int);
Table *createTable(void);
void clearTable(Table *);
int addItem(Table *),
    findItem(Table *),
    delItem(Table *),
    showTable(Table *);

int (*functions[])(Table *) = {NULL, addItem, findItem, delItem, showTable};

int main(){
    Table *hashtable;
    int rc;

    hashtable = createTable();

    while((rc = dialog(messages, size_messages)))
        if(!functions[rc](hashtable))
            break;
    
    printf("Thats's all. Bye!\n");

    clearTable(hashtable);

    return 0;
}

// вернет 0, если введено что-то плохое
int getInt(int *input){
    int check;
    do{
        check = scanf("%d", input);
        if(check == 0){
            printf("%s\n","Error!");
            scanf("%*[^\n]");
        }
    }while(check == 0);

    return (check < 0) ? 0:1;
}

// вернет указатель на созданную строку
char *getStr(void){
    fflush(stdin);
    char *ptr = (char *)malloc(1);
    char buf[81];
    int check, length = 0;
    *ptr = '\0';

    // buf[strcspn(buf, "\n")] = 0;

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
            ptr = (char *)realloc(ptr, length + 1);
            strcat(ptr, buf);
        }
    }while(check > 0);

    return ptr;
}

int dialog(const char *messages[], int N){
    char *error_message = "";
    int rc, i;
    
    do{
        puts(error_message);
        error_message = "You are wrong. Repeat, please.";
        for(i = 0; i < N; i++) 
            puts(messages[i]);
        printf("\nMake your choice --> ");

        if(!getInt(&rc))
            return 0;
    }while (rc < 0 || rc >= N);

    return rc;
}

/*
int hash(char *key){
    int slot = 0;

    for(int i = 0; i < strlen(key); i++){
        slot = slot * 37 + key[i];
    }

    slot = slot % TABLE_SIZE;

    return slot;
}
*/

int hash(int key){
    return key % TABLE_SIZE;
}

int insert(Table *hashtable, int key1, int key2, char *data){

    int slot1, slot2, count, slotx;
    Item *point1, *point2, *point, *deleted;
    
    deleted = hashtable -> deleted;
    count = 0;

    slot1 = hash(key1);
    slot2 = hash(key2);
    point1 = hashtable -> space1[slot1];
    point2 = hashtable -> space2[slot2];

    point = (Item *)malloc(sizeof(Item) * 1);
    hashtable -> space2[slot2] = point; // сразу записываем адресс нового в ячейку
    point -> slot2 = slot2; // Потому что space2 не перемешивается
    point -> key1 = key1;
    point -> key2 = key2;
    point -> data = (char *)malloc(strlen(data) + 1);
    strcpy(point -> data, data);
    point -> version1 = 0;
    point -> version2 = 0;

    
    if(!point1 || (point1 == deleted)){
        hashtable -> space1[slot1] = point;
        point -> slot1 = slot1;
    }else{
        while(hashtable -> space1[slot1]){
            if(hashtable -> space1[slot1] == deleted){
                hashtable -> space1[slot1] = point;
                point -> slot1 = slot1;

                slotx = (slot1 + 1) % TABLE_SIZE;
                while(hashtable -> space1[slotx] && (count < hashtable -> size)){
                    if((hashtable -> space1[slotx]) == deleted){
                        slotx = (slotx + 1) % TABLE_SIZE;
                        continue;
                    }
                    if((hashtable -> space1[slotx] -> key1) == key1)
                        point -> version1 += 1;
                    count ++;
                    slotx = (slotx + 1) % TABLE_SIZE;
                }
                break; // тут возникает (ПРОБЛЕМА!!!)
            }
            if((hashtable -> space1[slot1]) -> key1 == key1)
                point -> version1 += 1;
            count ++;
            slot1 = (slot1 + 1) % TABLE_SIZE;
        }
        if(!hashtable -> space1[slot1]){
            hashtable -> space1[slot1] = point;
            point -> slot1 = slot1;
        }
    }

    if(!point2){ // Для space2 мне не важна проверка удаленных элементов, потому что
                 // там все списком.
        point -> next = NULL;
    }else{
        point -> next = point2;
        while(point2){
            if(point2 -> key2 == key2)
                point -> version2 += 1;      
                // !!!!!!!!!!!!!!!!!    
            point2 = point2 -> next;
        }
    }

    hashtable -> size += 1;
    return 0;
}

int find(Table *hashtable, int key, int space, Item **list){
    if(hashtable -> size == 0)
        return 3; // The table is empty

    int slot;
    Item *point, *previous, *copy, *deleted;

    deleted = hashtable -> deleted;

    slot = hash(key);

    if(!space){ // поиск в space1 (ВСЕ ПРОТЕСТИРОВАЛ) +++
        point = hashtable -> space1[slot];
        if(!point)
            return 1; // Key not found
        
        int count = 0;

        while(hashtable -> space1[slot] && (count < hashtable -> size)){

            if(point == deleted){
                slot = (slot + 1) % TABLE_SIZE;
                point = hashtable -> space1[slot];
                continue;
            }
            
            count ++;
            point = hashtable -> space1[slot];
            slot = (slot + 1) % TABLE_SIZE;

            if(point -> key1 == key){
                copy = (Item *)malloc(sizeof(Item) * 1);
                *copy = *point;
                if(!*list){
                    *list = copy;
                    previous = copy; // Запомнили
                    previous -> next = NULL;
                    // count ++;
                    continue;
                }
                // count ++;
                previous -> next = copy;
                previous = copy;
            }
        }


        if(!*list)
            return 1; // Key not found

        previous -> next = NULL;


    }else{ // поиск в space2 
        point = hashtable -> space2[slot];
        
        while(point && (point -> key2 != key))
            point = point -> next;
        
        if(!point)
            return 1; // Key not found

        copy = (Item *)malloc(sizeof(Item) * 1);
        *copy = *point;
        copy -> next = NULL;
        previous = copy;

        point = point -> next;   

        while(point){
            if(point -> key2 == key){
                copy = (Item *)malloc(sizeof(Item) * 1);
                *copy = *point;
                copy -> next = previous;
                previous = copy;
            }
            point = point -> next;
        }
        *list = copy;

        if(!*list)
            return 1; // Key not found
    }

    return 0;
}

int delete(Table *hashtable, int key, int space, int choise){
    if(hashtable -> size == 0)
        return 3; // The table is empty
    
    const char *error = "";
    int version, rc, position; //slot1, slot2;
    Item *head, *tmp_clean, *point, *pointx, *previous, *deleted;

    head = NULL;
    deleted = hashtable -> deleted;

    if(choise){ //  удаление конкретной версий
        do{
            printf("%s\n", error);
            printf("\nEnter version number --> ");
            error = "You are wrong repeat, please!";
            if(!getInt(&version)) 
                return 4; // Error Программа не завершится. Проблема.
        }while(version < 0 || version > hashtable -> size);

        if(!space){// space1
            rc = find(hashtable, key, space, &head);

            if(!rc){ // Поиск OK
                point = head;

                while(point && (point -> version1 != version))
                    point = point -> next;

                (hashtable -> space1[point -> slot1]) = deleted;
                
                position = point -> slot2;
                point = hashtable -> space2[position];

                if((point -> key1 == key) && (point -> version1 == version)){
                    hashtable -> space2[position] = point -> next;
                    return 0; // OK
                }

                previous = point;

                while(point){
                    if((point -> key1 == key) && (point -> version1 == version)){
                        previous -> next = point -> next;
                        break;
                    }
                    previous = point;
                    point = point -> next;
                }

                // Удаление списка копий
                point = head;
                while(point){
                    tmp_clean = point;
                    point = point -> next;
                    free(tmp_clean);
                }

                hashtable -> size -= 1;
            }
        }else{ // space2
            position = hash(key);
            point = hashtable -> space2[position];

            if(!point)
                return 1; // Key not found

            if((point -> key2 == key) && (point -> version2 == version)){
                    hashtable -> space2[position] = point -> next;
                    (hashtable -> space1[point -> slot1]) = deleted;
                    return 0; // OK
            }
            
            previous = point;

            while(point){
                if((point -> key2 == key) && (point -> version2 == version)){
                    previous -> next = point -> next;
                    (hashtable -> space1[point -> slot1]) = deleted;
                    break;
                }
                previous = point;
                point = point -> next;
            }

            return 1; // Key not found
        }

    }else{ // удаление всех версий
        if(!space){ //space1
            rc = find(hashtable, key, space, &head);

            if(!rc){ // Поиск OK
                point = head;

                while(point){
                    (hashtable -> space1[point -> slot1]) = deleted;

                    // position = point -> slot2;
                    // version = point -> version1;
                    pointx = hashtable -> space2[point -> slot2];

                    if((pointx -> key1 == key) && (pointx -> version1 == point -> version1)){
                        hashtable -> space2[point -> slot2] = pointx -> next;
                        point = point -> next;
                        continue;
                    }

                    previous = pointx;

                    while(pointx){
                        if((pointx -> key1 == key) && (pointx -> version1 == point -> version1)){
                            previous -> next = pointx -> next;
                            break;
                        }
                        previous = pointx;
                        pointx = pointx -> next;
                    }

                    hashtable -> size -= 1;

                    point = point -> next;
                }


                // Удаление списка копий
                point = head;
                while(point){
                    tmp_clean = point;
                    point = point -> next;
                    free(tmp_clean);
                }
            }
        }else{ //space2
               // мне тут вообще не нужен поиск
            
            position = hash(key);
            point = hashtable -> space2[position];

            if(!point)
                return 1; // Key not found

            while(point && (point -> key2 == key)){
                hashtable -> space1[point -> slot1] = deleted;
                point = point -> next;
            }

            hashtable -> space2[position] = point;

            
            if(!point)
                return 0; // OK 

            previous = point;
            point = point -> next;

            while(point){
                if(point -> key2 != key){
                    previous -> next = point;
                    previous = point;
                }else 
                    hashtable -> space1[point -> slot1] = deleted;
                point = point -> next;
            }
            previous -> next = NULL;

            return 0; // OK // Вернет OK если введен ключ, которого нет в таблице (ПРОБЛЕМА)
        }
    }

    return rc;
}

int addItem(Table *hashtable){
    int key1, key2, rc;
    char *data;
    const char *error = "";

    if(hashtable -> size == TABLE_SIZE){
        printf("%s\n", error_message[2]); // Table overflow

        return 1;
    }

    printf("\nEnter first key --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key1))
            return 0;
    }while (key1 < -10000 || key1 >= 10000);
    
    error = "";

    printf("Enter second key --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key2))
            return 0;
    }while (key2 < -10000 || key2 >= 10000);
    
    printf("Enter data --> ");
    data = getStr();
    if(data == NULL)
        return 0;

    rc = insert(hashtable, key1, key2, data);
    printf("%s: %d, %d\n", error_message[rc], key1, key2);
    free(data);

    return 1;
}

int findItem(Table *hashtable){
    int key;
    const char *error = "";
    int rc, space;
    Item *head = NULL; // должен поменяться на указатель на начало списка
    Item *point = NULL;
    Item *tmp_clean = NULL;

    printf("\n-------------------Search-------------------|\n");
    printf("Key you want to find --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key))
            return 0;
    }while (key < -10000 || key >= 10000);

    error = "";

    do{
        printf("%s\n", error);
        printf("0. Key from the space1.\n1. Key from the space2.\nMake a choice --> ");
        error = "\nYou are wrong repeat, please!\n";
        if(!getInt(&space)) 
            return 0;
    }while(!(space == 0 || space == 1));

    rc = find(hashtable, key, space, &head);
    printf("\n%s: %d\n", error_message[rc], key);

    if(!rc){
        // вывод списка копий
        point = head;
        printf("%s\n ", "-------------------Your list-------------------|");
        while(point){
            printf("\t-\t%d(%d) %d(%d)\t%s\t[%d] [%d]\t\n", 
                    point -> key1, point -> version1, point -> key2, point -> version2,
                    point -> data, point -> slot1, point -> slot2);
            point = point -> next;
        }

        // удаление списка копий
        point = head;
        while(point){
            tmp_clean = point;
            point = point -> next;
            free(tmp_clean);
        }
    }

    return 1;
}

int delItem(Table *hashtable){
    const char *error = "";
    int rc, space, choise;
    int key;
    
    printf("\n-------------------Delete-------------------|\n");
    printf("Enter key --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key))
            return 0;
    }while (key < -10000 || key >= 10000);

    do{
        printf("%s\n", error);
        printf("\n0. Key from the space1.\n1. Key from the space2.\nMake a choice --> ");
        error = "You are wrong repeat, please!";
        if(!getInt(&space)) 
            return 0;
    }while(!(space == 0 || space == 1));

    error = "";

    do{
        printf("%s\n", error);
        printf("\n0. I want to delete all versions.\n1. I want to delete a specific version\nMake a choise --> ");
        error = "You are wrong repeat, please!";
        if(!getInt(&choise)) 
            return 0;
    }while(!(choise == 0 || choise == 1));

    rc = delete(hashtable, key, space, choise);
    printf("\n%s: %d\n", error_message[rc], key);

    return 1;  
}

Table *createTable(void){
    Table *hashtable = (Table *)malloc(sizeof(Table) * 1);
    if(hashtable == NULL)
        return NULL;

    hashtable -> deleted = (Item *)malloc(sizeof(Item) * 1);
    if(hashtable -> deleted == NULL)
        return NULL;

    hashtable -> space1 = (Item **)malloc(sizeof(Item *) * TABLE_SIZE);
    if(hashtable -> space1 == NULL)
        return NULL;
    hashtable -> space2 = (Item **)malloc(sizeof(Item *) * TABLE_SIZE);
    if(hashtable -> space2 == NULL)
        return NULL;

    for(int i = 0; i < TABLE_SIZE; i++){
        hashtable -> space1[i] = NULL;
        hashtable -> space2[i] = NULL;
    }

    hashtable -> size = 0; 

    return hashtable;
}

int showTable(Table *hashtable){
    Item *point1, *point2, *deleted;

    deleted = hashtable -> deleted;

    printf("\n-------------------KEY SPACE ONE-------------------|-------------------KEY SPACE TWO-------------------\n");
    for(int i = 0; i < TABLE_SIZE; i++){
        point1 = hashtable -> space1[i];
        point2 = hashtable -> space2[i];
           if(point1 == NULL)
                printf("\t%d\t---( ) ---( )\t---\t--- ---\t\t", i);
            else if(point1 == deleted)
                printf("\t%d\t       \tDELETED\t         \t\t", i);
            else{
                printf("\t%d\t%d(%d) %d(%d)\t%s\t[%d] [%d]\t\t", 
                        i, point1 -> key1, point1 -> version1, point1 -> key2, point1 -> version2,
                        point1 -> data, point1 -> slot1, point1 -> slot2);
            }

            if(point2 == NULL)
                printf("\t%d\t---( ) ---( )\t---\t--- ---\t\n", i);
            else{
                const char *link = "";
                for(;;){
                    printf("%s\t%d\t%d(%d) %d(%d)\t%s\t[%d] [%d]\n",
                        link, i, point2 -> key1, point2 -> version1, point2 -> key2, point2 -> version2,
                        point2 -> data, point2 -> slot1, point2 -> slot2);
                    // link = "\t \t---( ) ---( )\t---\t--- ---\t";
                    link = "\t \t             \t   \t        \t";
                    if(point2 -> next == NULL)
                        break;
                    point2 = point2-> next;
                }
            }       
    }
    printf("\n");

    return 1;
}

void clearTable(Table *hashtable){
    for(int count = 0; count < hashtable -> size; count ++){
        if(hashtable -> space1[count]){
            hashtable -> space1[count] = NULL;
            hashtable -> space2[(hashtable -> space1[count]) -> slot2] = NULL;
            free(hashtable -> space1[count]);
        }
    }
}
