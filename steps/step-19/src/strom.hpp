#pragma once    ///start

#include <iostream>
#include "data.hpp"
#include "likelihood.hpp"
#include "tree_summary.hpp"
#include "partition.hpp"
#include "lot.hpp"
#include "chain.hpp"
#include "output_manager.hpp"
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
            bool                                    processAssignmentString(Model::SharedPtr m, const std::string & which, const std::string & definition);
            void                                    handleAssignmentStrings(Model::SharedPtr m, const boost::program_options::variables_map & vm, std::string label, const std::vector<std::string> & definitions, std::string default_definition); 
            bool                                    splitAssignmentString(const std::string & definition, std::vector<std::string> & vector_of_subset_names, std::vector<double>  & vector_of_values);
            void                                    sample(unsigned iter, Chain & chain);

            void                                    readData();
            void                                    readTrees();
            void                                    showPartitionInfo();
            void                                    showBeagleInfo();
            void                                    showMCMCInfo();
            void                                    calcHeatingPowers();
            void                                    initChains();
            void                                    startTuningChains();
            void                                    stopTuningChains();
            void                                    stepChains(unsigned iteration, bool sampling);
            void                                    swapChains();
            void                                    stopChains();
            void                                    swapSummary() const;
            void                                    showChainTuningInfo() const;

            double                                  _expected_log_likelihood;
            
            double                                  _topo_prior_C;  ///!a
            bool                                    _allow_polytomies;
            bool                                    _resolution_class_prior;  ///!b

            std::string                             _data_file_name;
            std::string                             _tree_file_name;
            Partition::SharedPtr                    _partition;

            Data::SharedPtr                         _data;
            std::vector<Likelihood::SharedPtr>      _likelihoods;
            TreeSummary::SharedPtr                  _tree_summary;
            Lot::SharedPtr                          _lot;

            unsigned                                _random_seed;
            unsigned                                _num_iter;
            unsigned                                _print_freq;
            unsigned                                _sample_freq;

            unsigned                                _num_burnin_iter; 
            bool                                    _using_stored_data;
            bool                                    _use_gpu;
            bool                                    _ambig_missing;
            unsigned                                _num_chains;
            double                                  _heating_lambda;
            std::vector<Chain>                      _chains;
            std::vector<double>                     _heating_powers;
            std::vector<unsigned>                   _swaps;

            bool                                    _use_underflow_scaling;

            static std::string                      _program_name;
            static unsigned                         _major_version;
            static unsigned                         _minor_version;
            
            OutputManager::SharedPtr                _output_manager;

    };
    ///end_class_declaration

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
        _expected_log_likelihood    = 0.0;
        _use_underflow_scaling      = false;
        _data                       = nullptr;
        _lot                        = nullptr;
        _random_seed                = 1;
        _num_iter                   = 1000;
        _print_freq                 = 1;
        _sample_freq                = 1;
        _output_manager             = nullptr;
        
        _topo_prior_C               = 1.0;  ///!c
        _allow_polytomies           = true;
        _resolution_class_prior     = true;  ///!d

        _using_stored_data          = true;
        _likelihoods.clear();
        _num_burnin_iter            = 1000;
        _heating_lambda             = 0.5;
        _num_chains                 = 1;
        _chains.resize(0);
        _heating_powers.resize(0);
        _swaps.resize(0);
    }   ///end_clear

    inline void Strom::processCommandLineOptions(int argc, const char * argv[]) {   ///begin_processCommandLineOptions
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
            ("seed,z",        boost::program_options::value(&_random_seed)->default_value(1),   "pseudorandom number seed")
            ("niter,n",       boost::program_options::value(&_num_iter)->default_value(1000),   "number of MCMC iterations")
            ("printfreq",  boost::program_options::value(&_print_freq)->default_value(1),   "skip this many iterations before reporting progress")
            ("samplefreq",  boost::program_options::value(&_sample_freq)->default_value(1),   "skip this many iterations before sampling next")
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
            ("topopriorC", boost::program_options::value(&_topo_prior_C)->default_value(1.0), "topology prior C: tree (or resolution class) with m internal nodes has probability C time greater than tree (or resolution class) with m+1 internal nodes.") ///!e
            ("allowpolytomies", boost::program_options::value(&_allow_polytomies)->default_value(true), "yes or no; if yes, then topopriorC and polytomyprior are used, otherwise topopriorC and polytomyprior are ignored")
            ("resclassprior", boost::program_options::value(&_resolution_class_prior)->default_value(true), "if yes, topologypriorC will apply to resolution classes; if no, topologypriorC will apply to individual tree topologies") ///!f
            ("expectedLnL", boost::program_options::value(&_expected_log_likelihood)->default_value(0.0), "log likelihood expected")
            ("nchains",       boost::program_options::value(&_num_chains)->default_value(1),                "number of chains")
            ("heatfactor",    boost::program_options::value(&_heating_lambda)->default_value(0.5),          "determines how hot the heated chains are")
            ("burnin",        boost::program_options::value(&_num_burnin_iter)->default_value(100),         "number of iterations used to burn in chains")
            ("usedata",       boost::program_options::value(&_using_stored_data)->default_value(true),      "use the stored data in calculating likelihoods (specify no to explore the prior)")
            ("gpu",           boost::program_options::value(&_use_gpu)->default_value(true),                "use GPU if available")
            ("ambigmissing",  boost::program_options::value(&_ambig_missing)->default_value(true),          "treat all ambiguities as missing data")
            ("underflowscaling",  boost::program_options::value(&_use_underflow_scaling)->default_value(true),          "scale site-likelihoods to prevent underflow (slower but safer)")
        ;   ///end_add_options
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

        // Be sure number of chains is greater than or equal to 1
        if (_num_chains < 1)
            throw XStrom("nchains must be a positive integer greater than 0");

        // Be sure heatfactor is between 0 and 1
        if (_heating_lambda <= 0.0 || _heating_lambda > 1.0)
            throw XStrom("heatfactor must be a real number in the interval (0.0,1.0]");
        
        if (!_using_stored_data)
            std::cout << "\n*** Not using stored data (posterior = prior) ***\n" << std::endl;
            
        // Allocate a separate model for each chain
        for (unsigned c = 0; c < _num_chains; c++) {
            Likelihood::SharedPtr likelihood = Likelihood::SharedPtr(new Likelihood());
            likelihood->setPreferGPU(_use_gpu);
            likelihood->setAmbiguityEqualsMissing(_ambig_missing);
            Model::SharedPtr m = likelihood->getModel();
            m->setSubsetDataTypes(_partition->getSubsetDataTypes());
            handleAssignmentStrings(m, vm, "statefreq", partition_statefreq, "default:equal");
            handleAssignmentStrings(m, vm, "rmatrix",   partition_rmatrix,   "default:equal");
            handleAssignmentStrings(m, vm, "omega",     partition_omega,     "default:0.1"  );
            handleAssignmentStrings(m, vm, "ncateg",    partition_ncateg,    "default:1"    );
            handleAssignmentStrings(m, vm, "ratevar",   partition_ratevar,   "default:1.0"  );
            handleAssignmentStrings(m, vm, "pinvar",    partition_pinvar,    "default:0.0"  );
            handleAssignmentStrings(m, vm, "relrate",   partition_relrates,  "default:equal");
            handleAssignmentStrings(m, vm, "tree",      partition_tree,      "default:1");
            _likelihoods.push_back(likelihood);
        }
    }
    
    inline void Strom::handleAssignmentStrings(Model::SharedPtr m, const boost::program_options::variables_map & vm, std::string label, const std::vector<std::string> & definitions, std::string default_definition) {
        if (vm.count(label) > 0) {
            bool first = true;
            for (auto s : definitions) {
                bool is_default = processAssignmentString(m, label, s);
                if (is_default && !first)
                    throw XStrom(boost::format("default specification must be first %s encountered") % label);
                first = false;
            }
        }
        else {
            processAssignmentString(m, label, default_definition);
        }
    }
    
    inline bool Strom::processAssignmentString(Model::SharedPtr m, const std::string & which, const std::string & definition) {
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
                    m->setSubsetStateFreqs(freqs, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    m->setSubsetStateFreqs(freqs, _partition->findSubsetByName(s), fixed);
                }
            }
        }
        else if (which == "rmatrix") {
            QMatrix::freq_xchg_ptr_t xchg = std::make_shared<QMatrix::freq_xchg_t>(vector_of_values);
            if (vector_of_subset_names[0] == "default") {
                default_found = true;
                for (unsigned i = 0; i < num_subsets_defined; i++)
                    m->setSubsetExchangeabilities(xchg, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    m->setSubsetExchangeabilities(xchg, _partition->findSubsetByName(s), fixed);
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
                    m->setSubsetOmega(omega, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    m->setSubsetOmega(omega, _partition->findSubsetByName(s), fixed);
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
                    m->setSubsetIsInvarModel(invar_model, i);
                    m->setSubsetPinvar(p, i, fixed);
                }
            }
            else {
                for (auto s : vector_of_subset_names) {
                    unsigned i = _partition->findSubsetByName(s);
                    m->setSubsetIsInvarModel(invar_model, i);
                    m->setSubsetPinvar(p, i, fixed);
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
                    m->setSubsetRateVar(rv, i, fixed);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    m->setSubsetRateVar(rv, _partition->findSubsetByName(s), fixed);
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
                    m->setSubsetNumCateg(ncat, i);
            }
            else {
                for (auto s : vector_of_subset_names) {
                    m->setSubsetNumCateg(ncat, _partition->findSubsetByName(s));
                }
            }
        }
        else if (which == "tree") {
            if (vector_of_values.size() > 1)
                throw XStrom(boost::format("expecting 1 value for tree, found %d values") % vector_of_values.size());
            unsigned tree_index = vector_of_values[0];
            assert(tree_index > 0);
            m->setTreeIndex(tree_index - 1, fixed);
            if (vector_of_subset_names[0] != "default")
                throw XStrom("tree must be assigned to default only");
        }
        else {
            assert(which == "relrate");
            if (vector_of_subset_names[0] != "default")
                throw XStrom("relrate must be assigned to default only");
            m->setSubsetRelRates(vector_of_values, fixed);
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

    inline void Strom::sample(unsigned iteration, Chain & chain) {  ///begin_sample
        if (chain.getHeatingPower() < 1.0)
            return;
            
        bool time_to_sample = (bool)(iteration % _sample_freq == 0);
        bool time_to_report = (bool)(iteration % _print_freq == 0);
        if (time_to_sample || time_to_report) {
            double logLike = chain.getLogLikelihood();
            double logPrior = chain.calcLogJointPrior();
            double TL = chain.getTreeManip()->calcTreeLength();
            unsigned m = chain.getTreeManip()->calcResolutionClass();   ///!g
            if (time_to_report) {
                if (logPrior == Updater::getLogZero())
                    _output_manager->outputConsole(boost::str(boost::format("%12d %12.5f %12s %12.5f") % iteration % logLike % "-infinity" % TL));
                else
                    _output_manager->outputConsole(boost::str(boost::format("%12d %12.5f %12.5f %12.5f") % iteration % logLike % logPrior % TL));
            }
            if (time_to_sample) {
                _output_manager->outputTree(iteration, chain.getTreeManip());
                _output_manager->outputParameters(iteration, logLike, logPrior, TL, m, chain.getModel());   ///!h
            }
        }
    }   ///end_sample

    inline void Strom::calcHeatingPowers() {
        // Specify chain heating power (e.g. _heating_lambda = 0.2)
        // chain_index  power
        //      0       1.000 = 1/(1 + 0.2*0)
        //      1       0.833 = 1/(1 + 0.2*1)
        //      2       0.714 = 1/(1 + 0.2*2)
        //      3       0.625 = 1/(1 + 0.2*3)
        unsigned i = 0;
        for (auto & h : _heating_powers) {
            h = 1.0/(1.0 + _heating_lambda*i++);
        }
    }

    inline void Strom::showChainTuningInfo() const {
        for (unsigned idx = 0; idx < _num_chains; ++idx) {
            for (auto & c : _chains) {
                if (c.getChainIndex() == idx) {
                    _output_manager->outputConsole(boost::str(boost::format("\nChain %d (power %.5f)") % idx % c.getHeatingPower()));
                    std::vector<std::string> names = c.getUpdaterNames();
                    std::vector<double> lambdas    = c.getLambdas();
                    std::vector<double> accepts    = c.getAcceptPercentages();
                    std::vector<unsigned> nupdates = c.getNumUpdates();
                    unsigned n = (unsigned)names.size();
                    _output_manager->outputConsole(boost::str(boost::format("%35s %15s %15s %15s") % "Updater" % "Tuning Param." % "Accept %" % "No. Updates"));
                    for (unsigned i = 0; i < n; ++i) {
                        _output_manager->outputConsole(boost::str(boost::format("%35s %15.3f %15.1f %15d") % names[i] % lambdas[i] % accepts[i] % nupdates[i]));
                    }
                }
            }
        }
    }
    
    inline void Strom::startTuningChains() {
        _swaps.assign(_num_chains*_num_chains, 0);
        for (auto & c : _chains) {
            c.startTuning();
        }
    } 
    
    inline void Strom::stopTuningChains() {
        _swaps.assign(_num_chains*_num_chains, 0);
        for (auto & c : _chains) {
            c.stopTuning();
        }
    }
    
    inline void Strom::stepChains(unsigned iteration, bool sampling) {
        for (auto & c : _chains) {
             c.nextStep(iteration);
            if (sampling)
                sample(iteration, c);
        }
    }

    inline void Strom::swapChains() {
        if (_num_chains == 1)
            return;

        // Select two chains at random to swap
        // If _num_chains = 3...
        //  i  j  = (i + 1 + randint(0,1)) % _num_chains
        // ---------------------------------------------
        //  0  1  = (0 + 1 +      0      ) %     3
        //     2  = (0 + 1 +      1      ) %     3
        // ---------------------------------------------
        //  1  2  = (1 + 1 +      0      ) %     3
        //     0  = (1 + 1 +      1      ) %     3
        // ---------------------------------------------
        //  2  0  = (2 + 1 +      0      ) %     3
        //     1  = (2 + 1 +      1      ) %     3
        // ---------------------------------------------
        unsigned i = _lot->randint(0, _num_chains-1);
        unsigned j = i + 1 + _lot->randint(0, _num_chains-2);
        j %= _num_chains;

        assert(i != j && i >=0 && i < _num_chains && j >= 0 && j < _num_chains);

        // Determine upper and lower triangle cells in _swaps vector
        unsigned smaller = _num_chains;
        unsigned larger  = _num_chains;
        double index_i   = _chains[i].getChainIndex();
        double index_j   = _chains[j].getChainIndex();
        if (index_i < index_j) {
            smaller = index_i;
            larger  = index_j;
        }
        else {
            smaller = index_j;
            larger  = index_i;
        }
        unsigned upper = smaller*_num_chains + larger;
        unsigned lower = larger*_num_chains  + smaller;
        _swaps[upper]++;

        // Propose swap of chains i and j
        // Proposed state swap will be successful if a uniform random deviate is less than R, where
        //    R = Ri * Rj = (Pi(j) / Pi(i)) * (Pj(i) / Pj(j))
        // Chain i: power = a, kernel = pi
        // Chain j: power = b, kernel = pj
        //      pj^a         pi^b
        // Ri = ----    Rj = ----
        //      pi^a         pj^b
        // log R = (a-b) [log(pj) - log(pi)]

        double heat_i       = _chains[i].getHeatingPower();
        double log_kernel_i = _chains[i].calcLogLikelihood() + _chains[i].calcLogJointPrior();

        double heat_j       = _chains[j].getHeatingPower();
        double log_kernel_j = _chains[j].calcLogLikelihood() + _chains[j].calcLogJointPrior();

        double logR = (heat_i - heat_j)*(log_kernel_j - log_kernel_i);

        double logu = _lot->logUniform();
        if (logu < logR) {
            // accept swap
            _swaps[lower]++;
            _chains[j].setHeatingPower(heat_i);
            _chains[i].setHeatingPower(heat_j);
            _chains[j].setChainIndex(index_i);
            _chains[i].setChainIndex(index_j);
            std::vector<double> lambdas_i = _chains[i].getLambdas();
            std::vector<double> lambdas_j = _chains[j].getLambdas();
            _chains[i].setLambdas(lambdas_j);
            _chains[j].setLambdas(lambdas_i);
        }
    }

    inline void Strom::stopChains() {
        for (auto & c : _chains)
            c.stop();
    }

    inline void Strom::swapSummary() const {
        if (_num_chains > 1) {
            unsigned i, j;
            std::cout << "\nSwap summary (upper triangle = no. attempted swaps; lower triangle = no. successful swaps):" << std::endl;

            // column headers
            std::cout << boost::str(boost::format("%12s") % " ");
            for (i = 0; i < _num_chains; ++i)
                std::cout << boost::str(boost::format(" %12d") % i);
            std::cout << std::endl;

            // top line
            std::cout << boost::str(boost::format("%12s") % "------------");
            for (i = 0; i < _num_chains; ++i)
                std::cout << boost::str(boost::format("-%12s") % "------------");
            std::cout << std::endl;

            // table proper
            for (i = 0; i < _num_chains; ++i) {
                std::cout << boost::str(boost::format("%12d") % i);
                for (j = 0; j < _num_chains; ++j) {
                    if (i == j)
                        std::cout << boost::str(boost::format(" %12s") % "---");
                    else
                        std::cout << boost::str(boost::format(" %12.5f") % _swaps[i*_num_chains + j]);
                }
                std::cout << std::endl;
            }

            // bottom line
            std::cout << boost::str(boost::format("%12s") % "------------");
            for (i = 0; i < _num_chains; ++i)
                std::cout << boost::str(boost::format("-%12s") % "------------");
            std::cout << std::endl;
        }
    } 

    inline void Strom::initChains() {   ///begin_initChains
        // Create _num_chains chains
        _chains.resize(_num_chains);
        
        // Create _num_chains by _num_chains swap matrix
        _swaps.assign(_num_chains*_num_chains, 0);

        // Create heating power vector
        _heating_powers.assign(_num_chains, 1.0);
        calcHeatingPowers();
        
        // Initialize chains
        for (unsigned chain_index = 0; chain_index < _num_chains; ++chain_index) {
            auto & c        = _chains[chain_index];
            auto likelihood = _likelihoods[chain_index];
            auto m          = likelihood->getModel();
            
            // Finish setting up models
            m->setTopologyPriorOptions(_allow_polytomies, _resolution_class_prior, _topo_prior_C);   ///!i
            m->setSubsetNumPatterns(_data->calcNumPatternsVect());
            m->setSubsetSizes(_partition->calcSubsetSizes());
            m->activate();
            if (chain_index == 0)
                std::cout << "\n" << m->describeModel() << std::endl;
            else
                m->describeModel();
                
            // Finish setting up likelihoods
            likelihood->setData(_data);
            likelihood->useUnderflowScaling(_use_underflow_scaling);
            likelihood->initBeagleLib();
            likelihood->useStoredData(_using_stored_data);
            
            // Build list of updaters, one for each free parameter in the model
            unsigned num_free_parameters = c.createUpdaters(m, _lot, likelihood);   ///!xxx
            if (num_free_parameters == 0)
                throw XStrom("MCMC skipped because there are no free parameters in the model");

            // Tell the chain that it should adapt its updators (at least initially)
            c.startTuning();

            // Set heating power to precalculated value
            c.setChainIndex(chain_index);
            c.setHeatingPower(_heating_powers[chain_index]);
                        
            // Give the chain a starting tree
            std::string newick = _tree_summary->getNewick(m->getTreeIndex());
            c.setTreeFromNewick(newick);

            // Print headers in output files and make sure each updator has its starting value
            c.start();
        }
    }   ///end_initChains

    inline void Strom::readData() {
        std::cout << "\n*** Reading and storing the data in the file " << _data_file_name << std::endl;
        _data = Data::SharedPtr(new Data());
        _data->setPartition(_partition);
        _data->getDataFromFile(_data_file_name);
    }
    
    inline void Strom::readTrees() {
        assert(_data);
        assert(_likelihoods.size() > 0 && _likelihoods[0]);
        auto m = _likelihoods[0]->getModel();
        unsigned tree_index = m->getTreeIndex();
        std::cout << "\n*** Reading and storing tree number " << (tree_index + 1) << " in the file " << _tree_file_name << std::endl;
        _tree_summary = TreeSummary::SharedPtr(new TreeSummary());
        _tree_summary->readTreefile(_tree_file_name, 0);

        Tree::SharedPtr tree = _tree_summary->getTree(tree_index);
        if (tree->numLeaves() != _data->getNumTaxa())
            throw XStrom(boost::format("Number of taxa in tree (%d) does not equal the number of taxa in the data matrix (%d)") % tree->numLeaves() % _data->getNumTaxa());
    }

    inline void Strom::showPartitionInfo() {
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
    }

    inline void Strom::showBeagleInfo() {
        assert(_likelihoods.size() > 0 && _likelihoods[0]);
        std::cout << "\n*** BeagleLib " << _likelihoods[0]->beagleLibVersion() << " resources:\n";
        std::cout << "Preferred resource: " << (_use_gpu ? "GPU" : "CPU") << std::endl;
        std::cout << "Available resources:" << std::endl;
        std::cout << _likelihoods[0]->availableResources() << std::endl;
        std::cout << "Resources used:" << std::endl;
        std::cout << _likelihoods[0]->usedResources() << std::endl;
    }
    
    inline void Strom::showMCMCInfo() {
        assert(_likelihoods.size() > 0 && _likelihoods[0]);
        std::cout << "\n*** MCMC analysis beginning..." << std::endl;
        if (_likelihoods[0]->usingStoredData()) {
            unsigned tree_index = _likelihoods[0]->getModel()->getTreeIndex();
            Tree::SharedPtr tree = _tree_summary->getTree(tree_index);
            double lnL = _chains[0].getLogLikelihood();
            std::cout << boost::str(boost::format("Starting log likelihood = %.5f") % lnL) << std::endl;
        }
        else
            std::cout << "Exploring prior" << std::endl;
    
        if (_expected_log_likelihood != 0.0)
            std::cout << boost::str(boost::format("      (expecting %.3f)") % _expected_log_likelihood) << std::endl;
    
        std::cout << "Number of chains is " << _num_chains << std::endl;
        std::cout << "Burning in for " << _num_burnin_iter << " iterations." << std::endl;
        std::cout << "Running after burn-in for " << _num_iter << " iterations." << std::endl;
        std::cout << "Sampling every " << _sample_freq << " iterations." << std::endl;
        std::cout << "Sample size is " << (int)(_num_iter/_sample_freq) << std::endl;
                
    }
    
    inline void Strom::run() {
        std::cout << "Starting..." << std::endl;
        std::cout << "Current working directory: " << boost::filesystem::current_path() << std::endl;
        
        try {
            readData();
            readTrees();
            showPartitionInfo();

            // Create a Lot object that generates (pseudo)random numbers
            _lot = Lot::SharedPtr(new Lot);
            _lot->setSeed(_random_seed);

            // Create  Chain objects
            initChains();
            
            showBeagleInfo();
            showMCMCInfo();

            // Create an output manager and open output files
            _output_manager.reset(new OutputManager);
#if 1 //POLTMP
            Updater::_om = _output_manager;
#endif
            _output_manager->outputConsole(boost::str(boost::format("\n%12s %12s %12s %12s") % "iteration" % "logLike" % "logPrior" % "TL"));
            _output_manager->openTreeFile("trees.tre", _data);
            _output_manager->openParameterFile("params.txt", _chains[0].getModel());
            sample(0, _chains[0]);
                        // Burn-in the chains
            startTuningChains();
            for (unsigned iteration = 1; iteration <= _num_burnin_iter; ++iteration) {
                stepChains(iteration, false);
                swapChains();
            }
            stopTuningChains();

            // Sample the chains
            for (unsigned iteration = 1; iteration <= _num_iter; ++iteration) {
                stepChains(iteration, true);
                swapChains();
            }
            showChainTuningInfo();
            stopChains();
            
            // Create swap summary
            swapSummary();

            // Close output files
            _output_manager->closeTreeFile();
            _output_manager->closeParameterFile();
        }
        catch (XStrom & x) {
            std::cerr << "Strom encountered a problem:\n  " << x.what() << std::endl;
        }

        std::cout << "\nFinished!" << std::endl;
    }

}
