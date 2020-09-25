#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct Item{
    int key1, key2;
    int version1, version2; 
    int slot1, slot2;
    int offset;
    int len;
    struct Item *next;
}Item;

typedef struct Table{
    int TABLE_SIZE;
    int size;
    Item *deleted;
    Item **space1;
    Item **space2;
    FILE *fd; // file.dat
}Table;

const char *messages[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);
const char *error_message[] = {"OK", "Key not found", "Table overflow", "The table is empty", "Error"};

void createTable(Table *, int);
int loadTable(Table *);
int dialog(const char *[], int);
void clearTable(Table *);

char *getStr(void);
int getInt(int *);
int load(Table *, char *);
int create(Table *, char *, int);
char *findInfo(Table *, int);
int save(Table *);

int hash(int , int);
int insert(Table *, int, int, char *);
int find(Table *, int, int, Item **);
int delete(Table *, int, int, int);

int addItem(Table *),
    findItem(Table *),
    delItem(Table *),
    showTable(Table *);

int (*functions[])(Table *) = {NULL, addItem, findItem, delItem, showTable};


int main(){
    Table *hashtable;
    int rc;

    hashtable = (Table *)malloc(sizeof(Table) * 1);
    if(hashtable == NULL)
        return 0;

    if(loadTable(hashtable) == 0)
        return 1;

    showTable(hashtable);

    while((rc = dialog(messages, size_messages)))
        if(!functions[rc](hashtable))
            break;

    save(hashtable);

    clearTable(hashtable);

    printf("Thats's all. Bye!\n");

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

int loadTable(Table *hashtable){
    int TABLE_SIZE;
    char *fname = NULL;

    printf("Enter file name --> ");
    fname = getStr();
    if(fname == NULL)
        return 0;

    if(load(hashtable, fname) == 0){
        printf("Enter possible table size --> ");
        if(!getInt(&TABLE_SIZE)){
            free(fname);

            return 0;
        }
        if(create(hashtable, fname, TABLE_SIZE) == 0)
            return 0;
    }
    free(fname);

    return 1;
}

int load(Table *hashtable, char *fname){
    hashtable -> fd = fopen(fname, "r+b");
    if(hashtable -> fd == NULL)
        return 0;

    int busy;
    Item *point = NULL;
    Item item = {0, 0, 0, 0, 0, 0, 0, 0, NULL};

    
    fread(&hashtable -> TABLE_SIZE, sizeof(int), 1, hashtable -> fd);
    fread(&hashtable -> size, sizeof(int), 1, hashtable -> fd);

    createTable(hashtable, hashtable -> TABLE_SIZE);

    for(int count = 0; count < hashtable -> TABLE_SIZE; count ++){
        fread(&busy, sizeof(int), 1, hashtable -> fd);
        if(!busy){
            fseek(hashtable -> fd, sizeof(int) * 8, SEEK_CUR);
            // fread(&item, sizeof(int), 8, hashtable -> fd);
            continue;
        }

        fread(&item, sizeof(int), 8, hashtable -> fd);
        
        point = (Item *)malloc(sizeof(Item) * 1);
        *point = item;

        // Может ли быть в file.dat данных быть больше чем в таблице?
        while(hashtable -> space1[point -> slot1]){
            point -> slot1 = (point -> slot1 + 1) % hashtable -> TABLE_SIZE;
        }
        hashtable -> space1[point -> slot1] = point;
        
        if(hashtable -> space2[point -> slot2])
            point -> next = hashtable -> space2[point -> slot2];
        hashtable -> space2[point -> slot2] = point;
    }

    return 1;
}

int create(Table *hashtable, char *fname, int TABLE_SIZE){
    int busy = 0;
    Item item = {0, 0, 0, 0, 0, 0, 0, 0, NULL};

    hashtable -> TABLE_SIZE = TABLE_SIZE;
    hashtable -> size = 0;
    hashtable -> fd = fopen(fname, "w+b");
    if(hashtable -> fd == NULL)
        return 0;
    
    createTable(hashtable, hashtable -> TABLE_SIZE);

    fwrite(&hashtable -> TABLE_SIZE, sizeof(int), 1, hashtable -> fd);
    fwrite(&hashtable -> size, sizeof(int), 1, hashtable -> fd);
    
    for(int count = 0; count < hashtable -> TABLE_SIZE; count ++){
        fwrite(&busy, sizeof(int), 1, hashtable -> fd);
        fseek(hashtable -> fd, sizeof(int) * 8, SEEK_CUR);
        // fwrite(&item, sizeof(int), 8, hashtable -> fd);
    }

    return 1;
}

char *findInfo(Table *hashtable, int slot){
    char *data = NULL;
    Item *point = hashtable -> space1[slot];

    data = (char *)malloc(point -> len);
    fseek(hashtable -> fd, point -> offset, SEEK_SET);
    fread(data, sizeof(char), point -> len, hashtable -> fd);

    return data;
}

int save(Table *hashtable){
    int busy = 0;
    Item item = {0, 0, 0, 0, 0, 0, 0, 0, NULL};


    fseek(hashtable -> fd, sizeof(int), SEEK_SET);
    fwrite(&hashtable -> size, sizeof(int), 1, hashtable -> fd);

    for(int count = 0; count < hashtable -> TABLE_SIZE; count ++){
        if(hashtable -> space1[count] && (hashtable -> space1[count] != hashtable -> deleted)){
            busy = 1;
            fwrite(&busy, sizeof(int), 1, hashtable -> fd);
            fwrite(hashtable -> space1[count], sizeof(int), 8, hashtable -> fd);
        }else{
            busy = 0;
            fwrite(&busy, sizeof(int), 1, hashtable -> fd);
            // fwrite(&item, sizeof(int), 8, hashtable -> fd);
            fseek(hashtable -> fd, sizeof(int) * 8, SEEK_CUR);
        }
    }

    fclose(hashtable -> fd);

    hashtable -> fd = NULL;

    return 1;
}

int hash(int key, int TABLE_SIZE){
    return key % TABLE_SIZE;
}

int insert(Table *hashtable, int key1, int key2, char *data){

    int TABLE_SIZE, slot1, slot2, count;
    Item *point1, *point2, *point, *deleted;
    
    TABLE_SIZE = hashtable -> TABLE_SIZE;
    deleted = hashtable -> deleted;
    count = 0;

    slot1 = hash(key1, TABLE_SIZE);
    slot2 = hash(key2, TABLE_SIZE);
    point1 = hashtable -> space1[slot1];
    point2 = hashtable -> space2[slot2];

    point = (Item *)malloc(sizeof(Item) * 1);
    hashtable -> space2[slot2] = point; // сразу записываем адресс нового в ячейку
    point -> slot2 = slot2; // Потому что space2 не перемешивается
    point -> key1 = key1;
    point -> key2 = key2;

    point -> len = strlen(data) + 1;
    fseek(hashtable -> fd, 0, SEEK_END);
    point -> offset = ftell(hashtable -> fd);
    fwrite(data, sizeof(char), point -> len, hashtable -> fd);

    point -> version1 = 0;
    point -> version2 = 0;


    if(!point2){ // Для space2 мне не важна проверка удаленных элементов, потому что там все списком.
        point -> next = NULL;
    }else{
        point -> next = point2;
        while(point2){
            if(point2 -> key2 == key2)
                if(point2 -> version2 >= point -> version2){
                    point -> version2 = point2 -> version2;
                    point -> version2 += 1;
                }

            point2 = point2 -> next;
        }
    }
    
    // if(!point1 || (point1 == deleted)){
    if(!point1){
        hashtable -> space1[slot1] = point;
        point -> slot1 = slot1;
    }else{
        while(hashtable -> space1[slot1]){
            if(hashtable -> space1[slot1] == deleted){
                point -> slot1 = slot1;

                slot1 = (slot1 + 1) % TABLE_SIZE;
                while(hashtable -> space1[slot1] && (count < hashtable -> size)){
                    if((hashtable -> space1[slot1]) == deleted){
                        slot1 = (slot1 + 1) % TABLE_SIZE;
                        continue;
                    }
                    if((hashtable -> space1[slot1] -> key1) == key1)
                        if((hashtable -> space1[slot1] -> version1) >= point -> version1){
                            point -> version1 = (hashtable -> space1[slot1]) -> version1;
                            point -> version1 += 1;
                        }

                    count ++;
                    slot1 = (slot1 + 1) % TABLE_SIZE;
                }
                
                hashtable -> space1[point -> slot1] = point;
                hashtable -> size += 1;

                return 0;
            }

            if((hashtable -> space1[slot1]) -> key1 == key1)
                if((hashtable -> space1[slot1] -> version1) >= point -> version1){
                    point -> version1 = (hashtable -> space1[slot1]) -> version1;
                    point -> version1 += 1;
                }
            
            slot1 = (slot1 + 1) % TABLE_SIZE;
        }

        hashtable -> space1[slot1] = point;
        point -> slot1 = slot1;

    }

    hashtable -> size += 1;

    return 0;
}

int find(Table *hashtable, int key, int space, Item **list){
    if(hashtable -> size == 0)
        return 3; // The table is empty

    int slot, TABLE_SIZE;
    Item *point, *previous, *copy, *deleted;

    TABLE_SIZE = hashtable -> TABLE_SIZE;
    deleted = hashtable -> deleted;

    slot = hash(key, TABLE_SIZE);

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
    int TABLE_SIZE, version, rc, position;
    Item *head, *tmp_clean, *point, *pointx, *previous, *deleted;

    TABLE_SIZE = hashtable -> TABLE_SIZE;
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

                if(!point)
                    return 1; // Key not found

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
            position = hash(key, TABLE_SIZE);
            point = hashtable -> space2[position];

            if(!point)
                return 1; // Key not found

            if((point -> key2 == key) && (point -> version2 == version)){
                    hashtable -> space2[position] = point -> next;
                    (hashtable -> space1[point -> slot1]) = deleted;
                    hashtable -> size -= 1;
                    return 0; // OK
            }
            
            previous = point;

            while(point){
                if((point -> key2 == key) && (point -> version2 == version)){
                    previous -> next = point -> next;
                    (hashtable -> space1[point -> slot1]) = deleted;
                    hashtable -> size -= 1;
                    return 0; // ОК
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

                    pointx = hashtable -> space2[point -> slot2];

                    if((pointx -> key1 == key) && (pointx -> version1 == point -> version1)){
                        hashtable -> space2[point -> slot2] = pointx -> next;
                        point = point -> next;
                        hashtable -> size -= 1;
                        continue;
                    }

                    previous = pointx;

                    while(pointx){
                        if((pointx -> key1 == key) && (pointx -> version1 == point -> version1)){
                            previous -> next = pointx -> next;
                            hashtable -> size -= 1;
                            break;
                        }
                        previous = pointx;
                        pointx = pointx -> next;
                    }

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
            
            position = hash(key, TABLE_SIZE);
            point = hashtable -> space2[position];

            if(!point)
                return 1; // Key not found

            while(point && (point -> key2 == key)){
                hashtable -> space1[point -> slot1] = deleted;
                point = point -> next;
                hashtable -> size -= 1;
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
                }else{
                    hashtable -> space1[point -> slot1] = deleted;
                    hashtable -> size -= 1;
                }
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

    if(hashtable -> size == hashtable -> TABLE_SIZE){
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
                    findInfo(hashtable, point -> slot1), point -> slot1, point -> slot2);
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

void createTable(Table *hashtable, int TABLE_SIZE){

    hashtable -> deleted = (Item *)malloc(sizeof(Item) * 1);
    // if(hashtable -> deleted == NULL)
        // return NULL;

    hashtable -> space1 = (Item **)malloc(sizeof(Item *) * TABLE_SIZE);
    // if(hashtable -> space1 == NULL)
        // return NULL;
    hashtable -> space2 = (Item **)malloc(sizeof(Item *) * TABLE_SIZE);
    // if(hashtable -> space2 == NULL)
        // return NULL;

    for(int i = 0; i < TABLE_SIZE; i++){
        hashtable -> space1[i] = NULL;
        hashtable -> space2[i] = NULL;
    }
}

int showTable(Table *hashtable){
    Item *point1, *point2, *deleted;

    deleted = hashtable -> deleted;
    printf("%d\n", hashtable -> size);
    printf("\n-------------------KEY SPACE ONE-------------------|-------------------KEY SPACE TWO-------------------\n");
    for(int i = 0; i < hashtable -> TABLE_SIZE; i++){
        point1 = hashtable -> space1[i];
        point2 = hashtable -> space2[i];
           if(point1 == NULL)
                printf("\t%d\t---( ) ---( )\t---\t--- ---\t\t", i);
            else if(point1 == deleted)
                printf("\t%d\t       \tDELETED\t         \t\t", i);
            else{
                printf("\t%d\t%d(%d) %d(%d)\t%s\t[%d] [%d]\t\t", 
                        i, point1 -> key1, point1 -> version1, point1 -> key2, point1 -> version2,
                        findInfo(hashtable, i), point1 -> slot1, point1 -> slot2);
            }

            if(point2 == NULL)
                printf("\t%d\t---( ) ---( )\t---\t--- ---\t\n", i);
            else{
                const char *link = "";
                for(;;){
                    printf("%s\t%d\t%d(%d) %d(%d)\t%s\t[%d] [%d]\n",
                        link, i, point2 -> key1, point2 -> version1, point2 -> key2, point2 -> version2,
                        findInfo(hashtable, point2 -> slot1), point2 -> slot1, point2 -> slot2);
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
        if(hashtable -> space1[count])
            free(hashtable -> space1[count]);
    }
    free(hashtable -> space1);
    free(hashtable -> space2); 
    free(hashtable);
}
