#ifndef STUDENTAI_H
#define STUDENTAI_H

#include "AI.h"
#include "Board.h"
#include "climits"

#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and existing functions and variables.

class Node {
public:
    int value;
    int depth;
    vector<Node *> children;
    Move last_move;
    bool is_max;

    Node(int value, int depth, bool is_max) : value(value), depth(depth), is_max(is_max) {};
};

class ReturnObj{
public:
    int value;
    Move move;
    ReturnObj(int val, const Move& move) : value(val), move(move){};
};

class StudentAI : public AI {
public:
    Board board;

    StudentAI(int col, int row, int p);

    virtual Move GetMove(Move board);

    void buildTree(Node *cur_node, Board &cur_board);

    void deleteTree(Node *node);

    ReturnObj searchTree(Node *node, int alpha, int beta) const;

    static int eval(const Board &cur_board, int ai_player);

    int max_depth;

};

#endif //STUDENTAI_H
