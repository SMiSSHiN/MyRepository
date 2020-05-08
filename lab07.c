#include <stdio.h>
#include <malloc.h>
#include <string.h>

//<<<<<<<<<<<<<<<<<<<<<<<<<----->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//      2)Организовать дерево
//+++// Граничное значение? Массив указаелей? Основные ключи текущего уровня?
// <<<<<<<<<<<<<<<<<<<<<<<<<Вставка>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//+++// Вставка в упорядоченный массив
//+++// Создание нового узла
// 
// <<<<<<<<<<<<<<<<<<<<<<<<<Удаление>>>>>>>>>>>>>>>>>>>>>>>>
//+++// Поиск
////Поиск по упорядоченному массиву
// Поиск соседних
// <<<<<<<<<<<<<<<<<<Файлы>>>>>>>>>
// Сохранение в файл
// Чтение из файла
//      4)Таймирование
// ---
// ---
//
//


#define N 3
#define k 2
#define SPACE 20

typedef struct Item{
    int key[k];
    int offset;
    int len;
}Item;

typedef struct Node{
    int border; // Что придумать с border?
    Item *elements[N];
    struct Node *left, *right;
}Node;



const char *messages[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show", "5. Timing"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);
const char *error_message[] = {"OK", "Duplicate key", "Key not found", "The table is empty"};


char *getStr(void);
int getInt(int *);

int median(Item *elements[], unsigned int);
int areKeysSame(int key[], int find_key[]);
Node *newNode(Node *);
void showArray(Item *elements[]);
void showNode(Node *, int);

Node *insert(Node *, int key[], unsigned int);
Node *find(Node *, int key[], unsigned depth);
void delTree(Node **);
// int showTable(Node *root);

int dialog(const char *[], int);
int addNode(Node *),
    findNode(Node *),
    delItem(Node *),
    showTree(Node *),
    timingTree(Node *);


// int (*functions[])(Table *) = {NULL, addItem, findItem, delItem, showTable, timingTree};
int (*functions[])(Node *) = {NULL, addNode, findNode, NULL, showTree, NULL};


int main(){
    Node *root = NULL;
    int rc;

    root = newNode(root);

    int keys[][k] = {{3,6}, {17,15}, {12, 15}, {1, 1}, {55, 1}, {65, 6}, {5, 6}, {11, 3}, {14, 7}, {0, 7}, {13, 3}, {17, 4}};

    for(int count = 0; count < 12; count ++)
        root = insert(root, keys[count], 0);


    while((rc = dialog(messages, size_messages)))
        if(!functions[rc](root))
            break;

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

int median(Item *elements[], unsigned cd){
    int sum = 0;

    for(int count = 0; count < N; count ++)
        sum += elements[count] -> key[cd];

    return (int )(sum/N);
}

int areKeysSame(int key[], int find_key[]){
    for(int i = 0; i < k; i++)
        if(key[i] != find_key[i])
            return 0;

    return 1;
}

Node *newNode(Node *node){
    node = (Node *)malloc(sizeof(Node) * 1);

    node -> border = 0;

    node -> left = NULL;
    node -> right = NULL;

    for(int count = 0; count < N; count ++)
        node -> elements[count] = NULL;

    return node;
}

void showArray(Item *elements[]){
    int count = 0;

    printf("[(%d, %d)", elements[count] -> key[0], elements[count] -> key[1]);
    count ++;

    while((count < N) && (elements[count])){
        printf(" (%d, %d)", elements[count] -> key[0], elements[count] -> key[1]);
        count ++;
    }
    printf("]\n");
}

int dialog(const char *messages[], int n){
    char *ermsg = "";
    int rc, i;
    
    do{
        puts(ermsg);
        ermsg = "You are wrong. Repeat, please.";

        for(i = 0; i < n; i++) 
            puts(messages[i]);
        printf("\nMake your choice --> ");

        if(!getInt(&rc))
            return 0;
    }while (rc < 0 || rc >= n);

    return rc;
}

int addNode(Node *root){
    int key[k];
    char *data;

    int rc = 0;
    // Node *node = NULL;

    const char *error = "";

 /*
    // Поиск по дереву
    node = find(*root, key);

    // Duplicate key
    if(!node){
        rc = 1;
        printf("%s: %d, %d\n", error_message[rc], key1, key2);

        return 1;
    }
 */

    printf("\nEnter first key --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key[0]))
            return 0;
    }while (key[0] < -10000 || key[0] >= 10000);
    
    error = "";

    printf("Enter second key --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key[1]))
            return 0;
    }while (key[1] < -10000 || key[1] >= 10000);
    
    printf("Enter data --> ");
    data = getStr();
    if(data == NULL)
        return 0;

    root = insert(root, key, 0);
    printf("%s: %d, %d\n", error_message[rc], key[0], key[1]);
    free(data);

    return 1;
}

int findNode(Node *root){
    int key[k];
    int rc = 0;

    Node *node = NULL;

    const char *error = "";

    printf("\n-------------------Search-------------------|\n");
    printf("Key1 you want to find --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key[0]))
            return 0;
    }while (key[0] < -10000 || key[0] >= 10000);

    error = "";

    printf("Key2 you want to find --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&key[1]))
            return 0;
    }while (key[1] < -10000 || key[1] >= 10000);

    node = find(root, key, 0);
    if(node == NULL)
        rc = 2;
    printf("\n%s: %d, %d\n", error_message[rc], key[0], key[1]);

    return 1;
}

Node *insert(Node *root, int key[], unsigned depth){

    int count = 0;
    Item *item = NULL;

    unsigned int cd = depth % k;

    // Поиск

    // Создаем новый node 
    if(root == NULL)
        root = newNode(root);

    // Проходимся по массиву из указателей на item
    while((count < N) && (root -> elements[count]))
        count ++;
    
    if(count <= N -1){
        count = N;

        while(count >= 0 && (root -> elements[count] == NULL))
            count --;

        if(count < 0){
            root -> elements[0] = (Item *)malloc(sizeof(Item) * 1);

            item =  root -> elements[0];

            for(int i = 0; i < k; i++)
                item -> key[i] = key[i];
        }

        while(count >= 0 && ((root -> elements[count]) -> key[cd] > key[cd])){
            root -> elements[count + 1] = root -> elements[count];
            count -= 1;
        }

        root -> elements[count + 1] = (Item *)malloc(sizeof(Item) * 1);

        item =  root -> elements[count + 1];

        for(int i = 0; i < k; i++)
            item -> key[i] = key[i];
    }

 /*
    if(count == N-1)
        root -> border = median(root -> elements, cd);
 */ 
    cd = (cd + 1) % k;

    if(count == N){
        root -> border = median(root -> elements, cd);

        if(key[cd] <= (root -> border))
            root -> left = insert(root -> left, key, depth + 1);
        else
            root -> right = insert(root -> right, key, depth + 1);
    }

    return root;
}

Node *find(Node *root, int key[], unsigned depth){
    int count = 0;

    if(root == NULL)
        return NULL;

    // Просматривается весь массив, но должен быть поиск по упорядоченному массиву
    while((count < N) && (root -> elements[count])){
        if(areKeysSame((root -> elements[count]) -> key, key))
            return root;
        count ++;
    }

    unsigned cd = depth % k;

    if(key[cd] <= root -> border)
        return find(root -> left, key, depth + 1);
    
    return find(root -> right, key, depth + 1);
}

/*
int showTable(Node *root){
    if(root == NULL){
        printf("---\t[...]\n");

        return 1;
    }

    printf("%d ", root -> border);
    showArray(root -> elements);

    showTable(root -> left);
    showTable(root -> right);
}
*/

int showTree(Node *root){
    showNode(root, 0);

    return 1;
}

void showNode(Node *root, int space){
    if(root == NULL)
        return;
    
    space += SPACE;

    showNode(root -> right, space);

    printf("\n");
    for(int i = SPACE; i < space; i++)
        printf(" ");

    printf("%d", root -> border);
    showArray(root -> elements);

    showNode(root -> left, space);
}
