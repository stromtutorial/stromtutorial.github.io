#include <iostream> ///start
#include "node.hpp"
#include "tree.hpp"
#include "tree_manip.hpp"

using namespace strom;

const double Node::_smallest_edge_length = 1.0e-12;

int main(int argc, const char * argv[]) {
    std::cout << "Starting..." << std::endl;
    TreeManip tm;
    tm.createTestTree();
    std::cout << tm.makeNewick(3) << std::endl; ///!a
    std::cout << "\nFinished!" << std::endl;

    return 0;
}   ///end
