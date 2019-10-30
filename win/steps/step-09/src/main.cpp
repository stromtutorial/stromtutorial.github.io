#include <iostream> ///start
#include "strom.hpp"

using namespace strom;

// static data member initializations
std::string  Strom::_program_name        = "strom";
unsigned     Strom::_major_version       = 1;
unsigned     Strom::_minor_version       = 0;
const double Node::_smallest_edge_length = 1.0e-12;
GeneticCode::genetic_code_definitions_t GeneticCode::_definitions = { ///!start_definitions
                             // codon order is alphabetical: i.e. AAA, AAC, AAG, AAT, ACA, ..., TTT
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
}; ///!end_definitions

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
} ///end
