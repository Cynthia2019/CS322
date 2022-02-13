#include <tiling.h>
#include <queue>

namespace L3 {
    // TODO: what if tile is a line instead of a tree
    bool Tile::match(TreeNode *t, vector<TreeNode *> &subtrees) {
        queue<TreeNode *> tileque, treeque;
        vector<TreeNode *> subt;

        tileque.push(this->root);
        treeque.push(t);


        while (!tileque.empty()) {
            TreeNode *currtile = tileque.front();
            tileque.pop();
            TreeNode *currtree = treeque.front();
            treeque.pop();
            if (currtree == nullptr) {
                return false;
            }
            if (currtile->val != currtree->val) {
                return false;
            }
            if (currtile->oprand1) {
                tileque.push(currtile->oprand1);
                treeque.push(currtree->oprand1);
            } else if (currtree->oprand1){
                subt.push_back(currtree->oprand1);
            }
            if (currtile->oprand2) {
                tileque.push(currtile->oprand2);
                treeque.push(currtree->oprand2);
            } else if (currtree->oprand2){
                subt.push_back(currtree->oprand2);
            }
        }

        subtrees = subt;
        return true;
    }

    vector<L2::Instruction *> Tile::getInstructions() {
        return instructions;
    }

    int64_t Tile::getSize() {
        if (size == -1) {
            queue<TreeNode *> tileque;
            tileque.push(this->root);

            size = 0;
            while (!tileque.empty()) {
                TreeNode *currtile = tileque.front();
                tileque.pop();
                size++;
                if (currtile->oprand1) {
                    tileque.push(currtile->oprand1);
                }
                if (currtile->oprand2) {
                    tileque.push(currtile->oprand2);
                }
            }
        }
        return size;
    }

    void tiling(TreeNode *root, vector<Tile *>&res) {
        vector<Tile *> all_tiles;
        vector<TreeNode *> subtrees;
        for (auto t :all_tiles) {
            if (t->match(root, subtrees)) {
                res.push_back(t);
                break;
            }
        }

        for (auto sub: subtrees) {
            tiling(sub, res);
        }
    }

}