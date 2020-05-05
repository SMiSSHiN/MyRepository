#include <stdio.h>
#include <malloc.h>
#include <string.h>

//<<<<<<<<<<<<<<<<<<<<<<<<<----->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//      2)Организовать дерево
//+++// Граничное значение? Массив указаелей? Основные ключи текущего уровня?
// <<<<<<<<<<<<<<<<<<<<<<<<<Вставка>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Вставка в упорядоченный массив
// Создание нового узла
// 
// <<<<<<<<<<<<<<<<<<<<<<<<<Удаление>>>>>>>>>>>>>>>>>>>>>>>>
// Поиск
// Поиск соседних
//      3)Таймирование
// ---
// ---
//
//


#define N 3
#define k 2

typedef struct Item{
    int key[k];
    int offset;
    int len;
}Item;

typedef struct Node{
    int border;
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

Node *insert(Node *, int key[], unsigned int);
Node *find(Node *, int key[], unsigned depth);
void delTree(Node **);


int dialog(const char *[], int);
int addNode(Node *),
    findNode(Node *),
    delItem(Node *),
    showTable(Node *),
    timingTree(Node *);


// int (*functions[])(Table *) = {NULL, addItem, findItem, delItem, showTable, timingTree};
int (*functions[])(Node *) = {NULL, addNode, findNode, NULL, showTable, NULL};


int main(){
    Node *root = NULL;
    int rc;

    root = newNode(root);

    int keys[][k] = {{3,6}, {17,15}, {12, 15}, {1, 1}};

    for(int count = 0; count < 4; count ++)
        root = insert(root, keys[count], 0);

    showTable(root);

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

    node -> left = NULL;
    node -> right = NULL;

    for(int count = 0; count < N; count ++)
        node -> elements[count] = NULL;

    return node;
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
    printf("\n%s: %d, %d\n", error_message[rc], key[0], key[1]);

    return 1;
}

Node *insert(Node *root, int key[], unsigned depth){

    int count = 0;
    Item *item = NULL;

    unsigned int cd = depth % k;

    // Создаем новый node 
    if(root == NULL)
        root = newNode(root);

    // Проходимся по массиву из item
    while((count < N) && (root -> elements[count]))
        count ++;
    
    if(count <= N -1){
        // Упорядоченная вставка
        // 
        //
        root -> elements[count] = (Item *)malloc(sizeof(Item) * 1);

        item =  root -> elements[count];

        for(int i = 0; i < k; i++)
            item -> key[i] = key[i];
    }

 /*
    if(count == N-1)
        root -> border = median(root -> elements, cd);
 */

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
    }

    unsigned cd = depth % k;

    if(key[cd] <= root -> border)
        return find(root -> left, key, depth + 1);
    
    return find(root -> right, key, depth + 1);
}

int showTable(Node *root){
    Node *node = root;
    Item *item = NULL;

    int count = 0;

    if(node == NULL)
        printf("---\t[...]\n");
    else{
        item = node -> elements[count];

        printf("%d [", node -> border);
        printf("(%d, %d),", item -> key[0], item -> key[1]);

        count ++;

        while((count < N) && (node -> elements[count])){
            item = node -> elements[count];
            printf(" (%d, %d)", item -> key[0], item -> key[1]);
            count ++;
        }
        printf("]\n");
    }

    return 1;
}
