
#include <tiling.h>
#include <iostream>
#include <queue>

using namespace std;

extern bool is_debug;
namespace L3 {

    bool TileRule_one::verify(TreeNode *tree) {
        Number *n;
        if (n = dynamic_cast<Number *>(tree->val)) {
            return n->get() == 1;
        } else {
            return false;
        }
    }

    bool TileRule_1248::verify(TreeNode *tree) {
        Number *n;
        if (n = dynamic_cast<Number *>(tree->val)) {
            return n->get() == 1 || n->get() == 2 || n->get() == 4 || n->get() == 8;
        } else {
            return false;
        }
    }

    bool TileNode::isLeaf() {
        return oprand1 == nullptr && oprand2 == nullptr;
    }

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

        if (rule && !rule->verify(tree)) {
            return false; // it obeys our rule
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
            return true; // tile is null, it matches any tree
        }
        if (tree == nullptr) {
            if (is_debug) cout << "tree pt is null" << endl;
            return false; // tile has this node, tree doen't have
        }
        bool curr = tile->match(tree); // match op and type
        if (!curr) {
            if (is_debug) cout << "op or type not match" << endl;
            return false;
        }


        bool mapchaged = false;
        // if this item is required to be the same as previous variables
        if (nodemap.count({tile->tile_type, tile->id}) != 0) {
            if (tree->val != nodemap[{tile->tile_type, tile->id}]->val) {
                if (is_debug) cout << "not same variable" << endl;
                return false; // different from previous stored variable
            }
        } else {
            // haven't seen it yet, store it
            mapchaged = true;
            nodemap[{tile->tile_type, tile->id}] = tree;
        }

        if (tile->isLeaf()) {
            if (is_debug) cout << "pushing: " << endl;
            subtrees.push_back(tree);
        }
        bool left = match_helper(tile->oprand1, tree->oprand1, subtrees, nodemap);
        bool right = match_helper(tile->oprand2, tree->oprand2, subtrees, nodemap);

        if (left && right)
            return true;

        left = match_helper(tile->oprand1, tree->oprand2, subtrees, nodemap);
        right = match_helper(tile->oprand2, tree->oprand1, subtrees, nodemap);
        if (left && right)
            return true;
        
        if (mapchaged) {
            nodemap.erase({tile->tile_type, tile->id});
        }
        return false;
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
        root->id = 0;
        root->tile_type |= TileNodeTypeVariable;
        root->op = new Operation(op);
        root->oprand1 = new TileNode();
        root->oprand1->tile_type |= TileNodeTypeNumber;
        root->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1->id = 1;
        root->oprand2 = new TileNode();
        root->oprand2->tile_type |= TileNodeTypeNumber;
        root->oprand2->tile_type |= TileNodeTypeVariable;
        root->oprand2->id = 2;
    }
    Tile_math_specialized::Tile_math_specialized(std::string op, bool left) {
        this->root = new TileNode();
        root->id = 0;
        root->tile_type |= TileNodeTypeVariable;
        root->op = new Operation(op);
        root->oprand1 = new TileNode();
        root->oprand2 = new TileNode();
        //dst = oprand1
        if (left) {
            root->oprand1->tile_type |= TileNodeTypeVariable;
            root->oprand1->id = 0;
            root->oprand2->tile_type |= TileNodeTypeNumber;
            root->oprand2->tile_type |= TileNodeTypeVariable;
            root->oprand2->id = 1;
        } else { //dst = oprand2
            root->oprand2->tile_type |= TileNodeTypeVariable;
            root->oprand2->id = 0;
            root->oprand1->tile_type |= TileNodeTypeNumber;
            root->oprand1->tile_type |= TileNodeTypeVariable;
            root->oprand1->id = 1;
        }

    }

    Tile_assign::Tile_assign() {
        root = new TileNode();
        root->op = new Operation("<-");
        root->id = 0;
        root->tile_type |= TileNodeTypeVariable;
        root->oprand1 = new TileNode();
        root->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1->tile_type |= TileNodeTypeLabel;
        root->oprand1->tile_type |= TileNodeTypeNumber;
        root->id = 1;
    }

    Tile_load::Tile_load() {
        root = new TileNode();
        root->op = new Operation("load");
        root->id = 0;
        root->tile_type |= TileNodeTypeVariable;
        root->oprand1 = new TileNode();
        root->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1->id = 1;
    }

    Tile_store::Tile_store() {
        root = new TileNode();
        root->op = new Operation("store");
        root->id = 0;
        root->tile_type |= TileNodeTypeVariable;
        root->oprand1 = new TileNode();
        root->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1->tile_type |= TileNodeTypeLabel;
        root->oprand1->tile_type |= TileNodeTypeNumber;
        root->oprand1->id = 1;
    }

    Tile_br::Tile_br() {
        root = new TileNode(); 
        root->op = new Operation("br"); 
        root->id = 0; 
        root->tile_type |= TileNodeTypeOp; 
        root->oprand1 = new TileNode(); 
        root->oprand1->tile_type |= TileNodeTypeLabel;
        root->oprand1->id = 1; 
    }

    Tile_br_t::Tile_br_t() {
        root = new TileNode(); 
        root->op = new Operation("br"); 
        root->id = 0; 
        root->tile_type |= TileNodeTypeOp; 
        root->oprand1 = new TileNode(); 
        root->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1->tile_type |= TileNodeTypeNumber;
        root->oprand1->id = 1; 
        root->oprand2 = new TileNode(); 
        root->oprand2->tile_type |= TileNodeTypeLabel;
        root->oprand2->id = 2; 
    }

    Tile_return::Tile_return() {
        root = new TileNode(); 
        root->id = 0; 
        root->tile_type |= TileNodeTypeOp;
    }
    Tile_return_t::Tile_return_t() {
        root = new TileNode(); 
        root->id = 0; 
        root->tile_type |= TileNodeTypeOp;
        root->oprand1 = new TileNode(); 
        root->oprand1->tile_type |= TileNodeTypeVariable; 
        root->oprand1->tile_type |= TileNodeTypeNumber;
        root->oprand1->id = 1;         
    }
    Tile_increment::Tile_increment(bool is_increment, bool left) {
        root = new TileNode();
        root->id = 0;
        root->tile_type |= TileNodeTypeVariable;
        TileNode *var, *num;
        if (is_increment) {
            root->op = new Operation("+");
        } else {
            root->op = new Operation("-");
        }
        var = new TileNode();
        var->tile_type |= TileNodeTypeVariable;
        var->id = 0;
        num = new TileNode();
        num->tile_type |= TileNodeTypeNumber;
        num->rule = new TileRule_one();
        num->id = 1;

        if (left) {
            root->oprand1 = var;
            root->oprand2 = num;
        } else {
            root->oprand2 = var;
            root->oprand1 = num;
        }
    }

    Tile_at::Tile_at() {
        root = new TileNode();
        root->id = 0;
        root->op = new Operation("+");
        root->tile_type |= TileNodeTypeVariable;

        root->oprand1 = new TileNode();
        root->oprand1->op = new Operation("*");
        root->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1->id = 1;

        root->oprand2 = new TileNode();
        root->oprand2->tile_type |= TileNodeTypeVariable;
        root->oprand2->id = 2;

        root->oprand1->oprand1 = new TileNode();
        root->oprand1->oprand1->tile_type |= TileNodeTypeVariable;
        root->oprand1->oprand1->id = 3;

        root->oprand1->oprand2 = new TileNode();
        root->oprand1->oprand2->tile_type |= TileNodeTypeNumber;
        root->oprand1->oprand2->id = 4;
        root->oprand1->oprand2->rule = new TileRule_1248();
    }

    void tiling(TreeNode *root, vector<Tile *>&res, const vector<Tile *> all_tiles) {
        vector<TreeNode *> subtrees;
        bool flag = false;
        if (root->oprand1 == nullptr && root->oprand2 == nullptr) {
            return; // a leaf node skip
        }
        for (auto t :all_tiles) {
            if (t->match(root, subtrees)) {
                flag = true;
                cout << "matched: " << endl;
                root->printNode(root, 0);
                res.push_back(t);
                break;
            }
        }

        if (flag) {
            for (auto sub: subtrees) {
                tiling(sub, res, all_tiles);
            }
        } else {
            cerr << "warning: cannot match to any tile." << endl;
        }
        return;
    }

    
    vector<Tile *> getAllTiles() {
        Tile *at = new Tile_at();
        Tile *inc_left = new Tile_increment("+", true);
        // Tile *inc_right = new Tile_increment("+", false);
        Tile *dec_left = new Tile_increment("-", true);
        // Tile *dec_right = new Tile_increment("-", false);
        Tile *plus_left = new Tile_math_specialized("+", true);
        // Tile *plus_right = new Tile_math_specialized("+", false);

        Tile *minus_left = new Tile_math_specialized("-", true);
        // Tile *minus_right = new Tile_math_specialized("-", false);
        Tile *mul_left = new Tile_math_specialized("*", true);
        // Tile *mul_right = new Tile_math_specialized("*", false);
        Tile *and_left = new Tile_math_specialized("&", true);
        // Tile *and_right = new Tile_math_specialized("&", false);
        Tile *sl_left = new Tile_math_specialized("<<", true);
        // Tile *sl_right = new Tile_math_specialized("<<", false);
        Tile *sr_left = new Tile_math_specialized(">>", true);
        // Tile *sr_right = new Tile_math_specialized(">>", false);
        Tile *plus_g = new Tile_math("+");
        Tile *minus_g = new Tile_math("-");
        Tile *mul_g = new Tile_math("*");
        Tile *and_g = new Tile_math("&");
        Tile *sl_g = new Tile_math("<<");
        Tile *sr_g = new Tile_math(">>");
        Tile *ass = new Tile_assign();
        Tile *load = new Tile_load();
        Tile *store = new Tile_store();


        vector<Tile *> all_tiles = {
            at,
            inc_left,
            // inc_right,
            dec_left,
            // dec_right,
            plus_left,
            // plus_right,
            minus_left,
            // minus_right,
            mul_left,
            // mul_right,
            and_left,
            // and_right,
            sl_left,
            // sl_right,
            sr_left,
            // sr_right,
            plus_g,
            minus_g,
            mul_g,
            and_g,
            sl_g,
            sr_g,
            ass,
            load,
            store,
        };

        return all_tiles;
    }

}