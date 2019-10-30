#pragma once

#include <iostream>
#include "data.hpp"
#include "likelihood.hpp"
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
            bool                                    processAssignmentString(const std::string & which, const std::string & definition);
            void                                    handleAssignmentStrings(const boost::program_options::variables_map & vm, std::string label, const std::vector<std::string> & definitions, std::string default_definition);
            bool                                    splitAssignmentString(const std::string & definition, std::vector<std::string> & vector_of_subset_names, std::vector<double>  & vector_of_values);

            double                                  _expected_log_likelihood;

            std::string                             _data_file_name;
            std::string                             _tree_file_name;
            Partition::SharedPtr                    _partition;

            Data::SharedPtr                         _data;
            Model::SharedPtr                        _model;
            Likelihood::SharedPtr                   _likelihood;
            TreeSummary::SharedPtr                  _tree_summary;

            bool                                    _use_gpu;
            bool                                    _ambig_missing;
            bool                                    _use_underflow_scaling; ///!a

            static std::string                      _program_name;
            static unsigned                         _major_version;
            static unsigned                         _minor_version;

    };

    // member function bodies go here

    inline Strom::Strom() {
        //std::cout << "Constructing a Strom" << std::endl;
        clear();
    }

    inline Strom::~Strom() {
        //std::cout << "Destroying a Strom" << std::endl;
    }

    inline void Strom::clear() {    ///begin_clear
        _data_file_name             = "";
        _tree_file_name             = "";
        _tree_summary               = nullptr;
        _partition.reset(new Partition());
        _use_gpu                    = true;
        _ambig_missing              = true;
        _model.reset(new Model());
        _expected_log_likelihood    = 0.0;
        _use_underflow_scaling      = false;    ///!b
        _data                       = nullptr; 
        _likelihood                 = nullptr;
    }

    inline void Strom::processCommandLineOptions(int argc, const char * argv[]) {
        std::vector<std::string> partition_statefreq;
        std::vector<std::string> partition_rmatrix;
        std::vector<std::string> partition_omega;
        std::vector<std::string> partition_ratevar;
        std::vector<std::string> partition_pinvar;
        std::vector<std::string> partition_ncateg;
        std::vector<std::string> partition_subsets;
        std::vector<std::string> partition_relrates;
        std::vector<std::string> partition_tree;
        boost::program_options::variables_map vm;
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("version,v", "show program version")
            ("datafile,d",  boost::program_options::value(&_data_file_name)->required(), "name of a data file in NEXUS format")
            ("treefile,t",  boost::program_options::value(&_tree_file_name)->required(), "name of a tree file in NEXUS format")
            ("subset",  boost::program_options::value(&partition_subsets), "a string defining a partition subset, e.g. 'first:1-1234\3' or 'default[codon:standard]:1-3702'")
            ("ncateg,c", boost::program_options::value(&partition_ncateg), "number of categories in the discrete Gamma rate heterogeneity model")
            ("statefreq", boost::program_options::value(&partition_statefreq), "a string defining state frequencies for one or more data subsets, e.g. 'first,second:0.1,0.2,0.3,0.4'")
            ("omega", boost::program_options::value(&partition_omega), "a string defining the nonsynonymous/synonymous rate ratio omega for one or more data subsets, e.g. 'first,second:0.1'")
            ("rmatrix", boost::program_options::value(&partition_rmatrix), "a string defining the rmatrix for one or more data subsets, e.g. 'first,second:1,2,1,1,2,1'")
            ("ratevar", boost::program_options::value(&partition_ratevar), "a string defining the among-site rate variance for one or more data subsets, e.g. 'first,second:2.5'")
            ("pinvar", boost::program_options::value(&partition_pinvar), "a string defining the proportion of invariable sites for one or more data subsets, e.g. 'first,second:0.2'")
            ("relrate", boost::program_options::value(&partition_relrates), "a string defining the (unnormalized) relative rates for all data subsets (e.g. 'default:3,1,6').")
            ("tree", boost::program_options::value(&partition_tree), "the index of the tree in the tree file (first tree has index = 1)")
            ("expectedLnL", boost::program_options::value(&_expected_log_likelihood)->default_value(0.0), "log likelihood expected")
            ("gpu",           boost::program_options::value(&_use_gpu)->default_value(true),                "use GPU if available")
            ("ambigmissing",  boost::program_options::value(&_ambig_missing)->default_value(true),          "treat all ambiguities as missing data")
            ("underflowscaling",  boost::program_options::value(&_use_underflow_scaling)->default_value(false),          "scale site-likelihoods to prevent underflow (slower but safer)") ///!c
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
        
        _model->setSubsetDataTypes(_partition->getSubsetDataTypes());
        
        handleAssignmentStrings(vm, "statefreq", partition_statefreq, "default:equal");
        handleAssignmentStrings(vm, "rmatrix",   partition_rmatrix,   "default:equal");
        handleAssignmentStrings(vm, "omega",     partition_omega,     "default:0.1"  );
        handleAssignmentStrings(vm, "ncateg",    partition_ncateg,    "default:1"    );
        handleAssignmentStrings(vm, "ratevar",   partition_ratevar,   "default:1.0"  );
        handleAssignmentStrings(vm, "pinvar",    partition_pinvar,    "default:0.0"  );
        handleAssignmentStrings(vm, "relrate",   partition_relrates,  "default:equal");
        handleAssignmentStrings(vm, "tree",      partition_tree,      "default:1"    ); 
    }  
    
    inline void Strom::handleAssignmentStrings(const boost::program_options::variables_map & vm, std::string label, const std::vector<std::string> & definitions, std::string default_definition) {
        if (vm.count(label) > 0) {
            bool first = true;
            for (auto s : definitions) {
                bool is_default = processAssignmentString(label, s);
                if (is_default && !first)
                    throw XStrom(boost::format("default specification must be first %s encountered") % label);
                first = false;
            }
        }
        else {
            processAssignmentString(label, default_definition);
        }
    }
    
    inline bool Strom::processAssignmentString(const std::string & which, const std::string & definition) {
        unsigned num_subsets_defined = _partition->getNumSubsets();
        std::vector<std::string> vector_of_subset_names;
        std::vector<double> vector_of_values;
        bool fixed = splitAssignmentString(definition, vector_of_subset_names, vector_of_values);
        
        if (vector_of_values.size() == 1 && vector_of_values[0] == -1 && !(which == "statefreq" || which == "rmatrix" || which == "relrate"))
            throw XStrom("Keyword equal is only allowed for statefreq, rmatrix, and relrate");

        // Assign values to subsets in model
        bool default_found = false;
        if (which == "statefreq") {
            QMatrix::freq_xchg_ptr_t freqs = std::make_shared<QMatrix::freq_xchg_t>(vector_of_values);
            if (vector_of_subset_names[0] == "default") {
                default_found = true;
                for (unsigned i = 0; i < num_subsets_defined; i++)
                    _model->setSubsetStateFreqs(freqs, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    _model->setSubsetStateFreqs(freqs, _partition->findSubsetByName(s), fixed);
                }
            }
        }
        else if (which == "rmatrix") {
            QMatrix::freq_xchg_ptr_t xchg = std::make_shared<QMatrix::freq_xchg_t>(vector_of_values);
            if (vector_of_subset_names[0] == "default") {
                default_found = true;
                for (unsigned i = 0; i < num_subsets_defined; i++)
                    _model->setSubsetExchangeabilities(xchg, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    _model->setSubsetExchangeabilities(xchg, _partition->findSubsetByName(s), fixed);
                }
            }
        }
        else if (which == "omega") {
            if (vector_of_values.size() > 1)
                throw XStrom(boost::format("expecting 1 value for omega, found %d values") % vector_of_values.size());
            QMatrix::omega_ptr_t omega = std::make_shared<QMatrix::omega_t>(vector_of_values[0]);
            if (vector_of_subset_names[0] == "default") {
                default_found = true;
                for (unsigned i = 0; i < num_subsets_defined; i++)
                    _model->setSubsetOmega(omega, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    _model->setSubsetOmega(omega, _partition->findSubsetByName(s), fixed);
                }
            }
        }
        else if (which == "pinvar") {
            if (vector_of_values.size() > 1)
                throw XStrom(boost::format("expecting 1 value for pinvar, found %d values") % vector_of_values.size());
            ASRV::pinvar_ptr_t p = std::make_shared<double>(vector_of_values[0]);
            bool invar_model = (*p > 0);
            if (vector_of_subset_names[0] == "default") {
                default_found = true;
                for (unsigned i = 0; i < num_subsets_defined; i++) {
                    _model->setSubsetIsInvarModel(invar_model, i);
                    _model->setSubsetPinvar(p, i, fixed);
                }
            }
            else {
                for (auto s : vector_of_subset_names) {
                    unsigned i = _partition->findSubsetByName(s);
                    _model->setSubsetIsInvarModel(invar_model, i);
                    _model->setSubsetPinvar(p, i, fixed);
                }
            }
        }
        else if (which == "ratevar") {
            if (vector_of_values.size() > 1)
                throw XStrom(boost::format("expecting 1 value for ratevar, found %d values") % vector_of_values.size());
            ASRV::ratevar_ptr_t rv = std::make_shared<double>(vector_of_values[0]);
            if (vector_of_subset_names[0] == "default") {
                default_found = true;
                for (unsigned i = 0; i < num_subsets_defined; i++)
                    _model->setSubsetRateVar(rv, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    _model->setSubsetRateVar(rv, _partition->findSubsetByName(s), fixed);
                }
            }
        }
        else if (which == "ncateg") {
            if (vector_of_values.size() > 1)
                throw XStrom(boost::format("expecting 1 value for ncateg, found %d values") % vector_of_values.size());
            unsigned ncat = vector_of_values[0];
            if (vector_of_subset_names[0] == "default") {
                default_found = true;
                for (unsigned i = 0; i < num_subsets_defined; i++)
                    _model->setSubsetNumCateg(ncat, i);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    _model->setSubsetNumCateg(ncat, _partition->findSubsetByName(s));
                }
            }
        }
        else if (which == "tree") {
            if (vector_of_values.size() > 1)
                throw XStrom(boost::format("expecting 1 value for tree, found %d values") % vector_of_values.size());
            unsigned tree_index = vector_of_values[0];
            assert(tree_index > 0);
            _model->setTreeIndex(tree_index - 1, fixed);
            if (vector_of_subset_names[0] != "default")
                throw XStrom("tree must be assigned to default only");
        }
        else {
            assert(which == "relrate");
            if (vector_of_subset_names[0] != "default")
                throw XStrom("relrate must be assigned to default only");
            _model->setSubsetRelRates(vector_of_values, fixed);
        }

        return default_found;
    }

    inline bool Strom::splitAssignmentString(const std::string & definition, std::vector<std::string> & vector_of_subset_names, std::vector<double>  & vector_of_values) {
        // Split subset names from definition
        std::vector<std::string> twoparts;
        boost::split(twoparts, definition, boost::is_any_of(":"));
        if (twoparts.size() != 2)
            throw XStrom("Expecting exactly one colon in assignment");
        std::string comma_delimited_subset_names_string = twoparts[0];
        std::string comma_delimited_value_string = twoparts[1];
        boost::to_lower(comma_delimited_value_string);
        
        // Check for brackets indicating that parameter should be fixed
        // now see if before_colon contains a data type specification in square brackets
        bool fixed = false;
        const char * pattern_string = R"(\s*\[(.+?)\]\s*)";
        std::regex re(pattern_string);
        std::smatch match_obj;
        bool matched = std::regex_match(comma_delimited_value_string, match_obj, re);
        if (matched) {
            comma_delimited_value_string = match_obj[1];
            fixed = true;
        }
        
        if (comma_delimited_value_string == "equal") {
            vector_of_values.resize(1);
            vector_of_values[0] = -1;
        }
        else {
            // Convert comma_delimited_value_string to vector_of_strings
            std::vector<std::string> vector_of_strings;
            boost::split(vector_of_strings, comma_delimited_value_string, boost::is_any_of(","));

            // Convert vector_of_strings to vector_of_values (single values in case of ratevar, ncateg, and pinvar)
            vector_of_values.resize(vector_of_strings.size());
            std::transform(
                vector_of_strings.begin(),      // start of source vector
                vector_of_strings.end(),        // end of source vector
                vector_of_values.begin(),       // start of destination vector
                [](const std::string & vstr) {  // anonymous function that translates
                    return std::stod(vstr);     // each string element to a double
                }
            );
            assert(vector_of_values.size() > 0);
        }
        
        // Split comma_delimited_subset_names_string into vector_of_subset_names
        boost::split(vector_of_subset_names, comma_delimited_subset_names_string, boost::is_any_of(","));
        
        // Sanity check: at least one subset name must be provided
        if (vector_of_subset_names.size() == 0) {
            XStrom("At least 1 subset must be provided in assignments (use \"default\" if not partitioning)");
        }
        
        // Sanity check: if no partition was defined, then values should be assigned to "default" subset
        // and if "default" is in the list of subset names, it should be the only thing in that list
        unsigned num_subsets_defined = _partition->getNumSubsets();
        std::vector<std::string>::iterator default_iter = std::find(vector_of_subset_names.begin(), vector_of_subset_names.end(), std::string("default"));
        bool default_found = (default_iter != vector_of_subset_names.end());
        if (default_found) {
            if (vector_of_subset_names.size() > 1)
                throw XStrom("The \"default\" specification cannot be mixed with other subset-specific parameter specifications");
        }
        else if (num_subsets_defined == 0) {
            throw XStrom("Must specify partition before assigning values to particular subsets (or assign to subset \"default\")");
        }
        return fixed;
    }

    inline void Strom::run() {
        std::cout << "Starting..." << std::endl;
        std::cout << "Current working directory: " << boost::filesystem::current_path() << std::endl;

        try {
            std::cout << "\n*** Reading and storing the data in the file " << _data_file_name << std::endl;
            _data = Data::SharedPtr(new Data());
            _data->setPartition(_partition);
            _data->getDataFromFile(_data_file_name);
            
            _model->setSubsetNumPatterns(_data->calcNumPatternsVect());
            _model->setSubsetSizes(_partition->calcSubsetSizes());
            _model->activate();

            // Report information about data partition subsets
            unsigned nsubsets = _data->getNumSubsets();
            std::cout << "\nNumber of taxa: " << _data->getNumTaxa() << std::endl;
            std::cout << "Number of partition subsets: " << nsubsets << std::endl;
            for (unsigned subset = 0; subset < nsubsets; subset++) {
                DataType dt = _partition->getDataTypeForSubset(subset);
                std::cout << "  Subset " << (subset+1) << " (" << _data->getSubsetName(subset) << ")" << std::endl;
                std::cout << "    data type: " << _partition->getDataTypeForSubset(subset).getDataTypeAsString() << std::endl;
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
            _likelihood->useUnderflowScaling(_use_underflow_scaling);   ///!d

            std::cout << "\n*** Model description" << std::endl;
            std::cout << _model->describeModel() << std::endl;
            _likelihood->setModel(_model);

            _likelihood->initBeagleLib();

            std::cout << "\n*** Reading and storing the first tree in the file " << _tree_file_name << std::endl;
            _tree_summary = TreeSummary::SharedPtr(new TreeSummary());
            _tree_summary->readTreefile(_tree_file_name, 0);
            Tree::SharedPtr tree = _tree_summary->getTree(0);
            
            if (tree->numLeaves() != _data->getNumTaxa())
                throw XStrom(boost::format("Number of taxa in tree (%d) does not equal the number of taxa in the data matrix (%d)") % tree->numLeaves() % _data->getNumTaxa());

            std::cout << "\n*** Calculating the likelihood of the tree" << std::endl;
            TreeManip tm(tree);
            tm.selectAllPartials();
            tm.selectAllTMatrices();
            double lnL = _likelihood->calcLogLikelihood(tree);
            tm.deselectAllPartials();
            tm.deselectAllTMatrices();
            std::cout << boost::str(boost::format("log likelihood = %.5f") % lnL) << std::endl;
            
            if (_expected_log_likelihood != 0.0) 
                std::cout << boost::str(boost::format("      (expecting %.3f)") % _expected_log_likelihood) << std::endl;
            
        }
        catch (XStrom & x) {
            std::cerr << "Strom encountered a problem:\n  " << x.what() << std::endl;
        }

        std::cout << "\nFinished!" << std::endl;
    }

}
