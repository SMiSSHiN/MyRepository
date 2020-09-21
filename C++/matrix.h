// inline и template ф-ии пишутся в заголовочном файле
#pragma once

#include <iostream> 

// Шаблон ф-ии ввода вещественного числа
template <class T>
int getNum(T &num){
    std::cin >> num;
    if(!std::cin.good())
        return -1;
    return 1;
}

namespace spareMatrix{

    struct Element{
        int column;
        double value;
        struct Element *next;
    };

    struct Info{
        int elementsNum; // Кол-во элементов в строке
        Element *next;   // Указатель на начало строки
    };

    struct Matrix{
        int lines; // Кол-во строк
        Info *vector = nullptr; // Динамический массив из Info
    };

    Info *create(int &rlines, int check = -1);
    void input(Matrix &matrix);

    template <class T>
    void addElement(Matrix &matrix, int line, int column, T value){
        Element *tmp = matrix.vector[line].next;
        Element *new_element = new Element;

        matrix.vector[line].next = new_element;

        // Формирую список
        new_element -> next = tmp;

        // Копирую всю информацию...
        new_element -> column = column;
        new_element -> value = value;

        matrix.vector[line].elementsNum += 1;
    }

    void function(Matrix &new_matrix, Matrix matrix);
    void bubblesort(Matrix &matrix);
    void swap(Info &a, Info &b);
    void output(const char *message, Matrix *);
    void erase(Matrix &matrix, int lines);

    Element *findColumn(Element *element, int column);
}
