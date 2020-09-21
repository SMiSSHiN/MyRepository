#include <iostream> 
#include <typeinfo>
#include "matrix.h"

namespace spareMatrix{

    Info *create(int &rlines, int check){

        const char *error_message = "";
        Info *vector = nullptr; 

        int lines;

        if(check < 0){
            do{
                std::cout << error_message << std::endl;
                std::cout << "Enter number of lines --> ";
                error_message = "You are wrong, repeat please";
                if(getNum(lines) < 0)
                    return nullptr;
            }while (lines < 0);
        }else
            lines = check;
        
        try{
            vector = new Info[lines];
        }
        catch (std::bad_alloc &ba){
            std::cout << "------ too many rows in matrix: " << ba.what() << std::endl;
            return nullptr;
        }

        for(int i = 0; i < lines; i++){
            vector[i].elementsNum = 0;
            vector[i].next = nullptr;
        }

        rlines = lines;
        return vector;
    }

    void input(Matrix &matrix){

        const char *error_message = "";
        int count; // Переменная для кол-ва столбцов в каждой строке

        Element element = {0, 0, nullptr}; // Считываем сюда новую информацию
        Element *point = nullptr;

        for(int i = 0; i < matrix.lines; i++){
            std::cout << std::endl << "---------------Enter items for line: " << i << "..." << std:: endl;
            error_message = "";
            
            do{
                std::cout << error_message;
                std::cout << "How many elements do you want to insert? --> ";
                error_message = "You are wrong, repeat please  --> ";
                if(getNum(count) < 0)
                    return ; // Как поступить в случае с void ???
            }while (count < 0);

            for(int j = 0; j < count; j++){

                std::cout << "Column number and value --> ";
                error_message = "";

                do{
                    std::cout << error_message;
                    error_message = "You are wrong, repeat please  --> ";
                    if(getNum(element.column) < 0){
                        erase(matrix, i + 1);
                        return ; // Как поступить в случае с void ???
                    }
                }while(element.column < 0);

                if(getNum(element.value) < 0){
                    erase(matrix, i + 1);
                    return ; // Как поступить в случае с void ???
                }

                // Проверить, занята ли эта позиция
                point = matrix.vector[i].next;

                while(point){
                    if(point -> column == element.column){
                        std::cout << "This position is already taken..." << std::endl;
                        j --;

                        break;
                    }

                    point = point -> next;
                }
                if(point)
                    continue;

                addElement(matrix, i, element.column, element.value);
            }
        }      
    }

    void function(Matrix &new_matrix, Matrix matrix){
        for(int i = 0; i < matrix.lines; i++){
            Element *point = matrix.vector[i].next; // Указатель на самый первый элемент списка

            while(point){
                if(point -> value > 0)
                    addElement(new_matrix, i, point -> column, point -> value);

                point = point -> next;
            }
        }

        // output("----------------------------New Spare matrix----------------------------", &new_matrix);

        bubblesort(new_matrix);
        /*
            // Сортировка кол-ва элементов по возрастанию
            ////////////////////////////////////////////////////////////////////////////////////////////////
            int i, j;

            for(i = 0; i < new_matrix.lines - 1; i++){
                for(j = 0; j < new_matrix.lines - i - 1; j++)
                    if(new_matrix.vector[j].elementsNum > new_matrix.vector[j + 1].elementsNum)
                        swap(new_matrix.vector[j], new_matrix.vector[j + 1]);
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////
        */
    }

    void bubblesort(Matrix &matrix){
        int i, j;

        for(i = 0; i < matrix.lines - 1; i++){
            for(j = 0; j < matrix.lines - i - 1; j++)
                if(matrix.vector[j].elementsNum > matrix.vector[j + 1].elementsNum)
                    swap(matrix.vector[j], matrix.vector[j + 1]);
        }
    }

    void swap(Info &a, Info &b){
        int tmp = a.elementsNum;
        a.elementsNum = b.elementsNum;
        b.elementsNum = tmp;

        Element *tmp_point = a.next;
        a.next = b.next;
        b.next = tmp_point;
    }

    void output(const char *message, Matrix *matrix){

        std::cout << std::endl << message << std::endl << std::endl;

        Info *vector = matrix -> vector;

        for(int i = 0; i < matrix -> lines; i++){
            Element *element = vector[i].next;
            int max_column = 0;

            std::cout << "[" << (i + 1) << ", " << vector[i].elementsNum << "] : ";

            while(element){
                if(element -> column > max_column)
                    max_column = element -> column;

                element = element -> next;
            }

            element = vector[i].next;

            Element *find_element;

            // while(element){

                for(int j = 0; j < max_column + 1; j++){
                    find_element = findColumn(vector[i].next, j);

                    if(find_element)
                        std::cout << find_element -> value << "[" << find_element -> column << "]  ";
                    else
                        std::cout << 0 << "[" << j << "]  ";
                }

                // std::cout << element -> value << "[" << element -> column << "]  ";
                
                // element = element -> next;
            // }

            std::cout << std::endl;
        }
    }

    void erase(Matrix &matrix, int lines){
        Element *point, *tmp;

        for(int i = 0; i < lines; i++){
            point = matrix.vector[i].next;

            while(point){
                tmp = point;
                point = point -> next;

                delete tmp;
            }   
        }

        delete[] matrix.vector;

        matrix.lines = 0;
        matrix.vector = nullptr;
    }

    Element *findColumn(Element *element, int column){
        while(element){
            if(element -> column == column)
                return element;

            element = element -> next;
        }

        return nullptr;
    }
}