#pragma once

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

    inline ASRV::ASRV() {
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
    }

    inline const ASRV::ratevar_ptr_t ASRV::getRateVarSharedPtr() const {
        return _ratevar;
    }

    inline double ASRV::getRateVar() const {
        assert(_ratevar);
        return *_ratevar;
    }

    inline const ASRV::pinvar_ptr_t ASRV::getPinvarSharedPtr() const {
        return _pinvar;
    }

    inline double ASRV::getPinvar() const {
        assert(_pinvar);
        return *_pinvar;
    }

    inline const double * ASRV::getRates() const {
        return &_rates[0];
    }

    inline const double * ASRV::getProbs() const {
        return &_probs[0];
    }

    inline bool ASRV::getIsInvarModel() const {
        return _invar_model;
    }

    inline unsigned ASRV::getNumCateg() const {
        return _num_categ;
    }
    
    inline void ASRV::setNumCateg(unsigned ncateg) {
        _num_categ = ncateg;
        recalcASRV();
    }
    
    inline void ASRV::setRateVarSharedPtr(ratevar_ptr_t ratevar) {
        _ratevar = ratevar;
        recalcASRV();
    }
    
    inline void ASRV::setRateVar(double v) {
        *_ratevar = v;
        recalcASRV();
    }
    
    inline void ASRV::setPinvarSharedPtr(pinvar_ptr_t pinvar) {
        _pinvar = pinvar;
        recalcASRV();
    }
    
    inline void ASRV::setPinvar(double p) {
        *_pinvar = p;
        recalcASRV();
    }
    
    inline void ASRV::setIsInvarModel(bool is_invar_model) {
        _invar_model = is_invar_model;
        recalcASRV();
    }

    inline void ASRV::fixRateVar(bool is_fixed) {
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
    }
    
    inline void ASRV::recalcASRV() {
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
    }

}
