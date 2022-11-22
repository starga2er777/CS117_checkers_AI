#include "StudentAI.h"
#include <random>

// The following part should be completed by students.
// The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col, int row, int p)
        : AI(col, row, p) {
    board = Board(col, row, p);
    board.initializeGame();
    player = 2;
    // set the max depth so that it finishes in a proper time
    max_depth = 4;
}

Move StudentAI::GetMove(Move move) {
    if (move.seq.empty()) {
        player = 1;
    } else {
        board.makeMove(move, player == 1 ? 2 : 1);
    }
    Node *root = new Node(eval(board, player), 0, true);
    buildTree(root, board);

    ReturnObj result = searchTree(root, INT_MIN, INT_MAX);
    Move res = result.move;
    board.makeMove(res, player);
    deleteTree(root);

    return res;
}

// build the min/max tree
void StudentAI::buildTree(Node *cur_node, Board &cur_board) {
    // jump out of recursion if depth > max depth
    if (cur_node->depth >= max_depth)
        return;

    // determine next player
    int nxt_player = cur_node->is_max ? player : 3 - player;

    vector<vector<Move>> moves = cur_board.getAllPossibleMoves(nxt_player);
    for (auto &move_list: moves) {
        for (auto &nxt_move: move_list) {
            // copy board
            Board temp_board = cur_board;
            temp_board.makeMove(nxt_move, nxt_player);
            // evaluate the value after that move
            int nxt_value = eval(temp_board, player);
            Node *child = new Node(nxt_value, cur_node->depth + 1, !cur_node->is_max);
            child->last_move = nxt_move;
            // build child
            buildTree(child, temp_board);
            cur_node->children.push_back(child);
        }
    }
}

// Evaluate
int StudentAI::eval(const Board &cur_board, const int ai_player) {
    string ai_color = ai_player == 1 ? "B" : "W";
    string opponent_color = ai_player == 1 ? "W" : "B";
    int res = 0;
    int pawn_value = 3;
    int king_value = 6;
    for (int i = 0; i < cur_board.row; ++i) {
        for (int j = 0; j < cur_board.col; ++j) {
            Checker checker = cur_board.board[i][j];
            if (checker.color == ai_color) {
                // king's value - border punishment
                // pawn's value + position bonus
                if (checker.isKing)
                    res += king_value - (i == 0 || i == cur_board.row - 1);
                else
                    res += pawn_value + (ai_player == 1 ? (i > cur_board.row / 2) : (i < cur_board.row / 2));
            } else if (checker.color == opponent_color) {
                if (checker.isKing)
                    res -= king_value - (i == 0 || i == cur_board.row - 1);
                else
                    res -= pawn_value + (ai_player == 1 ? (i < cur_board.row / 2) : (i > cur_board.row / 2));
            }
        }
    }
    return res;
}

// MINMAX
ReturnObj StudentAI::searchTree(Node *node, int alpha, int beta) const {
    if (node->depth >= max_depth)
        return {node->value, node->last_move};
    ReturnObj return_obj(node->value, node->last_move);
    if (node->is_max) {
        // max node
        int best_value = INT_MIN;
        for (auto &i: node->children) {
            ReturnObj obj = searchTree(i, alpha, beta);
            if (obj.value > best_value) {
                return_obj.value = i->value;
                return_obj.move = i->last_move;
                best_value = obj.value;
            }
            alpha = max(alpha, best_value);
            if (beta <= alpha) break;
        }
    } else {
        // min node
        int best_value = INT_MAX;
        for (auto &i: node->children) {
            ReturnObj obj = searchTree(i, alpha, beta);
            if (obj.value < best_value) {
                return_obj.value = i->value;
                return_obj.move = i->last_move;
                best_value = obj.value;
            }
            beta = min(beta, best_value);
            if (beta <= alpha) break;
        }
    }
    return return_obj;
}

// release memory
void StudentAI::deleteTree(Node *node) {
    if (!node) return;
    for (auto &i: node->children) deleteTree(i);
    delete node;
}
