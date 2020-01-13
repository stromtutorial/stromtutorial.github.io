#pragma once    ///start

#include <iostream>
#include "data.hpp"
#include "likelihood.hpp"   ///!a
#include "tree_summary.hpp"
#include "partition.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace strom {

    class Strom {
        public:
                                                    Strom();
                                                    ~Strom();

            void                                    clear();
            void                                    processCommandLineOptions(int argc, const char * argv[]);
            void                                    run();
        
        private:

            std::string                             _data_file_name;
            std::string                             _tree_file_name;
            Partition::SharedPtr                    _partition;

            Data::SharedPtr                         _data; 
            Likelihood::SharedPtr                   _likelihood;        ///!c
            TreeSummary::SharedPtr                  _tree_summary;

            bool                                    _use_gpu;           ///!d
            bool                                    _ambig_missing;     ///!e

            static std::string                      _program_name;
            static unsigned                         _major_version;
            static unsigned                         _minor_version;

    };
    ///end_class_declaration
    // member function bodies go here

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
        _partition.reset(new Partition());
        _use_gpu        = true; ///!f
        _ambig_missing  = true; ///!g
        _data = nullptr; 
        _likelihood = nullptr;
    }   ///end_clear

    inline void Strom::processCommandLineOptions(int argc, const char * argv[]) {   ///begin_processCommandLineOptions
        std::vector<std::string> partition_subsets;
        boost::program_options::variables_map vm;
        boost::program_options::options_description desc("Allowed options");    
        desc.add_options()
            ("help,h", "produce help message")
            ("version,v", "show program version")
            ("datafile,d",  boost::program_options::value(&_data_file_name)->required(), "name of a data file in NEXUS format")
            ("treefile,t",  boost::program_options::value(&_tree_file_name)->required(), "name of a tree file in NEXUS format") ///!treefilerequired
            ("subset",  boost::program_options::value(&partition_subsets), "a string defining a partition subset, e.g. 'first:1-1234\3' or 'default[codon:standard]:1-3702'")
            ("gpu",           boost::program_options::value(&_use_gpu)->default_value(true),                "use GPU if available")    ///!gpuoption                
            ("ambigmissing",  boost::program_options::value(&_ambig_missing)->default_value(true),          "treat all ambiguities as missing data")    ///!ambigmissingoption  
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
    
        // If user specified --subset on command line, break specified partition subset 
        // definition into name and character set string and add to _partition
        if (vm.count("subset") > 0) {
            _partition.reset(new Partition());
            for (auto s : partition_subsets) {
                _partition->parseSubsetDefinition(s);
            }
        }
    }   ///end_processCommandLineOptions

    inline void Strom::run() {  ///begin_run
        std::cout << "Starting..." << std::endl;
        std::cout << "Current working directory: " << boost::filesystem::current_path() << std::endl;

        try {
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
                std::cout << "    ambiguity: " << (_ambig_missing || dt.isCodon() ? "treated as missing data (faster)" : "handled appropriately (slower)") << std::endl;
                }
            
            std::cout << "\n*** Resources available to BeagleLib " << _likelihood->beagleLibVersion() << ":\n";
            std::cout << _likelihood->availableResources() << std::endl;

            std::cout << "\n*** Creating the likelihood calculator" << std::endl;
            _likelihood = Likelihood::SharedPtr(new Likelihood());
            _likelihood->setPreferGPU(_use_gpu);
            _likelihood->setAmbiguityEqualsMissing(_ambig_missing);
            _likelihood->setData(_data);
            _likelihood->initBeagleLib();

            std::cout << "\n*** Reading and storing the first tree in the file " << _tree_file_name << std::endl;
            _tree_summary = TreeSummary::SharedPtr(new TreeSummary());
            _tree_summary->readTreefile(_tree_file_name, 0);
            Tree::SharedPtr tree = _tree_summary->getTree(0);

            if (tree->numLeaves() != _data->getNumTaxa())
                throw XStrom(boost::format("Number of taxa in tree (%d) does not equal the number of taxa in the data matrix (%d)") % tree->numLeaves() % _data->getNumTaxa());

            std::cout << "\n*** Calculating the likelihood of the tree" << std::endl;
            double lnL = _likelihood->calcLogLikelihood(tree);
            std::cout << boost::str(boost::format("log likelihood = %.5f") % lnL) << std::endl;
            std::cout << "      (expecting -278.83767)" << std::endl;
        }
        catch (XStrom & x) {
            std::cerr << "Strom encountered a problem:\n  " << x.what() << std::endl;
        }

        std::cout << "\nFinished!" << std::endl;
    }   ///end_run 

}
