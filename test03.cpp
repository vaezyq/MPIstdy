//
// Created by 22126 on 2021/10/12.
//


#pragma comment(lib, "D:\\MPI\\Lib\\x86\\msmpi.lib")

#include <iostream>
#include "graphMatrix.h"

#include "basicDistance.h"
#include "crossUpdate.h"
#include "elseBlockUpdate.h"

#include <chrono>

#include <omp.h>

using namespace std::chrono;




int main06() {


    int len = 15;
    int block_len = 3;

    graphMatrix gm(len);
    int (**mat);

    mat = (int **) malloc(len * sizeof(int *));
    for (int i = 0; i < len; i++) {
        mat[i] = (int *) malloc(len * sizeof(int));
    }
    mat = gm.getArray();



    //第一步 计算对角线区域的dis，不可并行



    for (int k = 0; k < len / block_len; ++k) {


//            每一步之间是不可以并行的，但是每一步可以分三个小操作，操作2、3可以并行


        basicDistance bc(block_len * k, block_len);
        //第一步 计算对角线区域的dis，不可并行
        mat = bc.calculate(mat);

        //第二步 计算十字架上区域的dis(可并行block_len-1个线程)


        for (int i = 0; i < len / block_len; ++i) {
            if (i == k) {
                continue;
            }


            {


//            十字架横向的更新
                crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);


                {
                    mat = cs_row.calculateCrossBlock(mat);
                };

//            十字架竖向的更新
                crossUpdate cs_col(i * block_len, block_len * k, block_len, k * block_len);


                {
                    mat = cs_col.calculateCrossBlock(mat);
                };


            };


            //            第三步 剩余区域的更新

            for (int i = 0; i < len / block_len; ++i) {

                {
                    for (int j = 0; j < len / block_len; ++j) {

                        {
                            if (i != k || j != k) {
                                elseBlockUpdate ebu(i * block_len, j * block_len, block_len, k * block_len);

                                {
                                    mat = ebu.calcuateElseBlock(mat);
                                };

                            }

                        };
                    }
                };
            }


        }
    }


    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            cout << mat[i][j] << "  ";
        }
        cout << endl;
    }

    cout << "***********" << endl;



    basicDistance bc(0, len);
    mat = bc.calculate(mat);
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            cout << mat[i][j] << "  ";
        }
        cout << endl;
    }

    cout << "***************" << endl;
    return 0;


}