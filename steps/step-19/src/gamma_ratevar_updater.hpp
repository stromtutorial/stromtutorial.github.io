#pragma once

#include "model.hpp"
#include "updater.hpp"
#include "asrv.hpp"

#define POLNEW

#if defined(POLNEW)
#include <cmath>
#endif

namespace strom {
    
    class GammaRateVarUpdater : public Updater {
    
        public:
            typedef std::shared_ptr<GammaRateVarUpdater> SharedPtr;

                                        GammaRateVarUpdater(ASRV::SharedPtr asrv);
                                        ~GammaRateVarUpdater();
        
            virtual void                clear();
            double                      getCurrentPoint() const;

            // mandatory overrides of pure virtual functions
            virtual double              calcLogPrior();
            virtual void                revert();
            virtual void                proposeNewState();
        
        private:
        
            double                      _prev_point;
            ASRV::SharedPtr             _asrv;
    };

    inline GammaRateVarUpdater::GammaRateVarUpdater(ASRV::SharedPtr asrv) {
        //std::cout << "GammaRateVarUpdater being created" << std::endl;
        clear();
        _name = "Gamma Rate Variance";
        assert(asrv);
        _asrv = asrv;
    }

    inline GammaRateVarUpdater::~GammaRateVarUpdater() {
        //std::cout << "GammaRateVarUpdater being destroyed" << std::endl;
        _asrv.reset();
    }

    inline void GammaRateVarUpdater::clear() {
        Updater::clear();
        _prev_point = 0.0;
        _asrv = nullptr;
        reset();
    }

    inline double GammaRateVarUpdater::getCurrentPoint() const {
        return *(_asrv->getRateVarSharedPtr());
    }
    
    inline double GammaRateVarUpdater::calcLogPrior() {
#if 0 //defined(POLNEW)
        // Assumes Lognormal(mu,sigma) prior with
        // mean     = exp(mu + sigma^2/2)
        // variance = exp(2 mu + 2 sigma^2) - exp(2 mu + sigma^2)
        // density function f(x) = exp(-(log x - mu)^2/(2 sigma^2))/(x sigma sqrt(2 pi))
        assert(_prior_parameters.size() == 2);
        double prior_mu    = _prior_parameters[0];
        double prior_sigma = _prior_parameters[1];
        double prior_sigma_squared = std::pow(prior_sigma,2);
        
        double log_prior = 0.0;
        double curr_point = getCurrentPoint();
        if (curr_point > 0.0) {
            log_prior -= std::pow(std::log(curr_point) - prior_mu, 2)/(2.0*prior_sigma_squared);
            log_prior -= std::log(curr_point);
            log_prior -= std::log(prior_sigma);
            log_prior -= 0.5*std::log(2.0*M_PI);
        }
        else
            log_prior = Updater::_log_zero;
        return log_prior;
#else
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
#if 1
        else if (curr_point == 0.0) {
            if (prior_a == 1.0) {
                assert(prior_b > 0.0);
                return -std::log(prior_b);
            }
            else if (prior_a > 1.0) {
                log_prior = Updater::_log_zero;
            }
            else {
                // prior_a < 1.0
                log_prior = -Updater::_log_zero;
            }
        }
#endif
        else
            log_prior = Updater::_log_zero;
        return log_prior;
#endif
    }

    inline void GammaRateVarUpdater::revert() {
        _asrv->setRateVar(_prev_point);
    }

#if defined(POLNEW)
    inline void GammaRateVarUpdater::proposeNewState() {
        // Save copy of _curr_point in case revert is necessary.
        _prev_point = getCurrentPoint();

        // Propose new value using window of width _lambda centered over _prev_point
        double u = _lot->uniform();
        double new_point = (_prev_point - _lambda/2.0) + u*_lambda;
        assert(new_point != 0.0);
        if (new_point < 0.0)
            new_point *= -1.0;
        _asrv->setRateVar(new_point);
        
        // Calculate log of Hastings ratio
        _log_hastings_ratio = 0.0;  // symmetric proposal
        
        // This proposal invalidates all transition matrices and partials
        _tree_manipulator->selectAllPartials();
        _tree_manipulator->selectAllTMatrices();
    }
#else
    inline void GammaRateVarUpdater::proposeNewState() {
        // Save copy of _curr_point in case revert is necessary.
        _prev_point = getCurrentPoint();

        // Propose new value using multiplier with boldness _lambda
        double m = exp(_lambda*(_lot->uniform() - 0.5));
        _asrv->setRateVar(m*_prev_point);
        
        // Calculate log of Hastings ratio
        _log_hastings_ratio = log(m);
        
        // This proposal invalidates all transition matrices and partials
        _tree_manipulator->selectAllPartials();
        _tree_manipulator->selectAllTMatrices();
    }
#endif

}
