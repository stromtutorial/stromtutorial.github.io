#pragma once    ///start

#include <iostream>
#include "data.hpp"             ///!a
#include "tree_summary.hpp"
#include "partition.hpp"        ///!b
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp> ///!c

namespace strom {

    class Strom {
        public:
                                        Strom();
                                        ~Strom();

            void                        clear();
            void                        processCommandLineOptions(int argc, const char * argv[]);
            void                        run();
        
        private:

            std::string                 _data_file_name;
            std::string                 _tree_file_name;
            
            Partition::SharedPtr        _partition;         ///!d
            Data::SharedPtr             _data;              ///!l

            TreeSummary::SharedPtr      _tree_summary;

            static std::string          _program_name;
            static unsigned             _major_version;
            static unsigned             _minor_version;

    };

    // member function bodies go here
    ///end_class_declaration
    inline Strom::Strom() {
        //std::cout << "Constructing a Strom" << std::endl;
        clear();
    }

    inline Strom::~Strom() {
        //std::cout << "Destroying a Strom" << std::endl;
    }

    inline void Strom::clear() {    ///begin_clear
        _data_file_name = "";
        _tree_file_name = "";
        _tree_summary   = nullptr;
        _partition.reset(new Partition());  ///!k
    }   ///end_clear

    inline void Strom::processCommandLineOptions(int argc, const char * argv[]) {   ///begin_processCommandLineOptions
        std::vector<std::string> partition_subsets; ///!e
        boost::program_options::variables_map vm;
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("version,v", "show program version")
            ("datafile,d",  boost::program_options::value(&_data_file_name)->required(), "name of a data file in NEXUS format") ///!f
            ("treefile,t",  boost::program_options::value(&_tree_file_name), "name of a tree file in NEXUS format") ///!g
            ("subset",  boost::program_options::value(&partition_subsets), "a string defining a partition subset, e.g. 'first:1-1234\3' or 'default[codon:standard]:1-3702'")   ///!h
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
    
        // If user specified --subset on command line, break specified partition subset     ///!i
        // definition into name and character set string and add to _partition
        if (vm.count("subset") > 0) {
            _partition.reset(new Partition());
            for (auto s : partition_subsets) {
                _partition->parseSubsetDefinition(s);
            }
        }   ///!j
    }   ///end_processCommandLineOptions

    inline void Strom::run() {  ///begin_run
        std::cout << "Starting..." << std::endl;
        std::cout << "Current working directory: " << boost::filesystem::current_path() << std::endl;   ///!cwd
        
        try {   ///!begin_try
            std::cout << "\n*** Reading and storing the data in the file " << _data_file_name << std::endl;
            _data = Data::SharedPtr(new Data());
            _data->setPartition(_partition);
            _data->getDataFromFile(_data_file_name);

            // Report information about data partition subsets
            unsigned nsubsets = _data->getNumSubsets();
            std::cout << "\nNumber of taxa: " << _data->getNumTaxa() << std::endl;
            std::cout << "Number of partition subsets: " << nsubsets << std::endl;
            for (unsigned subset = 0; subset < nsubsets; subset++) {
                DataType dt = _partition->getDataTypeForSubset(subset);
                std::cout << "  Subset " << (subset+1) << " (" << _data->getSubsetName(subset) << ")" << std::endl;
                std::cout << "    data type: " << dt.getDataTypeAsString() << std::endl;
                std::cout << "    sites:     " << _data->calcSeqLenInSubset(subset) << std::endl;
                std::cout << "    patterns:  " << _data->getNumPatternsInSubset(subset) << std::endl;
                }
            }   ///!end_try
        catch (XStrom & x) {
            std::cerr << "Strom encountered a problem:\n  " << x.what() << std::endl;
        }

        std::cout << "\nFinished!" << std::endl;
    }   ///end_run

}   ///end























