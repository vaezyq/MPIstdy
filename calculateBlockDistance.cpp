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

void calculateBlockDistanceOne(int **mat, int &len, int &block_len) {


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


#pragma omp parallel   num_threads(len / block_len)
            {
                if (omp_get_thread_num() == i) {

//            十字架横向的更新
                    crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);

#pragma omp critical
                    {
                        mat = cs_row.calculateCrossBlock(mat);
                    };

//            十字架竖向的更新
                    crossUpdate cs_col(i * block_len, block_len * k, block_len, k * block_len);

#pragma omp critical
                    {
                        mat = cs_col.calculateCrossBlock(mat);
                    };
                }

            };


            //            第三步 剩余区域的更新

            int N = (len / block_len) * (len / block_len);
            for (int i = 0; i < len / block_len; ++i) {

//#pragma omp parallel  num_threads(len / block_len)
                {
//                    if (omp_get_thread_num() == i) {
                    for (int j = 0; j < len / block_len; ++j) {
#pragma omp parallel  num_threads(N)
                        {
                            for (int m = 0; m < N; ++m) {
                                if (omp_get_thread_num() == k) {
                                    if (i != k || j != k) {
                                        elseBlockUpdate ebu(i * block_len, j * block_len, block_len, k * block_len);
#pragma omp critical
                                        {
                                            mat = ebu.calcuateElseBlock(mat);
                                        };

                                    }
                                }
                            }

//                                if (omp_get_thread_num() == j) {

//                            }
//                                };
                        }
//                        };
                    }
                }

            }


        }
    }





//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << mat[i][j] << "  ";
//        }
//        cout << endl;
//    }




//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << mat[i][j] << "  ";
//        }
//        cout << endl;
//    }

    cout << "***************" << endl;


}