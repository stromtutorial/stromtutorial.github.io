#pragma once

#include <algorithm>
#include <vector>
#include "datatype.hpp"
#include "qmatrix.hpp"
#include "asrv.hpp"
#include "libhmsbeagle/beagle.h"
#include <boost/format.hpp> ///!x
#include <boost/math/distributions/gamma.hpp>
#include <Eigen/Dense>

namespace strom {
    
    class Likelihood;

    class Model {   ///begin_model
        
        friend class Likelihood;

        public:
            typedef std::vector<ASRV::SharedPtr>      asrv_vect_t;
            typedef std::vector<QMatrix::SharedPtr>   qmat_vect_t;
            typedef std::vector<unsigned>             subset_sizes_t;
            typedef std::vector<DataType>             subset_datatype_t;
            typedef std::vector<double>               subset_relrate_vect_t;
            typedef std::vector<QMatrix::SharedPtr>   state_freq_params_t;
            typedef std::vector<QMatrix::SharedPtr>   exchangeability_params_t;
            typedef std::vector<QMatrix::SharedPtr>   omega_params_t;
            typedef std::vector<ASRV::SharedPtr>      ratevar_params_t;
            typedef std::vector<ASRV::SharedPtr>      pinvar_params_t;
            typedef boost::shared_ptr<Model>          SharedPtr;
        
                                        Model();
                                        ~Model();

            void                        activate();
            void                        inactivate();
            
            std::string                 describeModel();

            void                        setSubsetDataTypes(const subset_datatype_t & datatype_vect);

            void                        setSubsetRateVar(ASRV::ratevar_ptr_t ratevar, unsigned subset, bool fixed);
            void                        setSubsetPinvar(ASRV::pinvar_ptr_t pinvar, unsigned subset, bool fixed);
            void                        setSubsetExchangeabilities(QMatrix::freq_xchg_ptr_t exchangeabilities, unsigned subset, bool fixed);
            void                        setSubsetStateFreqs(QMatrix::freq_xchg_ptr_t state_frequencies, unsigned subset, bool fixed);
            void                        setSubsetOmega(QMatrix::omega_ptr_t omega, unsigned subset, bool fixed);

            void                        setSubsetRelRates(subset_relrate_vect_t & relrates, bool fixed);
            subset_relrate_vect_t &     getSubsetRelRates();
            bool                        isFixedSubsetRelRates() const;
            double                      calcNormalizingConstantForSubsetRelRates() const;

            void                        setTreeIndex(unsigned i, bool fixed);
            unsigned                    getTreeIndex() const;
            bool                        isFixedTree() const;
            unsigned                    getNumSubsets() const;
            unsigned                    getNumSites() const;
            unsigned                    getSubsetNumSites(unsigned subset) const;
            const QMatrix &             getQMatrix(unsigned subset) const;
            const ASRV &                getASRV(unsigned subset) const;

            void                        setSubsetIsInvarModel(bool is_invar, unsigned subset);
            bool                        getSubsetIsInvarModel(unsigned subset) const;

            void                        setSubsetSizes(const subset_sizes_t nsites_vect);
            subset_sizes_t &            getSubsetSizes();

            void                        setSubsetNumPatterns(const subset_sizes_t npatterns_vect);
            unsigned                    getSubsetNumPatterns(unsigned subset) const;

            void                        setSubsetNumCateg(unsigned ncateg, unsigned subset);
            unsigned                    getSubsetNumCateg(unsigned subset) const;

            state_freq_params_t &       getStateFreqParams();
            exchangeability_params_t &  getExchangeabilityParams();
            omega_params_t &            getOmegaParams();
            ratevar_params_t &          getRateVarParams();
            pinvar_params_t &           getPinvarParams();
        
            int                         setBeagleEigenDecomposition(int beagle_instance, unsigned subset, unsigned instance_subset);
            int                         setBeagleStateFrequencies(int beagle_instance, unsigned subset, unsigned instance_subset);
            int                         setBeagleAmongSiteRateVariationRates(int beagle_instance, unsigned subset, unsigned instance_subset);
            int                         setBeagleAmongSiteRateVariationProbs(int beagle_instance, unsigned subset, unsigned instance_subset);

            std::string                 paramNamesAsString(std::string sep) const;      ///!a
            std::string                 paramValuesAsString(std::string sep) const;     ///!b

        private:
        
            void                        clear();
        
            unsigned                    _num_subsets;
            unsigned                    _num_sites;
            subset_sizes_t              _subset_sizes;
            subset_sizes_t              _subset_npatterns;
            subset_datatype_t           _subset_datatypes;
            qmat_vect_t                 _qmatrix;
            asrv_vect_t                 _asrv;
        
            bool                        _tree_index;
            bool                        _tree_fixed;

            bool                        _subset_relrates_fixed;
            subset_relrate_vect_t       _subset_relrates;
        
            state_freq_params_t         _state_freq_params;
            exchangeability_params_t    _exchangeability_params;
            omega_params_t              _omega_params;
            ratevar_params_t            _ratevar_params;
            pinvar_params_t             _pinvar_params;
        };  ///end_model
    
    // member function bodies go here
    
    inline Model::Model() {
        //std::cout << "Constructing a Model" << std::endl;
        clear();
    }

    inline Model::~Model() {
        //std::cout << "Destroying a Model" << std::endl;
    }

    inline void Model::clear() {
        _num_subsets = 0;
        _num_sites = 0;
        _tree_index = 0;
        _tree_fixed = false;
        _subset_relrates_fixed = false;
        _subset_relrates.clear();
        _subset_sizes.clear();
        _subset_npatterns.clear();
        _subset_datatypes.clear();
        _qmatrix.clear();
        _asrv.clear();
    }
    
    inline std::string Model::describeModel() {
        // Creates summary such as following and returns as a string:
        //
        // Partition information:
        //
        //          data subset           1           2           3
        //    -----------------------------------------------------
        //           num. sites          20          20          20
        //        num. patterns           7           5          17
        //          num. states           4           4           4
        //      rate categories           4           1           4
        //
        // Parameter linkage:
        //
        //          data subset           1           2           3
        //    -----------------------------------------------------
        //          state freqs           1           1           1
        //    exchangeabilities           1           1           2
        //        rate variance           1           2           3
        //               pinvar           1           2           -
        
        // Start with empty parameter vectors
        _state_freq_params.clear();
        _exchangeability_params.clear();
        _omega_params.clear();
        _ratevar_params.clear();
        _pinvar_params.clear();

        // Sets used to determine which parameters are linked across subsets
        std::set<double *> freqset;
        std::set<double *> xchgset;
        std::set<double *> omegaset;
        std::set<double *> ratevarset;
        std::set<double *> pinvarset;
        std::set<double *> relrateset;

        // Vectors of pointers to distinct parameters
        std::vector<double *> unique_freq;
        std::vector<double *> unique_xchg;
        std::vector<double *> unique_omega;
        std::vector<double *> unique_ratevar;
        std::vector<double *> unique_pinvar;
        std::vector<double *> unique_relrate;

        // Map for storing strings that will contain the information for each row
        std::map<std::string, std::string> ss = {
            {"subset",    ""},
            {"dashes",    ""},
            {"freqs",     ""},
            {"xchg",      ""},
            {"omega",     ""},
            {"ratevar",   ""},
            {"pinvar",    ""},
            {"ncateg",    ""},
            {"nsites",    ""},
            {"npatterns", ""},
            {"nstates",   ""}
        };
        
        // Ensure that the subset relative rates are fixed if there is only one
        // subset; otherwise the subset relative rates will be added to the list
        // of free parameters that are updated, which makes no sense in this case
        if (_num_subsets == 1)
            _subset_relrates_fixed = true;

        // Loop through subsets, building up rows as we go
        for (unsigned i = 0; i < _num_subsets; i++) {
            // Ensure that for subsets in which the number of rate categories is 1 that
            // the gamma rate variance is fixed; otherwise the gamma rate variance will
            // be added to the list of free parameters that are updated, which makes
            // no sense in this case
            if (_asrv[i]->getNumCateg() == 1) {
                _asrv[i]->fixRateVar(true);
            }
        
            unsigned index;
            ss["subset"] += boost::str(boost::format("%12d") % (i+1));
            ss["dashes"] += "------------";

            // Determine whether state freqs are unique for this subset
            QMatrix::freq_xchg_ptr_t pfreq = _qmatrix[i]->getStateFreqsSharedPtr();
            QMatrix::freq_xchg_t & freq = *pfreq;
            double * freq_addr = &freq[0];
            auto f = freqset.insert(freq_addr);
            if (f.second) {
                unique_freq.push_back(freq_addr);
                if (!_qmatrix[i]->isFixedStateFreqs())
                    _state_freq_params.push_back(_qmatrix[i]);
                index = (unsigned)unique_freq.size();
            }
            else {
                auto iter = std::find(unique_freq.begin(), unique_freq.end(), freq_addr);
                index = (unsigned)std::distance(unique_freq.begin(), iter) + 1;
            }
            ss["freqs"] += boost::str(boost::format("%12d") % index);

            // Determine whether exchangeabilities are unique for this subset
            if (_subset_datatypes[i].isNucleotide()) {
                QMatrix::freq_xchg_ptr_t pxchg = _qmatrix[i]->getExchangeabilitiesSharedPtr();
                QMatrix::freq_xchg_t & xchg = *pxchg;
                double * xchg_addr = &xchg[0];
                auto x = xchgset.insert(xchg_addr);
                if (x.second) {
                    unique_xchg.push_back(xchg_addr);
                    if (!_qmatrix[i]->isFixedExchangeabilities())
                        _exchangeability_params.push_back(_qmatrix[i]);
                    index = (unsigned)unique_xchg.size();
                }
                else {
                    auto iter = std::find(unique_xchg.begin(), unique_xchg.end(), xchg_addr);
                    index = (unsigned)std::distance(unique_xchg.begin(), iter) + 1;
                }
                ss["xchg"] += boost::str(boost::format("%12d") % index);
            }
            else {
                ss["xchg"] += boost::str(boost::format("%12s") % "-");
            }
            
            // Determine whether omega is unique for this subset
            if (_subset_datatypes[i].isCodon()) {
                QMatrix::omega_ptr_t pomega = _qmatrix[i]->getOmegaSharedPtr();
                QMatrix::omega_t omegavalue = *pomega;
                double * omega_addr = &omegavalue;
                auto o = omegaset.insert(omega_addr);
                if (o.second) {
                    unique_omega.push_back(omega_addr);
                    if (!_qmatrix[i]->isFixedOmega())
                        _omega_params.push_back(_qmatrix[i]);
                    index = (unsigned)unique_omega.size();
                }
                else {
                    auto iter = std::find(unique_omega.begin(), unique_omega.end(), omega_addr);
                    index = (unsigned)std::distance(unique_omega.begin(), iter) + 1;
                }
                ss["omega"] += boost::str(boost::format("%12d") % index);
            }
            else {
                ss["omega"] += boost::str(boost::format("%12s") % "-");
            }

            // Determine whether rate variance is unique for this subset
            ASRV::ratevar_ptr_t pratevar = _asrv[i]->getRateVarSharedPtr();
            double & ratevar = *pratevar;
            double * ratevar_addr = &ratevar;
            auto r = ratevarset.insert(ratevar_addr);
            if (r.second) {
                unique_ratevar.push_back(ratevar_addr);
                if (!_asrv[i]->isFixedRateVar())
                    _ratevar_params.push_back(_asrv[i]);
                index = (unsigned)unique_ratevar.size();
            }
            else {
                auto iter = std::find(unique_ratevar.begin(), unique_ratevar.end(), ratevar_addr);
                index = (unsigned)std::distance(unique_ratevar.begin(), iter) + 1;
            }
            ss["ratevar"] += boost::str(boost::format("%12d") % index);
            
            // Determine whether pinvar is unique for this subset
            if (_asrv[i]->getIsInvarModel()) {
                ASRV::pinvar_ptr_t ppinvar = _asrv[i]->getPinvarSharedPtr();
                double & pinvar = *ppinvar;
                double * pinvar_addr = &pinvar;
                auto r = pinvarset.insert(pinvar_addr);
                if (r.second) {
                    unique_pinvar.push_back(pinvar_addr);
                    if (!_asrv[i]->isFixedPinvar())
                        _pinvar_params.push_back(_asrv[i]);
                    index = (unsigned)unique_pinvar.size();
                }
                else {
                    auto iter = std::find(unique_pinvar.begin(), unique_pinvar.end(), pinvar_addr);
                    index = (unsigned)std::distance(unique_pinvar.begin(), iter) + 1;
                }
                ss["pinvar"] += boost::str(boost::format("%12d") % index);
            }
            else {
                ss["pinvar"] += boost::str(boost::format("%12s") % "-");
            }
            
            // Save number of rate categories for this subset
            ss["ncateg"] += boost::str(boost::format("%12d") % _asrv[i]->getNumCateg());

            // Save number of sites for this subset
            ss["nsites"] += boost::str(boost::format("%12d") % _subset_sizes[i]);

            // Save number of patterns for this subset
            ss["npatterns"] += boost::str(boost::format("%12d") % _subset_npatterns[i]);

            // Save number of states for this subset
            if (_subset_datatypes.size() == _num_subsets)
                ss["nstates"] += boost::str(boost::format("%12d") % _subset_datatypes[i].getNumStates());
            else
                ss["nstates"] += boost::str(boost::format("%12s") % "?");

        }
        std::string s = "Partition information:\n\n";
        
        s += boost::str(boost::format("%20s%s\n") % "data subset" % ss["subset"]);
        s += boost::str(boost::format("%20s%s\n") % "-----------------" % ss["dashes"]);
        s += boost::str(boost::format("%20s%s\n") % "num. sites" % ss["nsites"]);
        s += boost::str(boost::format("%20s%s\n") % "num. patterns" % ss["npatterns"]);
        s += boost::str(boost::format("%20s%s\n") % "num. states" % ss["nstates"]);
        s += boost::str(boost::format("%20s%s\n") % "rate categories" % ss["ncateg"]);

        s += "\nParameter linkage:\n\n";
        
        s += boost::str(boost::format("%20s%s\n") % "data subset" % ss["subset"]);
        s += boost::str(boost::format("%20s%s\n") % "-----------------" % ss["dashes"]);
        s += boost::str(boost::format("%20s%s\n") % "state freqs" % ss["freqs"]);
        s += boost::str(boost::format("%20s%s\n") % "exchangeabilities" % ss["xchg"]);
        s += boost::str(boost::format("%20s%s\n") % "omega" % ss["omega"]);
        s += boost::str(boost::format("%20s%s\n") % "rate variance" % ss["ratevar"]);
        s += boost::str(boost::format("%20s%s\n") % "pinvar" % ss["pinvar"]);
        
        s += "\nParameter values for each subset:\n";

        s += "\n  relative rate:\n";
        for (unsigned i = 0; i < _num_subsets; i++) {
            s += boost::str(boost::format("  %12d: %g\n") % (i+1) % _subset_relrates[i]);
        }
        
        s += "\n  state freqs:\n";
        for (unsigned i = 0; i < _num_subsets; i++) {
            QMatrix::freq_xchg_t & freqs = *(_qmatrix[i]->getStateFreqsSharedPtr());
            std::vector<std::string> freqs_as_strings(freqs.size());
            std::transform(freqs.begin(), freqs.end(), freqs_as_strings.begin(), [](double freq) {return boost::str(boost::format("%g") % freq);});
            std::string tmp = boost::algorithm::join(freqs_as_strings, ",");
            s += boost::str(boost::format("  %12d: (%s)\n") % (i+1) % tmp);
        }

        s += "\n  exchangeabilities:\n";
        for (unsigned i = 0; i < _num_subsets; i++) {
            if (_subset_datatypes[i].isNucleotide()) {
                QMatrix::freq_xchg_t & xchg = *(_qmatrix[i]->getExchangeabilitiesSharedPtr());
                std::vector<std::string> xchg_as_strings(xchg.size());
                std::transform(xchg.begin(), xchg.end(), xchg_as_strings.begin(), [](double x) {return boost::str(boost::format("%g") % x);});
                std::string tmp = boost::algorithm::join(xchg_as_strings, ",");
                s += boost::str(boost::format("  %12d: (%s)\n") % (i+1) % tmp);
            }
            else {
                s += boost::str(boost::format("  %12d: -\n") % (i+1));
            }
        }

        s += "\n  omega:\n";
        for (unsigned i = 0; i < _num_subsets; i++) {
            if (_subset_datatypes[i].isCodon()) {
                double omega = *(_qmatrix[i]->getOmegaSharedPtr());
                s += boost::str(boost::format("  %12d: %g\n") % (i+1) % omega);
            }
            else {
                s += boost::str(boost::format("  %12d: -\n") % (i+1));
            }
        }

        s += "\n  rate variance:\n";
        for (unsigned i = 0; i < _num_subsets; i++) {
            if (_asrv[i]->getNumCateg() > 1) {
                double ratevar = *(_asrv[i]->getRateVarSharedPtr());
                s += boost::str(boost::format("  %12d: %g\n") % (i+1) % ratevar);
            }
            else
                s += boost::str(boost::format("  %12d: -\n") % (i+1));
        }

        s += "\n  pinvar:\n";
        for (unsigned i = 0; i < _num_subsets; i++) {
            double pinvar = *(_asrv[i]->getPinvarSharedPtr());
            bool is_invar_model = _asrv[i]->getIsInvarModel();
            if (is_invar_model)
                s += boost::str(boost::format("  %12d: %g\n") % (i+1) % pinvar);
            else
                s += boost::str(boost::format("  %12d: -\n") % (i+1));
        }

        return s;
    }
    
    inline unsigned Model::getSubsetNumPatterns(unsigned subset) const {
        assert(subset < _num_subsets);
        return _subset_npatterns[subset];
    }        
    
    inline unsigned Model::getSubsetNumSites(unsigned subset) const {
        assert(subset < _num_subsets);
        return _subset_sizes[subset];
    }
    
    inline unsigned Model::getNumSites() const {
        return _num_sites;
    }
    
    inline unsigned Model::getNumSubsets() const {
        return _num_subsets;
    }
    
    inline unsigned Model::getSubsetNumCateg(unsigned subset) const {
        assert(subset < _num_subsets);
        assert(_asrv.size() == _num_subsets);
        assert(_asrv[subset]);
        return _asrv[subset]->getNumCateg();
    }
    
    inline bool Model::getSubsetIsInvarModel(unsigned subset) const {
        assert(subset < _num_subsets);
        assert(_asrv.size() == _num_subsets);
        assert(_asrv[subset]);
        return _asrv[subset]->getIsInvarModel();
    }
    
    inline const QMatrix & Model::getQMatrix(unsigned subset) const {
        assert(subset < _num_subsets);
        return *(_qmatrix[subset]);
    }
    
    inline const ASRV & Model::getASRV(unsigned subset) const {
        assert(subset < _num_subsets);
        return *(_asrv[subset]);
    }
    
    inline Model::state_freq_params_t & Model::getStateFreqParams() {
        return _state_freq_params;
    }
    
    inline Model::exchangeability_params_t & Model::getExchangeabilityParams() {
        return _exchangeability_params;
    }
    
    inline Model::omega_params_t & Model::getOmegaParams() {
        return _omega_params;
    }
    
    inline Model::ratevar_params_t & Model::getRateVarParams() {
        return _ratevar_params;
    }
    
    inline Model::pinvar_params_t & Model::getPinvarParams() {
        return _pinvar_params;
    }
    
    inline double Model::calcNormalizingConstantForSubsetRelRates() const {
        // normalize _relrates so that expected relative rate across subsets equals 1.0
        double normalizing_constant = 0.0;
        for (unsigned s = 0; s < _num_subsets; s++) {
            normalizing_constant += _subset_sizes[s]*_subset_relrates[s]/_num_sites;
        }
        return normalizing_constant;
    }

    inline Model::subset_sizes_t & Model::getSubsetSizes() {
        return _subset_sizes;
    }
    
    inline void Model::setSubsetSizes(const subset_sizes_t nsites_vect) {
        assert(nsites_vect.size() == _num_subsets);
        _subset_sizes.resize(_num_subsets);
        std::copy(nsites_vect.begin(), nsites_vect.end(), _subset_sizes.begin());
        _num_sites = std::accumulate(_subset_sizes.begin(), _subset_sizes.end(), 0);
    }

    inline void Model::setSubsetNumPatterns(const subset_sizes_t npatterns_vect) {
        assert(npatterns_vect.size() == _num_subsets);
        _subset_npatterns.resize(_num_subsets);
        std::copy(npatterns_vect.begin(), npatterns_vect.end(), _subset_npatterns.begin());
    }
    
    inline void Model::setSubsetDataTypes(const subset_datatype_t & datatype_vect) {
        _num_subsets = (unsigned)datatype_vect.size();

        _qmatrix.clear();
        _qmatrix.resize(_num_subsets);

        _asrv.clear();
        _asrv.resize(_num_subsets);

        _subset_datatypes.resize(_num_subsets);
        std::copy(datatype_vect.begin(), datatype_vect.end(), _subset_datatypes.begin());
        
		_subset_relrates.assign(_num_subsets, 1.0);
        
        for (unsigned s = 0; s < _num_subsets; s++) {
            _asrv[s].reset(new ASRV());
            if (_subset_datatypes[s].isNucleotide())
                _qmatrix[s].reset(new QMatrixNucleotide());
            else if (_subset_datatypes[s].isCodon()) {
                GeneticCode::SharedPtr gcptr = _subset_datatypes[s].getGeneticCode();
                _qmatrix[s].reset(new QMatrixCodon(gcptr));
                }
            else
                throw XStrom(boost::format("Only nucleotide or codon data allowed in this version, you specified data type \"%s\" for subset %d") % _subset_datatypes[s].getDataTypeAsString() % (s+1));
        }
    }

    inline void Model::setSubsetNumCateg(unsigned ncateg, unsigned subset) {
        assert(subset < _num_subsets);
        if (ncateg < 1) {
            throw XStrom(boost::str(boost::format("number of categories used for among-site rate variation must be greater than zero but the value %d was supplied") % ncateg));
        }
        _asrv[subset]->setNumCateg(ncateg);
    }
    
    inline void Model::setSubsetRateVar(ASRV::ratevar_ptr_t ratevar, unsigned subset, bool fixed) {
        assert(subset < _num_subsets);
        assert(ratevar);
        if (*ratevar < 0.0)
            throw XStrom(boost::str(boost::format("rate variance must be greater than or equal to zero but the value %.5f was supplied") % *ratevar));
        _asrv[subset]->setRateVarSharedPtr(ratevar);
        _asrv[subset]->fixRateVar(fixed);
    }
    
    inline void Model::setSubsetPinvar(ASRV::pinvar_ptr_t pinvar, unsigned subset, bool fixed) {
        assert(subset < _num_subsets);
        assert(pinvar);
        if (*pinvar < 0.0)
            throw XStrom(boost::str(boost::format("proportion of invariable sites must be greater than or equal to zero but the value %.5f was supplied") % *pinvar));
        if (*pinvar >= 1.0)
            throw XStrom(boost::str(boost::format("proportion of invariable sites must be less than one but the value %.5f was supplied") % *pinvar));
        _asrv[subset]->setPinvarSharedPtr(pinvar);
        _asrv[subset]->fixPinvar(fixed);
    }
    
    inline void Model::setSubsetIsInvarModel(bool is_invar, unsigned subset) {
        assert(subset < _num_subsets);
        _asrv[subset]->setIsInvarModel(is_invar);
    }
    
    inline void Model::setSubsetExchangeabilities(QMatrix::freq_xchg_ptr_t exchangeabilities, unsigned subset, bool fixed) {
        assert(subset < _num_subsets);
        if (!_subset_datatypes[subset].isCodon()) {
            double first_xchg = (*exchangeabilities)[0];
            if (first_xchg == -1)
                _qmatrix[subset]->setEqualExchangeabilities(exchangeabilities);
            else
                _qmatrix[subset]->setExchangeabilitiesSharedPtr(exchangeabilities);
            _qmatrix[subset]->fixExchangeabilities(fixed);
        }
    }
    
    inline void Model::setSubsetStateFreqs(QMatrix::freq_xchg_ptr_t state_frequencies, unsigned subset, bool fixed) {
        assert(subset < _num_subsets);
        double first_freq = (*state_frequencies)[0];
        if (first_freq == -1)
            _qmatrix[subset]->setEqualStateFreqs(state_frequencies);
        else
            _qmatrix[subset]->setStateFreqsSharedPtr(state_frequencies);
        _qmatrix[subset]->fixStateFreqs(fixed);
    }
    
    inline void Model::setSubsetOmega(QMatrix::omega_ptr_t omega, unsigned subset, bool fixed) {
        assert(subset < _num_subsets);
        assert(*omega > 0.0);
        if (_subset_datatypes[subset].isCodon()) {
            _qmatrix[subset]->setOmegaSharedPtr(omega);
            _qmatrix[subset]->fixOmega(fixed);
        }
    }
    
    inline void Model::setSubsetRelRates(subset_relrate_vect_t & relrates, bool fixed) {   ///begin_setSubsetRelRates
        assert(_num_subsets > 0);
        assert(relrates.size() > 0);
        if (relrates[0] == -1)
            _subset_relrates.assign(_num_subsets, 1.0);
        else
            _subset_relrates.assign(relrates.begin(), relrates.end());
        _subset_relrates_fixed = fixed;
    }   ///end_setSubsetRelRates

    inline Model::subset_relrate_vect_t & Model::getSubsetRelRates() {   ///begin_getSubsetRelRates
        return _subset_relrates;
    }   ///end_getSubsetRelRates
    
    inline bool Model::isFixedSubsetRelRates() const {  ///begin_isFixedSubsetRelRates
        return _subset_relrates_fixed;
    }   ///end_isFixedSubsetRelRates
    
    inline void Model::activate() {
        for (auto q : _qmatrix)
            q->setActive(true);
    }

    inline void Model::inactivate() {
        for (auto q : _qmatrix)
            q->setActive(false);
    }

    inline int Model::setBeagleEigenDecomposition(int beagle_instance, unsigned subset, unsigned instance_subset) {
        assert(subset < _qmatrix.size());
        const double * pevec = _qmatrix[subset]->getEigenvectors();
        const double * pivec = _qmatrix[subset]->getInverseEigenvectors();
        const double * pival = _qmatrix[subset]->getEigenvalues();
        int code = beagleSetEigenDecomposition(
            beagle_instance,    // Instance number (input)
            instance_subset,    // Index of eigen-decomposition buffer (input)
            pevec,              // Flattened matrix (stateCount x stateCount) of eigen-vectors (input)
            pivec,              // Flattened matrix (stateCount x stateCount) of inverse-eigen- vectors (input)
            pival);             // Vector of eigenvalues

        return code;
    }
    
    inline int Model::setBeagleStateFrequencies(int beagle_instance, unsigned subset, unsigned instance_subset) {
        assert(subset < _qmatrix.size());
        const double * pfreq = _qmatrix[subset]->getStateFreqs();
        int code = beagleSetStateFrequencies(
             beagle_instance,   // Instance number (input)
             instance_subset,   // Index of state frequencies buffer (input)
             pfreq);            // State frequencies array (stateCount) (input)

        return code;
    }
    
    inline int Model::setBeagleAmongSiteRateVariationRates(int beagle_instance, unsigned subset, unsigned instance_subset) {
        assert(subset < _asrv.size());
        const double * prates = _asrv[subset]->getRates();
        int code = beagleSetCategoryRatesWithIndex(
            beagle_instance,    // Instance number (input)
            instance_subset,    // Index of category rates buffer (input)
            prates);            // Array containing categoryCount rate scalers (input)

        return code;
    }
    
    inline int Model::setBeagleAmongSiteRateVariationProbs(int beagle_instance, unsigned subset, unsigned instance_subset) {
        assert(subset < _asrv.size());
        const double * pprobs = _asrv[subset]->getProbs();
        int code = beagleSetCategoryWeights(
            beagle_instance,    // Instance number (input)
            instance_subset,    // Index of category weights buffer (input)
            pprobs);            // Category weights array (categoryCount) (input)

        return code;
    }
        
    inline std::string Model::paramNamesAsString(std::string sep) const {   ///begin_paramNamesAsString
        unsigned k;
        std::string s = "";
        if (_num_subsets > 1) {
            for (k = 0; k < _num_subsets; k++) {
                s += boost::str(boost::format("m-%d%s") % k % sep);
            }
        }
        for (k = 0; k < _num_subsets; k++) {
            if (_subset_datatypes[k].isNucleotide()) {
                s += boost::str(boost::format("rAC-%d%srAG-%d%srAT-%d%srCG-%d%srCT-%d%srGT-%d%s") % k % sep % k % sep % k % sep % k % sep % k % sep % k % sep);
                s += boost::str(boost::format("piA-%d%spiC-%d%spiG-%d%spiT-%d%s") % k % sep % k % sep % k % sep % k % sep);
            }
            else if (_subset_datatypes[k].isCodon()) {
                s += boost::str(boost::format("omega-%d%s") % k % sep);
                for (std::string codon : _subset_datatypes[0].getGeneticCode()->_codons)
                    s += boost::str(boost::format("pi%s-%d%s") % codon % k % sep);
            }
            if (_asrv[k]->getIsInvarModel()) {
                s += boost::str(boost::format("pinvar-%d%s") % k % sep);
            }
            if (_asrv[k]->getNumCateg() > 1) {
                s += boost::str(boost::format("ratevar-%d%s") % k % sep);
            }
        }
        return s;
    }   ///end_paramNamesAsString

    inline std::string Model::paramValuesAsString(std::string sep) const {  ///begin_paramValuesAsString
        unsigned k;
        std::string s = "";
        if (_num_subsets > 1) {
            for (k = 0; k < _num_subsets; k++) {
                s += boost::str(boost::format("%.5f%s") % _subset_relrates[k] % sep);
            }
        }
        for (k = 0; k < _num_subsets; k++) {
            if (_subset_datatypes[k].isNucleotide()) {
                QMatrix::freq_xchg_t x = *_qmatrix[k]->getExchangeabilitiesSharedPtr();
                s += boost::str(boost::format("%.5f%s%.5f%s%.5f%s%.5f%s%.5f%s%.5f%s") % x[0] % sep % x[1] % sep % x[2] % sep % x[3] % sep % x[4] % sep % x[5] % sep);
                QMatrix::freq_xchg_t f = *_qmatrix[k]->getStateFreqsSharedPtr();
                s += boost::str(boost::format("%.5f%s%.5f%s%.5f%s%.5f%s") % f[0] % sep % f[1] % sep % f[2] % sep % f[3] % sep);
            }
            else if (_subset_datatypes[k].isCodon()) {
                s += boost::str(boost::format("%.5f%s") % _qmatrix[k]->getOmega() % sep);
                QMatrix::freq_xchg_t f = *_qmatrix[k]->getStateFreqsSharedPtr();
                for (unsigned m = 0; m < _subset_datatypes[0].getNumStates(); m++)
                    s += boost::str(boost::format("%.5f%s") % f[m] % sep);
            }
            if (_asrv[k]->getIsInvarModel()) {
                s += boost::str(boost::format("%.5f%s") % _asrv[k]->getPinvar() % sep);
            }
            if (_asrv[k]->getNumCateg() > 1) {
                s += boost::str(boost::format("%.5f%s") % _asrv[k]->getRateVar() % sep);
            }
        }
        return s;
    }   ///end_paramValuesAsString
    
    inline void Model::setTreeIndex(unsigned i, bool fixed) {   ///begin_setTreeIndex
        _tree_index = i;
        _tree_fixed = fixed;
    }   ///end_setTreeIndex

    inline unsigned Model::getTreeIndex() const {   ///begin_getTreeIndex
        return _tree_index;
    }   ///end_getTreeIndex
    
    inline bool Model::isFixedTree() const {
        return _tree_fixed;
    }

}
