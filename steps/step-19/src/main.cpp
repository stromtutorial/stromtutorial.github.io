#include <limits>   ///start
#include <iostream>
#include "strom.hpp"
#include "debug_stuff.hpp"  //DEBUGSTUFF

using namespace strom;

// static data member initializations
bool     DebugStuff::_ignore         = true;    //DEBUGSTUFF
unsigned DebugStuff::_partial_offset = 0;       //DEBUGSTUFF
unsigned DebugStuff::_tmatrix_offset = 0;       //DEBUGSTUFF
unsigned DebugStuff::_which_iter     = 0;       //DEBUGSTUFF
unsigned DebugStuff::_tree_index     = 0;       //DEBUGSTUFF

std::string  Strom::_program_name        = "strom";
unsigned     Strom::_major_version       = 1;
unsigned     Strom::_minor_version       = 0;
const double Node::_smallest_edge_length = 1.0e-12;
const double Updater::_log_zero          = -std::numeric_limits<double>::max();
GeneticCode::genetic_code_definitions_t GeneticCode::_definitions = { // codon order is alphabetical: i.e. AAA, AAC, AAG, AAT, ACA, ..., TTT
    {"standard",             "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSS*CWCLFLF"},
    {"vertmito",             "KNKNTTTT*S*SMIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSWCWCLFLF"},
    {"yeastmito",            "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSWCWCLFLF"},
    {"moldmito",             "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSWCWCLFLF"},
    {"invertmito",           "KNKNTTTTSSSSMIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSWCWCLFLF"},
    {"ciliate",              "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVVQYQYSSSS*CWCLFLF"},
    {"echinomito",           "NNKNTTTTSSSSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSWCWCLFLF"},
    {"euplotid",             "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSCCWCLFLF"},
    {"plantplastid",         "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSS*CWCLFLF"},
    {"altyeast",             "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLSLEDEDAAAAGGGGVVVV*Y*YSSSS*CWCLFLF"},
    {"ascidianmito",         "KNKNTTTTGSGSMIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSWCWCLFLF"},
    {"altflatwormmito",      "NNKNTTTTSSSSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVVYY*YSSSSWCWCLFLF"},
    {"blepharismamacro",     "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*YQYSSSS*CWCLFLF"},
    {"chlorophyceanmito",    "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*YLYSSSS*CWCLFLF"},
    {"trematodemito",        "NNKNTTTTSSSSMIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSSWCWCLFLF"},
    {"scenedesmusmito",      "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*YLY*SSS*CWCLFLF"},
    {"thraustochytriummito", "KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSS*CWC*FLF"}
};

int main(int argc, const char * argv[]) {
    DebugStuff::_tree_index = 0;        //DEBUGSTUFF
    DebugStuff::debugOpenTreeFile();    //DEBUGSTUFF

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

    DebugStuff::debugCloseTreeFile();    //DEBUGSTUFF
    
#if 0   //POLTMP
    TreeSummary sumt;
    try {
        sumt.readTreefile("trees.tre", 1);
        }
    catch(NxsException x) {
        std::cerr << "Program aborting due to errors encountered reading tree file." << std::endl;
        std::cerr << x.what() << std::endl;
        std::exit(0);
        }
    sumt.showResClassSummary();
#endif

    return 0;
}   ///end
