#include <L3.h>
#include <../L2/src/L2.h>
#include <stdint.h>
// #include <L2.h>
#include <instructionSelect.h>

namespace L3 {

    class TileNode {
#define TileNodeTypeNumber 0x0001
#define TileNodeTypeVariable 0x0002
#define TileNodeTypeLabel 0x0004
#define TileNodeTypeString 0x0008
#define TileNodeTypeOp 0x0010

        public:
        unsigned short tile_type = 0;
        int64_t id = 0;
        Operation *op = nullptr;
        TileNode *oprand1 = nullptr;
        TileNode *oprand2 = nullptr;
        bool match(TreeNode *);
        bool isLeaf();
    };

    class Tile {
        public:
        // Tile();
        /**
         * @brief check if current tile matches a tree.
         * 
         * @param subtrees if the match process is successful, this is set to
         * be a list of subtrees. left untouched otherwise.
         * @return true if matching is successful
         * @return false unsuccessful
         */
        bool match(TreeNode *, vector<TreeNode *> &subtrees);
        int64_t getSize();
        // void accept();
        // vector<L2::Instruction *> getInstructions();
        
        private:
        int64_t size = -1;
        // vector<L2::Instruction *> instructions;
        protected:
        TileNode *root;
    };

    class Tile_math: public Tile {
        Tile_math();
        Tile_math(std::string op);
    };

<<<<<<< HEAD
    class Tile_math_basic : public Tile_math {
        public:
        Tile_math_basic(string op);
    }; 
    class Tile_math_same_1 : public Tile_math {
        public:
        Tile_math_same_1(string op);
    };
    class Tile_math_same_2 : public Tile_math {
        public:
        Tile_math_same_2(string op);
    };
    class Tile_math_same_12 : public Tile_math {
        public:
        Tile_math_same_12(string op);
    };

=======
    class Tile_assign: public Tile {
        public:
        Tile_assign();
    };

    class Tile_load: public Tile {
        public:
        Tile_load();
    };

    class Tile_store: public Tile {
        public:
        Tile_store();
    };

    /**
     * @brief *= += -= <<= >>= &=
     * 
     */
    class Tile_math_specialized: public Tile {
        public:
        Tile_math_specialized(std::string, bool left);
    }
>>>>>>> 08ffd74e8ced3afa8c8fba1305a350aa8dd4bda6
    /**
     * @brief do tiling using biggest munch
     * 
     * @param root the root of the tree to be tiled
     * @param res the result
     */
    void tiling(TreeNode *root, vector<Tile *>&res, const vector<Tile *> all_tiles);
}