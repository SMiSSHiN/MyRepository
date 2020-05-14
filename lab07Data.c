#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//<<<<<<<<<<<<<<<<<<<<<<<<<----->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//      2)Организовать дерево
//+++// Граничное значение? Массив указаелей? Основные ключи текущего уровня?
// <<<<<<<<<<<<<<<<<<<<<<<<<Вставка>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//+++// Вставка в упорядоченный массив
//+++// Вставка с данными
//+++// Создание нового узла
//+++// Отказ при вставке дубликатов
// <<<<<<<<<<<<<<<<<<<<<<<<<Удаление>>>>>>>>>>>>>>>>>>>>>>>>
//+++// Поиск
//+++//Поиск по упорядоченному массиву
//<<<<<<<<<<<<<<<<<<<Поиск соседних>>>>>>>>>>>>>>>>>>>>>>>>
//+++// Просто написать проход по всему дереву..
// <<<<<<<<<<<<<<<<<Delete>>>>>>>>>>>>>>>>
//+++// Удаление из упорядоченного массива 
// <<<<<<<<<<<<<<<<<<Файлы>>>>>>>>>>>>>>>
// // Чтение из файла
//      4)Таймирование
//+++// Время поиска
//---// Поиск элементов в Таймировании
//
//


#define N 3
#define k 2
#define SPACE 20

typedef struct Item{
    int key[k];
    char *data;
}Item;

typedef struct Node{
    int border;
    Item *elements[N];
    struct Node *left, 
                *right;
}Node;



const char *messages[] = {"0. Quit", "1. Add", "2. Find", "3. Find neighbor", "4. Delete", "5. Show", "6. Timing"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);
const char *error_message[] = {"OK", "Duplicate key", "Key not found", "The table is empty"};


char *getStr(void);
int getInt(int *);

int median(Item *elements[], unsigned int);
int areKeysSame(int key[], int find_key[]);
int binarySearch(Item *elements[], int key[], unsigned depth);
int binarySearchEndArray(Item *elements[]);
Node *newNode(Node *);
void showArray(Item *elements[]);
void showNode(Node *, int, unsigned depth);
void deleteFromArray(Item *elements[], int );
float calculateDistance(Item *, int key[]);

Node *insertWithData(Node *, int key[], char *, unsigned int);
Node *insert(Node *, int key[], unsigned int);
Item *find(Node *, int key[], unsigned depth);
void findNearest(Node *, int key[], unsigned depth, float *, Item **);
Item *delete(Node *, int key[], unsigned depth);
void preorderTraversal(Node *);
void delTree(Node *);

int loadTree(Node *);

int dialog(const char *[], int);
int addNode(Node *),
    findNode(Node *),
    findNeighbor(Node *),
    delItem(Node *),
    showTree(Node *),
    timingTree(Node *);


int (*functions[])(Node *) = {NULL, addNode, findNode, findNeighbor, delItem, showTree, timingTree};


int main(){
    Node *root = NULL;
    int rc;

    // Корневой узел
    root = newNode(root);

    // if(loadTree(root) == 0)
    //     return 1;
    


    // int keys[][k] = {{1,1}, {3,6}, {8, 4}, {7, 8}, {10, 4}, {6, 6}, {8, 2}, {6, 1}, {2, 5}, {4, 1}, {7, 6}, {2, 9}, {5, 4}, {4, 4}, {3, 3}};

    // for(int count = 0; count < 15; count ++)
    //     insert(root, keys[count], 0);

    while((rc = dialog(messages, size_messages)))
        if(!functions[rc](root))
            break;

    delTree(root);
    
    root -> left = NULL;
    root -> right = NULL;

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

// Должна считаться для неполных массивов
int median(Item *elements[], unsigned cd){
    int sum = 0;

    for(int count = 0; count < N && elements[count]; count ++)
        sum += elements[count] -> key[cd];

    return (int )(sum/N);
}

int areKeysSame(int key[], int find_key[]){
    for(int i = 0; i < k; i++)
        if(key[i] != find_key[i])
            return 0;

    return 1;
}

// За один проход
int binarySearch(Item *elements[], int key[], unsigned depth){
    int count = 0;
    int j = 0;
    int m = N - 1;

    unsigned cd = depth % k;

    while(count <= m){
        j = (count + m)/2;

        if(elements[j] && areKeysSame(elements[j] -> key, key))
            return j;

        if(elements[j] == NULL || key[cd] < (elements[j]) -> key[cd])
            m = j - 1;
        else
            count = j + 1;
    }

    return -1;
}

int binarySearchEndArray(Item *elements[]){
    int count = 0;
    int m = N - 1;
    int j = 0;

    while(count <= m){
        j = (count + m)/2;

        // Конец N - 1
        if(j == N - 1 && elements[j])
            return j;

        if(elements[j] && !elements[j + 1])
            return j;

        if(elements[j] == NULL)
            m = j - 1;
        else
            count = j + 1;
    }

    return -1; // Массив пуст
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

    printf("[");

    if(elements[count])
        printf("(%d, %d, %s)", elements[count] -> key[0], elements[count] -> key[1], elements[count] -> data);
    else
        printf("(-, -)");
    count ++;

    while(count < N){
        if(elements[count])
            printf(" (%d, %d, %s)", elements[count] -> key[0], elements[count] -> key[1], elements[count] -> data);
        else
            printf(" (-, -)");

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
    Item *item = NULL;

    const char *error = "";

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

    item = find(root, key, 0);
    if(item != NULL){
        rc = 1;

        printf("%s: %d, %d\n", error_message[rc], key[0], key[1]);
        free(data);

        return 1;
    }

    root = insertWithData(root, key, data, 0);
    printf("%s: %d, %d\n", error_message[rc], key[0], key[1]);
    free(data);

    return 1;
}

int findNode(Node *root){
    int key[k];
    int rc = 0;

    Item *item = NULL;

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

    item = find(root, key, 0);
    if(item == NULL)
        rc = 2;

    printf("\n%s: %d, %d\n", error_message[rc], key[0], key[1]);

    return 1;
}

int findNeighbor(Node *root){
    int key[k];
    int rc = 0;

    float distanse = 1000000; // Меняется функцией findNearest
    Item *nearest_item = NULL; // Меняется функцией findNearest

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

    findNearest(root, key, 0, &distanse, &nearest_item);
    if(nearest_item == NULL){
        rc = 3; // Возможно только если дерево пустое
        printf("\n%s: %d, %d\n", error_message[rc], key[0], key[1]);

        return 1;
    }

    printf("\n%s: %d, %d\n", error_message[rc], nearest_item -> key[0], nearest_item -> key[1]);
    printf("\n%s: %d, %d\n", error_message[rc], key[0], key[1]);
        

    return 1;
}

int delItem(Node *root){
    int rc = 0;
    int key[N];

    Item *item = NULL;

    const char *error = "";
    
    printf("\n-------------------Delete-------------------|\n");
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

    item = delete(root, key, 0);
    if(item == NULL)
        rc = 2;

    printf("\n%s: %d, %d\n", error_message[rc], key[0], key[1]);

    return 1;  
}

int timingTree(Node *root){
    int n = 10, keys[10000][k], key[k], cnt = 1000000, i, m;
    clock_t first, last;
    root = NULL;

    srand(time(NULL));

    while(n-- > 0){
        for(i = 0; i < 10000; i++){
            keys[i][0] = rand() * rand();
            keys[i][1] = rand() * rand();
        }
        for(i = 0; i < cnt; ){
            key[0] = rand() * rand();
            key[1] = rand() * rand();
            if(!find(root, key, 0)){
                root = insert(root, key, 0);
                i++;
            }
        }
        m = 0;
        first = clock();
        for(i = 0; i < 10000; i++)
            if(find(root, keys[i], 0))
                m++;
        last = clock();

        printf("%d items was found\n", m);
        printf("test #%d, number of nodes = %d, time = %d\n",
                10 - n, (10 - n)*cnt, last - first);
    }

    delTree(root);
    return 1;
}

// Для создания дерева
Node *insertWithData(Node *root, int key[], char *data, unsigned int depth){
    Item *item = NULL;

    unsigned int cd = depth % k; 

    // Создаем новый node 
    if(root == NULL)
        root = newNode(root);

    int count = binarySearchEndArray(root -> elements);
    
    // Упорядоченная вставка
    if(count < N - 1 && !root -> left && !root -> right){
        while(count >= 0 && ((root -> elements[count]) -> key[cd] > key[cd])){
            root -> elements[count + 1] = root -> elements[count];
            count -= 1;
        }

        root -> elements[count + 1] = (Item *)malloc(sizeof(Item) * 1);

        item =  root -> elements[count + 1];

        for(int i = 0; i < k; i++)
            item -> key[i] = key[i];

        // вставить data
        item -> data = (char *)malloc(strlen(data) + 1);
        strcpy(item -> data, data);
    }else{
        if(!root -> left && !root -> right)
            root -> border = median(root -> elements, cd);

        if(key[cd] <= (root -> border))
            root -> left = insertWithData(root -> left, key, data, depth + 1);
        else
            root -> right = insertWithData(root -> right, key, data, depth + 1);
    }

    return root;
}

// Для таймирования
Node *insert(Node *root, int key[], unsigned depth){
    Item *item = NULL;

    unsigned int cd = depth % k; 

    // Создаем новый node 
    if(root == NULL)
        root = newNode(root);

    int count = binarySearchEndArray(root -> elements);
    
    // Упорядоченная вставка
    if(count < N - 1 && !root -> left && !root -> right){
        while(count >= 0 && ((root -> elements[count]) -> key[cd] > key[cd])){
            root -> elements[count + 1] = root -> elements[count];
            count -= 1;
        }

        root -> elements[count + 1] = (Item *)malloc(sizeof(Item) * 1);

        item =  root -> elements[count + 1];

        for(int i = 0; i < k; i++)
            item -> key[i] = key[i];
    }else{
        if(!root -> left && !root -> right)
            root -> border = median(root -> elements, cd);

        if(key[cd] <= (root -> border))
            root -> left = insert(root -> left, key, depth + 1);
        else
            root -> right = insert(root -> right, key, depth + 1);
    }

    return root;
}

Item *find(Node *root, int key[], unsigned depth){
    int position = 0;

    if(root == NULL)
        return NULL;

    position = binarySearch(root -> elements, key, depth);
    if(position >= 0)
        return root -> elements[position];

    unsigned cd = depth % k;

    if(key[cd] <= root -> border)
        return find(root -> left, key, depth + 1);
    
    return find(root -> right, key, depth + 1);
}

void findNearest(Node *root, int key[], unsigned depth, float *distanse, Item **nearest_item){
    int neighbor[k];
    int count = 0;
    float curent_distance;

    if(root == NULL)
        return;

    // проитерирлваться по массиву и засунуть ближайший в nearest_item
    while(count < N && root -> elements[count]){

        curent_distance = calculateDistance(root -> elements[count], key);

        printf("Dist: %f \nCur Key: (%d, %d)\n\n", curent_distance, (root -> elements[count]) -> key[0], (root -> elements[count]) -> key[1]);

        if(*distanse > curent_distance){
            *distanse = curent_distance;
            *nearest_item = root -> elements[count];
        }

        count ++;
    }

    
    findNearest(root -> left, key, depth + 1, distanse, nearest_item);
    findNearest(root -> right, key, depth + 1, distanse, nearest_item);
    
}

float calculateDistance(Item *item, int key[]){
    return sqrt(pow(item -> key[0] - key[0], 2) + pow(item -> key[1] - key[1], 2));
}

Item *delete(Node *root, int key[], unsigned depth){
    int position = 0;

    if(root == NULL)
        return NULL;

    position = binarySearch(root -> elements, key, depth);
    if(position >= 0){
        Item *item = root -> elements[position];

        deleteFromArray(root -> elements, position);

        return item;
    }

    unsigned cd = depth % k;

    if(key[cd] <= root -> border)
        return delete(root -> left, key, depth + 1);
    
    return delete(root -> right, key, depth + 1);


}

// Загружает Tree из файла
int loadTree(Node *root){
    FILE *fd = NULL;
    char *fname = NULL;

    printf("Enter file name --> "); // tree.txt
    fname = getStr();
    if(fname == NULL)
        return 0;


    return 1;
}

void preorderTraversal(Node *root){
    if(root == NULL)
        return;

    preorderTraversal(root -> left);
    preorderTraversal(root -> right);
}

void delTree(Node *root){
    if(root == NULL)
        return;

    Node *left, *right;

    left = root -> left;
    right = root -> right;

    free(root);

    preorderTraversal(left);
    preorderTraversal(right);
}

int showTree(Node *root){
    showNode(root, 0, 0);

    return 1;
}

// Симметричный обход
void showNode(Node *root, int space, unsigned depth){
    if(root == NULL)
        return;
    
    space += SPACE;

    showNode(root -> right, space, depth + 1);

    printf("\n");
    for(int i = SPACE; i < space; i++)
        printf(" ");

    unsigned cd = depth % k;

    if(cd)
        printf("(-, %d) ", root -> border);
    else
        printf("(%d, -) ", root -> border);

    printf("\n");
    for(int i = SPACE; i < space - 8; i++)
        printf(" ");

    showArray(root -> elements);

    showNode(root -> left, space, depth + 1);
}

void deleteFromArray(Item *elements[], int position){
    int m = binarySearchEndArray(elements);
    printf("End array %d\n", m);

    free(elements[position]);
    elements[position] = NULL;

    while(position <= m - 1){
        elements[position] = elements[position + 1];
        position ++;
    }

    elements[m] = NULL;
}
