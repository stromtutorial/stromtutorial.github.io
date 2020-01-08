#pragma once    ///start

#include <vector>
#include <boost/math/distributions/gamma.hpp>

namespace strom {

    class ASRV {

        public:
            typedef std::vector<double>         rate_prob_t;
            typedef std::shared_ptr<double>     relrate_ptr_t;
            typedef std::shared_ptr<double>     ratevar_ptr_t;
            typedef std::shared_ptr<double>     pinvar_ptr_t;
            typedef std::shared_ptr<ASRV>       SharedPtr;
        
                                                ASRV();
            virtual                             ~ASRV();
        
            void                                clear();
        
            void                                setNumCateg(unsigned ncateg);
            unsigned                            getNumCateg() const;

            void                                setRateVarSharedPtr(ratevar_ptr_t ratevar);
            void                                setRateVar(double v);
            const ratevar_ptr_t                 getRateVarSharedPtr() const;
            double                              getRateVar() const;
            void                                fixRateVar(bool is_fixed);
            bool                                isFixedRateVar() const;

            void                                setPinvarSharedPtr(pinvar_ptr_t pinvar);
            void                                setPinvar(double p);
            const pinvar_ptr_t                  getPinvarSharedPtr() const;
            double                              getPinvar() const;
            void                                fixPinvar(bool is_fixed);
            bool                                isFixedPinvar() const;

            void                                setIsInvarModel(bool is_invar_model);
            bool                                getIsInvarModel() const;

            const double *                      getRates() const;
            const double *                      getProbs() const;

        private:
        
            virtual void                        recalcASRV();

            unsigned                            _num_categ;
            bool                                _invar_model;
        
            ratevar_ptr_t                       _ratevar;
            pinvar_ptr_t                        _pinvar;
        
            bool                                _ratevar_fixed;
            bool                                _pinvar_fixed;
        
            rate_prob_t                         _rates;
            rate_prob_t                         _probs;
    };
    
    // Member function bodies go here
    ///end_class_declaration
    inline ASRV::ASRV() {   ///begin_constructor
        //std::cout << "Constructing a ASRV" << std::endl;
        clear();
    }

    inline ASRV::~ASRV() {
        //std::cout << "Destroying a ASRV" << std::endl;
    }

    inline void ASRV::clear() {
        // Rate homogeneity is the default
        _invar_model = false;
        _ratevar_fixed = false;
        _pinvar_fixed = false;
        _ratevar = std::make_shared<double>(1.0);
        _pinvar = std::make_shared<double>(0.0);
        _num_categ = 1;
        recalcASRV();
    }   ///end_clear

    inline const ASRV::ratevar_ptr_t ASRV::getRateVarSharedPtr() const {   ///begin_getRateVarSharedPtr
        return _ratevar;
    }   ///end_getRateVarSharedPtr

    inline double ASRV::getRateVar() const {   ///begin_getRateVar
        assert(_ratevar);
        return *_ratevar;
    }   ///end_getRateVar

    inline const ASRV::pinvar_ptr_t ASRV::getPinvarSharedPtr() const {   ///begin_getPinvarSharedPtr
        return _pinvar;
    }   ///end_getPinvarSharedPtr

    inline double ASRV::getPinvar() const {   ///begin_getPinvar
        assert(_pinvar);
        return *_pinvar;
    }   ///end_getPinvar

    inline const double * ASRV::getRates() const {   ///begin_getRates
        return &_rates[0];
    }   ///end_getRates

    inline const double * ASRV::getProbs() const {   ///begin_getProbs
        return &_probs[0];
    }   ///end_getProbs

    inline bool ASRV::getIsInvarModel() const {   ///begin_getIsInvarModel
        return _invar_model;
    }   ///end_getIsInvarModel

    inline unsigned ASRV::getNumCateg() const {   ///begin_getNumCateg
        return _num_categ;
    }   ///end_getNumCateg
    
    inline void ASRV::setNumCateg(unsigned ncateg) {   ///begin_setNumCateg
        _num_categ = ncateg;
        recalcASRV();
    }   ///end_setNumCateg
    
    inline void ASRV::setRateVarSharedPtr(ratevar_ptr_t ratevar) {   ///begin_setRateVarSharedPtr
        _ratevar = ratevar;
        recalcASRV();
    }   ///end_setRateVarSharedPtr
    
    inline void ASRV::setRateVar(double v) {   ///begin_setRateVar
        *_ratevar = v;
        recalcASRV();
    }   ///end_setRateVar
    
    inline void ASRV::setPinvarSharedPtr(pinvar_ptr_t pinvar) {   ///begin_setPinvarSharedPtr
        _pinvar = pinvar;
        recalcASRV();
    }   ///end_setPinvarSharedPtr
    
    inline void ASRV::setPinvar(double p) {   ///begin_setPinvar
        *_pinvar = p;
        recalcASRV();
    }   ///end_setPinvar
    
    inline void ASRV::setIsInvarModel(bool is_invar_model) {   ///begin_setIsInvarModel
        _invar_model = is_invar_model;
        recalcASRV();
    }   ///end_setIsInvarModel

    inline void ASRV::fixRateVar(bool is_fixed) {  ///begin_fixRateVar
        _ratevar_fixed = is_fixed;
    }

    inline void ASRV::fixPinvar(bool is_fixed) {
        _pinvar_fixed = is_fixed;
    }

    inline bool ASRV::isFixedRateVar() const {
        return _ratevar_fixed;
    }

    inline bool ASRV::isFixedPinvar() const {
        return _pinvar_fixed;
    }   ///end_isFixedPinvar

    inline void ASRV::recalcASRV() {   ///begin_recalcASRV
        // This implementation assumes discrete gamma among-site rate heterogeneity
        // using a _num_categ category discrete gamma distribution with equal category
        // probabilities and Gamma density with mean 1.0 and variance _rate_var.
        // If _invar_model is true, then rate probs will sum to 1 - _pinvar rather than 1
        // and the mean rate will be 1/(1 - _pinvar) rather than 1; the rest of the invariable
        // sites component of the model is handled outside the ASRV class.
        
        // _num_categ, _rate_var, and _pinvar must all have been assigned in order to compute rates and probs
        if ( (!_ratevar) || (!_num_categ) || (!_pinvar) )
            return;
        
        double pinvar = *_pinvar;
        assert(pinvar >= 0.0);
        assert(pinvar <  1.0);

        assert(_num_categ > 0);

        double equal_prob = 1.0/_num_categ;
        double mean_rate_variable_sites = 1.0;
        if (_invar_model)
            mean_rate_variable_sites /= (1.0 - pinvar);
        
        _rates.assign(_num_categ, mean_rate_variable_sites);
        _probs.assign(_num_categ, equal_prob);

        double rate_variance = *_ratevar;
        assert(rate_variance >= 0.0);
        
        if (_num_categ == 1 || rate_variance == 0.0)
            return;

        double alpha = 1.0/rate_variance;
        double beta = rate_variance;
    
        boost::math::gamma_distribution<> my_gamma(alpha, beta);
        boost::math::gamma_distribution<> my_gamma_plus(alpha + 1.0, beta);

        double cum_upper        = 0.0;
        double cum_upper_plus   = 0.0;
        double upper            = 0.0;
        double cum_prob         = 0.0;
        for (unsigned i = 1; i <= _num_categ; ++i) {
            double cum_lower_plus       = cum_upper_plus;
            double cum_lower            = cum_upper;
            cum_prob                    += equal_prob;

            if (i < _num_categ) {
                upper                   = boost::math::quantile(my_gamma, cum_prob);
                cum_upper_plus          = boost::math::cdf(my_gamma_plus, upper);
                cum_upper               = boost::math::cdf(my_gamma, upper);
            }
            else {
                cum_upper_plus          = 1.0;
                cum_upper               = 1.0;
            }

            double numer                = cum_upper_plus - cum_lower_plus;
            double denom                = cum_upper - cum_lower;
            double r_mean               = (denom > 0.0 ? (alpha*beta*numer/denom) : 0.0);
            _rates[i-1]        = r_mean*mean_rate_variable_sites;
        }
    }   ///end_recalcASRV

}   ///end
