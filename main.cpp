#include <iostream>
#include <limits>

#include "vector.h"

using namespace Data_Types;

int main(int argc, char*argv[]){
    /*
    Vector a; // Вызывается конструктор по-умолчанию
    Vector b(1, 0); // Если не инициализировать размер, то qsz будет мусор
    Vector c(5);
    Vector f(); // Обьявляется ф-ия

    Vector d[5]; // Бужет использоваться конструктор по-умолчанию для каждого элемента массива

    // std::cout << a.SZ << std::endl; // Ошибка, так нельзя
    a.print(std::cout);

    std::cout << std::endl << std::endl;

    b.print(std::cout);

    std::cout << std::endl << std::endl;

    c.print(std::cout);

    std::cout << std::endl << std::endl;

    d[3].print(std::cout);

    int fuck[] = {4, 9, -2};
    int cock[100];
    // Vector e(cock);
    Vector e(fuck);

    std::cout << std::endl << std::endl;

    e.print(std::cout);

    std::cout << e.max() << std::endl;

    // e.sort().print(std::cout);

    e.add(e.sort()).print(std::cout); // [4, 9, -2] + [-2, 4, 9]

    e.push(100).print(std::cout);

    int vector[20];

    std::cout << e.copy(vector, 1, 3)[3] << std::endl;

    int *ar = e.copy(vector, 1, 3);
    e.copy(vector, 1, 3);

    for(int i = 0; i < 3; i++)
        std::cout << ar[i] << ", ";
    std::cout << std::endl;

    for(int i = 0; i < 20; i ++)
        std::cout << vector[i] << ", ";
    std::cout << std::endl;

    Vector x = x.input();

    x.print(std::cout);
    */

    Vector vector(3, -1);
    int fl1, fl2 = 1;

    while(fl1){
        std::cout << std::endl << "------------------------------------------------------" << std::endl;
        while(fl2){
            try{
                vector.input();
                std::cout << "Your vector --> "; vector.print(std::cout);

                break;
            }
            catch(std::exception &re){
                std::cout << re.what() << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                fl2 = 1;

                continue;
            }
        }

        std::cout << "Sorted vector --> "; vector.sort().print(std::cout);
        std::cout << "Max element --> " << vector.max() << std::endl;

        int test_vector[] = {-1, 0, 3, 5, 90, 23};
        Vector elements(test_vector);
        std::cout << "Vector[..] + Elements[..] --> "; vector.add(elements).print(std::cout);

        int s_index, len;

        std::cout << std::endl << "Enter s_index and len --> ";
        std::cin >> s_index;
        std::cin >> len;
        if(!std::cin.good()){
            std::cout << "You have entered incorrect data..." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            continue;
        }

        try{
            vector.copy(elements, s_index, len);
            std::cout << "Copied vector --> "; elements.print(std::cout);
        }
        catch(std::exception &re){
            std::cout << re.what() << std::endl;

            continue;
        }

        int value;

        std::cout << "Enter pushed value --> ";
        std::cin >> value;
        if(!std::cin.good()){
            std::cout << "You have entered incorrect data..." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            continue;
        }

        try{
            vector.push(value);
            std::cout << "New vector --> "; vector.print(std::cout);
        }
        catch(std::exception &re){
            std::cout << re.what() << std::endl;

            continue;
        }
        std::cout << "------------------------------------------------------";

    }

    return 0;
}
