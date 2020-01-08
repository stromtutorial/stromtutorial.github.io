#pragma once    ///start

#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include "libhmsbeagle/beagle.h"
#include "tree.hpp"
#include "data.hpp"
#include "xstrom.hpp"

namespace strom {

    class Likelihood {  ///begin_class_declaration
        public:
                                                    Likelihood();
                                                    ~Likelihood();

            void                                    setRooted(bool is_rooted);
            void                                    setPreferGPU(bool prefer_gpu);
            void                                    setAmbiguityEqualsMissing(bool ambig_equals_missing);

            bool                                    usingStoredData() const;
            void                                    useStoredData(bool using_data);

            std::string                             beagleLibVersion() const;
            std::string                             availableResources() const;
            std::string                             usedResources() const;

            void                                    initBeagleLib();
            void                                    finalizeBeagleLib(bool use_exceptions);

            double                                  calcLogLikelihood(Tree::SharedPtr t);

            Data::SharedPtr                         getData();
            void                                    setData(Data::SharedPtr d);

            void                                    clear();
            
            unsigned                                calcNumEdgesInFullyResolvedTree() const;
            unsigned                                calcNumInternalsInFullyResolvedTree() const;

        private:
        
            struct InstanceInfo {
                int handle;
                int resourcenumber;
                std::string resourcename;
                unsigned nstates;
                unsigned nratecateg;
                unsigned npatterns;
                unsigned partial_offset;
                unsigned tmatrix_offset;
                std::vector<unsigned> subsets;
                
                InstanceInfo() : handle(-1), resourcenumber(-1), resourcename(""), nstates(0), nratecateg(0), npatterns(0), partial_offset(0), tmatrix_offset(0) {}
            };

            typedef std::pair<unsigned, int>        instance_pair_t;

            unsigned                                getScalerIndex(Node * nd, InstanceInfo & info) const;   ///!a
            unsigned                                getPartialIndex(Node * nd, InstanceInfo & info) const;  ///!b
            unsigned                                getTMatrixIndex(Node * nd, InstanceInfo & info, unsigned subset_index) const;   ///!c
            void                                    updateInstanceMap(instance_pair_t & p, unsigned subset);
            void                                    newInstance(unsigned nstates, int nrates, std::vector<unsigned> & subset_indices);
            void                                    setTipStates();
            void                                    setTipPartials();
            void                                    setPatternPartitionAssignments();
            void                                    setPatternWeights();
            void                                    setAmongSiteRateHeterogenetity();
            void                                    setModelRateMatrix();
            void                                    addOperation(InstanceInfo & info, Node * nd, Node * lchild, Node * rchild, unsigned subset_index);
            void                                    queuePartialsRecalculation(Node * nd, Node * lchild, Node * rchild);
            void                                    queueTMatrixRecalculation(Node * nd);
            void                                    defineOperations(Tree::SharedPtr t);
            void                                    updateTransitionMatrices();
            void                                    calculatePartials();
            double                                  calcInstanceLogLikelihood(InstanceInfo & inst, Tree::SharedPtr t);


            std::vector<InstanceInfo>               _instances;
            std::map<int, std::string>              _beagle_error;
            std::map<int, std::vector<int> >        _operations;
            std::map<int, std::vector<int> >        _pmatrix_index;
            std::map<int, std::vector<double> >     _edge_lengths;
            std::map<int, std::vector<int> >        _eigen_indices;
            std::map<int, std::vector<int> >        _category_rate_indices;
            double                                  _relrate_normalizing_constant;

            std::vector<int>                        _subset_indices;
            std::vector<int>                        _parent_indices;
            std::vector<int>                        _child_indices;
            std::vector<int>                        _tmatrix_indices;
            std::vector<int>                        _weights_indices;
            std::vector<int>                        _freqs_indices;
            std::vector<int>                        _scaling_indices;
        
            Data::SharedPtr                         _data;
            unsigned                                _ntaxa;
            bool                                    _rooted;
            bool                                    _prefer_gpu;
            bool                                    _ambiguity_equals_missing;
            bool                                    _using_data;

        public:
            typedef std::shared_ptr< Likelihood >   SharedPtr;
    }; 

    // member function bodies go here
    ///end_class_declaration
    inline Likelihood::Likelihood() {   ///begin_constructor
        //std::cout << "Constructing a Likelihood" << std::endl;
        clear();
    }   ///end_constructor

    inline Likelihood::~Likelihood() {  ///begin_destructor
        //std::cout << "Destroying a Likelihood" << std::endl;
        finalizeBeagleLib(false);
        clear();
    }   ///end_destructor
    
    inline unsigned Likelihood::calcNumEdgesInFullyResolvedTree() const {   ///begin_calcNumEdgesInFullyResolvedTree
        assert(_ntaxa > 0);
        return (_rooted ? (2*_ntaxa - 2) : (2*_ntaxa - 3));
    }  ///end_calcNumEdgesInFullyResolvedTree
    
    inline unsigned Likelihood::calcNumInternalsInFullyResolvedTree() const {  ///begin_calcNumInternalsInFullyResolvedTree
        assert(_ntaxa > 0);
        return (_rooted ? (_ntaxa - 1) : (_ntaxa - 2));
    }  ///end_calcNumInternalsInFullyResolvedTree

    inline void Likelihood::finalizeBeagleLib(bool use_exceptions) {  ///begin_finalizeBeagleLib
        // Close down all BeagleLib instances if active
        for (auto info : _instances) {
            if (info.handle >= 0) {
                int code = beagleFinalizeInstance(info.handle);
                if (code != 0) {
                    if (use_exceptions)
                        throw XStrom(boost::format("Likelihood failed to finalize BeagleLib instance. BeagleLib error code was %d (%s).") % code % _beagle_error[code]);
                    else
                        std::cerr << boost::format("Likelihood destructor failed to finalize BeagleLib instance. BeagleLib error code was %d (%s).") % code % _beagle_error[code] << std::endl;
                }
            }
        }
        _instances.clear();
    }  ///end_finalizeBeagleLib

    inline void Likelihood::clear() {  ///begin_clear
        finalizeBeagleLib(true);
        
        _ntaxa                      = 0;
        _rooted                     = false;
        _prefer_gpu                 = false;
        _ambiguity_equals_missing   = true;
        _using_data                 = true;
        _data                       = nullptr;
        
        _operations.clear();
        _pmatrix_index.clear();
        _edge_lengths.clear();
        _eigen_indices.clear();
        _category_rate_indices.clear();
        _relrate_normalizing_constant = 1.0;
        _subset_indices.assign(1, 0);
        _parent_indices.assign(1, 0);
        _child_indices.assign(1, 0);
        _tmatrix_indices.assign(1, 0);
        _weights_indices.assign(1, 0);
        _freqs_indices.assign(1, 0);
        _scaling_indices.assign(1, 0);
        
        // Store BeagleLib error codes so that useful
        // error messages may be provided to the user
        _beagle_error.clear();
        _beagle_error[0]  = std::string("success");
        _beagle_error[-1] = std::string("unspecified error");
        _beagle_error[-2] = std::string("not enough memory could be allocated");
        _beagle_error[-3] = std::string("unspecified exception");
        _beagle_error[-4] = std::string("the instance index is out of range, or the instance has not been created");
        _beagle_error[-5] = std::string("one of the indices specified exceeded the range of the array");
        _beagle_error[-6] = std::string("no resource matches requirements");
        _beagle_error[-7] = std::string("no implementation matches requirements");
        _beagle_error[-8] = std::string("floating-point range exceeded");
    }  ///end_clear

    inline std::string Likelihood::beagleLibVersion() const {  ///begin_beagleLibVersion
        return std::string(beagleGetVersion());
    } 
    
    inline std::string Likelihood::availableResources() const {
        BeagleResourceList * rsrcList = beagleGetResourceList();
        std::string s;
        for (int i = 0; i < rsrcList->length; ++i) {
            std::string desc = rsrcList->list[i].description;
            boost::trim(desc);
            if (desc.size() > 0)
                s += boost::str(boost::format("  resource %d: %s (%s)\n") % i % rsrcList->list[i].name % desc);
            else
                s += boost::str(boost::format("  resource %d: %s\n") % i % rsrcList->list[i].name);
        }
        boost::trim_right(s);
        return s;
    }
    
    inline std::string Likelihood::usedResources() const {
        std::string s;
        for (unsigned i = 0; i < _instances.size(); i++) {
            s += boost::str(boost::format("  instance %d: %s (resource %d)\n") % _instances[i].handle % _instances[i].resourcename % _instances[i].resourcenumber);
        }
        return s;
    }   ///end_usedResources
    
    inline Data::SharedPtr Likelihood::getData() {  ///begin_getData
        return _data;
    }
    
    inline void Likelihood::setData(Data::SharedPtr data) {
        assert(_instances.size() == 0);
        assert(!data->getDataMatrix().empty());
        _data = data;
    }  ///end_setData

    inline void Likelihood::setRooted(bool is_rooted) {  ///begin_setRooted
        assert(_instances.size() == 0 || _rooted == is_rooted); // can't change rooting status after initBeagleLib called
        _rooted = is_rooted;
    }  ///end_setRooted
    
    inline void Likelihood::setAmbiguityEqualsMissing(bool ambig_equals_missing) { ///begin_setAmbiguityEqualsMissing
        // Can't change GPU preference status after initBeagleLib called
        assert(_instances.size() == 0 || _ambiguity_equals_missing == ambig_equals_missing);
        _ambiguity_equals_missing = ambig_equals_missing;
    } ///end_setAmbiguityEqualsMissing
    
    inline void Likelihood::setPreferGPU(bool prefer_gpu) {  ///begin_setPreferGPU
        // Can't change GPU preference status after initBeagleLib called
        assert(_instances.size() == 0 || _prefer_gpu == prefer_gpu);
        _prefer_gpu = prefer_gpu;
    }  ///end_setPreferGPU
    
    inline bool Likelihood::usingStoredData() const {   ///begin_usingStoredData
        return _using_data;
    }   ///end_usingStoredData
    
    inline void Likelihood::useStoredData(bool using_data) {  ///begin_useStoredData
        _using_data = using_data;
    }  ///end_useStoredData
    
    inline void Likelihood::initBeagleLib() {  ///begin_initBeagleLib
        assert(_data);

        // Close down any existing BeagleLib instances
        finalizeBeagleLib(true);

        _ntaxa = _data->getNumTaxa();
        
        unsigned nsubsets = _data->getNumSubsets();
        std::set<instance_pair_t> nstates_ncateg_combinations;
        std::map<instance_pair_t, std::vector<unsigned> > subsets_for_pair;
        for (unsigned subset = 0; subset < nsubsets; subset++) {
            // Create a pair comprising number of states and number of rate categories
            unsigned nstates = _data->getNumStatesForSubset(subset);
            int nrates = 1; // assuming no rate heterogeneity for now
            instance_pair_t p = std::make_pair(nstates, nrates);
            
            // Add combo to set
            nstates_ncateg_combinations.insert(p);
            subsets_for_pair[p].push_back(subset);
        }

        // Create one instance for each distinct nstates-nrates combination
        _instances.clear();
        for (auto p : nstates_ncateg_combinations) {
            newInstance(p.first, p.second, subsets_for_pair[p]);
            
            InstanceInfo & info = *_instances.rbegin();
            std::cout << boost::str(boost::format("Created BeagleLib instance %d (%d states, %d rate%s, %d subset%s)") % info.handle % info.nstates % info.nratecateg % (info.nratecateg == 1 ? "" : "s") % info.subsets.size() % (info.subsets.size() == 1 ? "" : "s")) << std::endl;
        }
        
        if (_ambiguity_equals_missing)
            setTipStates();
        else
            setTipPartials();
        setPatternWeights();
        setPatternPartitionAssignments();
    }  ///end_initBeagleLib
    
    inline void Likelihood::newInstance(unsigned nstates, int nrates, std::vector<unsigned> & subset_indices) {  ///begin_newInstance
        unsigned num_subsets = (unsigned)subset_indices.size();
        
        unsigned ngammacat = (unsigned)nrates;
        
        unsigned num_patterns = 0;
        for (auto s : subset_indices) {
            num_patterns += _data->getNumPatternsInSubset(s);
        }
        
        unsigned num_internals = calcNumInternalsInFullyResolvedTree();

        unsigned num_edges = calcNumEdgesInFullyResolvedTree();
        unsigned num_nodes = num_edges + 1;
        unsigned num_transition_probs = num_nodes*num_subsets;
        
        long requirementFlags = 0;

        long preferenceFlags = BEAGLE_FLAG_PRECISION_SINGLE | BEAGLE_FLAG_THREADING_CPP;
        if (_prefer_gpu)
            preferenceFlags |= BEAGLE_FLAG_PROCESSOR_GPU;
        else
            preferenceFlags |= BEAGLE_FLAG_PROCESSOR_CPU;
        
        BeagleInstanceDetails instance_details;
        unsigned npartials = num_internals + _ntaxa;
        unsigned nsequences = 0;
        if (_ambiguity_equals_missing) {
            npartials -= _ntaxa;
            nsequences += _ntaxa;
        }
        
        int inst = beagleCreateInstance(
             _ntaxa,                           // tips
             npartials,                        // partials
             nsequences,                       // sequences
             nstates,                          // states
             num_patterns,                     // patterns (total across all subsets that use this instance)
             num_subsets,                      // models (one for each distinct eigen decomposition)
             num_subsets*num_transition_probs, // transition matrices (one for each node in each subset)
             ngammacat,                        // rate categories
             0,                                // scale buffers
             NULL,                             // resource restrictions
             0,                                // length of resource list
             preferenceFlags,                  // preferred flags
             requirementFlags,                 // required flags
             &instance_details);               // pointer for details
        
        if (inst < 0) {
            // beagleCreateInstance returns one of the following:
            //   valid instance (0, 1, 2, ...)
            //   error code (negative integer)
            throw XStrom(boost::str(boost::format("Likelihood init function failed to create BeagleLib instance (BeagleLib error code was %d)") % _beagle_error[inst]));
        }
        
        InstanceInfo info;
        info.handle         = inst;
        info.resourcenumber = instance_details.resourceNumber;
        info.resourcename   = instance_details.resourceName;
        info.nstates        = nstates;
        info.nratecateg     = ngammacat;
        info.subsets        = subset_indices;
        info.npatterns      = num_patterns;
        info.partial_offset = num_internals;
        info.tmatrix_offset = num_nodes;
        _instances.push_back(info);
    } ///end_newInstance

    inline void Likelihood::setTipStates() {   ///begin_setTipStates
        assert(_instances.size() > 0);
        assert(_data);
        Data::state_t one = 1;

        for (auto & info : _instances) {
            std::vector<int> states(info.nstates*info.npatterns);
            
            // Loop through all rows of the data matrix, setting the tip states for one taxon each row
            unsigned t = 0;
            for (auto & row : _data->getDataMatrix()) {
            
                // Loop through all subsets assigned to this instance
                unsigned k = 0;
                for (unsigned s : info.subsets) {
                
                    // Loop through all patterns in this subset
                    auto interval = _data->getSubsetBeginEnd(s);
                    for (unsigned p = interval.first; p < interval.second; p++) {
                    
                        // d is the state for taxon t, pattern p (in subset s)
                        // d is stored as a bit field (e.g., for nucleotide data, A=1, C=2, G=4, T=8, ?=15),
                        // but BeagleLib expects states to be integers (e.g. for nucleotide data,
                        // A=0, C=1, G=2, T=3, ?=4).
                        Data::state_t d = row[p];
                        
                        // Handle common nucleotide case separately
                        if (info.nstates == 4) {
                            if (d == 1)
                                states[k++] = 0;
                            else if (d == 2)
                                states[k++] = 1;
                            else if (d == 4)
                                states[k++] = 2;
                            else if (d == 8)
                                states[k++] = 3;
                            else
                                states[k++] = 4;
                        }
                        else {
                            // This case is for any other data type except nucleotide
                            int s = -1;
                            for (unsigned b = 0; b < info.nstates; b++) {
                                if (d == one << b) {
                                    s = b;
                                    break;
                                }
                            }
                            if (s == -1)
                                states[k++] = info.nstates;
                            else
                                states[k++] = s;
                        }
                    } // pattern loop
                }   // subset loop

            int code = beagleSetTipStates(
                info.handle,    // Instance number
                t,              // Index of destination compactBuffer
                &states[0]);    //  Pointer to compact states vector

            if (code != 0)
                throw XStrom(boost::format("failed to set tip state for taxon %d (\"%s\"; BeagleLib error code was %d)") % (t+1) % _data->getTaxonNames()[t] % code % _beagle_error[code]);
            ++t;
            }
        }
    }   ///end_setTipStates

    inline void Likelihood::setTipPartials() {  ///begin_setTipPartials
        assert(_instances.size() > 0);
        assert(_data);
        Data::state_t one = 1;
        
        for (auto & info : _instances) {
            if (info.nstates != 4)  ///begin_unconstrain
                throw XStrom(boost::format("This program can handle only 4-state DNA/RNA data. You specified data having %d states for at least one data subset.") % info.nstates);  ///end_unconstrain

            std::vector<double> partials(info.nstates*info.npatterns);
            
            // Loop through all rows of data matrix, setting the tip states for one taxon each row
            unsigned t = 0;
            for (auto & row : _data->getDataMatrix()) {
            
                // Loop through all subsets assigned to this instance
                unsigned k = 0;
                for (unsigned s : info.subsets) {
                
                    // Loop through all patterns in this subset
                    auto interval = _data->getSubsetBeginEnd(s);
                    for (unsigned p = interval.first; p < interval.second; p++) {
                    
                        // d is the state for taxon t, pattern p (in subset s)
                        Data::state_t d = row[p];
                        
                        // Handle common nucleotide case separately
                        if (info.nstates == 4) {
                            partials[k++] = d & 1 ? 1.0 : 0.0;  ///begin_fourpartials
                            partials[k++] = d & 2 ? 1.0 : 0.0;
                            partials[k++] = d & 4 ? 1.0 : 0.0;
                            partials[k++] = d & 8 ? 1.0 : 0.0;  ///end_fourpartials
                        }
                        else {
                            // This case is for any other data type except nucleotide
                            for (unsigned b = 0; b < info.nstates; b++) {
                                partials[k++] = d & (one << b) ? 1.0 : 0.0;
                            }
                        }
                    }
                }
                
            int code = beagleSetTipPartials(
                info.handle,    // Instance number
                t,              // Index of destination compactBuffer
                &partials[0]);  // Pointer to compact states vector

            if (code != 0)
                throw XStrom(boost::format("failed to set tip state for taxon %d (\"%s\"; BeagleLib error code was %d)") % (t+1) % _data->getTaxonNames()[t] % code % _beagle_error[code]);
            ++t;
            }
        }
    } ///end_setTipPartials

    inline void Likelihood::setPatternPartitionAssignments() {  ///begin_setPatternPartitionAssignments
        assert(_instances.size() > 0);
        assert(_data);
        
        // beagleSetPatternPartitions does not need to be called if data are unpartitioned
        // (and, in fact, BeagleLib only supports partitioning for 4-state instances if GPU is used,
        // so not calling beagleSetPatternPartitions allows unpartitioned codon model analyses)
        if (_instances.size() == 1 && _instances[0].subsets.size() == 1)
            return;
        
        Data::partition_key_t v;

        // Loop through all instances
        for (auto & info : _instances) {
            unsigned nsubsets = (unsigned)info.subsets.size();
            v.resize(info.npatterns);
            unsigned pattern_index = 0;

            // Loop through all subsets assigned to this instance
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                // Loop through all patterns in this subset
                auto interval = _data->getSubsetBeginEnd(s);
                for (unsigned p = interval.first; p < interval.second; p++) {
                    v[pattern_index++] = instance_specific_subset_index;
                }
                ++instance_specific_subset_index;
            }

            int code = beagleSetPatternPartitions(
               info.handle, // instance number
               nsubsets,    // number of data subsets (equals 1 if data are unpartitioned)
               &v[0]);      // vector of subset indices: v[i] = 0 means pattern i is in subset 0

            if (code != 0) {
                throw XStrom(boost::format("failed to set pattern partition. BeagleLib error code was %d (%s)") % code % _beagle_error[code]);
            }
        }
    }  ///end_setPatternPartitionAssignments
    
    inline void Likelihood::setPatternWeights() {  ///begin_setPatternWeights
        assert(_instances.size() > 0);
        assert(_data);
        Data::pattern_counts_t v;
        auto pattern_counts = _data->getPatternCounts();
        assert(pattern_counts.size() > 0);

        // Loop through all instances
        for (auto & info : _instances) {
            v.resize(info.npatterns);
            unsigned pattern_index = 0;

            // Loop through all subsets assigned to this instance
            for (unsigned s : info.subsets) {
            
                // Loop through all patterns in this subset
                auto interval = _data->getSubsetBeginEnd(s);
                for (unsigned p = interval.first; p < interval.second; p++) {
                    v[pattern_index++] = pattern_counts[p];
                }
            }

            int code = beagleSetPatternWeights(
               info.handle,   // instance number
               &v[0]);        // vector of pattern counts: v[i] = 123 means pattern i was encountered 123 times

            if (code != 0)
                throw XStrom(boost::format("Failed to set pattern weights for instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]);
        }
    }  ///end_setPatternWeights

    inline void Likelihood::setAmongSiteRateHeterogenetity() {  ///begin_setAmongSiteRateHeterogenetity
        assert(_instances.size() > 0);
        int code = 0;
        
        // For now we are assuming rates among sites are equal
        double rates[1] = {1.0};
        double probs[1] = {1.0};

        // Loop through all instances
        for (auto & info : _instances) {

            // Loop through all subsets assigned to this instance
            for (unsigned instance_specific_subset_index = 0; instance_specific_subset_index < info.subsets.size(); instance_specific_subset_index++) {
            
                code = beagleSetCategoryRatesWithIndex(
                    info.handle,                    // instance number
                    instance_specific_subset_index, // subset index
                    rates);                         // vector containing rate scalers
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set category rates for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));

                code = beagleSetCategoryWeights(
                    info.handle,                    // Instance number
                    instance_specific_subset_index, // Index of category weights buffer
                    probs);                         // Category weights array (categoryCount)
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set category probabilities for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));
            }
        }
    }  ///end_setAmongSiteRateHeterogenetity

    inline void Likelihood::setModelRateMatrix() {  ///begin_setModelRateMatrix
        assert(_instances.size() > 0);
        int code = 0;
        double state_freqs[4] = {0.25, 0.25, 0.25, 0.25};
        
        double eigenvalues[4] = {
            -4.0/3.0,
            -4.0/3.0,
            -4.0/3.0,
            0.0
            };

        double eigenvectors[16] = {
            -1,   -1,  -1,  1,
             0,    0,   1,  1,
             0,    1,   0,  1,
             1,    0,   0,  1
            };

        double inverse_eigenvectors[16] = {
            -0.25,   -0.25,   -0.25,   0.75,
            -0.25,   -0.25,    0.75,  -0.25,
            -0.25,    0.75,   -0.25,  -0.25,
             0.25,    0.25,    0.25,   0.25
            };

        // Loop through all instances
        for (auto & info : _instances) {

            // Loop through all subsets assigned to this instance
            for (unsigned instance_specific_subset_index = 0; instance_specific_subset_index < info.subsets.size(); instance_specific_subset_index++) {

                code = beagleSetStateFrequencies(
                     info.handle,                       // Instance number
                     instance_specific_subset_index,    // Index of state frequencies buffer
                     state_freqs);                      // State frequencies array (stateCount)
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set state frequencies for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));

                 code = beagleSetEigenDecomposition(
                    info.handle,                            // Instance number
                    instance_specific_subset_index,         // Index of eigen-decomposition buffer
                    (const double *)eigenvectors,           // Flattened matrix (stateCount x stateCount) of eigenvectors
                    (const double *)inverse_eigenvectors,   // Flattened matrix (stateCount x stateCount) of inverse-eigenvectors
                    eigenvalues);                           // Vector of eigenvalues
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set eigen decomposition for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));
            }
        }
    } ///end_setModelRateMatrix
    
    inline unsigned Likelihood::getScalerIndex(Node * nd, InstanceInfo & info) const {  ///begin_getScalerIndex
        return BEAGLE_OP_NONE;
    }   ///end_getScalerIndex
    
    inline unsigned Likelihood::getPartialIndex(Node * nd, InstanceInfo & info) const { ///begin_getPartialIndex
        // Note: do not be tempted to subtract _ntaxa from pindex: BeagleLib does this itself
        assert(nd->_number >= 0);
        return nd->_number;
    } ///end_getPartialIndex
    
    inline unsigned Likelihood::getTMatrixIndex(Node * nd, InstanceInfo & info, unsigned subset_index) const {  ///begin_getTMatrixIndex
        unsigned tindex = subset_index*info.tmatrix_offset + nd->_number;
        return tindex;
    }   ///end_getTMatrixIndex
      
    inline void Likelihood::defineOperations(Tree::SharedPtr t) {   ///begin_defineOperations
        assert(_instances.size() > 0);
        assert(t);
        assert(t->isRooted() == _rooted);
        
        _relrate_normalizing_constant = 1.0; // assuming subset rates all 1.0 for now

        // Start with a clean slate
        for (auto & info : _instances) {
            _operations[info.handle].clear();
            _pmatrix_index[info.handle].clear();
            _edge_lengths[info.handle].clear();
            _eigen_indices[info.handle].clear();
            _category_rate_indices[info.handle].clear();
        }
                
        // Loop through all nodes in reverse level order
        for (auto nd : boost::adaptors::reverse(t->_levelorder)) {
            assert(nd->_number >= 0);
            if (!nd->_left_child) {
                // This is a leaf
                queueTMatrixRecalculation(nd);
            }
            else {
                // This is an internal node
                queueTMatrixRecalculation(nd);

                // Internal nodes have partials to be calculated, so define
                // an operation to compute the partials for this node
                Node * lchild = nd->_left_child;
                assert(lchild);
                Node * rchild = lchild->_right_sib;
                assert(rchild);
                queuePartialsRecalculation(nd, lchild, rchild);
            }
        }
    }  ///end_defineOperations
    
    inline void Likelihood::queuePartialsRecalculation(Node * nd, Node * lchild, Node * rchild) {   ///begin_queuePartialsRecalculation
        for (auto & info : _instances) {
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                addOperation(info, nd, lchild, rchild, instance_specific_subset_index);
                ++instance_specific_subset_index;
            }
        }
    }   ///end_queuePartialsRecalculation
    
    inline void Likelihood::queueTMatrixRecalculation(Node * nd) {  ///begin_queueTMatrixRecalculation
        double subset_relative_rate = 1.0;  // assuming all subsets have equal relative rates for now
        for (auto & info : _instances) {
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                unsigned tindex = getTMatrixIndex(nd, info, instance_specific_subset_index);
                _pmatrix_index[info.handle].push_back(tindex);
                _edge_lengths[info.handle].push_back(nd->_edge_length*subset_relative_rate);
                _eigen_indices[info.handle].push_back(s);
                _category_rate_indices[info.handle].push_back(s);

                ++instance_specific_subset_index;
            }
        }
    }   ///end_queueTMatrixRecalculation
    
    inline void Likelihood::addOperation(InstanceInfo & info, Node * nd, Node * lchild, Node * rchild, unsigned subset_index) { ///begin_addOperation
        assert(nd);
        assert(lchild);
        assert(rchild);

        // 1. destination partial to be calculated
        int partial_dest = getPartialIndex(nd, info);
        _operations[info.handle].push_back(partial_dest);

        // 2. destination scaling buffer index to write to
        int scaler_index = getScalerIndex(nd, info);
        _operations[info.handle].push_back(scaler_index);

        // 3. destination scaling buffer index to read from
        _operations[info.handle].push_back(BEAGLE_OP_NONE);

        // 4. left child partial index
        int partial_lchild = getPartialIndex(lchild, info);
        _operations[info.handle].push_back(partial_lchild);

        // 5. left child transition matrix index
        unsigned tindex_lchild = getTMatrixIndex(lchild, info, subset_index);
        _operations[info.handle].push_back(tindex_lchild);

        // 6. right child partial index
        int partial_rchild = getPartialIndex(rchild, info);
        _operations[info.handle].push_back(partial_rchild);

        // 7. right child transition matrix index
        unsigned tindex_rchild = getTMatrixIndex(rchild, info, subset_index);
        _operations[info.handle].push_back(tindex_rchild);

        if (info.subsets.size() > 1) {
            // 8. index of partition subset
            _operations[info.handle].push_back(subset_index);
            
            // 9. cumulative scale index
            _operations[info.handle].push_back(BEAGLE_OP_NONE);
        }
    }  ///end_addOperation
    
    inline void Likelihood::updateTransitionMatrices() {    ///begin_updateTransitionMatrices
        assert(_instances.size() > 0);
        if (_pmatrix_index.size() == 0)
            return;

        // Loop through all instances
        for (auto & info : _instances) {
            int code = 0;

            unsigned nsubsets = (unsigned)info.subsets.size();
            if (nsubsets > 1) {
                code = beagleUpdateTransitionMatricesWithMultipleModels(
                    info.handle,                                // Instance number
                    &_eigen_indices[info.handle][0],            // Index of eigen-decomposition buffer
                    &_category_rate_indices[info.handle][0],    // category rate indices
                    &_pmatrix_index[info.handle][0],            // transition probability matrices to update
                    NULL,                                       // first derivative matrices to update
                    NULL,                                       // second derivative matrices to update
                    &_edge_lengths[info.handle][0],             // List of edge lengths
                    (int)_pmatrix_index[info.handle].size());   // Length of lists
            }
            else {
                code = beagleUpdateTransitionMatrices(
                    info.handle,                                // Instance number
                    0,                                          // Index of eigen-decomposition buffer
                    &_pmatrix_index[info.handle][0],            // transition probability matrices to update
                    NULL,                                       // first derivative matrices to update
                    NULL,                                       // second derivative matrices to update
                    &_edge_lengths[info.handle][0],             // List of edge lengths
                    (int)_pmatrix_index[info.handle].size());   // Length of lists
            }

            if (code != 0)
                throw XStrom(boost::str(boost::format("Failed to update transition matrices for instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));      
        } 
    }   ///end_updateTransitionMatrices
    
    inline void Likelihood::calculatePartials() {  ///begin_calculatePartials
        assert(_instances.size() > 0);
        if (_operations.size() == 0)
            return;
        int code = 0;
        
        // Loop through all instances
        for (auto & info : _instances) {
            unsigned nsubsets = (unsigned)info.subsets.size();

            if (nsubsets > 1) {
                code = beagleUpdatePartialsByPartition(
                    info.handle,                                                    // Instance number
                    (BeagleOperationByPartition *) &_operations[info.handle][0],    // BeagleOperation list specifying operations
                    (int)(_operations[info.handle].size()/9));                      // Number of operations
                if (code != 0)
                    throw XStrom(boost::format("failed to update partials. BeagleLib error code was %d (%s)") % code % _beagle_error[code]);
            }
            else {
                // no partitioning, just one data subset
                code = beagleUpdatePartials(
                    info.handle,                                        // Instance number
                    (BeagleOperation *) &_operations[info.handle][0],   // BeagleOperation list specifying operations
                    (int)(_operations[info.handle].size()/7),           // Number of operations
                    BEAGLE_OP_NONE);                                    // Index number of scaleBuffer to store accumulated factors
                if (code != 0) 
                    throw XStrom(boost::format("failed to update partials. BeagleLib error code was %d (%s)") % code % _beagle_error[code]);
            }
        } 
    } ///end_calculatePartials
    
    inline double Likelihood::calcInstanceLogLikelihood(InstanceInfo & info, Tree::SharedPtr t) {   ///begin_calcInstanceLogLikelihood
        int code = 0;
        unsigned nsubsets = (unsigned)info.subsets.size();
        assert(nsubsets > 0);
        
        // Assuming there are as many transition matrices as there are edge lengths
        assert(_pmatrix_index[info.handle].size() == _edge_lengths[info.handle].size());

        int state_frequency_index  = 0;
        int category_weights_index = 0;
        int cumulative_scale_index = BEAGLE_OP_NONE;
        int child_partials_index   = getPartialIndex(t->_root, info);
        int parent_partials_index  = getPartialIndex(t->_preorder[0], info);
        int parent_tmatrix_index   = getTMatrixIndex(t->_preorder[0], info, 0);

        // storage for results of the likelihood calculation
        std::vector<double> subset_log_likelihoods(nsubsets, 0.0);
        double log_likelihood = 0.0;

        if (nsubsets > 1) {
            _parent_indices.assign(nsubsets, parent_partials_index);
            _child_indices.assign(nsubsets, child_partials_index);
            _weights_indices.assign(nsubsets, category_weights_index);
            _scaling_indices.resize(nsubsets); 
            _subset_indices.resize(nsubsets);
            _freqs_indices.resize(nsubsets);
            _tmatrix_indices.resize(nsubsets);

            for (unsigned s = 0; s < nsubsets; s++) {
                _scaling_indices[s]  = BEAGLE_OP_NONE; 
                _subset_indices[s]  = s;
                _freqs_indices[s]   = s;
                _tmatrix_indices[s] = getTMatrixIndex(t->_preorder[0], info, s); //index_focal_child + s*tmatrix_skip;
            }
            code = beagleCalculateEdgeLogLikelihoodsByPartition(
                info.handle,                 // instance number
                &_parent_indices[0],         // indices of parent partialsBuffers
                &_child_indices[0],          // indices of child partialsBuffers
                &_tmatrix_indices[0],        // transition probability matrices for this edge
                NULL,                        // first derivative matrices
                NULL,                        // second derivative matrices
                &_weights_indices[0],        // weights to apply to each partialsBuffer
                &_freqs_indices[0],          // state frequencies for each partialsBuffer
                &_scaling_indices[0],        // scaleBuffers containing accumulated factors
                &_subset_indices[0],         // indices of subsets
                nsubsets,                    // partition subset count
                1,                           // number of distinct eigen decompositions
                &subset_log_likelihoods[0],  // address of vector of log likelihoods (one for each subset)
                &log_likelihood,             // destination for resulting log likelihood
                NULL,                        // destination for vector of first derivatives (one for each subset)
                NULL,                        // destination for first derivative
                NULL,                        // destination for vector of second derivatives (one for each subset)
                NULL);                       // destination for second derivative
        }
        else {
            code = beagleCalculateEdgeLogLikelihoods(
                info.handle,                 // instance number
                &parent_partials_index,      // indices of parent partialsBuffers
                &child_partials_index,       // indices of child partialsBuffers
                &parent_tmatrix_index,       // transition probability matrices for this edge
                NULL,                        // first derivative matrices
                NULL,                        // second derivative matrices
                &category_weights_index,     // weights to apply to each partialsBuffer
                &state_frequency_index,      // state frequencies for each partialsBuffer
                &cumulative_scale_index,     // scaleBuffers containing accumulated factors
                1,                           // Number of partialsBuffer
                &log_likelihood,             // destination for log likelihood
                NULL,                        // destination for first derivative
                NULL);                       // destination for second derivative
        }

        if (code != 0)
            throw XStrom(boost::str(boost::format("failed to calculate edge logLikelihoods in CalcLogLikelihood. BeagleLib error code was %d (%s)") % code % _beagle_error[code]));

        return log_likelihood;
    }  ///end_calcInstanceLogLikelihood
    
    inline double Likelihood::calcLogLikelihood(Tree::SharedPtr t) {  ///begin_calcLogLikelihood
        assert(_instances.size() > 0);
        
        if (!_using_data)
            return 0.0;

        // Must call setData before calcLogLikelihood
        assert(_data);

        if (t->_is_rooted)
            throw XStrom("This version of the program can only compute likelihoods for unrooted trees");

        // Assuming "root" is leaf 0
        assert(t->_root->_number == 0 && t->_root->_left_child == t->_preorder[0] && !t->_preorder[0]->_right_sib);

        setModelRateMatrix();
        setAmongSiteRateHeterogenetity();
        defineOperations(t);
        updateTransitionMatrices();
        calculatePartials();

        double log_likelihood = 0.0;
        for (auto & info : _instances) {
            log_likelihood += calcInstanceLogLikelihood(info, t);
        }
        
        return log_likelihood;
    }  ///end_calcLogLikelihood

}   ///end
