#pragma once    ///start

#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include "libhmsbeagle/beagle.h"
#include "tree.hpp"
#include "tree_manip.hpp"   ///!a
#include "data.hpp"
#include "model.hpp"
#include "xstrom.hpp"

#define POLNEWWAY 1

namespace strom {

    class Likelihood {
        public:
                                                    Likelihood();
                                                    ~Likelihood();

            void                                    setRooted(bool is_rooted);
            void                                    setPreferGPU(bool prefer_gpu);
            void                                    setAmbiguityEqualsMissing(bool ambig_equals_missing);
        
            bool                                    usingStoredData() const;
            void                                    useStoredData(bool using_data);
            void                                    useUnderflowScaling(bool do_scaling);

            std::string                             beagleLibVersion() const;
            std::string                             availableResources() const;
            std::string                             usedResources() const;

            void                                    initBeagleLib();
            void                                    finalizeBeagleLib(bool use_exceptions);

            double                                  calcLogLikelihood(Tree::SharedPtr t);

            Data::SharedPtr                         getData();
            void                                    setData(Data::SharedPtr d);

            Model::SharedPtr                        getModel();
            void                                    setModel(Model::SharedPtr m);

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
                bool invarmodel;
                std::vector<unsigned> subsets;
                
                InstanceInfo() : handle(-1), resourcenumber(-1), resourcename(""), nstates(0), nratecateg(0), npatterns(0), partial_offset(0), tmatrix_offset(0), invarmodel(false) {}
            };

            typedef std::pair<unsigned, int>        instance_pair_t;

            bool                                    isPolytomy(Node * nd) const;    ///!b
            unsigned                                countChildren(Node * nd) const; ///!c
#if POLNEWWAY //POLTMP
            unsigned                                getScalerIndex(Node * nd, InstanceInfo & info) const;
#endif
            unsigned                                getPartialIndex(Node * nd, InstanceInfo & info) const;
            unsigned                                getTMatrixIndex(Node * nd, InstanceInfo & info, unsigned subset_index) const;
            void                                    updateInstanceMap(instance_pair_t & p, unsigned subset);
            void                                    newInstance(unsigned nstates, int nrates, std::vector<unsigned> & subset_indices);
            void                                    setTipStates();
            void                                    setTipPartials();
            void                                    setPatternPartitionAssignments();
            void                                    setPatternWeights();
            void                                    setAmongSiteRateHeterogenetity();
            void                                    setModelRateMatrix();
            void                                    addOperation(InstanceInfo & info, Node * nd, Node * lchild, Node * rchild, unsigned subset_index);
            void                                    defineOperations(Tree::SharedPtr t);
            void                                    updateTransitionMatrices();
            void                                    calculatePartials(Tree::SharedPtr t);
            double                                  calcInstanceLogLikelihood(InstanceInfo & inst, Tree::SharedPtr t);


            std::vector<InstanceInfo>               _instances;
            std::map<int, std::string>              _beagle_error;
            std::map<int, std::vector<int> >        _operations;
            std::map<int, std::vector<int> >        _pmatrix_index;
            std::map<int, std::vector<double> >     _edge_lengths;
#if POLNEWWAY
            std::map<int, std::vector<int> >        _eigen_indices;
            std::map<int, std::vector<int> >        _category_rate_indices;
#endif

            std::vector<int>                        _subset_indices;
            std::vector<int>                        _parent_indices;
            std::vector<int>                        _child_indices;
            std::vector<int>                        _tmatrix_indices;
            std::vector<int>                        _weights_indices;
            std::vector<int>                        _freqs_indices;
            std::vector<int>                        _scaling_indices;

            Model::SharedPtr                        _model;

            Data::SharedPtr                         _data;
            unsigned                                _ntaxa;
            bool                                    _rooted;
            bool                                    _prefer_gpu;
            bool                                    _ambiguity_equals_missing;
            bool                                    _underflow_scaling;
            bool                                    _using_data;
        
            std::stack<Node *>                      _polytomy_stack;    ///!d
            std::vector<double>                     _identity_matrix;   ///!e

        public:
            typedef std::shared_ptr< Likelihood >   SharedPtr;
    };
    ///end_class_declaration
    // member function bodies go here

    inline Likelihood::Likelihood() {
        //std::cout << "Constructing a Likelihood" << std::endl;
        clear();
    }

    inline Likelihood::~Likelihood() {
        //std::cout << "Destroying a Likelihood" << std::endl;
        finalizeBeagleLib(false);
        clear();
    }
    
    inline unsigned Likelihood::calcNumEdgesInFullyResolvedTree() const {
        assert(_ntaxa > 0);
        return (_rooted ? (2*_ntaxa - 2) : (2*_ntaxa - 3));
    }
    
    inline unsigned Likelihood::calcNumInternalsInFullyResolvedTree() const {
        assert(_ntaxa > 0);
        return (_rooted ? (_ntaxa - 1) : (_ntaxa - 2));
    }

    inline void Likelihood::finalizeBeagleLib(bool use_exceptions) {
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
    }

    inline void Likelihood::clear() {   ///begin_clear
        finalizeBeagleLib(true);
        
        _ntaxa                      = 0;
        _rooted                     = false;
        _prefer_gpu                 = false;
        _ambiguity_equals_missing   = true;
        _underflow_scaling          = false;
        _using_data                 = true;
        _data                       = nullptr;
        
        _operations.clear();
        _pmatrix_index.clear();
        _edge_lengths.clear();
#if POLNEWWAY
        _eigen_indices.clear();
        _category_rate_indices.clear();
#endif
        _subset_indices.assign(1, 0);
        _parent_indices.assign(1, 0);
        _child_indices.assign(1, 0);
        _tmatrix_indices.assign(1, 0);
        _weights_indices.assign(1, 0);
        _freqs_indices.assign(1, 0);
        _scaling_indices.assign(1, 0);
        _identity_matrix.assign(1, 0.0);    ///!f

        _model = Model::SharedPtr(new Model());        

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
    }   ///end_clear

    inline std::string Likelihood::beagleLibVersion() const {
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
    }
    
    inline Data::SharedPtr Likelihood::getData() {
        return _data;
    }
    
    inline void Likelihood::setData(Data::SharedPtr data) {
        assert(_instances.size() == 0);
        assert(!data->getDataMatrix().empty());
        _data = data;
    }

    inline Model::SharedPtr Likelihood::getModel() {
        return _model;
    }
    
    inline void Likelihood::setModel(Model::SharedPtr m) {
        assert(_instances.size() == 0); // can't change model after initBeagleLib called
        _model = m;
    }

    inline void Likelihood::setRooted(bool is_rooted) {
        assert(_instances.size() == 0 || _rooted == is_rooted); // can't change rooting status after initBeagleLib called
        _rooted = is_rooted;
    }
    
    inline void Likelihood::setAmbiguityEqualsMissing(bool ambig_equals_missing) {
        // Can't change GPU preference status after initBeagleLib called
        assert(_instances.size() == 0 || _ambiguity_equals_missing == ambig_equals_missing);
        _ambiguity_equals_missing = ambig_equals_missing;
    }
    
    inline void Likelihood::setPreferGPU(bool prefer_gpu) {
        // Can't change GPU preference status after initBeagleLib called
        assert(_instances.size() == 0 || _prefer_gpu == prefer_gpu);
        _prefer_gpu = prefer_gpu;
    }
    
    inline bool Likelihood::usingStoredData() const {
        return _using_data;
    }
    
    inline void Likelihood::useStoredData(bool using_data) {
        _using_data = using_data;
    }

    inline void Likelihood::useUnderflowScaling(bool do_scaling) { 
        _underflow_scaling = do_scaling;
    } 

    inline void Likelihood::initBeagleLib() {
        assert(_data);
        assert(_model);

        // Close down any existing BeagleLib instances
        finalizeBeagleLib(true);

        _ntaxa = _data->getNumTaxa();
        
        unsigned nsubsets = _data->getNumSubsets();
        std::set<instance_pair_t> nstates_ncateg_combinations;
        std::map<instance_pair_t, std::vector<unsigned> > subsets_for_pair;
        for (unsigned subset = 0; subset < nsubsets; subset++) {
            // Create a pair comprising number of states and number of rate categories
            unsigned nstates = _data->getNumStatesForSubset(subset);
            bool invar_model = _model->getSubsetIsInvarModel(subset);
            int nrates = (invar_model ? -1 : 1)*_model->getSubsetNumCateg(subset);
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
            std::cout << boost::str(boost::format("Created BeagleLib instance %d (%d states, %d rate%s, %d subset%s, %s invar. sites model)") % info.handle % info.nstates % info.nratecateg % (info.nratecateg == 1 ? "" : "s") % info.subsets.size() % (info.subsets.size() == 1 ? "" : "s") % (info.invarmodel ? "is" : "not")) << std::endl;
        }
        
        if (_ambiguity_equals_missing)
            setTipStates();
        else
            setTipPartials();
        setPatternWeights();
        setPatternPartitionAssignments();
    }
    
    inline void Likelihood::newInstance(unsigned nstates, int nrates, std::vector<unsigned> & subset_indices) { ///begin_newInstance
        // Create an identity matrix used for computing partials    ///!g
        // for polytomies (represents the transition matrix
        // for the zero-length edges inserted to arbitrarily 
        // resolve each polytomy)
        _identity_matrix.assign(nstates*nstates*nrates, 0.0);
        for (unsigned k = 0; k < nrates; k++) {
            unsigned offset = k*nstates*nstates;
            _identity_matrix[0+offset]  = 1.0;
            _identity_matrix[5+offset]  = 1.0;
            _identity_matrix[10+offset] = 1.0;
            _identity_matrix[15+offset] = 1.0;
        }   ///!h
        
        unsigned num_subsets = (unsigned)subset_indices.size();
        //...   
        ///after_identity_matrix_init
                
        bool is_invar_model = (nrates < 0 ? true : false);
        unsigned ngammacat = (unsigned)(is_invar_model ? -nrates : nrates);
        
        unsigned num_patterns = 0;
        for (auto s : subset_indices) {
            num_patterns += _data->getNumPatternsInSubset(s);
        }
        
        unsigned num_internals = calcNumInternalsInFullyResolvedTree();
        unsigned num_edges = calcNumEdgesInFullyResolvedTree();
        unsigned num_transition_probs = num_edges*num_subsets;
        
        // add 1 so that subroot node will have a tmatrix, root tip's tmatrix is never used
        num_transition_probs += 1;

        long requirementFlags = 0;

        long preferenceFlags = BEAGLE_FLAG_PRECISION_SINGLE | BEAGLE_FLAG_THREADING_CPP;
        if (_underflow_scaling) {
            preferenceFlags |= BEAGLE_FLAG_SCALING_MANUAL;
            preferenceFlags |= BEAGLE_FLAG_SCALERS_LOG;     //POLNEW    //POLTMP
        }
        if (_prefer_gpu)
            preferenceFlags |= BEAGLE_FLAG_PROCESSOR_GPU;
        else
            preferenceFlags |= BEAGLE_FLAG_PROCESSOR_CPU;
        
        BeagleInstanceDetails instance_details;
        unsigned npartials = num_internals + _ntaxa;
#if POLNEWWAY //POLTMP
        unsigned nscalers = num_internals;  // one for every internal node
#endif
        unsigned nsequences = 0;
        if (_ambiguity_equals_missing) {
            npartials -= _ntaxa;
            nsequences += _ntaxa;
        }
        
        int inst = beagleCreateInstance(
             _ntaxa,                        // tips
             2*npartials,                   // partials
             nsequences,                    // sequences
             nstates,                       // states
             num_patterns,                  // patterns (total across all subsets that use this instance)
             num_subsets,                   // models (one for each distinct eigen decomposition)
             2*num_subsets*num_transition_probs, // transition matrices (one for each edge in each subset)
             ngammacat,                     // rate categories
#if POLNEWWAY //POLTMP
             (_underflow_scaling ? 2*nscalers + 1 : 0),  // scale buffers (+1 is for the cumulative scaler at index 0)
#else
             (_underflow_scaling ? num_internals + num_subsets : 0),    // scale buffers
#endif
             NULL,                          // resource restrictions
             0,                             // length of resource list
             preferenceFlags,               // preferred flags
             requirementFlags,              // required flags
             &instance_details);            // pointer for details
        
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
        info.invarmodel     = is_invar_model;
        info.subsets        = subset_indices;
        info.npatterns      = num_patterns;
        info.partial_offset = num_internals;
        info.tmatrix_offset = num_edges;
        _instances.push_back(info);
    }   ///end_newInstance

    inline void Likelihood::setTipStates() {
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
    }

    inline void Likelihood::setTipPartials() {
        assert(_instances.size() > 0);
        assert(_data);
        Data::state_t one = 1;
        
        for (auto & info : _instances) {
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
                            partials[k++] = d & 1 ? 1.0 : 0.0;
                            partials[k++] = d & 2 ? 1.0 : 0.0;
                            partials[k++] = d & 4 ? 1.0 : 0.0;
                            partials[k++] = d & 8 ? 1.0 : 0.0;
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
    }

    inline void Likelihood::setPatternPartitionAssignments() {
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
    }
    
    inline void Likelihood::setPatternWeights() {
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
    }

    inline void Likelihood::setAmongSiteRateHeterogenetity() {
        assert(_instances.size() > 0);
        int code = 0;
        
        // Loop through all instances
        for (auto & info : _instances) {

            // Loop through all subsets assigned to this instance
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                code = _model->setBeagleAmongSiteRateVariationRates(info.handle, s, instance_specific_subset_index);
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set category rates for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));
            
                code = _model->setBeagleAmongSiteRateVariationProbs(info.handle, s, instance_specific_subset_index);
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set category probabilities for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));
                    
                ++instance_specific_subset_index;
            }
        }
    }

    inline void Likelihood::setModelRateMatrix() {
        // Loop through all instances
        for (auto & info : _instances) {

            // Loop through all subsets assigned to this instance
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                int code = _model->setBeagleStateFrequencies(info.handle, s, instance_specific_subset_index);
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set state frequencies for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));

                code = _model->setBeagleEigenDecomposition(info.handle, s, instance_specific_subset_index);
                if (code != 0)
                    throw XStrom(boost::str(boost::format("Failed to set eigen decomposition for BeagleLib instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));
                
                ++instance_specific_subset_index;
            }
        }
    }
    
    inline bool Likelihood::isPolytomy(Node * nd) const {   ///begin_isPolytomy
        Node * lchild = nd->_left_child;
        assert(lchild);    // should only call this function for internal nodes
        
        Node * rchild = lchild->_right_sib;
        if (rchild && rchild->_right_sib)
            return true;
        return false;
    }   ///end_isPolytomy
    
    inline unsigned Likelihood::countChildren(Node * nd) const {    ///begin_countChildren
        Node * child = nd->_left_child;
        assert(child);    // should only call this function for internal nodes
        unsigned nchildren = 0;
        while (child) {
            nchildren++;
            child = child->_right_sib;
        }
        return nchildren;
    }   ///end_countChildren
    
#if POLNEWWAY //POLTMP
    inline unsigned Likelihood::getScalerIndex(Node * nd, InstanceInfo & info) const {
        if (!nd->_parent) {
            std::cerr << "oops1" << std::endl;
        }
        if (!nd->_left_child) {
            std::cerr << "oops2" << std::endl;
        }
        assert(nd->_parent && nd->_left_child); // nd is supposed to be an internal node and not the tip root node
        unsigned sindex = nd->_number - _ntaxa + 1; // +1 to skip the cumulative scaler vector
        if (nd->isAltPartial())
            sindex += info.partial_offset;
        return sindex;
    }
#endif
    
    inline unsigned Likelihood::getPartialIndex(Node * nd, InstanceInfo & info) const {
        unsigned pindex = nd->_number;
        // do not be tempted to subtract _ntaxa from pindex: BeagleLib does this itself
        if (nd->_parent && nd->_left_child) {
            if (nd->isAltPartial())
                pindex += info.partial_offset;
        }
        return pindex;
    }
    
    inline unsigned Likelihood::getTMatrixIndex(Node * nd, InstanceInfo & info, unsigned subset_index) const {
        unsigned tindex = 2*subset_index*info.tmatrix_offset + nd->_number;
        if (nd->isAltTMatrix())
            tindex += info.tmatrix_offset;
        return tindex;
    }
    
    inline void Likelihood::addOperation(InstanceInfo & info, Node * nd, Node * lchild, Node * rchild, unsigned subset_index) {
        assert(nd);
        assert(lchild);
        assert(rchild);
#if POLNEWWAY //POLTMP
#else
        unsigned num_subsets = (unsigned)info.subsets.size();
#endif

        // 1. destination partial to be calculated
        int partial_dest = getPartialIndex(nd, info);
        _operations[info.handle].push_back(partial_dest);

        // 2. destination scaling buffer index to write to
        int scaler_index = BEAGLE_OP_NONE;
        if (_underflow_scaling) {
#if POLNEWWAY //POLTMP
            scaler_index = getScalerIndex(nd, info);
#else
            scaler_index = nd->_number - _ntaxa + num_subsets;
#endif
        }
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
#if POLNEWWAY //POLTMP
            _operations[info.handle].push_back(BEAGLE_OP_NONE); // accumulate in calcInstanceLogLikelihood
#else
            if (_underflow_scaling)
                _operations[info.handle].push_back(subset_index);
            else
                _operations[info.handle].push_back(BEAGLE_OP_NONE);
#endif
        }
        
#if 1 //POLTMP
    std::cerr << "\naddOperation:" << std::endl;
    std::cerr << boost::str(boost::format("%6d partial_dest") % partial_dest) << std::endl;
    std::cerr << boost::str(boost::format("%6d scaler_index") % scaler_index) << std::endl;
    std::cerr << boost::str(boost::format("%6d partial_lchild") % partial_lchild) << std::endl;
    std::cerr << boost::str(boost::format("%6d tindex_lchild") % tindex_lchild) << std::endl;
    std::cerr << boost::str(boost::format("%6d partial_rchild") % partial_rchild) << std::endl;
    std::cerr << boost::str(boost::format("%6d tindex_rchild") % tindex_rchild) << std::endl;
    std::cerr << std::endl;
#endif
    }
    
    inline void Likelihood::defineOperations(Tree::SharedPtr t) {   ///begin_defineOperations
        assert(_instances.size() > 0);
        assert(t);
        assert(t->isRooted() == _rooted);
        assert(_polytomy_stack.empty());    ///!i

        double relrate_normalizing_constant = _model->calcNormalizingConstantForSubsetRelRates();
        Model::subset_relrate_vect_t & subset_relrates = _model->getSubsetRelRates();

        // Loop through all instances
        for (auto & info : _instances) {
            _operations[info.handle].clear();
            _pmatrix_index[info.handle].clear();
            _edge_lengths[info.handle].clear();
#if POLNEWWAY
            _eigen_indices[info.handle].clear();
            _category_rate_indices[info.handle].clear();
#endif

            // Loop through all subsets assigned to this instance
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                double subset_relative_rate = subset_relrates[s]/relrate_normalizing_constant;

                // Loop through all nodes in reverse level order
                for (auto nd : boost::adaptors::reverse(t->_levelorder)) {
                    assert(nd->_number >= 0);
                    if (!nd->_left_child) {
                        // This is a leaf
                        if (nd->isSelTMatrix()) {
                            unsigned tindex = getTMatrixIndex(nd, info, instance_specific_subset_index);
                            _pmatrix_index[info.handle].push_back(tindex);
                            _edge_lengths[info.handle].push_back(nd->_edge_length*subset_relative_rate);
#if POLNEWWAY
                            _eigen_indices[info.handle].push_back(s);
                            _category_rate_indices[info.handle].push_back(s);
#endif
                        }
                    }
                    // ...
                    else {
                        // This is an internal node
                        if (nd->isSelTMatrix()) {
                            unsigned tindex = getTMatrixIndex(nd, info, instance_specific_subset_index);
                            _pmatrix_index[info.handle].push_back(tindex);
                            _edge_lengths[info.handle].push_back(nd->_edge_length*subset_relative_rate);
#if POLNEWWAY
                            _eigen_indices[info.handle].push_back(s);
                            _category_rate_indices[info.handle].push_back(s);
#endif
                        }

                        // Internal nodes have partials to be calculated, so define
                        // an operation to compute the partials for this node
                        if (nd->isSelPartial()) {
                            if (isPolytomy(nd)) {   ///!j
                                // Internal node is a polytomy
                                TreeManip tm(t);
                                unsigned nchildren = countChildren(nd);
                                Node * a = nd->_left_child;
                                Node * b = a->_right_sib;
                                Node * c = 0;
                                for (unsigned k = 0; k < nchildren - 2; k++) {
                                    c = tm.getUnusedNode();
                                    c->_left_child = a;
                                    _polytomy_stack.push(c);    ///!k

                                    // Set the transition matrix equal to the identity matrix   ///!l
                                    unsigned tindex = getTMatrixIndex(c, info, instance_specific_subset_index);
                                    int code = beagleSetTransitionMatrix(info.handle, tindex, &_identity_matrix[0], 1);
                                    if (code != 0)
                                        throw XStrom(boost::str(boost::format("Failed to set transition matrix for instance %d. BeagleLib error code was %d (%s)") % info.handle % code % _beagle_error[code]));    ///!m

                                    // Add an operation to compute the partial for this fake internal node  ///!n
                                    addOperation(info, c, a, b, instance_specific_subset_index);    ///!o
                                    
                                    // Tackle next arm of the polytomy  ///!p
                                    b = b->_right_sib;
                                    a = c;
                                }   ///!q
                                
                                // Now add operation to compute the partial for the real internal node  ///!r
                                addOperation(info, nd, a, b, instance_specific_subset_index);
                            }
                            else {
                                // Internal node is not a polytomy
                                Node * lchild = nd->_left_child;
                                assert(lchild);
                                Node * rchild = lchild->_right_sib;
                                assert(rchild);
                                addOperation(info, nd, lchild, rchild, instance_specific_subset_index);
                            }   ///!s
                        }   // isSelPartial
                    }   // internal node
                }   // nd loop
                ++instance_specific_subset_index;
            }   // subsets loop
        } // instances loop
    }   ///end_defineOperations
    
    inline void Likelihood::updateTransitionMatrices() {
        assert(_instances.size() > 0);
        if (_pmatrix_index.size() == 0)
            return;
#if POLNEWWAY
#else
        unsigned num_edges = calcNumEdgesInFullyResolvedTree();
        std::vector<int> eigen_indices;
        std::vector<int> category_rate_indices;
#endif
        
        // Loop through all instances
        for (auto & info : _instances) {
            int code = 0;

            unsigned nsubsets = (unsigned)info.subsets.size();
            if (nsubsets > 1) {
#if POLNEWWAY
#else
                eigen_indices.resize(nsubsets*num_edges, 0);
                category_rate_indices.resize(nsubsets*num_edges, 0);
                unsigned k = 0;
                for (unsigned s = 0; s < nsubsets; s++) {
                    for (unsigned v = 0; v < num_edges; v++) {
                        eigen_indices[k] = s;
                        category_rate_indices[k] = s;
                        k++;
                    }
                }
#endif
                code = beagleUpdateTransitionMatricesWithMultipleModels(
                    info.handle,                                // Instance number
#if POLNEWWAY
                    &_eigen_indices[info.handle][0],            // Index of eigen-decomposition buffer
                    &_category_rate_indices[info.handle][0],    // category rate indices
#else
                    &eigen_indices[0],                          // Index of eigen-decomposition buffer
                    &category_rate_indices[0],                  // category rate indices
#endif
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
                
        }   // instance loop
    }
    
    inline void Likelihood::calculatePartials(Tree::SharedPtr t) {
        assert(_instances.size() > 0);
        if (_operations.size() == 0)
            return;
        int code = 0;
        int cumulative_scale_index = 0;
        
        // Loop through all instances
        for (auto & info : _instances) {
            unsigned nsubsets = (unsigned)info.subsets.size();

            if (nsubsets > 1) {
#if POLNEWWAY   //POLTMP
                //temporary
                if (_underflow_scaling) {
                    for (unsigned s = 0; s < nsubsets; ++s) {
                        code = beagleResetScaleFactorsByPartition(info.handle, cumulative_scale_index, s);
                        if (code != 0)
                            throw XStrom(boost::str(boost::format("failed to reset scale factors for subset %d in calculatePartials. BeagleLib error code was %d (%s)") % s % code % _beagle_error[code]));
                    }
                }
#else
                if (_underflow_scaling) {
                    for (unsigned s = 0; s < nsubsets; ++s) {
                        code = beagleResetScaleFactorsByPartition(info.handle, s, s);
                        if (code != 0)
                            throw XStrom(boost::str(boost::format("failed to reset scale factors for subset %d in calculatePartials. BeagleLib error code was %d (%s)") % s % code % _beagle_error[code]));
                    }
                }
#endif
                code = beagleUpdatePartialsByPartition(
                    info.handle,                                                    // Instance number
                    (BeagleOperationByPartition *) &_operations[info.handle][0],    // BeagleOperation list specifying operations
                    (int)(_operations[info.handle].size()/9));                      // Number of operations
                    
#if 1   //POLTMP output partials
                // Output partials
                Node * nd7 = t->_preorder[0];
                Node * nd3 = nd7->_left_child;
                Node * nd6 = nd3->_right_sib;
                Node * nd1 = nd6->_left_child;
                Node * nd5 = nd1->_right_sib;
                Node * nd2 = nd5->_left_child;
                Node * nd4 = nd2->_right_sib;
                int code = 0;
                unsigned buffer_index = 5;
                unsigned scale_index = BEAGLE_OP_NONE;
                std::vector<double> tmp(3*4, 0.0);
                std::cerr << "\nPartials:" << std::endl;
                for (auto & info : _instances) {
                    buffer_index = getPartialIndex(nd5, info);
                    scale_index = getScalerIndex(nd5, info);;
                    std::cerr << boost::str(boost::format("instance %d, buffer index %d, scale index %d:") % info.handle % buffer_index % scale_index) << std::endl;
                    code = beagleGetPartials(info.handle, buffer_index, scale_index, &tmp[0]);
                    assert(code == 0);
                    for (unsigned k = 0; k < 4; k++) {
                        std::cerr << boost::str(boost::format("%6d %12.5f %6d %12.5f %6d %12.5f") % (k+1) % tmp[k] % (k+5) % tmp[k+4] % (k+9) % tmp[k+8]) << std::endl;
                    }
                                        
                    buffer_index = getPartialIndex(nd6, info);
                    scale_index = getScalerIndex(nd6, info);;
                    std::cerr << boost::str(boost::format("instance %d, buffer index %d, scale index %d:") % info.handle % buffer_index % scale_index) << std::endl;
                    code = beagleGetPartials(info.handle, buffer_index, scale_index, &tmp[0]);
                    assert(code == 0);
                    for (unsigned k = 0; k < 4; k++) {
                        std::cerr << boost::str(boost::format("%6d %12.5f %6d %12.5f %6d %12.5f") % (k+1) % tmp[k] % (k+5) % tmp[k+4] % (k+9) % tmp[k+8]) << std::endl;
                    }
                                        
                    buffer_index = getPartialIndex(nd7, info);
                    scale_index = getScalerIndex(nd7, info);;
                    std::cerr << boost::str(boost::format("instance %d, buffer index %d, scale index %d:") % info.handle % buffer_index % scale_index) << std::endl;
                    code = beagleGetPartials(info.handle, buffer_index, scale_index, &tmp[0]);
                    assert(code == 0);
                    for (unsigned k = 0; k < 4; k++) {
                        std::cerr << boost::str(boost::format("%6d %12.5f %6d %12.5f %6d %12.5f") % (k+1) % tmp[k] % (k+5) % tmp[k+4] % (k+9) % tmp[k+8]) << std::endl;
                    }
                    
                    buffer_index = getPartialIndex(nd7, info);
                    scale_index = cumulative_scale_index;
                    std::cerr << boost::str(boost::format("instance %d, buffer index %d, scale index %d:") % info.handle % buffer_index % scale_index) << std::endl;
                    code = beagleGetPartials(info.handle, buffer_index, scale_index, &tmp[0]);
                    assert(code == 0);
                    for (unsigned k = 0; k < 4; k++) {
                        std::cerr << boost::str(boost::format("%6d %12.5f %6d %12.5f %6d %12.5f") % (k+1) % tmp[k] % (k+5) % tmp[k+4] % (k+9) % tmp[k+8]) << std::endl;
                    }
                    
                }
                std::cerr << std::endl;
#endif
            }
            else {
                // no partitioning, just one data subset
#if POLNEWWAY   //POLTMP
#else
                if (_underflow_scaling) {
                    code = beagleResetScaleFactors(info.handle, cumulative_scale_index);
                    if (code != 0)
                        throw XStrom(boost::str(boost::format("failed to reset scale factors in calculatePartials. BeagleLib error code was %d (%s)") % code % _beagle_error[code]));
                }
#endif

                code = beagleUpdatePartials(
                    info.handle,                                        // Instance number
                    (BeagleOperation *) &_operations[info.handle][0],   // BeagleOperation list specifying operations
                    (int)(_operations[info.handle].size()/7),           // Number of operations
#if POLNEWWAY   //POLTMP
                    BEAGLE_OP_NONE);                                    // Index number of scaleBuffer to store accumulated factors
#else
                    (_underflow_scaling ? 0 : BEAGLE_OP_NONE));         // Index number of scaleBuffer to store accumulated factors
#endif
            }
            
            if (code != 0) {
                throw XStrom(boost::format("failed to update partials. BeagleLib error code was %d (%s)") % code % _beagle_error[code]);
            }
        }   // instance loop
    }
    
    inline double Likelihood::calcInstanceLogLikelihood(InstanceInfo & info, Tree::SharedPtr t) {
        int code = 0;
        unsigned nsubsets = (unsigned)info.subsets.size();
        assert(nsubsets > 0);
        //unsigned nedges = calcNumEdgesInFullyResolvedTree();                    //eliminate

        // Assuming there are as many transition matrices as there are edge lengths
        assert(_pmatrix_index[info.handle].size() == _edge_lengths[info.handle].size());

        int state_frequency_index  = 0;
        int category_weights_index = 0;
        int cumulative_scale_index = (_underflow_scaling ? 0 : BEAGLE_OP_NONE);
        int child_partials_index   = getPartialIndex(t->_root, info);
        int parent_partials_index = getPartialIndex(t->_preorder[0], info);
        int parent_tmatrix_index = getTMatrixIndex(t->_preorder[0], info, 0);

        // storage for results of the likelihood calculation
        std::vector<double> subset_log_likelihoods(nsubsets, 0.0);
        double log_likelihood = 0.0;
        
#if POLNEWWAY //POLTMP
        if (_underflow_scaling) {
            // Create vector of all scaling vector indices in current use
            std::vector<int> internal_node_scaler_indices;
            for (auto nd : t->_preorder) {
                if (nd->_left_child) {
                    unsigned s = getScalerIndex(nd, info);
                    internal_node_scaler_indices.push_back(s);
                }
            }
            
            if (nsubsets == 1) {
                code = beagleResetScaleFactors(info.handle, cumulative_scale_index);
                if (code != 0)
                    throw XStrom(boost::str(boost::format("failed to reset scale factors in calcInstanceLogLikelihood. BeagleLib error code was %d (%s)") % code % _beagle_error[code]));

                code = beagleAccumulateScaleFactors(
                     info.handle,
                     &internal_node_scaler_indices[0],
                     (int)internal_node_scaler_indices.size(),
                     cumulative_scale_index);
                if (code != 0)
                    throw XStrom(boost::str(boost::format("failed to accumulate scale factors in calcInstanceLogLikelihood. BeagleLib error code was %d (%s)") % code % _beagle_error[code]));
            }
            else {
                for (unsigned s = 0; s < nsubsets; ++s) {
                    code = beagleResetScaleFactorsByPartition(info.handle, s, cumulative_scale_index);
                    if (code != 0)
                        throw XStrom(boost::str(boost::format("failed to reset scale factors for subset %d in calcInstanceLogLikelihood. BeagleLib error code was %d (%s)") % s % code % _beagle_error[code]));
                        
                    code = beagleAccumulateScaleFactorsByPartition(
                        info.handle,
                        &internal_node_scaler_indices[0],
                        (int)internal_node_scaler_indices.size(),
                        cumulative_scale_index,
                        s);
                    if (code != 0)
                        throw XStrom(boost::str(boost::format("failed to acccumulate scale factors for subset %d in calcInstanceLogLikelihood. BeagleLib error code was %d (%s)") % s % code % _beagle_error[code]));
                }
            }
                    
#if 1   //POLTMP output partials
            // Output partials for node 7 using accumulated scalers
            unsigned buffer_index = 7;
            unsigned scale_index = 0;
            std::vector<double> tmp(3*4, 0.0);
            std::cerr << "\nPartials using accumulated scalers:" << std::endl;
            std::cerr << boost::str(boost::format("instance %d, buffer index %d, scale index %d:") % info.handle % buffer_index % scale_index) << std::endl;
            code = beagleGetPartials(info.handle, buffer_index, scale_index, &tmp[0]);
            assert(code == 0);
            for (unsigned k = 0; k < 3*4; k++) {
                std::cerr << boost::str(boost::format("%12d %12.5f") % (k+1) % tmp[k]) << std::endl;
            }
            std::cerr << std::endl;
#endif
        }
#endif

        if (nsubsets > 1) {
            _parent_indices.assign(nsubsets, parent_partials_index);
            _child_indices.assign(nsubsets, child_partials_index);
            _weights_indices.assign(nsubsets, category_weights_index);
            _scaling_indices.resize(nsubsets);
            _subset_indices.resize(nsubsets);
            _freqs_indices.resize(nsubsets);
            _tmatrix_indices.resize(nsubsets);

            for (unsigned s = 0; s < nsubsets; s++) {
#if POLNEWWAY //POLTMP
                _scaling_indices[s]  = (_underflow_scaling ? 0 : BEAGLE_OP_NONE);
#else
                _scaling_indices[s]  = (_underflow_scaling ? s : BEAGLE_OP_NONE);
#endif
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
#if 0 //POLTMP
            std::cerr << "logL for subset 0: " << subset_log_likelihoods[0] << std::endl;
            std::cerr << "logL for subset 1: " << subset_log_likelihoods[1] << std::endl;
            std::cerr << std::endl;
#endif
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
        
        // ...
        
        if (code != 0) {
            throw XStrom(boost::str(boost::format("failed to calculate edge log-likelihoods in calcInstanceLogLikelihood. BeagleLib error code was %d (%s)") % code % _beagle_error[code]));
        }
        
        if (info.invarmodel) {
            auto monomorphic = _data->getMonomorphic();
            auto counts = _data->getPatternCounts();
            std::vector<double> site_log_likelihoods(info.npatterns, 0.0);
            double * siteLogLs = &site_log_likelihoods[0];

            beagleGetSiteLogLikelihoods(info.handle, siteLogLs);

            // Loop through all subsets assigned to this instance
            double lnL = 0.0;
            unsigned i = 0;
            for (unsigned s : info.subsets) {
                const ASRV & asrv = _model->getASRV(s);
                const QMatrix & qmatrix = _model->getQMatrix(s);
                const double * freq = qmatrix.getStateFreqs();
                
                double pinvar = *(asrv.getPinvarSharedPtr());
                assert(pinvar >= 0.0 && pinvar <= 1.0);

                if (pinvar == 0.0) {
                    // log likelihood for this subset is equal to the sum of site log-likelihoods
                    auto interval = _data->getSubsetBeginEnd(s);
                    for (unsigned p = interval.first; p < interval.second; p++) {
                        lnL += counts[p]*site_log_likelihoods[i++];
                    }
                }
                else {
                    // Loop through all patterns in this subset
                    double log_pinvar = log(pinvar);
                    double log_one_minus_pinvar = log(1.0 - pinvar);
                    auto interval = _data->getSubsetBeginEnd(s);
                    for (unsigned p = interval.first; p < interval.second; p++) {
                        // Loop through all states for this pattern
                        double invar_like = 0.0;
                        if (monomorphic[p] > 0) {
                            for (unsigned k = 0; k < info.nstates; ++k) {
                                Data::state_t x = (Data::state_t)1 << k;
                                double condlike = (x & monomorphic[p] ? 1.0 : 0.0);
                                double basefreq = freq[k];
                                invar_like += condlike*basefreq;
                            }
                        }
                        double site_lnL = site_log_likelihoods[i++];
                        double log_like_term = log_one_minus_pinvar + site_lnL;
                        if (invar_like > 0.0) {
                            double log_invar_term = log_pinvar + log(invar_like);
                            double site_log_like = (log_like_term + log(1.0 + exp(log_invar_term - log_like_term)));
                            lnL += counts[p]*site_log_like;
                        }
                        else {
                            lnL += counts[p]*log_like_term;
                        }
                    }
                }
            }
            log_likelihood = lnL;
        }

        return log_likelihood;
    }
    
    inline double Likelihood::calcLogLikelihood(Tree::SharedPtr t) {    ///begin_calcLogLikelihood
        assert(_instances.size() > 0);
        
        if (!_using_data)
            return 0.0;
        
        // Must call setData and setModel before calcLogLikelihood
        assert(_data);
        assert(_model);

        if (t->_is_rooted)
            throw XStrom("This version of the program can only compute likelihoods for unrooted trees");

        // Assuming "root" is leaf 0
        assert(t->_root->_number == 0 && t->_root->_left_child == t->_preorder[0] && !t->_preorder[0]->_right_sib);

        setModelRateMatrix();
        setAmongSiteRateHeterogenetity();
        defineOperations(t);
        updateTransitionMatrices();
        calculatePartials(t);
        
        // We no longer need fake internal nodes used to compute partials for polytomies    ///!t
        TreeManip tm(t);
        while (!_polytomy_stack.empty()) {
            tm.putUnusedNode(_polytomy_stack.top());
            _polytomy_stack.pop();
        }   ///!u

        double log_likelihood = 0.0;
        for (auto & info : _instances) {
            log_likelihood += calcInstanceLogLikelihood(info, t);
        }
        
#if 0   //POLTMP
        // Output pattern log-likelihoods
        for (auto & info : _instances) {
            std::vector<double> site_loglikes(info.npatterns, 0.0);
            beagleGetSiteLogLikelihoods(info.handle, &site_loglikes[0]);
            std::cout << std::endl;
            std::cout << "Pattern log-likelihoods:" << std::endl;
            for (unsigned i = 0; i < info.npatterns; i++) {
                double pattern_lnL = site_loglikes[i];
                std::cout << boost::format("%6d %12.8f") % (i+1) % pattern_lnL << std::endl;
            }
            std::cout << std::endl;
        }

        // Output partials
        unsigned buffer_index = 0;
        unsigned scale_index = 0;
        unsigned code = 0;
        std::vector<double> tmp(3*4, 0.0);
        std::cerr << "\nPartials:" << std::endl;
        for (auto & info : _instances) {
            buffer_index = 7; scale_index = 3;
            std::cerr << boost::str(boost::format("subset %d buffer %d scale %d:") % info.handle % buffer_index % scale_index) << std::endl;
            code = beagleGetPartials(info.handle, buffer_index, scale_index, &tmp[0]);
            assert(code == 0);
            for (unsigned k = 0; k < 3*4; k++) {
                std::cerr << boost::str(boost::format("%12d %12.5f") % (k+1) % tmp[k]) << std::endl;
            }

            buffer_index = 7; scale_index = 0;
            std::cerr << boost::str(boost::format("subset %d buffer %d scale %d:") % info.handle % buffer_index % scale_index) << std::endl;
            code = beagleGetPartials(info.handle, buffer_index, scale_index, &tmp[0]);
            assert(code == 0);
            for (unsigned k = 0; k < 3*4; k++) {
                std::cerr << boost::str(boost::format("%12d %12.5f") % (k+1) % tmp[k]) << std::endl;
            }
        }

        // Output scale buffers
        //unsigned num_internals = calcNumInternalsInFullyResolvedTree();
        //for (auto & info : _instances) {
        //    std::cerr << "\nSubset scaling factors for instance " << info.handle << std::endl;
        //    unsigned nsubsets = (unsigned)info.subsets.size();
        //    std::vector<double> scaling_factors(num_internals + nsubsets, 0.0);
        //    int code = 0;
        //
        //    for (unsigned j = 0; j < 1000; j++) {
        //        scaling_factors.assign(num_internals + nsubsets, 0.0);
        //        code = beagleGetScaleFactors(info.handle, j, &scaling_factors[0]);
        //        assert(code == 0);
        //        std::cerr << "*** index = " << j << std::endl;
        //        for (unsigned i = 0; i < num_internals + nsubsets; i++) {
        //            std::cerr << boost::str(boost::format("%12d %12.5f") % (i+1) % scaling_factors[i]) << std::endl;
        //        }
        //    }
        //}
#endif
        
        return log_likelihood;
    }   ///end_calcLogLikelihood

}

        // Output site log-likelihoods
//        const Data::pattern_counts_t & counts = _data->getPatternCounts();
//        const Data::partition_key_t & subset = _data->getPartitionKey();
//        unsigned npatterns = _data->getNumPatterns();
//        double* siteLogLs = (double*) malloc(sizeof(double) * npatterns);
//        beagleGetSiteLogLikelihoods(_instances, siteLogLs);
//        std::cout << "Site log-likelihoods:" << std::endl;
//        std::cout << boost::format("%6s %6s %6s %12s %12s") % "site" % "count" % "subset" % "siteLogL" % "cumlogL" << std::endl;
//        double cumlogL = 0.0;
//        unsigned current_subset = 0;
//        for (unsigned i = 0; i < npatterns; i++) {
//            if (subset[i] != (int)current_subset) {
//                current_subset = subset[i];
//                cumlogL = 0.0;
//                std::cout << std::endl;
//            }
//            double pattern_lnL = counts[i]*siteLogLs[i];
//            cumlogL += pattern_lnL;
//            std::cout << boost::format("%6d %6d %6d %12.8f %12.8f") % (i+1) % counts[i] % subset[i] % siteLogLs[i] % cumlogL << std::endl;
//        }
