#pragma once    ///start

#include "model.hpp"
#include "updater.hpp"
#include "asrv.hpp"

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

    // member function bodies go here
    ///end_class_declaration    
    inline GammaRateVarUpdater::GammaRateVarUpdater(ASRV::SharedPtr asrv) { ///begin_constructor
        //std::cout << "GammaRateVarUpdater being created" << std::endl;
        clear();
        _name = "Gamma Rate Variance";
        assert(asrv);
        _asrv = asrv;
    } ///end_constructor

    inline GammaRateVarUpdater::~GammaRateVarUpdater() { ///begin_destructor
        //std::cout << "GammaRateVarUpdater being destroyed" << std::endl;
        _asrv.reset();
    } ///end_destructor

    inline void GammaRateVarUpdater::clear() { ///begin_clear
        Updater::clear();
        _prev_point = 0.0;
        _asrv = nullptr;
        reset();
    } ///end_clear

    inline double GammaRateVarUpdater::getCurrentPoint() const { ///begin_getCurrentPoint
        return *(_asrv->getRateVarSharedPtr());
    } ///end_getCurrentPoint
    
    inline double GammaRateVarUpdater::calcLogPrior() { ///begin_calcLogPrior
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
    } ///end_calcLogPrior

    inline void GammaRateVarUpdater::revert() { ///begin_revert
        _asrv->setRateVar(_prev_point);
    } ///end_revert

    inline void GammaRateVarUpdater::proposeNewState() { ///begin_proposeNewState
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
    } ///end_proposeNewState

}   ///end
