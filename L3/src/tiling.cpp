#include <tiling.h>
#include <queue>

namespace L3 {

    bool TileNode::match(TreeNode *tree) {
        if (!tree) {
            return false;
        }
        if (op) {
            if (!tree->op) {
                return false;
            } else if (tree->op->toString() != op->toString()) {
                return false;
            }
        }

        ItemType t = tree->val->getType();
        switch (t) {
        case item_label:
            return tile_type & TileNodeTypeLabel;
        case item_number:
            return tile_type & TileNodeTypeNumber;
        case item_variable:
            return tile_type & TileNodeTypeVariable;
        case item_string:
            return tile_type & TileNodeTypeString;
        case item_operation:
            return tile_type & TileNodeTypeOp;
        default:
            return false;
            break;
        }
    }

    bool match_helper(TileNode *tile, TreeNode *tree, vector<TreeNode *> &subtrees,
            std::map<pair<short, int64_t>, TreeNode *> &nodemap) {
        if (tile == nullptr) {
            if (tree != nullptr) {
                subtrees.push_back(tree);
            }
            return true; // tile is null, it matches any tree
        }
        if (tree == nullptr) {
            return false; // tile has this node, tree doen't have
        }
        bool curr = tile->match(tree); // match op and type
        if (!curr) return false;

        // if this item is required to be the same as previous variables
        if (nodemap.count({tile->tile_type, tile->id}) != 0) {
            if (tree->val != nodemap[{tile->tile_type, tile->id}]->val) {
                return false; // different from previous stored variable
            }
        } else {
            // haven't seen it yet, store it
            nodemap[{tile->tile_type, tile->id}] = tree;
        }

        bool left = match_helper(tile->oprand1, tree->oprand1, subtrees, nodemap);
        bool right = match_helper(tile->oprand2, tree->oprand2, subtrees, nodemap);
        return left && right;
    }

    // TODO: what if tile is a line instead of a tree
    bool Tile::match(TreeNode *t, vector<TreeNode *> &subtrees) {
        std::map<pair<short, int64_t>, TreeNode *> nodemap;
        return match_helper(root, t, subtrees, nodemap);
        // queue<TileNode *> tileque;
        // queue<TreeNode *> treeque;
        // vector<TreeNode *> subt;

        // tileque.push(this->root);
        // treeque.push(t);


        // while (!tileque.empty()) {
        //     TileNode *currtile = tileque.front();
        //     tileque.pop();
        //     TreeNode *currtree = treeque.front();
        //     treeque.pop();
        //     if (currtree == nullptr) {
        //         return false;
        //     }
        //     if (currtile->op->toString() != currtree->op->toString()) {
        //         return false;
        //     }

        //     // TODO: if val matches
        //     if (currtile->val != currtree->val) {
        //         return false;
        //     }

        //     if (currtile->oprand1) {
        //         tileque.push(currtile->oprand1);
        //         treeque.push(currtree->oprand1);
        //     } else if (currtree->oprand1){
        //         subt.push_back(currtree->oprand1);
        //     }
        //     if (currtile->oprand2) {
        //         tileque.push(currtile->oprand2);
        //         treeque.push(currtree->oprand2);
        //     } else if (currtree->oprand2){
        //         subt.push_back(currtree->oprand2);
        //     }
        // }

        // subtrees = subt;
        // return true;
    }

    // vector<L2::Instruction *> Tile::getInstructions() {
    //     return instructions;
    // }

    int64_t Tile::getSize() {
        if (size == -1) {
            queue<TileNode *> tileque;
            tileque.push(this->root);

            size = 0;
            while (!tileque.empty()) {
                TileNode *currtile = tileque.front();
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

    Tile_math::Tile_math(std::string op) {
        this->root = new TileNode();
        // root->id = 0;
        root->tile_type |= TileNodeTypeVariable;
        root->op = new Operation(op);
        root->oprand1 = new TileNode();
        root->oprand1->tile_type |= TileNodeTypeNumber;
        root->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1 = new TileNode();
        root->oprand1->tile_type |= TileNodeTypeNumber;
        root->oprand1->tile_type |= TileNodeTypeVariable;
    }

    void tiling(TreeNode *root, vector<Tile *>&res, const vector<Tile *> all_tiles) {
        vector<TreeNode *> subtrees;
        for (auto t :all_tiles) {
            if (t->match(root, subtrees)) {
                res.push_back(t);
                break;
            }
        }

        for (auto sub: subtrees) {
            tiling(sub, res, all_tiles);
        }
    }

}