#ifndef MANUALAI_H
#define MANUALAI_H
#include "AI.h"
#include "StudentAI.h"
#pragma once

class ManualAI : public AI {
public:
    Board board;

    ManualAI(int col, int row, int p);

    Move GetMove(Move board) override;

    void buildTree(Node *cur_node, Board &cur_board);

    void deleteTree(Node *node);

    ReturnObj searchTree(Node *node, int alpha, int beta) const;

    static int eval(const Board &cur_board, int ai_player);

    int max_depth;

};

#endif //MANUALAI_H

