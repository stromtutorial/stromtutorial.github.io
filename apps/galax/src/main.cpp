#if 1

#include <iostream> ///start
#include <limits> ///start
#include "strom.hpp"
#include "galax.hpp"

using namespace strom;

// static data member initializations
const unsigned Galax::_ALLSUBSETS = std::numeric_limits<unsigned>::max();
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

#endif

#if 0
#include <iostream>
#include <regex>

int main() {
    std::string text = "some {test} data {asdf} more";
    std::regex re("\\{([^\\}]*)\\}");
    std::string out;
    std::string tmp;
    auto it = text.cbegin();
    auto end = text.cend();
    std::smatch match;
    for (; std::regex_search(it, end, match, re); it = match[0].second) {
        tmp.clear();
        tmp.append(it,end);
        std::cout << "tmp = |" << tmp << "|" << std::endl;
        out += "\nprefix = |";
        out += match.prefix();
        out += "|\nmatch  = |";
        out += match.str(); // replace here
        out += "|";
    }
    out += "\nfinal  = |";
    out.append(it, end);
    out += "|";
    std::cout << out << std::endl;
}
#endif

