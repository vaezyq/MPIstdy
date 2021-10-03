#pragma comment(lib, "D:\\MPI\\Lib\\x86\\msmpi.lib")

#include <iostream>
#include "graphMatrix.h"
#include <iomanip>
#include "basicDistance.h"
#include "crossUpdate.h"
#include "elseBlockUpdate.h"
#include "mpi.h"
#include <chrono>


using namespace std::chrono;

int main() {


    int len = 108;

    int block_len = 3;


    graphMatrix gm(len);
    int (**mat);

    mat = (int **) malloc(len * sizeof(int *));
    for (int i = 0; i < len; i++) {
        mat[i] = (int *) malloc(len * sizeof(int));
    }
    mat = gm.getArray();

    //MPI并行部分
    int comm_sz; //进程数量
    int my_rank; //目前的进程

//初始化MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << setw(5) << mat[i][j];
//        }
//        cout << endl;
//    }

    if (my_rank == 0) {
        auto start = high_resolution_clock::now();
        //区域划分



        for (int k = 0; k < len / block_len; ++k) {
            basicDistance bc(block_len * k, block_len);
            //第一步 计算对角线区域的dis
            mat = bc.calculate(mat);

            //第二步 计算十字架上区域的dis(可并行block_len-1个线程)
            for (int i = 0; i < len / block_len; ++i) {
                if (i == k) {
                    continue;
                }
//            十字架横向的更新
                crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);
                mat = cs_row.calculateCrossBlock(mat);
//            十字架竖向的更新
                crossUpdate cs_col(i * block_len, block_len * k, block_len, k * block_len);
                mat = cs_col.calculateCrossBlock(mat);


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

        auto stop = high_resolution_clock::now();


        auto duration = duration_cast<microseconds>(stop - start);

        cout << duration.count() << endl;
    }



//    cout << "块floyd计算结果" << endl;
//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << setw(5) << mat[i][j];
//        }
//        cout << endl;
//    }

    if (my_rank == 0) {


        auto start = high_resolution_clock::now();
        basicDistance bc(0, len);
        mat = bc.calculate(mat);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << duration.count() << endl;


    }

//    for (int i = 0; i < len; ++i) {
//        for (int j = 0; j < len; ++j) {
//            cout << setw(5) << mat[i][j];
//        }
//        cout << endl;
//    }



    auto start = high_resolution_clock::now();


    for (int k = 0; k < len / block_len; ++k) {

        basicDistance bc(block_len * k, block_len);
        //第一步 计算对角线区域的dis
        mat = bc.calculate(mat);

        if (my_rank != 0 && my_rank != k && my_rank < len / block_len) {
//            十字架横向的更新
            crossUpdate cs_row(block_len * k, my_rank * block_len, block_len, k * block_len);
            mat = cs_row.calculateCrossBlock(mat);
//            十字架竖向的更新
            crossUpdate cs_col(my_rank * block_len, block_len * k, block_len, k * block_len);
            mat = cs_col.calculateCrossBlock(mat);

            MPI_Send(mat, len, MPI_INT, 0, 0, MPI_COMM_WORLD);

        } else {
            //结果接受
            if (my_rank == 0) {
                for (int q = 1; q < my_rank < len / block_len; q++) {
                    if (q != k) {
                        MPI_Recv(mat, len, MPI_INT, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//                        mat更新


                    }
                }
            }


        }

    }


    for (int k = 0; k < len / block_len; ++k) {
//        一次迭代过程
//第一步 计算对角线上区域的元素,不可并行



        //第二步 计算十字架上区域的dis(可并行block_len-1个线程)
        for (int i = 0; i < len / block_len; ++i) {
            if (i == k) {
                continue;
            }

//            十字架横向的更新
            crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);
            mat = cs_row.calculateCrossBlock(mat);
//            十字架竖向的更新
            crossUpdate cs_col(i * block_len, block_len * k, block_len, k * block_len);
            mat = cs_col.calculateCrossBlock(mat);


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


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;

    MPI_Finalize();


}















