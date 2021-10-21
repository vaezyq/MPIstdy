//
// Created by 22126 on 2021/9/28.
//

#ifndef BLOCKPARALLEL_ELSEBLOCKUPDATE_H
#define BLOCKPARALLEL_ELSEBLOCKUPDATE_H


class elseBlockUpdate {
private:
    int col_index; //块起始
    int row_index;
    int len; //块长度
    int k_index; //定义进行更新的对角块

public:
    elseBlockUpdate(int colIndex, int rowIndex, int len, int kIndex);

    void calcuateElseBlock(int **array);


};


#endif //BLOCKPARALLEL_ELSEBLOCKUPDATE_H
