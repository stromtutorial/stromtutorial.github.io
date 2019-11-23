#pragma once    ///start

#include "model.hpp"
#include "updater.hpp"
#include "asrv.hpp"

namespace strom {
    
    class PinvarUpdater : public Updater {
    
        public:
            typedef std::shared_ptr<PinvarUpdater> SharedPtr;

                                        PinvarUpdater(ASRV::SharedPtr asrv);
                                        ~PinvarUpdater();
        
            virtual void                clear();
            double                      getCurrentPoint() const;

            // mandatory overrides of pure virtual functions
            virtual double              calcLogPrior(int & checklist);
            virtual void                revert();
            virtual void                proposeNewState();
        
        private:
        
            double                      _prev_point;
            ASRV::SharedPtr             _asrv;
    };

    // member function bodies go here
    ///end_class_declaration    
    inline PinvarUpdater::PinvarUpdater(ASRV::SharedPtr asrv) { ///begin_constructor
        //std::cout << "PinvarUpdater being created" << std::endl;
        clear();
        _name = "Proportion of Invariable Sites";
        assert(asrv);
        _asrv = asrv;
    } ///end_constructor

    inline PinvarUpdater::~PinvarUpdater() { ///begin_destructor
        //std::cout << "PinvarUpdater being destroyed" << std::endl;
        _asrv.reset();
    } ///end_destructor

    inline void PinvarUpdater::clear() { ///begin_clear
        Updater::clear();
        _prev_point = 0.0;
        _asrv = nullptr;
        reset();
    } ///end_clear

    inline double PinvarUpdater::getCurrentPoint() const { ///begin_getCurrentPoint
        return *(_asrv->getPinvarSharedPtr());
    } ///end_getCurrentPoint
    
    inline double PinvarUpdater::calcLogPrior(int & checklist) { ///begin_calcLogPrior
        if (checklist & Model::ProportionInvar)
            return 0.0;
        checklist |= Model::ProportionInvar;

        // Assumes Beta(a,b) prior with mean a/(a+b) and variance a*b/((a + b + 1)*(a + b)^2)
        assert(_prior_parameters.size() == 2);
        double prior_a = _prior_parameters[0];
        double prior_b = _prior_parameters[1];
        
        double log_prior = 0.0;
        double curr_point = getCurrentPoint();
        if (curr_point > 0.0 && curr_point < 1.0) {
            log_prior += (prior_a - 1.0)*std::log(curr_point);
            log_prior += (prior_b - 1.0)*std::log(1.0 - curr_point);
            log_prior += std::lgamma(prior_a + prior_b);
            log_prior -= std::lgamma(prior_a);
            log_prior -= std::lgamma(prior_b);
        }
        else
            log_prior = Updater::_log_zero;
        return log_prior;
    } ///end_calcLogPrior

    inline void PinvarUpdater::revert() { ///begin_revert
        _asrv->setPinvar(_prev_point);
    } ///end_revert

    inline void PinvarUpdater::proposeNewState() { ///begin_proposeNewState
        if (_lambda > 1.0)  ///!a
            _lambda = 1.0;  ///!b
        
        // Save copy of _curr_point in case revert is necessary.
        _prev_point = getCurrentPoint();
        
        // Propose new value using uniform window of width _lambda centered over _prev_point
        double p = (_prev_point - _lambda/2.0) + _lambda*_lot->uniform();
        if (p < 0.0)
            p = -p;
        else if (p > 1.0)
            p = 1.0 - (p - 1.0);
        _asrv->setPinvar(p);
        
        _log_hastings_ratio = 0.0;  //symmetric proposal
        
        // This proposal invalidates all transition matrices and partials
        _tree_manipulator->selectAllPartials();
        _tree_manipulator->selectAllTMatrices();
    } ///end_proposeNewState

}   ///end
