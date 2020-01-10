#pragma once    ///start

#include "model.hpp"
#include "updater.hpp"
#include "qmatrix.hpp"

namespace strom {
    
    class OmegaUpdater : public Updater {
    
        public:
            typedef std::shared_ptr<OmegaUpdater> SharedPtr;

                                        OmegaUpdater(QMatrix::SharedPtr q);
                                        ~OmegaUpdater();
        
            virtual void                clear();
            double                      getCurrentPoint() const;

            // mandatory overrides of pure virtual functions
            virtual double              calcLogPrior();
            virtual void                revert();
            virtual void                proposeNewState();
        
        private:
        
            double                      _prev_point;
            QMatrix::SharedPtr          _q;
    };

    // member function bodies go here    
    inline OmegaUpdater::OmegaUpdater(QMatrix::SharedPtr q) {
        //std::cout << "OmegaUpdater being created" << std::endl;
        clear();
        _name = "Omega";
        assert(q);
        _q = q;
    }

    inline OmegaUpdater::~OmegaUpdater() { 
        //std::cout << "OmegaUpdater being destroyed" << std::endl;
        _q.reset();
    }

    inline void OmegaUpdater::clear() {
        Updater::clear();
        _prev_point = 0.0;
        _q = nullptr;
        reset();
    }

    inline double OmegaUpdater::getCurrentPoint() const {
        return *(_q->getOmegaSharedPtr());
    } 
    
    inline double OmegaUpdater::calcLogPrior() {
        // Assumes Gamma(a,b) prior with mean a*b and variance a*b^2
        assert(_prior_parameters.size() == 2);
        double prior_a = _prior_parameters[0];
        double prior_b = _prior_parameters[1];
        
        double log_prior = 0.0;
        double curr_point = getCurrentPoint();
        if (curr_point > 0.0) {
            log_prior += (prior_a - 1.0)*std::log(curr_point);
            log_prior -= curr_point/prior_b;
            log_prior -= prior_a*std::log(prior_b);
            log_prior -= std::lgamma(prior_a);
        }
        else
            log_prior = Updater::_log_zero;
        return log_prior;
    } 

    inline void OmegaUpdater::revert() {
        _q->setOmega(_prev_point);
    }

    inline void OmegaUpdater::proposeNewState() {
        // Save copy of _curr_point in case revert is necessary.
        _prev_point = getCurrentPoint();
        
        // Propose new value using multiplier with boldness _lambda
        double m = exp(_lambda*(_lot->uniform() - 0.5));
        _q->setOmega(m*_prev_point);
        
        // Calculate log of Hastings ratio
        _log_hastings_ratio = log(m);
    } 

}   ///end
