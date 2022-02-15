
#pragma once
#include <L3.h>
#include <../L2/src/L2.h>
#include <stdint.h>
// #include <L2.h>
#include <instructionSelect.h>

namespace L3 {
    
    class TileRule {
        public:
        virtual bool verify(TreeNode *tree) = 0;
    };

    class TileRule_one: public TileRule {
        public:
        bool verify(TreeNode *tree) override;
    };

    class TileRule_1248: public TileRule {
        public:
        bool verify(TreeNode *) override;
    };

    class TileNode {
#define TileNodeTypeNumber 0x0001
#define TileNodeTypeVariable 0x0002
#define TileNodeTypeLabel 0x0004
#define TileNodeTypeString 0x0008
#define TileNodeTypeOp 0x0010

        public:
        TreeNode *matched_node = nullptr;
        unsigned short tile_type = 0;
        int64_t id = 0;
        Operation *op = nullptr;
        TileNode *oprand1 = nullptr;
        TileNode *oprand2 = nullptr;
        bool match(TreeNode *);
        bool isLeaf();
        TileRule *rule = nullptr;
    };

    class Tile {
        public:
        std::string name;
        // Tile();
        /**
         * @brief check if current tile matches a tree.
         * 
         * @param subtrees if the match process is successful, this is set to
         * be a list of subtrees. do not use this when return value is false.
         * @return true if matching is successful
         * @return false unsuccessful
         */
        bool match(TreeNode *, set<TreeNode *> &subtrees);
        int64_t getSize();
        // void accept();
        // vector<L2::Instruction *> getInstructions();
        virtual Tile *clone() = 0;
        
        private:
        int64_t size = -1;
        // vector<L2::Instruction *> instructions;
        protected:
        TileNode *root;
    };

    class Tile_math: public Tile {
        public:
        Tile_math(std::string op);
        Tile *clone() { return new Tile_math(root->op->toString()); };
    };

    class Tile_compare: public Tile {
        public:
        Tile_compare(std::string op);
        Tile *clone() { return new Tile_compare(root->op->toString()); };
    };

    class Tile_assign: public Tile {
        public:
        Tile_assign();
        Tile *clone() { return new Tile_assign(); };
    };

    class Tile_load: public Tile {
        public:
        Tile_load();
        Tile *clone() { return new Tile_load(); };
    };

    class Tile_store: public Tile {
        public:
        Tile_store();
        Tile *clone() { return new Tile_store(); };
    };

    class Tile_br: public Tile {
        public: 
        Tile_br();
        Tile *clone() { return new Tile_br(); };
    };

    class Tile_br_t: public Tile {
        public: 
        Tile_br_t();
        Tile *clone() { return new Tile_br_t(); };
    };

    class Tile_return : public Tile {
        public:
        Tile_return();
        Tile *clone() { return new Tile_return(); };
    };

    class Tile_return_t : public Tile {
        public:
        Tile_return_t();
        Tile *clone() { return new Tile_return_t(); };
    };

    /**
     * @brief *= += -= <<= >>= &=
     * 
     */
    class Tile_math_specialized: public Tile {
        public:
        Tile_math_specialized(std::string);
        Tile *clone() { return new Tile_math_specialized(root->op->toString()); };
    };

    class Tile_increment: public Tile {
        public:
        bool inc_or_dec;
        Tile_increment(bool inc_or_dec);
        Tile *clone() { return new Tile_increment(inc_or_dec); };
    };

    class Tile_at: public Tile {
        public:
        Tile_at();
        Tile *clone() { return new Tile_at(); };
    };

    /**
     * @brief do tiling using biggest munch
     * 
     * @param root the root of the tree to be tiled
     * @param res the result
     */
    void tiling(TreeNode *root, vector<Tile *>&res, const vector<Tile *> all_tiles);
    vector<Tile *> getAllTiles();
}