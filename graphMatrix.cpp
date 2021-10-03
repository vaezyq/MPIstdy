//
// Created by 22126 on 2021/9/28.
//

#include "graphMatrix.h"
#include <iostream>

graphMatrix::graphMatrix(int len) {
    this->length = len;
    srand(time(nullptr));
    int **a;


    array = (int **) malloc(this->length * sizeof(int *));
    for (int i = 0; i < this->length; i++) {
        array[i] = (int *) malloc(this->length * sizeof(int));
    }

    a = (int **) malloc(this->length * sizeof(int *));
    for (int i = 0; i < this->length; i++) {
        a[i] = (int *) malloc(this->length * sizeof(int));
    }

    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < length; ++j) {
            a[i][j] = rand() % 20;
        }
    }
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < length; ++j) {
            array[i][j] = (a[i][j] + a[j][i]) / 2;
        }
    }


}

int graphMatrix::getLength() const {
    return length;
}

void graphMatrix::setLength(int length) {
    graphMatrix::length = length;
}

int **graphMatrix::getArray() const {
    return array;
}

void graphMatrix::setArray(int **array) {
    graphMatrix::array = array;
}



