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

    //MPI���в���
    int comm_sz; //��������
    int my_rank; //Ŀǰ�Ľ���

//��ʼ��MPI
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
        //���򻮷�



        for (int k = 0; k < len / block_len; ++k) {
            basicDistance bc(block_len * k, block_len);
            //��һ�� ����Խ��������dis
            mat = bc.calculate(mat);

            //�ڶ��� ����ʮ�ּ��������dis(�ɲ���block_len-1���߳�)
            for (int i = 0; i < len / block_len; ++i) {
                if (i == k) {
                    continue;
                }
//            ʮ�ּܺ���ĸ���
                crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);
                mat = cs_row.calculateCrossBlock(mat);
//            ʮ�ּ�����ĸ���
                crossUpdate cs_col(i * block_len, block_len * k, block_len, k * block_len);
                mat = cs_col.calculateCrossBlock(mat);


            }


            //            ������ ʣ������ĸ���

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



//    cout << "��floyd������" << endl;
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
        //��һ�� ����Խ��������dis
        mat = bc.calculate(mat);

        if (my_rank != 0 && my_rank != k && my_rank < len / block_len) {
//            ʮ�ּܺ���ĸ���
            crossUpdate cs_row(block_len * k, my_rank * block_len, block_len, k * block_len);
            mat = cs_row.calculateCrossBlock(mat);
//            ʮ�ּ�����ĸ���
            crossUpdate cs_col(my_rank * block_len, block_len * k, block_len, k * block_len);
            mat = cs_col.calculateCrossBlock(mat);

            MPI_Send(mat, len, MPI_INT, 0, 0, MPI_COMM_WORLD);

        } else {
            //�������
            if (my_rank == 0) {
                for (int q = 1; q < my_rank < len / block_len; q++) {
                    if (q != k) {
                        MPI_Recv(mat, len, MPI_INT, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//                        mat����


                    }
                }
            }


        }

    }


    for (int k = 0; k < len / block_len; ++k) {
//        һ�ε�������
//��һ�� ����Խ����������Ԫ��,���ɲ���



        //�ڶ��� ����ʮ�ּ��������dis(�ɲ���block_len-1���߳�)
        for (int i = 0; i < len / block_len; ++i) {
            if (i == k) {
                continue;
            }

//            ʮ�ּܺ���ĸ���
            crossUpdate cs_row(block_len * k, i * block_len, block_len, k * block_len);
            mat = cs_row.calculateCrossBlock(mat);
//            ʮ�ּ�����ĸ���
            crossUpdate cs_col(i * block_len, block_len * k, block_len, k * block_len);
            mat = cs_col.calculateCrossBlock(mat);


        }

        //            ������ ʣ������ĸ���

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















