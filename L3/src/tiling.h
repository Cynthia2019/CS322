#include <L3.h>
#include <L2.h>
#include <instructionSelect.h>

namespace L3 {
    class Tile : public Tree {
        public:
        Tile();
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
        vector<L2::Instruction *> getInstructions();
        
        private:
        TreeNode *root;
        int64_t size = -1;
        vector<L2::Instruction *> instructions;
    };

    /**
     * @brief do tiling using biggest munch
     * 
     * @param root the root of the tree to be tiled
     * @param res the result
     */
    void tiling(TreeNode *root, vector<Tile *>&res);
}