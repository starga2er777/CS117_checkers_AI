#ifndef STUDENTAI_H
#define STUDENTAI_H

#include "AI.h"
#include "Board.h"
#include "climits"
#include <chrono>
#include <ctime>
#include "random"
#include "unordered_map"
#include "algorithm"
#include <cmath>

#pragma once

using namespace std;
//The following part should be completed by students.
//Students can modify anything except the class name and existing functions and variables.

class RandomGen {
public:
    static int getRandomInt(const unsigned long long max_val, const int min_val = 1) {
        random_device rd;
        default_random_engine eng{rd()};
        uniform_int_distribution<int> dist(min_val, max_val);
        return dist(eng) - 1;
    }
};


class Node {
public:
    vector<Node *> children;
    Node *parent;
    // the move that result this node
    Board board;
    int unfinished_num;
    // player# to take this move
    int player;
    Move move;
    vector<Move> unexpanded_moves;

    float wins;
    float visits;

    Node(Board &new_board, int player) : parent(nullptr), board(new_board), player(player), wins(0), visits(0) {
        unfinished_num = 0;
        vector<vector<Move>> all_moves = this->board.getAllPossibleMoves(player);
        for (auto &all_move: all_moves) {
            for (auto &tmp_move: all_move) {
                unexpanded_moves.push_back(tmp_move);
                ++unfinished_num;
            }
        }
    };

    Node *expand(Move &expand_move);

    void propagateComplete();
};

class StudentAI : public AI {
public:
    static const float c;
    static int ai_player;
    Board board;
    double computeTime;
    static vector<Node *> start_nodes;

    StudentAI(int col, int row, int p);

    Move GetMove(Move board) override;

    Move Monte_Carlo_Tree_Search();

    static float simulate(Node *cur_node);

    static void backPropagation(Node *node, float val);

    static Move chooseBest(Node *root);
};

Node *select(Node *root);

inline int getOpponent(const int player) {
    return 3 - player;
}

#endif //STUDENTAI_H
