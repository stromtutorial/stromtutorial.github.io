#pragma once    ///start

#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include "libhmsbeagle/beagle.h"
#include "tree.hpp"
#include "data.hpp"
#include "model.hpp"    ///!a
#include "xstrom.hpp"
///end_headers
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

            std::string                             beagleLibVersion() const;
            std::string                             availableResources() const;
            std::string                             usedResources() const;

            void                                    initBeagleLib();
            void                                    finalizeBeagleLib(bool use_exceptions);

            double                                  calcLogLikelihood(Tree::SharedPtr t);

            Data::SharedPtr                         getData();
            void                                    setData(Data::SharedPtr d);

            Model::SharedPtr                        getModel();                     ///getModel
            void                                    setModel(Model::SharedPtr m);   ///setModel

            void                                    clear();
            
            unsigned                                calcNumEdgesInFullyResolvedTree() const;
            unsigned                                calcNumInternalsInFullyResolvedTree() const;

        private:
        
            struct InstanceInfo {   ///begin_InstanceInfo
                int handle;
                int resourcenumber;
                std::string resourcename;
                unsigned nstates;
                unsigned nratecateg;
                unsigned npatterns;
                unsigned partial_offset;
                unsigned tmatrix_offset;
                bool invarmodel;                ///!b
                std::vector<unsigned> subsets;
                
                InstanceInfo() : handle(-1), resourcenumber(-1), resourcename(""), nstates(0), nratecateg(0), npatterns(0), partial_offset(0), tmatrix_offset(0), invarmodel(false) {}  ///!c
            };   ///end_InstanceInfo

            typedef std::pair<unsigned, int>        instance_pair_t;

            unsigned                                getScalerIndex(Node * nd, InstanceInfo & info) const;
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
        
            Model::SharedPtr                        _model; ///model_data_member

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

    inline void Likelihood::clear() {
        finalizeBeagleLib(true);
        
        _ntaxa      = 0;
        _rooted     = false;
        _prefer_gpu = false;
        _ambiguity_equals_missing   = true;
        _using_data = true;
        _data       = nullptr;
        
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
        
        _model = Model::SharedPtr(new Model());     ///model_clear     

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
    }

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

    inline Model::SharedPtr Likelihood::getModel() {  ///begin_getModel
        return _model;
    }
    
    inline void Likelihood::setModel(Model::SharedPtr m) {
        assert(_instances.size() == 0); // can't change model after initBeagleLib called
        _model = m;
    }   ///end_setModel

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
    
    inline void Likelihood::initBeagleLib() {   ///begin_initBeagleLib
        assert(_data);
        assert(_model); ///!d

        // Close down any existing BeagleLib instances
        finalizeBeagleLib(true);

        _ntaxa = _data->getNumTaxa();
        
        unsigned nsubsets = _data->getNumSubsets();
        std::set<instance_pair_t> nstates_ncateg_combinations;
        std::map<instance_pair_t, std::vector<unsigned> > subsets_for_pair;
        for (unsigned subset = 0; subset < nsubsets; subset++) {
            // Create a pair comprising number of states and number of rate categories
            unsigned nstates = _data->getNumStatesForSubset(subset);
            bool invar_model = _model->getSubsetIsInvarModel(subset);   ///!e
            int nrates = (invar_model ? -1 : 1)*_model->getSubsetNumCateg(subset);  ///!f
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
            std::cout << boost::str(boost::format("Created BeagleLib instance %d (%d states, %d rate%s, %d subset%s, %s invar. sites model)") % info.handle % info.nstates % info.nratecateg % (info.nratecateg == 1 ? "" : "s") % info.subsets.size() % (info.subsets.size() == 1 ? "" : "s") % (info.invarmodel ? "is" : "not")) << std::endl;    ///!g
        }
        
        if (_ambiguity_equals_missing)
            setTipStates();
        else
            setTipPartials();
        setPatternWeights();
        setPatternPartitionAssignments();
    }   ///end_initBeagleLib
    
    inline void Likelihood::newInstance(unsigned nstates, int nrates, std::vector<unsigned> & subset_indices) { ///begin_newInstance
        unsigned num_subsets = (unsigned)subset_indices.size();
        
        bool is_invar_model = (nrates < 0 ? true : false);                  ///!h
        unsigned ngammacat = (unsigned)(is_invar_model ? -nrates : nrates); ///!i
        
        unsigned num_patterns = 0;
        for (auto s : subset_indices) {
            num_patterns += _data->getNumPatternsInSubset(s);
        }
        
        unsigned num_internals = calcNumInternalsInFullyResolvedTree();

        // add 1 to num_edges so that subroot node will have a tmatrix, root tip's tmatrix is never used
        unsigned num_edges = 1 + calcNumEdgesInFullyResolvedTree();
        unsigned num_transition_probs = num_edges*num_subsets;
        
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
             num_subsets*num_transition_probs, // transition matrices (one for each edge in each subset)
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
        info.invarmodel     = is_invar_model;   ///!j
        info.subsets        = subset_indices;
        info.npatterns      = num_patterns;
        info.partial_offset = num_internals;
        info.tmatrix_offset = num_edges;
        _instances.push_back(info);
    } ///end_newInstance

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

    inline void Likelihood::setAmongSiteRateHeterogenetity() {  ///begin_setAmongSiteRateHeterogenetity
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
    }   ///end_setAmongSiteRateHeterogenetity

    inline void Likelihood::setModelRateMatrix() { ///begin_setModelRateMatrix
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
    } ///end_setModelRateMatrix
    
    inline unsigned Likelihood::getScalerIndex(Node * nd, InstanceInfo & info) const {
        //assert(nd->_parent && nd->_left_child); // nd is supposed to be an internal node and not the tip root node
        unsigned sindex = nd->_number - _ntaxa + 1; // +1 to skip the cumulative scaler vector
        if (nd->isAltPartial())
            sindex += info.partial_offset;
        return sindex;
    }
    
    inline unsigned Likelihood::getPartialIndex(Node * nd, InstanceInfo & info) const {
        // Note: do not be tempted to subtract _ntaxa from pindex: BeagleLib does this itself
        assert(nd->_number >= 0);
        unsigned pindex = nd->_number;
        if (pindex >= _ntaxa) {
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
    }
    
    inline void Likelihood::queuePartialsRecalculation(Node * nd, Node * lchild, Node * rchild) {
        // Loop through all instances
        for (auto & info : _instances) {
            // Loop through all subsets assigned to this instance
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                addOperation(info, nd, lchild, rchild, instance_specific_subset_index);
                ++instance_specific_subset_index;
        }
    }
    
    inline void Likelihood::queueTMatrixRecalculation(Node * nd) {  ///begin_queueTMatrixRecalculation
        Model::subset_relrate_vect_t & subset_relrates = _model->getSubsetRelRates();   ///!kk
        // Loop through all instances
        for (auto & info : _instances) {
            // Loop through all subsets assigned to this instance
            unsigned instance_specific_subset_index = 0;
            for (unsigned s : info.subsets) {
                double subset_relative_rate = subset_relrates[s]/_relrate_normalizing_constant; ///!kkk

                unsigned tindex = getTMatrixIndex(nd, info, instance_specific_subset_index);
                _pmatrix_index[info.handle].push_back(tindex);
                _edge_lengths[info.handle].push_back(nd->_edge_length*subset_relative_rate);
                _eigen_indices[info.handle].push_back(s);
                _category_rate_indices[info.handle].push_back(s);

                ++instance_specific_subset_index;
            }   // subsets loop
        } // instances loop
    }   ///end_queueTMatrixRecalculation
    
    inline void Likelihood::defineOperations(Tree::SharedPtr t) {///begin_defineOperations
        assert(_instances.size() > 0);
        assert(t);
        assert(t->isRooted() == _rooted);

        _relrate_normalizing_constant = _model->calcNormalizingConstantForSubsetRelRates(); ///!k
        //... ///l
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
    }
    
    inline void Likelihood::updateTransitionMatrices() {
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
                
        }   // instance loop
    }
    
    inline void Likelihood::calculatePartials() {
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
        }   // instance loop
    }
    
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
        
        // ... ///dots

        if (code != 0) {
            throw XStrom(boost::str(boost::format("failed to calculate edge log-likelihoods in calcInstanceLogLikelihood. BeagleLib error code was %d (%s)") % code % _beagle_error[code]));
        }
        
        if (info.invarmodel) {  ///!n
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
                            for (unsigned k = 0; k < info.nstates; ++k) {   ///begin_states_loop
                                Data::state_t x = (Data::state_t)1 << k;
                                double condlike = (x & monomorphic[p] ? 1.0 : 0.0);
                                double basefreq = freq[k];
                                invar_like += condlike*basefreq;
                            }   ///end_states_loop
                        }
                        double site_lnL = site_log_likelihoods[i++];
                        double log_like_term = log_one_minus_pinvar + site_lnL;
                        if (invar_like > 0.0) { 
                            double log_invar_term = log_pinvar + log(invar_like); ///begin_sitelnL
                            double site_log_like = (log_like_term + log(1.0 + exp(log_invar_term - log_like_term))); ///end_sitelnL
                            lnL += counts[p]*site_log_like;
                        }   
                        else {
                            lnL += counts[p]*log_like_term;
                        }
                    }
                }
            }
            log_likelihood = lnL;
        }   ///!o

        return log_likelihood;
    } ///end_calcInstanceLogLikelihood
    
    inline double Likelihood::calcLogLikelihood(Tree::SharedPtr t) {    ///begin_calcLogLikelihood
        assert(_instances.size() > 0);
        
        if (!_using_data)
            return 0.0;

        // Must call setData and setModel before calcLogLikelihood
        assert(_data);
        assert(_model); ///!p
        
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
    }   ///end_calcLogLikelihood

}
