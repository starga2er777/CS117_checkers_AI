#include "StudentAI.h"

// static members:
int StudentAI::ai_player;
const float StudentAI::c = 2.5f;
vector<Node *> StudentAI::start_nodes;

// The following part should be completed by students.
// The students can modify anything except the class name and existing functions and variables.
StudentAI::StudentAI(int col, int row, int p)
        : AI(col, row, p) {
    board = Board(col, row, p);
    board.initializeGame();
    player = 2;
    // set max simulation time (s)
    computeTime = 2.0f;
}

Move StudentAI::GetMove(Move move) {
    if (move.seq.empty()) {
        player = 1;
    } else {
        board.makeMove(move, player == 1 ? 2 : 1);
    }
    ai_player = player;
    Move res = Monte_Carlo_Tree_Search();
    return res;
}

Move StudentAI::Monte_Carlo_Tree_Search() {
    Node *root;
    root = new Node(this->board, this->player);
    // set the start node as the root
    root->parent = nullptr;
    auto start = chrono::system_clock::now();
    int sim_cnt = 0;
    while ((chrono::duration<double>(chrono::system_clock::now() - start).count() < computeTime) and
           root->unfinished_num > 0) {
//    while (root->unfinished_num > 0) {
        Node *node = select(root);
        float simulation_result = simulate(node);
        backPropagation(node, simulation_result);
        ++sim_cnt;
    }
    // printf("%d simulations processed.\n", sim_cnt);
    Move result_move = chooseBest(root);
    board.makeMove(result_move, player);
    return result_move;
}

float StudentAI::simulate(Node *cur_node) {
    // copy game board
    Board cur_board = cur_node->board;
    int turn = cur_node->player;
    while (true) {
        int res = cur_board.isWin(getOpponent(turn));
        // if game is over
        if (res != 0) {
            if (res == ai_player)
                return 1.0f;
            else if (res == -1)
                return 0.5f;
            else {
                return 0.0f;
            }
        }
        vector<vector<Move>> valid_moves = cur_board.getAllPossibleMoves(turn);
        // get a random move
        int idx_1 = RandomGen::getRandomInt(valid_moves.size());
        int idx_2 = RandomGen::getRandomInt(valid_moves[idx_1].size());
        cur_board.makeMove(valid_moves[idx_1][idx_2], turn);
        turn = getOpponent(turn);
    }
}

void StudentAI::backPropagation(Node *node, float val) {
    Node *cur_node = node;
    while (cur_node->parent) {
        cur_node->visits += 1;
        cur_node->wins += val;
        cur_node = cur_node->parent;
    }

    // repeat for root
    cur_node->visits += 1;
    cur_node->wins += val;
}

Move StudentAI::chooseBest(Node *root) {
    float max_wins = -1;
    float max_visits = -1;
    Move res;
    for (auto &i: root->children) {
        float child_wins = i->wins;
        float child_visits = i->visits;

        if (child_visits > max_visits) {
            max_visits = child_visits;
            max_wins = child_wins;
            res = i->move;
        } else if (child_visits == max_visits) {
            if (child_wins > max_wins) {
                max_wins = child_wins;
                res = i->move;
            }
        }
    }
    return res;
}


Node *select(Node *root) {
    while (root->unfinished_num > 0) {
        auto moves = root->board.getAllPossibleMoves(root->player);
        if (root->board.isWin(root->player)) {
            // game over
            root->propagateComplete();
            return root;
        } else if (root->unfinished_num > root->children.size()) {
            // expand a child node
            int rand_idx = RandomGen::getRandomInt(root->unexpanded_moves.size());
            auto it = root->unexpanded_moves.begin();
            for (int i = 0; i < rand_idx; ++i)
                it++;
            Move nxt_move = *it;
            // expand this unexpanded move
            root->unexpanded_moves.erase(it);
            // create child node
            Node *new_child = root->expand(nxt_move);
            new_child->parent = root;
            // TODO: add next start state
            return new_child;
        } else {
            // pick the most ideal child from all expanded child (UCB)
            bool is_opponent = root->player != StudentAI::ai_player;
            float node_visits = root->visits;
            vector<double> values;
            for (Node *child: root->children) {
                float visits = child->visits;
                float wins = is_opponent ? visits - child->wins : child->wins;
                //values.push_back(((double) wins / visits) + StudentAI::c * sqrt(2.0f * log(node_visits) / visits));
                values.push_back(((double) wins / visits) + StudentAI::c * sqrt(log(node_visits) / visits));
            }
            long long idx = max_element(values.begin(), values.end()) - values.begin();
            return root->children[idx];
        }
    }
    return nullptr;
}

// After all children of this move have been expanded to completion, tell the parent that it has one fewer expandable children
void Node::propagateComplete() {
    if (!parent)
        return;

    if (!unexpanded_moves.empty())
        --unfinished_num;

    parent->propagateComplete();
}

// expand a move: add a new child
Node *Node::expand(Move &expand_move) {
    Board new_board = this->board;
    new_board.makeMove(expand_move, this->player);
    Node *child_node = new Node(new_board, getOpponent(this->player));
    child_node->move = expand_move;
    this->children.push_back(child_node);
    return child_node;
}

