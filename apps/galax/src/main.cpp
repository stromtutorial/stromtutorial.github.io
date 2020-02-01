#include <iostream> ///start
#include <limits> ///start
#include "strom.hpp"
#include "phyloinfo.hpp"

using namespace strom;

// static data member initializations
//const unsigned Galax::_ALLSUBSETS = std::numeric_limits<unsigned>::max();
unsigned GalaxInfo::_sortby_index = 0;
std::string     Strom::_program_name        = "galax";
unsigned        Strom::_major_version       = 1;
unsigned        Strom::_minor_version       = 0;
const double    Node::_smallest_edge_length = 1.0e-12;
//const unsigned  Galax::_ALLSUBSETS          = std::numeric_limits<unsigned>::max();

int main(int argc, const char * argv[]) {
    Strom strom;
    try {
        strom.processCommandLineOptions(argc, argv);
        strom.run();
    }
    catch(std::exception & x) {
        std::cerr << "Exception: " << x.what() << std::endl;
        std::cerr << "Aborted." << std::endl;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}   ///end

