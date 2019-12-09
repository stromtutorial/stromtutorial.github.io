#include <limits>   ///start
#include <iostream>
#include "strom.hpp"

#include "debug_stuff.hpp"  //DEBUGSTUFF

using namespace strom;

//POLTMP
unsigned DebugStuff::_debug_add_tries   = 0;  //POLTMP
unsigned DebugStuff::_debug_add_accepts = 0;  //POLTMP
unsigned DebugStuff::_debug_del_tries   = 0;  //POLTMP
unsigned DebugStuff::_debug_del_accepts = 0;  //POLTMP

// static data member initializations
#if DEBUG_POLY  //POLY //POLTMP
bool     DebugStuff::_ignore         = false;    //DEBUGSTUFF
#else
bool     DebugStuff::_ignore         = true;    //DEBUGSTUFF
#endif
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
    
#if DEBUG_POLY  //POLY //POLTMP
    std::cerr << "Add-edge move:\n";
    std::cerr << boost::str(boost::format("%12d attempts") % DebugStuff::_debug_add_tries) << std::endl;
    std::cerr << boost::str(boost::format("%12d accepts") % DebugStuff::_debug_add_accepts) << std::endl;
    std::cerr << boost::str(boost::format("%12.1f acccept %%") % (100.0*DebugStuff::_debug_add_accepts/DebugStuff::_debug_add_tries)) << std::endl;
    std::cerr << "Del-edge move:\n";
    std::cerr << boost::str(boost::format("%12d attempts") % DebugStuff::_debug_del_tries) << std::endl;
    std::cerr << boost::str(boost::format("%12d accepts") % DebugStuff::_debug_del_accepts) << std::endl;
    std::cerr << boost::str(boost::format("%12.1f acccept %%") % (100.0*DebugStuff::_debug_del_accepts/DebugStuff::_debug_del_tries)) << std::endl;
#endif

    return 0;
}   ///end
