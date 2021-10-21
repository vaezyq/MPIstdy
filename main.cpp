//
// Created by 22126 on 2021/10/13.
//

//
// Created by 22126 on 2021/10/12.
//




#include <iostream>
#include "graphMatrix.h"

#include "basicDistance.h"
#include "crossUpdate.h"
#include "elseBlockUpdate.h"


#include <chrono>

#include <omp.h>

using namespace std::chrono;


void calculateBlockDistanceParallel(int **mat, int &len, int &block_len);

void calculateBlockDistance(int **mat, int &len, int &block_len);

void calculateBlockDistancePara(int **mat, int &len, int &block_len);


int main() {


    int len = 1000;
    int block_len = 10;

    graphMatrix gm(len);
    int (**mat);

    mat = (int **) malloc(len * sizeof(int *));
    for (int i = 0; i < len; i++) {
        mat[i] = (int *) malloc(len * sizeof(int));
    }
    mat = gm.getArray();


    int (**mat1);

    mat1 = (int **) malloc(len * sizeof(int *));
    for (int i = 0; i < len; i++) {
        mat1[i] = (int *) malloc(len * sizeof(int));
    }


    int (**mat2);

    mat2 = (int **) malloc(len * sizeof(int *));
    for (int i = 0; i < len; i++) {
        mat2[i] = (int *) malloc(len * sizeof(int));
    }

    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            mat2[i][j] = mat1[i][j] = mat[i][j];
        }
    }


    auto start = high_resolution_clock::now();
    calculateBlockDistance(mat2, len, block_len);


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);
    cout << "非并行块算法运行所需的时间" << endl;
    cout << duration.count() << endl;


//    cout << endl;
//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << mat2[i][j] << " ";
//        }
//        cout << endl;
//    }


    cout << "************" << endl;


    auto start2 = high_resolution_clock::now();
    calculateBlockDistancePara(mat1, len, block_len);


    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<nanoseconds>(stop2 - start2);
    cout << "并行块算法运行所需的时间" << endl;
    cout << duration2.count() << endl;

//    cout << endl;
//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << mat1[i][j] << " ";
//        }
//        cout << endl;
//    }

    cout << "************" << endl;


    auto start1 = high_resolution_clock::now();


    for (int k = 0; k < len; ++k) {
        for (int i = 0; i < len; ++i) {
            for (int j = 0; j < len; ++j) {
                mat[i][j] = (mat[i][j] > (mat[i][k] + mat[k][j])) ? (mat[i][k] + mat[k][j]) : mat[i][j];
            }
        }
    }


    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<nanoseconds>(stop1 - start1);
    cout << "原floyd算法运行计算所需的时间" << endl;
    cout << duration1.count() << endl;


//    cout << endl;
//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << mat[i][j] << " ";
//        }
//        cout << endl;
//    }

//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << mat[i][j] << "  ";
//        }
//        cout << endl;
//    }

    return 0;


}


void calculateBlockDistance(int **mat, int &len, int &block_len) {


    for (int k = 0; k < len / block_len; ++k) {

//            每一步之间是不可以并行的，但是每一步可以分三个小操作，操作2、3可以并行

        basicDistance bc(block_len * k, block_len);
        //第一步 计算对角线区域的dis，不可并行
        bc.calculate(mat);

        //第二步 计算十字架上区域的dis(可并行block_len-1个线程)

        for (int i = 0; i < len / block_len; ++i) {
            if (i == k) {
                continue;
            }
//            十字架横向的更新
            crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);
            cs_row.calculateCrossBlock(mat);

        }

        //            第三步 剩余区域的更新

        for (int i = 0; i < len / block_len; ++i) {
            for (int j = 0; j < len / block_len; ++j) {
                if (i != k || j != k) {
                    elseBlockUpdate ebu(i * block_len, j * block_len, block_len, k * block_len);
                    ebu.calcuateElseBlock(mat);
                }
            }
        }

    }


}


void calculateBlockDistancePara(int **mat, int &len, int &block_len) {


    for (int k = 0; k < len / block_len; ++k) {

//            每一步之间是不可以并行的，但是每一步可以分三个小操作，操作2、3可以并行

        basicDistance bc(block_len * k, block_len);
        //第一步 计算对角线区域的dis，不可并行
        bc.calculate(mat);

        //第二步 计算十字架上区域的dis(可并行block_len-1个线程)


#pragma omp parallel for num_threads(10) default(none) shared(len, block_len, k, mat)
        for (int i = 0; i < len / block_len; ++i) {
            if (i == k) {
                continue;
            }
//            十字架横向的更新
            crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);
            cs_row.calculateCrossBlock(mat);

        }


        //            第三步 剩余区域的更新
//#pragma omp parallel for num_threads(20) default(none) shared(len, block_len, k, mat)
//        for (int i = 0; i < len / block_len; ++i) {
//#pragma omp parallel for num_threads(20) default(none) shared(len, block_len, k, mat, i)
//            for (int j = 0; j < len / block_len; ++j) {
//                if (i != k || j != k) {
//                    elseBlockUpdate ebu(i * block_len, j * block_len, block_len, k * block_len);
//                    ebu.calcuateElseBlock(mat);
//                }
//            }
//        }


        int base = (len / block_len) * (len / block_len);
        int len_one = (len / block_len);
#pragma omp parallel for num_threads(10) default(none) shared(base, len_one, block_len, mat, k)
        for (int baseline = 0; baseline < base; ++baseline) {
            int i = baseline / (len_one);
            int j = baseline % len_one;
            if (i != k || j != k) {
                elseBlockUpdate ebu(i * block_len, j * block_len, block_len, k * block_len);
                ebu.calcuateElseBlock(mat);
            }
        }


    }


}