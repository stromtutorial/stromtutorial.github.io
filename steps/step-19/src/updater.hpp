#pragma once    ///start

#include "tree.hpp"
#include "tree_manip.hpp"
#include "lot.hpp"
#include "xstrom.hpp"
#include "likelihood.hpp"
#include "topo_prior_calculator.hpp"

#include "debug_stuff.hpp"  //DEBUGSTUFF

namespace strom {
    class Chain;

    class Updater {
    
        friend class Chain;

        public:
            typedef std::shared_ptr<Updater>        SharedPtr;
        
            TreeManip::SharedPtr                    getTreeManip() const;

                                                    Updater();
            virtual                                 ~Updater();

            void                                    setLikelihood(Likelihood::SharedPtr likelihood);
            void                                    setTreeManip(TreeManip::SharedPtr treemanip);
            void                                    setLot(Lot::SharedPtr lot);
            void                                    setLambda(double lambda);
            void                                    setHeatingPower(double p);
            void                                    setTuning(bool on);
            void                                    setTargetAcceptanceRate(double target);
            void                                    setPriorParameters(const std::vector<double> & c);
            void                                    setTopoPriorC(double c);
            void                                    setWeight(double w);
            void                                    calcProb(double wsum);

            double                                  getLambda() const;
            double                                  getWeight() const;
            double                                  getProb() const;
            double                                  getAcceptPct() const;
            double                                  getNumUpdates() const;
            std::string                             getUpdaterName() const;

            virtual void                            clear();

            virtual double                          calcLogPrior(int & checklist) = 0;
            double                                  calcLogTopologyPrior() const;
            double                                  calcLogEdgeLengthPrior() const;
            double                                  calcLogLikelihood() const;
            virtual double                          update(double prev_lnL);
        
            static double                           getLogZero();

        protected:

            virtual void                            reset();
            virtual void                            tune(bool accepted);

            virtual void                            revert() = 0;
            virtual void                            proposeNewState() = 0;

            Lot::SharedPtr                          _lot;
            Likelihood::SharedPtr                   _likelihood;
            TreeManip::SharedPtr                    _tree_manipulator;
            std::string                             _name;
            double                                  _weight;
            double                                  _prob;
            double                                  _lambda;
            double                                  _log_hastings_ratio;
            double                                  _target_acceptance;
            unsigned                                _naccepts;
            unsigned                                _nattempts;
            bool                                    _tuning;
            std::vector<double>                     _prior_parameters;
            double                                  _topo_prior_C;
            
            double                                  _heating_power;

            mutable PolytomyTopoPriorCalculator     _topo_prior_calculator;
            
            static const double                     _log_zero;
    };
 
    // member function bodies go here
    ///end_class_declaration
    inline Updater::Updater() { ///begin_constructor
        //std::cout << "Updater constructor called" << std::endl;
        clear();
    } ///end_constructor

    inline Updater::~Updater() { ///begin_destructor
        //std::cout << "Updater destructor called" << std::endl;
    } ///end_destructor

    inline void Updater::clear() { ///begin_clear
        _name                   = "updater";
        _tuning                 = true;
        _lambda                 = 0.0001;
        _weight                 = 1.0;
        _prob                   = 0.0;
        _target_acceptance      = 0.3;
        _naccepts               = 0;
        _nattempts              = 0;
        _heating_power          = 1.0;
        _topo_prior_C           = 1.0;
        _prior_parameters.clear();
        reset();
    } ///end_clear

    inline void Updater::reset() { ///begin_reset
        _log_hastings_ratio = 0.0;
    } ///end_reset

    inline void Updater::setLikelihood(Likelihood::SharedPtr likelihood) { ///begin_setLikelihood
        _likelihood = likelihood;
    } ///end_setLikelihood
    
    inline void Updater::setTreeManip(TreeManip::SharedPtr treemanip) { ///begin_setTreeManip
        _tree_manipulator = treemanip;
    } ///end_setTreeManip

    inline TreeManip::SharedPtr Updater::getTreeManip() const { ///begin_getTreeManip
        return _tree_manipulator;
    } ///end_getTreeManip

    inline void Updater::setLot(Lot::SharedPtr lot) { ///begin_setLot
        _lot = lot;
    } ///end_setLot
    
    inline void Updater::setHeatingPower(double p) { ///begin_setHeatingPower
        _heating_power = p;
    } ///end_setHeatingPower

    inline void Updater::setTopoPriorC(double c) { ///begin_setTopoPriorC
        _topo_prior_C = c;
    } ///end_setTopoPriorC

    inline void Updater::setLambda(double lambda) { ///begin_setLambda
        _lambda = lambda;
    } ///end_setLambda

    void Updater::setTuning(bool do_tune) { ///begin_setTuning
        _tuning = do_tune;
        _naccepts = 0;
        _nattempts = 0;
    } ///end_setTuning

    inline void Updater::tune(bool accepted) { ///begin_tune
        _nattempts++;
        if (_tuning) {
            double gamma_n = 10.0/(100.0 + (double)_nattempts);
            if (accepted)
                _lambda *= 1.0 + gamma_n*(1.0 - _target_acceptance)/(2.0*_target_acceptance);
            else
                _lambda *= 1.0 - gamma_n*0.5;

            // Prevent run-away increases in boldness for low-information marginal densities
            if (_lambda > 1000.0)
                _lambda = 1000.0;
        }
    } ///end_tune

    inline void Updater::setTargetAcceptanceRate(double target) { ///begin_setTargetAcceptanceRate
        _target_acceptance = target;
    } ///end_setTargetAcceptanceRate

    inline void Updater::setPriorParameters(const std::vector<double> & c) { ///begin_setPriorParameters
        _prior_parameters.clear();
        _prior_parameters.assign(c.begin(), c.end());
    } ///end_setPriorParameters
    
    inline void Updater::setWeight(double w) { ///begin_setWeight
        _weight = w;
    } ///end_setWeight
    
    inline void Updater::calcProb(double wsum) { ///begin_calcProb
        assert(wsum > 0.0);
        _prob = _weight/wsum;
    } ///end_calcProb

    inline double Updater::getLambda() const { ///begin_getLambda
        return _lambda;
    } ///end_getLambda
    
    inline double Updater::getWeight() const { ///begin_getWeight
        return _weight;
    } ///end_getWeight
    
    inline double Updater::getProb() const { ///begin_getProb
        return _prob;
    } ///end_getProb
    
    inline double Updater::getAcceptPct() const { ///begin_getAcceptPct
        return (_nattempts == 0 ? 0.0 : (100.0*_naccepts/_nattempts));
    } ///end_getAcceptPct

    inline double Updater::getNumUpdates() const { ///begin_getNumUpdates
        return _nattempts;
    } ///end_getNumUpdates

    inline std::string Updater::getUpdaterName() const { ///begin_getUpdaterName
        return _name;
    } ///end_getUpdaterName

    inline double Updater::calcLogLikelihood() const { ///begin_calcLogLikelihood
        return _likelihood->calcLogLikelihood(_tree_manipulator->getTree());
    } ///end_calcLogLikelihood

    inline double Updater::update(double prev_lnL) { ///begin_update
        int checklist = 0;
        double prev_log_prior = calcLogPrior(checklist);    // passing 0 for checklist ensures that all relevant priors will be calculated

        // Clear any nodes previously selected so that we can detect those nodes
        // whose partials and/or transition probabilities need to be recalculated
        _tree_manipulator->deselectAllPartials();
        _tree_manipulator->deselectAllTMatrices();

        // Set model to proposed state and calculate _log_hastings_ratio
        proposeNewState();
        
        // Use alternative partials and transition probability buffer for any selected nodes
        // This allows us to easily revert to the previous values if the move is rejected
        _tree_manipulator->flipPartialsAndTMatrices();

        DebugStuff::_tree_index++;
        DebugStuff::debugSaveTree(boost::str(boost::format("iter-%d") % DebugStuff::_which_iter), DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
        //std::cerr << "DebugStuff::debugSaveTree: " << boost::str(boost::format("iter-%d") % DebugStuff::_which_iter) << std::endl; //POLTMP

        // Calculate the log-likelihood and log-prior for the proposed state
        double log_likelihood = calcLogLikelihood();

        checklist = 0;
        double log_prior = calcLogPrior(checklist);
        
        // Decide whether to accept or reject the proposed state
        bool accept = true;
        if (log_prior > _log_zero) {
            double log_diff = _log_hastings_ratio;
            log_diff += _heating_power*((log_likelihood + log_prior) - (prev_lnL + prev_log_prior));

            double logu = _lot->logUniform();
            if (logu > log_diff)
                accept = false;
        }
        else
            accept = false;

        if (accept) {
            _naccepts++;
        }
        else {
            revert();
            _tree_manipulator->flipPartialsAndTMatrices();
            log_likelihood = prev_lnL;

            DebugStuff::_tree_index++;
            DebugStuff::debugSaveTree(boost::str(boost::format("revert-%d") % DebugStuff::_which_iter), DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
            //std::cerr << "DebugStuff::debugSaveTree: " << boost::str(boost::format("revert-%d") % DebugStuff::_which_iter) << std::endl; //POLTMP
        }

        tune(accept);
        reset();

        return log_likelihood;
    } ///end_update
    
    inline double Updater::calcLogTopologyPrior() const {   ///begin_calcLogTopologyPrior
        Tree::SharedPtr tree = _tree_manipulator->getTree();
        assert(tree);
        _topo_prior_calculator.chooseResolutionClassPrior();
        if (tree->isRooted())
            _topo_prior_calculator.chooseRooted();
        else
            _topo_prior_calculator.chooseUnrooted();
        _topo_prior_calculator.setNTax(tree->numLeaves());
        _topo_prior_calculator.setC(_topo_prior_C);
        unsigned m = tree->numInternals();
        double log_prior = _topo_prior_calculator.getLogNormalizedTopologyPrior(m);

        //double logc1 = _topo_prior_calculator.getLogCount(10,1) + _topo_prior_calculator.getLogNormalizedTopologyPrior(1);
        //double logc2 = _topo_prior_calculator.getLogCount(10,2) + _topo_prior_calculator.getLogNormalizedTopologyPrior(2);
        //double logc3 = _topo_prior_calculator.getLogCount(10,3) + _topo_prior_calculator.getLogNormalizedTopologyPrior(3);
        //double logc4 = _topo_prior_calculator.getLogCount(10,4) + _topo_prior_calculator.getLogNormalizedTopologyPrior(4);
        //double logc5 = _topo_prior_calculator.getLogCount(10,5) + _topo_prior_calculator.getLogNormalizedTopologyPrior(5);
        //double logc6 = _topo_prior_calculator.getLogCount(10,6) + _topo_prior_calculator.getLogNormalizedTopologyPrior(6);
        //double logc7 = _topo_prior_calculator.getLogCount(10,7) + _topo_prior_calculator.getLogNormalizedTopologyPrior(7);
        //double logc8 = _topo_prior_calculator.getLogCount(10,8) + _topo_prior_calculator.getLogNormalizedTopologyPrior(8);
        //std::cerr << boost::str(boost::format("m=1 log count=%.5f") % exp(logc1)) << std::endl;
        //std::cerr << boost::str(boost::format("m=2 prob=%.5f") % exp(logc2)) << std::endl;
        //std::cerr << boost::str(boost::format("m=3 prob=%.5f") % exp(logc3)) << std::endl;
        //std::cerr << boost::str(boost::format("m=4 prob=%.5f") % exp(logc4)) << std::endl;
        //std::cerr << boost::str(boost::format("m=5 prob=%.5f") % exp(logc5)) << std::endl;
        //std::cerr << boost::str(boost::format("m=6 prob=%.5f") % exp(logc6)) << std::endl;
        //std::cerr << boost::str(boost::format("m=7 prob=%.5f") % exp(logc7)) << std::endl;
        //std::cerr << boost::str(boost::format("m=8 prob=%.5f") % exp(logc8)) << std::endl;
        //std::cerr << std::endl;
        //std::cerr << boost::str(boost::format("m=1 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,1))) << std::endl;
        //std::cerr << boost::str(boost::format("m=2 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,2))) << std::endl;
        //std::cerr << boost::str(boost::format("m=3 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,3))) << std::endl;
        //std::cerr << boost::str(boost::format("m=4 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,4))) << std::endl;
        //std::cerr << boost::str(boost::format("m=5 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,5))) << std::endl;
        //std::cerr << boost::str(boost::format("m=6 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,6))) << std::endl;
        //std::cerr << boost::str(boost::format("m=7 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,7))) << std::endl;
        //std::cerr << boost::str(boost::format("m=7 count=%.0f") % exp(_topo_prior_calculator.getLogCount(10,8))) << std::endl;
        //float total_count = exp(_topo_prior_calculator.getLogCount(10,1));
        //total_count += exp(_topo_prior_calculator.getLogCount(10,2));
        //total_count += exp(_topo_prior_calculator.getLogCount(10,3));
        //total_count += exp(_topo_prior_calculator.getLogCount(10,4));
        //total_count += exp(_topo_prior_calculator.getLogCount(10,5));
        //total_count += exp(_topo_prior_calculator.getLogCount(10,6));
        //total_count += exp(_topo_prior_calculator.getLogCount(10,7));
        //total_count += exp(_topo_prior_calculator.getLogCount(10,8));
        //std::cerr << boost::str(boost::format("total count=%.0f") % total_count) << std::endl;
        //std::cerr << std::endl;
        //std::cerr << boost::str(boost::format("m=0 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(0))) << std::endl;
        //std::cerr << boost::str(boost::format("m=1 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(1))) << std::endl;
        //std::cerr << boost::str(boost::format("m=2 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(2))) << std::endl;
        //std::cerr << boost::str(boost::format("m=3 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(3))) << std::endl;
        //std::cerr << boost::str(boost::format("m=4 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(4))) << std::endl;
        //std::cerr << boost::str(boost::format("m=5 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(5))) << std::endl;
        //std::cerr << boost::str(boost::format("m=6 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(6))) << std::endl;
        //std::cerr << boost::str(boost::format("m=7 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(7))) << std::endl;
        //std::cerr << boost::str(boost::format("m=8 prob=%.12f") % exp(_topo_prior_calculator.getLogNormalizedTopologyPrior(8))) << std::endl;
        //std::cerr << std::endl;
        //std::cerr << boost::str(boost::format("m=0 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(0))) << std::endl;
        //std::cerr << boost::str(boost::format("m=1 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(1))) << std::endl;
        //std::cerr << boost::str(boost::format("m=2 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(2))) << std::endl;
        //std::cerr << boost::str(boost::format("m=3 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(3))) << std::endl;
        //std::cerr << boost::str(boost::format("m=4 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(4))) << std::endl;
        //std::cerr << boost::str(boost::format("m=5 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(5))) << std::endl;
        //std::cerr << boost::str(boost::format("m=6 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(6))) << std::endl;
        //std::cerr << boost::str(boost::format("m=7 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(7))) << std::endl;
        //std::cerr << boost::str(boost::format("m=8 prob=%.12f") % exp(_topo_prior_calculator.getLogTopologyPrior(8))) << std::endl;

        return log_prior;
        
        //double n = tree->numLeaves();
        //if (tree->isRooted())
        //    n += 1.0;
        //double log_num_topologies = std::lgamma(2.0*n - 5.0 + 1.0) - (n - 3.0)*std::log(2.0) - std::lgamma(n - 3.0 + 1.0);
        //return -log_num_topologies;
    }   ///end_calcLogTopologyPrior

    inline double Updater::calcLogEdgeLengthPrior() const { ///begin_calcLogEdgeLengthPrior
        double log_prior = 0.0;
        Tree::SharedPtr tree = _tree_manipulator->getTree();
#if 1 //POLTMP
        // standard exponential edge length prior (for debugging)
        double new_edgelen_mean = _prior_parameters[3];
        double exp_lambda = 1.0/new_edgelen_mean;
        double log_exp_lambda = std::log(exp_lambda);
        for (auto nd : tree->_preorder) {
            double v = nd->_edge_length;
            double log_prob = log_exp_lambda - exp_lambda*v;
            log_prior += log_prob;
        }
#else
        assert(tree);

        double TL = _tree_manipulator->calcTreeLength();
        //double n = tree->numLeaves();
        double num_edges = _tree_manipulator->countEdges();

        assert(_prior_parameters.size() == 3);
        double a = _prior_parameters[0];    // shape of Gamma prior on TL
        double b = _prior_parameters[1];    // scale of Gamma prior on TL
        double c = _prior_parameters[2];    // parameter of Dirichlet prior on edge length proportions

        // Calculate Gamma prior on tree length (TL)
        double log_gamma_prior_on_TL = (a - 1.0)*log(TL) - TL/b - a*log(b) - std::lgamma(a);

        // Calculate Dirichlet prior on edge length proportions
        //
        // Note that, for n edges, the Dirichlet prior density is
        //
        // p1^{c-1} p2^{c-1} ... pn^{c-1}
        // ------------------------------
        //    n*Gamma(c) / Gamma(n*c)
        //
        // where n = num_edges, pk = edge length k / TL and Gamma is the Gamma function.
        // If c == 1, then both numerator and denominator equal 1, so it is pointless
        // do loop over edge lengths.
        double log_edge_length_proportions_prior = std::lgamma(num_edges*c);
        if (c != 1.0) {
            for (auto nd : tree->_preorder) {
                double edge_length_proportion = nd->_edge_length/TL;
                log_edge_length_proportions_prior += (c - 1.0)*log(edge_length_proportion);
            }
            log_edge_length_proportions_prior -= std::lgamma(c)*num_edges;
        }

        log_prior = log_gamma_prior_on_TL + log_edge_length_proportions_prior;
#endif
        return log_prior;
    } ///end_calcLogEdgeLengthPrior

    inline double Updater::getLogZero() {
        return _log_zero;
    }
    
} ///end
