// Что лучше, массив указателей или массив вершин?
// Что лучше, вычислять длинну массива или сделать место для хранения числа кол-ва вершин?
// Почему линукс шалит?
//_________________________________________________________//
// ---A:--- Задать ориентированный граф
// Организовать работу с файлом
// ---B:--- Сделать добавление вершины
// -------- Сделать добавление нового ребра
// ---C:--- Написать проверку сохранения ацикличности
// ---D:--- Написать топологическую сортировку
// ---E:--- Написать удаление вершины.
// F: Убедиться, что все работает для большого кол-ва вершин
// ---G:--- Написать рандомную генерацию графа
// H: Написать таймирование
// ---I:--- Доделать удаление



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

// Размерность пр-ва
#define k 2
// Мн-во вершин графа
#define BORDER 10000

// Глобальная метка времени
int tiime;


typedef struct Vertex{
    char *name;
    int point[k];
    int color;
    int discover;
    int finish;
    struct ListNode *head; // Указатель на начало списка
}Vertex;

// Элемент списка смежностей
typedef struct ListNode{
    Vertex *vertex;
    struct ListNode *next;
}ListNode;

typedef struct Graph{
    FILE *fd;
    int size;
    Vertex **vertices;
}Graph;

const char *messages[] = {"0. Quit", "1. Add Vertex", "2. Add Edge", "3. Delete Vertex", "4. Topological sorting", "5. Random Graph", "6. Timing", "7. Show Graph"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);
const char *error_message[] = {"OK", "Vertex already exists", "Duplicate name", "Source vertex not found", "Destination vertex not found", "Duplicate adjacent vertex", "Acyclicity is broken"};


int getInt(int *);
char *getStr();

Graph *createGraph();
int areKeysSame(int point[], int find_point[]);
int findVertex(Graph *graph, char *name);
void insertToList(Vertex *src_vertex, Vertex *dest_vertex);
char *randString(int length);

int insertVertex(Graph *graph, char *name, int point[]);
int insertEdge(Graph *graph, char *src, char *dest);
int delVertex(Graph *graph, char *name);
void DFS(Graph *graph);
void DFS_Visit(Graph *graph, Vertex *vertex, int mode, ListNode **head);
int isAcyclic(Graph *graph);
void delGraph(Graph *graph);

int dialog(const char *[], int);
int loadGraph(Graph *graph);
int saveGraph(Graph *graph);
int addVertex(Graph *graph),
    addEdge(Graph *graph),
    deleteVertex(Graph *graph),
    topologicalSorting(Graph *graph),
    randomGraph(Graph *graph),
    timingGraph(Graph *graph),
    showGraph(Graph *graph);

int (*functions[])(Graph *) = {NULL, addVertex, addEdge, deleteVertex, topologicalSorting, randomGraph, timingGraph, showGraph};

int main(){

    int rc;
    Graph *graph = NULL;

    graph = createGraph();

    if(loadGraph(graph) == 0)
        return 1;

    while((rc = dialog(messages, size_messages)))
        if(!functions[rc](graph))
            break;

    if(saveGraph(graph) == 0)
        return 1;

    delGraph(graph);

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
char *getStr(){
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

int loadGraph(Graph *graph){
    char vertex_name[100], adj_name[100], *name, *fuck;
    int point[k], rc;

    point[0] = 0;
    point[1] = 1;

    // Типо ввести имя файла

    graph -> fd = fopen("graph.txt", "r+");
    if(graph -> fd == NULL){
        printf("File doesn't exist");

        // Тут нужно рандомно сгенерировать граф !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        return 1;
    }   

    // Считать названия вершин
    if(fgets(vertex_name, 100, graph -> fd) != NULL){
        if(fuck = strchr(vertex_name, '\n'))
            *fuck = '\0';

        name = strtok(vertex_name, " ");

        while(name != NULL){
            // Добавить вершину
            rc = insertVertex(graph, name, point);
            printf("%s: (%s, %d, %d)\n", error_message[rc], name, point[0], point[1]);

            name = strtok(NULL, " ");
        }
    }

    fuck = NULL;

    for(int i = 0; i < graph -> size; i++){
        // Считали строку смежных вершин
        if(fgets(adj_name, 100, graph -> fd) != NULL){
            if(fuck = strchr(adj_name, '\n'))
                *fuck = '\0';
                
            if(strcmp(adj_name, "") == 0)
                continue;

            Vertex *src_vertex = graph -> vertices[i];
        
            // Разделяем строку смежных вершин
            name = strtok(adj_name, " ");

            while(name != NULL){
                // Вставить ребро
                rc = insertEdge(graph, src_vertex -> name, name);
                printf("%s: (%s, %s)\n", error_message[rc], src_vertex -> name, name);

                name = strtok(NULL, " ");
            }
        }
    }

    return 1;
}

int saveGraph(Graph *graph){
    int i;
    Vertex *vertex, *adj_vertex;
    ListNode *node;

    // Введите имя файла
    
    // Пересоздается файл
    graph -> fd = fopen("savegraph.txt", "w");
    if(graph -> fd == NULL){
        printf("File doesn't exist");
        // Введите имя файла
        // Создается новый файл

        return 1;
    }

    for(i = 0; i < graph -> size; i++){
        vertex = graph -> vertices[i];

        // Сохранить все вершины
        fputs(vertex -> name, graph -> fd);
        fputs(" ", graph -> fd);
    }

    // Переходим на строчку вниз, чтобы начать писать смежные вершины
    fputs("\n", graph -> fd);

    for(i = 0; i < graph -> size; i++){
        vertex = graph -> vertices[i];

        node = vertex -> head;
        if(node == NULL){
            fputs("\n", graph -> fd);

            continue;
        }

        while(node){
            adj_vertex = node -> vertex;

            fputs(adj_vertex -> name, graph -> fd);
            fputs(" ", graph -> fd);

            node = node -> next; 
        }

        fputs("\n", graph -> fd);
    }

    fclose(graph -> fd);

    return 1;
}

Graph *createGraph(){
    // Выделяем память для графа
    Graph *graph = (Graph *)malloc(sizeof(Graph) * 1);

    // В массиве нет ни одного элемента
    graph -> fd = NULL;
    graph -> size = 0;
    graph -> vertices = NULL;

    return graph;
}

int addVertex(Graph *graph){
    int point[k], rc;
    char *name;
    const char *error = "";

    printf("\n\n_________________________ADD VERTEX_________________________");
    printf("\n____________________________________________________________\n\n");

    // Вводим имя вершины
    printf("Enter name --> ");
    name = getStr();
    if(name == NULL)
        return 0;

    // Вводим координаты вершины
    printf("Enter (x, y) --> ");
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&point[0]))
            return 0;
    }while(point[0] < -BORDER || point[0] > BORDER);
    error = "";
    do{
        printf(error);
        error = "You are wrong. Repeat, please.";
        if(!getInt(&point[1]))
            return 0;
    }while(point[1] < -BORDER || point[1] > BORDER);

    rc = insertVertex(graph, name, point);
    printf("%s: (%s, %d, %d)\n", error_message[rc], name, point[0], point[1]);
    free(name);

    return 1;
}

int addEdge(Graph *graph){
    char *src, *dest, rc;
    const char *error = "";

    printf("\n\n__________________________ADD EDGE__________________________");
    printf("\n____________________________________________________________\n\n");

    // Вводим source Vertex
    printf("Enter source name --> ");
    src = getStr();
    if(src == NULL)
        return 0;
    
    // Вводим destination Vertex
    printf("Enter destination name --> ");
    dest = getStr();
    if(dest == NULL)
        return 0;


    rc = insertEdge(graph, src, dest);
    printf("%s: (%s, %s)\n", error_message[rc], src, dest);
    free(src);
    free(dest);

    return 1;
}

int deleteVertex(Graph *graph){
    int rc;
    char *name;

    printf("\n\n_______________________DELETE VERTEX________________________");
    printf("\n____________________________________________________________\n\n");

    // Вводим source Vertex
    printf("Enter source name --> ");
    name = getStr();
    if(name == NULL)
        return 0;

    rc = delVertex(graph, name);
    printf("%s: %s\n", error_message[rc], name);
    free(name);

    return 1;
}

int topologicalSorting(Graph *graph){
    printf("\n\n____________________TOPOLOGICAL SORTING_____________________");
    printf("\n____________________________________________________________\n\n");

    DFS(graph);

    return 1;
}

// Добавление вершины в граф
int insertVertex(Graph *graph, char *name, int point[k]){

    int i;
    Vertex *vertex = NULL; 
    
    for(i = 0; i < graph -> size; i ++){
        vertex = graph -> vertices[i];

        if(strcmp(vertex -> name, name) == 0)
            return 2; // "Duplicate name"
    }    

    // Увеличиваем массив для добавления новой вершины
    graph -> vertices = (Vertex **)realloc(graph -> vertices, sizeof(Vertex *) * (i + 1));
    // Создаем новую вершину 
    vertex = (Vertex *)malloc(sizeof(Vertex) * 1);

    vertex -> name = (char *)malloc(strlen(name) + 1);
    strcpy(vertex -> name, name);
    for(int count = 0; count < k; count++)
        vertex -> point[count] = point[count];
    vertex -> color = -1;
    vertex -> head = NULL;

    // Помещаем новую вершину на последнюю позицию в массиве
    graph -> vertices[i] = vertex;
    
    graph -> size += 1;

    return 0; // "OK"
}

int insertEdge(Graph *graph, char *src, char *dest){

    int src_slot, dest_slot;
    Vertex *src_vertex, *dest_vertex, *adj_vertex;
    ListNode *node;

    src_slot = findVertex(graph, src);
    dest_slot = findVertex(graph, dest);
    if(src_slot < 0)
        return 3; // "Source vertex not found"
    if(dest_slot < 0)
        return 4; // "Destination vertex not found"
    
    if(strcmp(src, dest) == 0)
        return 6; // "Acyclicity is broken"

    // Указатель на вершину source
    src_vertex = graph -> vertices[src_slot];
    // Указатель на вершину destination
    dest_vertex = graph -> vertices[dest_slot];

    node = src_vertex -> head;

    // Проверка на дубликат смежной вершины
    while(node){
        adj_vertex = node -> vertex;

        if(strcmp(adj_vertex -> name, dest_vertex -> name) == 0)
            return 5; // "Duplicate adjacent vertex"

        node = node -> next;
    }

    insertToList(src_vertex, dest_vertex);

    // Проверка на ацикличность
    if(!isAcyclic(graph)){
        // Удалить добавленные связи
        node = src_vertex -> head;
        src_vertex -> head = node -> next;
        free(node);

        return 6; // "Acyclicity is broken"
    }

    return 0; // "OK"

}

int delVertex(Graph *graph, char *name){
    int i, del_slot;
    Vertex *vertex, *adj_vertex;
    ListNode *node, *tmp_node, *prev;

    // Если вершины нет в графе
    del_slot = findVertex(graph, name);
    if(del_slot < 0)
        return 3; // "Source vertex not found"

    // Нужно найти вершины в списках смежностей которых есть удаляемая вершина
    // и удалить ее оттуда
    for(i = 0; i < graph -> size; i ++){
        vertex = graph -> vertices[i];

        node = vertex -> head;

        // Если список смежности самой вершины
        if(strcmp(vertex -> name, name) == 0){
            
            // Удалить весь список смежности
            while(node){
                tmp_node = node;
                node = node -> next;
                free(tmp_node);
            }

            vertex -> head = NULL;

            del_slot = i;

            continue;
        }

        // Находим вершину в др. списках смежности
        // Если список смежности вершины пуст
        if(node == NULL)
            continue;

        adj_vertex = node -> vertex;

        // Если удаляется первая вершина
        if(strcmp(adj_vertex -> name, name) == 0){
            tmp_node = vertex -> head;
            vertex -> head = tmp_node -> next;
            free(tmp_node);

            continue;
        }

        prev = node;

        while(node){
            adj_vertex = node -> vertex;

            if(strcmp(adj_vertex -> name, name) == 0){
                tmp_node = node;
                prev -> next = node -> next;
                free(tmp_node);
                
                break;
            }

            prev = node;
            node = node -> next;
        }

    }

    vertex = graph -> vertices[del_slot];

    free(vertex);

    graph -> size -= 1;

    for(int count = del_slot; count < graph -> size; count ++)
        graph -> vertices[count] = graph -> vertices[count + 1];
    graph -> vertices = (Vertex **)realloc(graph -> vertices, sizeof(Vertex) * (graph -> size));

    return 0; // "OK"
}

// DFS для топологической сортировки
void DFS(Graph *graph){
    Vertex *vertex;
    ListNode *tmp_node, *head;
    int i;

    // Адрес указателя на начало списка сортированного графа
    // Память еще не выделена
    // Список пуст
    head = NULL;

    // Закрашиваем все вершины в белый
    for(i = 0; i < graph -> size; i++){
        vertex = graph -> vertices[i];

        vertex -> color = -1;
    }

    // Зануляем метку времени
    tiime = 0;

    for(i = 0; i < graph -> size; i++){
        vertex = graph -> vertices[i];

        if(vertex -> color == -1)
            DFS_Visit(graph, vertex, 1, &head);
    }

    // Переформировать массив + удалить список
    i = 0;

    while(head){
        graph -> vertices[i] = head -> vertex;
        i ++;

        tmp_node = head;
        head = head -> next;

        free(tmp_node);
    }

}

// Раскраска для сортировки и ацикличности
// mode = 0 - [ацикличность]
// mode = 1 - [топологическая сортировка]
void DFS_Visit(Graph *graph, Vertex *vertex, int mode, ListNode **head){
    int i;
    ListNode *node;
    Vertex *adj_vertex;

    // Красим в серый цвет
    vertex -> color = 0;
    tiime = tiime + 1;
    vertex -> discover = tiime;

    node = vertex -> head;

    // Для каждой верщины(v) из списка смежностей для вершины(u)
    while(node){
        adj_vertex = node -> vertex;

        if(adj_vertex -> color == -1)
            DFS_Visit(graph, adj_vertex, mode, head);

        node = node -> next;
    }

    // Красим в черный цвет
    vertex -> color = 1;
    tiime = tiime + 1;
    vertex -> finish = tiime;

    // node = vertex -> head;
    
    // Если топологическая сортировка
    if(mode == 1){
        // Вставить в начало пустого списка
        if(*head == NULL){
            *head = (ListNode *)malloc(sizeof(ListNode) * 1);
            (*head) -> vertex = vertex;
            (*head) -> next = NULL;
        }else{
            // Добавляемый в начало элемент
            node = (ListNode *)malloc(sizeof(ListNode) * 1);
            node -> vertex = vertex;
            node -> next = *head;

            *head = node;
        }
    }
}

// DFS для проверки ацикличности
int isAcyclic(Graph *graph){
    Vertex *vertex, *adj_vertex;
    ListNode *node;
    int i, isAcyclic = 1;

    // Закрашиваем все вершины в белый
    for(i = 0; i < graph -> size; i++){
        vertex = graph -> vertices[i];

        vertex -> color = -1;
    }

    // Зануляем метку времени
    tiime = 0; 
    
    // Раскрашиваем весь граф
    for(i = 0; i < graph -> size; i++){
        vertex = graph -> vertices[i];

        if(vertex -> color == -1)
            DFS_Visit(graph, vertex, 0, NULL);

        node = vertex -> head;
        
        // Как нормально сделать этот кусок? !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // Заново прохожусь по раскрашеному списку смежностей
        while(node){
            adj_vertex = node -> vertex;

            if(adj_vertex -> discover < vertex -> discover && adj_vertex -> discover < vertex -> finish && adj_vertex -> discover < adj_vertex -> finish &&
               vertex -> discover < vertex -> finish &&  vertex -> discover < adj_vertex -> finish &&
               vertex -> finish < adj_vertex -> finish)
                isAcyclic = 0; // Граф не ацикличен

            node = node -> next;
        }
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

    return isAcyclic;
}

int randomGraph(Graph *graph){
    char *name, *src, *dest;
    int mode, new_size, point[k], rc, i, src_slot, dest_slot;
    Vertex *vertex;

    i = graph -> size;

    printf("\n\n________________________RANDOM GRAPH________________________");
    printf("\n____________________________________________________________\n\n");

    graph = randGraph();

    // Случайное кол-во новых вершин
    new_size = graph -> size + ((rand() * rand()) % 10000);

    for(i; i < new_size; i++){

        // Сгенерировать имя
        name = randString(5000);

        // Сгенерировать координаты
        point[0] = rand() % 500;
        point[1] = rand() % 500;

        // Вставить вершину
        rc = insertVertex(graph, name, point);
        // printf("%s: (%s, %d, %d)\n", error_message[rc], name, point[0], point[1]);
        if(rc != 0){
            i--;
            continue;
        }

        // Сгенерировать ребро
        src_slot = rand() % graph -> size;
        vertex = graph -> vertices[src_slot];
        src = (char *)malloc(strlen(vertex -> name) + 1);
        strcpy(src, vertex -> name);

        dest_slot = rand() % graph -> size;
        vertex = graph -> vertices[dest_slot];
        dest = (char *)malloc(strlen(vertex -> name) + 1);
        strcpy(dest, vertex -> name);

        // Вставить ребро
        rc = insertEdge(graph, src, dest);
        // printf("%s: (%s, %s)\n", error_message[rc], src, dest);

    }

    return 1;
}

/*
Graph *randGraph(Graph *graph){
    char *name, *src, *dest;
    int mode, new_size, point[k], rc, i, src_slot, dest_slot;
    Vertex *vertex;

    i = graph -> size;

    // Случайное кол-во новых вершин
    new_size = graph -> size + ((rand() * rand()) % 10000);

    for(i; i < new_size; i++){

        // Сгенерировать имя
        name = randString(5000);

        // Сгенерировать координаты
        point[0] = rand() % 500;
        point[1] = rand() % 500;

        // Вставить вершину
        rc = insertVertex(graph, name, point);
        // printf("%s: (%s, %d, %d)\n", error_message[rc], name, point[0], point[1]);
        if(rc != 0){
            i--;
            continue;
        }

        // Сгенерировать ребро
        src_slot = rand() % graph -> size;
        vertex = graph -> vertices[src_slot];
        src = (char *)malloc(strlen(vertex -> name) + 1);
        strcpy(src, vertex -> name);

        dest_slot = rand() % graph -> size;
        vertex = graph -> vertices[dest_slot];
        dest = (char *)malloc(strlen(vertex -> name) + 1);
        strcpy(dest, vertex -> name);

        // Вставить ребро
        rc = insertEdge(graph, src, dest);
        // printf("%s: (%s, %s)\n", error_message[rc], src, dest);

    }

    return 1;
}
*/



int timingGraph(Graph *graph){
    int i, n = 10, cnt = 10000;
    clock_t first, last;

    printf("\n\n___________________________TIMING___________________________");
    printf("\n____________________________________________________________\n\n");

    // Создали новый граф, который в последствии будет удален
    Graph *time_graph = NULL; 
    time_graph = createGraph();   

    srand(time(NULL));  

    printf("           Topological Sorting                |\t           IsAcyclic               \n\n");

    while(n-- > 0){
        // Добавить в граф cnt вершин
        randomGraph(time_graph);

        first = clock();
        // Топологическая сортировка
        DFS(graph);
        last = clock();

        printf("test #%d, number of vertices = %d, time = %d\t",
                10 - n, (10 - n)*cnt, last - first);

        first = clock();
        // Проверка ацикличности
        isAcyclic(time_graph);
        last = clock();

        printf("test #%d, number of vertices = %d, time = %d\n",
        10 - n, (10 - n)*cnt, last - first);

    }

    delGraph(time_graph);

    return 1;
}

char *randString(int length){
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";   
    char *string = NULL;

    if(length){
        string = malloc(sizeof(char) * (length + 1));

        if(string){            
            for(int n = 0; n < length; n++){            
                int key = rand() % (int)(sizeof(charset) - 1);
                string[n] = charset[key];
            }

            string[length] = '\0';
        }
    }

    return string;
}

int areKeysSame(int point[], int find_point[]){
    for(int i = 0; i < k; i++)
        if(point[i] != find_point[i])
            return 0;

    return 1;
}

int findVertex(Graph *graph, char *name){
    Vertex *vertex = NULL;

    for(int i = 0; i < graph -> size; i ++){
        vertex = graph -> vertices[i];

        if(strcmp(vertex -> name, name) == 0)
            return i;
    }

    return -1;
}

void insertToList(Vertex *src_vertex, Vertex *dest_vertex){
    // Выделяю память для нового элемента списка
    ListNode *newNode = (ListNode *)malloc(sizeof(ListNode) * 1);
    
    newNode -> vertex = dest_vertex;

    if(src_vertex -> head == NULL){
        newNode -> next = NULL;
        src_vertex -> head = newNode;
    }
    else{
        newNode -> next = src_vertex -> head;
        src_vertex -> head = newNode;
    }
}

int showGraph(Graph *graph){
    Vertex *vertex, *adj_vertex;
    ListNode *node = NULL;

    printf("\n\n___________________________GRAPH___________________________");
    printf("\n____________________________________________________________\n\n");

	for(int i = 0; i < graph -> size; i++){
		vertex = graph -> vertices[i];
        
        printf("[%s, (%d, %d)] -> {", vertex -> name, vertex -> point[0], vertex -> point[1]);

        node = vertex -> head;

        if(node){
            // Указатель на смежную вершину
            adj_vertex = node -> vertex;

			printf("[%s, (%d, %d)]", adj_vertex -> name, adj_vertex -> point[0], adj_vertex -> point[1]);
			node = node -> next;
        }

		while(node){
            // Указатель на смежную вершину
            adj_vertex = node -> vertex;

			printf(", [%s, (%d, %d)]", adj_vertex -> name, adj_vertex -> point[0], adj_vertex -> point[1]);
			node = node -> next;
		}

		printf("}\n");
	}

    return 1;
}

void delGraph(Graph *graph){
    Vertex *vertex;
    ListNode *node, *tmp_node;

    for(int i = 0; i < graph -> size; i ++){
        vertex = graph -> vertices[i];

        node = vertex -> head;
        while(node){
            tmp_node = node;
            node = node -> next;

            free(tmp_node);
        }

        free(vertex);
    }

    graph -> fd = NULL;
    graph -> size = 0;
    free(graph -> vertices);
}
