//
// Created by 22126 on 2021/9/28.
//

#include "crossUpdate.h"

crossUpdate::crossUpdate(int rowIndex, int colIndex, int len, int blockIndex) : row_index(rowIndex),
                                                                                col_index(colIndex), len(len),
                                                                                blockIndex(blockIndex) {}

int **crossUpdate::calculateCrossBlock(int **array) {
//更新右边的区域
    for (int i = row_index; i < row_index + len; ++i) {
        for (int j = col_index; j < col_index + len; ++j) {
            for (int k = col_index; k < col_index + len; ++k)
                array[i][j] = array[i][j] > (array[i][k] + array[k][j]) ? (array[i][k] + array[k][j]) : array[i][j];
        }
    }

//    更新左边的区域.这里存在疑问
    for (int i = row_index; i < row_index + len; ++i) {
        for (int j = col_index; j < col_index + len; ++j) {
            for (int k = row_index; k < row_index + len; ++k)
                array[i][j] = array[i][j] > (array[i][k] + array[k][j]) ? (array[i][k] + array[k][j]) : array[i][j];
        }
    }

    return array;
}

















