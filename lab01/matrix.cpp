#include <iostream>
#include "matrix.h"

using namespace spareMatrix;

int main(){
    Matrix matrix = {0, nullptr};

    matrix.vector = create(matrix.lines);
    if(!matrix.vector){
        std::cout << "incorect data" << std::endl;
        erase(matrix, matrix.lines);

        return 1;
    }

    input(matrix);
    output("----------------------------Spare matrix----------------------------", &matrix);

    Matrix new_matrix = {0, nullptr};

    new_matrix.vector = create(new_matrix.lines, matrix.lines);
    if(!new_matrix.vector){
        std::cout << "incorect data" << std::endl;
        return 1;
    }

    function(new_matrix, matrix);
    output("----------------------------New Spare matrix----------------------------", &new_matrix);

    erase(matrix, matrix.lines);

    return 0;
}
