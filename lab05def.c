#include <stdio.h>
#include <malloc.h>

const int SIZE = 10;

struct Item{
    int key;
    char *info;
};

struct Table{
    int n;
    Item array[SIZE];
};

const char *messages[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show"};
const int size_messages = sizeof(messages) / sizeof(messages[0]);

int getInt(int *input);
int dialog(const char *messages, int N);

int main(){


    return 0;
}

int getInt(int *input){
    int check;
    do{
        check = scanf("%d", input);
        if(check == 0){
            printf("Error!\n");
            sccanf("%*[^\n]");
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
        error_message = "You are wrong. Repeat, please."
        for(i = 0; i < N; i++) puts(messages[i]);
        puts("Make your choice");

        n = getInt(&rc);
        if(n == 0) rc = 0;
    }while (rc < 0 || rc >= N);

    return rc;
}
