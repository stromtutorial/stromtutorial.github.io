#pragma once    ///start

#include <iostream>
#include "tree_summary.hpp"
#include <boost/program_options.hpp>

namespace strom {

    class Strom {
        public:
                                Strom();
                                ~Strom();

            void                clear();
            void                processCommandLineOptions(int argc, const char * argv[]);
            void                run();

        private:

            std::string            _data_file_name;
            std::string            _tree_file_name;

            TreeSummary::SharedPtr _tree_summary;

            static std::string     _program_name;
            static unsigned        _major_version;
            static unsigned        _minor_version;

    };

    // member function bodies go here
    ///end_class_declaration
    inline Strom::Strom() { ///begin_constructor
        //std::cout << "Constructing a Strom" << std::endl;
        clear();
    } ///end_constructor

    inline Strom::~Strom() { ///begin_destructor
        //std::cout << "Destroying a Strom" << std::endl;
    } ///end_destructor

    inline void Strom::clear() {    ///begin_clear
        _data_file_name = "";
        _tree_file_name = "";
        _tree_summary   = nullptr;
    }   ///end_clear

    inline void Strom::processCommandLineOptions(int argc, const char * argv[]) {   ///begin_processCommandLineOptions
        boost::program_options::variables_map vm;
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("version,v", "show program version")
            ("datafile,d",  boost::program_options::value(&_data_file_name), "name of a data file in NEXUS format")
            ("treefile,t",  boost::program_options::value(&_tree_file_name)->required(), "name of a tree file in NEXUS format")
        ;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        try {
            const boost::program_options::parsed_options & parsed = boost::program_options::parse_config_file< char >("strom.conf", desc, false);
            boost::program_options::store(parsed, vm);
        }
        catch(boost::program_options::reading_file & x) {
            std::cout << "Note: configuration file (strom.conf) not found" << std::endl;
        }
        boost::program_options::notify(vm);

        // If user specified --help on command line, output usage summary and quit
        if (vm.count("help") > 0) {
            std::cout << desc << "\n";
            std::exit(1);
        }

        // If user specified --version on command line, output version and quit
        if (vm.count("version") > 0) {
            std::cout << boost::str(boost::format("This is %s version %d.%d") % _program_name % _major_version % _minor_version) << std::endl;
            std::exit(1);
        }
    }   ///end_processCommandLineOptions

    inline void Strom::run() {   ///begin_run
        std::cout << "Starting..." << std::endl;

        try {
            // Create a new TreeSummary object and let _tree_summary point to it
            _tree_summary = TreeSummary::SharedPtr(new TreeSummary());

            // Read the tree file specified by the user
            _tree_summary->readTreefile(_tree_file_name, 0);
            Tree::SharedPtr tree = _tree_summary->getTree(0);

            // Summarize the trees read
            _tree_summary->showSummary();
        }
        catch (XStrom & x) {
            std::cerr << "Strom encountered a problem:\n  " << x.what() << std::endl;
        }

        std::cout << "\nFinished!" << std::endl;
    }   ///end_run

} ///end
