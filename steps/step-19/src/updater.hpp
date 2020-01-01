#pragma once    ///start

#include "tree.hpp"
#include "tree_manip.hpp"
#include "lot.hpp"
#include "xstrom.hpp"
#include "likelihood.hpp"
#include "topo_prior_calculator.hpp"    ///!a
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
            void                                    setTopologyPriorOptions(bool resclass, double C);    ///!b
            void                                    setWeight(double w);
            void                                    calcProb(double wsum);

            double                                  getLambda() const;
            double                                  getWeight() const;
            double                                  getProb() const;
            double                                  getAcceptPct() const;
            double                                  getNumUpdates() const;
            std::string                             getUpdaterName() const;

            virtual void                            clear();

            virtual double                          calcLogPrior() = 0;
            double                                  calcLogTopologyPrior() const;   ///!c
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
            double                                  _log_jacobian;          ///!d
            double                                  _target_acceptance;
            unsigned                                _naccepts;
            unsigned                                _nattempts;
            bool                                    _tuning;
            std::vector<double>                     _prior_parameters;
            
            double                                  _heating_power;
            mutable PolytomyTopoPriorCalculator     _topo_prior_calculator; ///!f
            
            static const double                     _log_zero;
    };
 
    // member function bodies go here
    ///end_class_declaration
    inline Updater::Updater() {
        //std::cout << "Updater constructor called" << std::endl;
        clear();
    } 

    inline Updater::~Updater() {
        //std::cout << "Updater destructor called" << std::endl;
    } 

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
        _prior_parameters.clear();
        reset();
    } ///end_clear

    inline void Updater::reset() { ///begin_reset
        _log_hastings_ratio = 0.0;
        _log_jacobian       = 0.0;  ///!h
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
#if 1   //POLTMP
        std::string moveabbr = "?";
        if (_name == "Tree Topology and Edge Proportions")
            moveabbr = "T";
        else if (_name == "Tree Length")
            moveabbr = "L";
        else if (_name == "Polytomies")
            moveabbr = "P";
#endif

        double prev_log_prior = calcLogPrior();
        
        // Clear any nodes previously selected so that we can detect those nodes
        // whose partials and/or transition probabilities need to be recalculated
        _tree_manipulator->deselectAllPartials();
        _tree_manipulator->deselectAllTMatrices();
        
#if 0  //POLTMP
        if (DebugStuff::_which_iter == 12) {
            std::cerr  << "hey!" << std::endl;
        }
#endif

        DebugStuff::debugSaveTree(boost::str(boost::format("pre-%s-%d") % moveabbr % DebugStuff::_which_iter), DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF

        // Set model to proposed state and calculate _log_hastings_ratio
        proposeNewState();
        
        // Use alternative partials and transition probability buffer for any selected nodes
        // This allows us to easily revert to the previous values if the move is rejected
        _tree_manipulator->flipPartialsAndTMatrices();

        DebugStuff::_tree_index++;
        DebugStuff::debugSaveTree(boost::str(boost::format("%s-proposed-flipfwd-%d") % moveabbr % DebugStuff::_which_iter), DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF

        // Calculate the log-likelihood and log-prior for the proposed state
        //_tree_manipulator->selectAllTMatrices();
        //_tree_manipulator->selectAllPartials();
        double log_likelihood = calcLogLikelihood();
        double log_prior = calcLogPrior();
        
        // Decide whether to accept or reject the proposed state
        bool accept = true;
        if (log_prior > _log_zero) {
            double log_R = 0.0; ///!i
            log_R += _heating_power*(log_likelihood - prev_lnL);
            log_R += _heating_power*(log_prior - prev_log_prior);
            log_R += _log_hastings_ratio;
            log_R += _log_jacobian; ///!j
            
            double logu = _lot->logUniform();

            if (logu > log_R)   ///!k
                accept = false; ///!l
        }
        else
            accept = false;

        if (accept) {
            _naccepts++;
#if 1 //POLTMP
            std::cerr << "~~~> ACCEPT: " << _name << std::endl;
            std::ofstream tmpf("tmplike-after-accept.nex");
            tmpf << "#nexus" << std::endl;
            tmpf << std::endl;
            tmpf << "begin paup;" << std::endl;
            tmpf << "  log start replace file=pauplog.txt;" << std::endl;
            tmpf << "  exe rbcl10.nex;" << std::endl;
            tmpf << "  set crit=like forcepolyt storebrlen increase=auto autoinc=1000;" << std::endl;
            tmpf << "end;" << std::endl;
            tmpf << std::endl;
            tmpf << "begin trees;" << std::endl;
            tmpf << "  translate" << std::endl;
            tmpf << "    1 Chlamydomonas_moewusii_M15842.1," << std::endl;
            tmpf << "    2 Mnium_hornum_AF226820.1," << std::endl;
            tmpf << "    3 Lycopodium_digitatum_L11055.1," << std::endl;
            tmpf << "    4 Adiantum_capillus_veneris_AY178864.1," << std::endl;
            tmpf << "    5 Gnetum_gnemon_L12680.2," << std::endl;
            tmpf << "    6 Pinus_radiata_AY497250.1_Monterrey_Pine," << std::endl;
            tmpf << "    7 Zea_mays_Z11973.1_Corn," << std::endl;
            tmpf << "    8 Coffea_arabica_X83631.1_Coffee," << std::endl;
            tmpf << "    9 Theobroma_cacao_AF022125.1_Chocolate," << std::endl;
            tmpf << "    10 Acer_saccharum_L01881.2_Sugar_Maple" << std::endl;
            tmpf << "  ;" << std::endl;
            tmpf << boost::str(boost::format("  tree test [%.5f] = %s;") % log_likelihood % _tree_manipulator->makeNewick(8)) << std::endl;
            tmpf << "end;" << std::endl;
            tmpf << std::endl;
            tmpf << "begin paup;" << std::endl;
            tmpf << "  lset nst=1 basefreq=equal rates=equal userbrlens;" << std::endl;
            tmpf << "  lscores 1;" << std::endl;
            tmpf << "  log stop;" << std::endl;
            tmpf << "  quit;" << std::endl;
            tmpf << "end;" << std::endl;
            tmpf.close();
            
            std::cerr << boost::str(boost::format("tmplike-after-accept: %.5f") % log_likelihood);
            std::cerr << std::endl;
#endif
        }
        else {
#if 1 //POLTMP
            std::cerr << "~~~> REJECT: " << _name << std::endl;
            std::ofstream tmpf("tmplike-before-revert.nex");
            tmpf << "#nexus" << std::endl;
            tmpf << std::endl;
            tmpf << "begin paup;" << std::endl;
            tmpf << "  log start replace file=pauplog.txt;" << std::endl;
            tmpf << "  exe rbcl10.nex;" << std::endl;
            tmpf << "  set crit=like forcepolyt storebrlen increase=auto autoinc=1000;" << std::endl;
            tmpf << "end;" << std::endl;
            tmpf << std::endl;
            tmpf << "begin trees;" << std::endl;
            tmpf << "  translate" << std::endl;
            tmpf << "    1 Chlamydomonas_moewusii_M15842.1," << std::endl;
            tmpf << "    2 Mnium_hornum_AF226820.1," << std::endl;
            tmpf << "    3 Lycopodium_digitatum_L11055.1," << std::endl;
            tmpf << "    4 Adiantum_capillus_veneris_AY178864.1," << std::endl;
            tmpf << "    5 Gnetum_gnemon_L12680.2," << std::endl;
            tmpf << "    6 Pinus_radiata_AY497250.1_Monterrey_Pine," << std::endl;
            tmpf << "    7 Zea_mays_Z11973.1_Corn," << std::endl;
            tmpf << "    8 Coffea_arabica_X83631.1_Coffee," << std::endl;
            tmpf << "    9 Theobroma_cacao_AF022125.1_Chocolate," << std::endl;
            tmpf << "    10 Acer_saccharum_L01881.2_Sugar_Maple" << std::endl;
            tmpf << "  ;" << std::endl;
            tmpf << boost::str(boost::format("  tree test [%.5f] = %s;") % log_likelihood % _tree_manipulator->makeNewick(8)) << std::endl;
            tmpf << "end;" << std::endl;
            tmpf << std::endl;
            tmpf << "begin paup;" << std::endl;
            tmpf << "  lset nst=1 basefreq=equal rates=equal userbrlens;" << std::endl;
            tmpf << "  lscores 1;" << std::endl;
            tmpf << "  log stop;" << std::endl;
            tmpf << "  quit;" << std::endl;
            tmpf << "end;" << std::endl;
            tmpf.close();
#endif
            revert();
#if 1   //POLTMP
            std::ofstream tmpf2("tmplike-after-revert.nex");
            tmpf2 << "#nexus" << std::endl;
            tmpf2 << std::endl;
            tmpf2 << "begin paup;" << std::endl;
            tmpf2 << "  log start replace file=pauplog.txt;" << std::endl;
            tmpf2 << "  exe rbcl10.nex;" << std::endl;
            tmpf2 << "  set crit=like forcepolyt storebrlen increase=auto autoinc=1000;" << std::endl;
            tmpf2 << "end;" << std::endl;
            tmpf2 << std::endl;
            tmpf2 << "begin trees;" << std::endl;
            tmpf2 << "  translate" << std::endl;
            tmpf2 << "    1 Chlamydomonas_moewusii_M15842.1," << std::endl;
            tmpf2 << "    2 Mnium_hornum_AF226820.1," << std::endl;
            tmpf2 << "    3 Lycopodium_digitatum_L11055.1," << std::endl;
            tmpf2 << "    4 Adiantum_capillus_veneris_AY178864.1," << std::endl;
            tmpf2 << "    5 Gnetum_gnemon_L12680.2," << std::endl;
            tmpf2 << "    6 Pinus_radiata_AY497250.1_Monterrey_Pine," << std::endl;
            tmpf2 << "    7 Zea_mays_Z11973.1_Corn," << std::endl;
            tmpf2 << "    8 Coffea_arabica_X83631.1_Coffee," << std::endl;
            tmpf2 << "    9 Theobroma_cacao_AF022125.1_Chocolate," << std::endl;
            tmpf2 << "    10 Acer_saccharum_L01881.2_Sugar_Maple" << std::endl;
            tmpf2 << "  ;" << std::endl;
            tmpf2 << boost::str(boost::format("  tree test [%.5f] = %s;") % log_likelihood % _tree_manipulator->makeNewick(8)) << std::endl;
            tmpf2 << "end;" << std::endl;
            tmpf2 << std::endl;
            tmpf2 << "begin paup;" << std::endl;
            tmpf2 << "  lset nst=1 basefreq=equal rates=equal userbrlens;" << std::endl;
            tmpf2 << "  lscores 1;" << std::endl;
            tmpf2 << "  log stop;" << std::endl;
            tmpf2 << "  quit;" << std::endl;
            tmpf2 << "end;" << std::endl;
            tmpf2.close();
            
            std::cerr << boost::str(boost::format("tmplike-before-revert: %.5f") % log_likelihood) << std::endl;
            std::cerr << boost::str(boost::format("tmplike-after-revert: %.5f") % prev_lnL) << std::endl;
#endif
            _tree_manipulator->flipPartialsAndTMatrices();
            log_likelihood = prev_lnL;
            
            DebugStuff::_tree_index++;
            DebugStuff::debugSaveTree(boost::str(boost::format("%s-reverted-flipback-%d") % moveabbr % DebugStuff::_which_iter), DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
        }

        tune(accept);
        reset();

        return log_likelihood;
    } ///end_update
    
    inline void Updater::setTopologyPriorOptions(bool resclass, double C) { ///begin_setTopologyPriorOptions
        _topo_prior_calculator.setC(C);
        if (resclass)
            _topo_prior_calculator.chooseResolutionClassPrior();
        else
            _topo_prior_calculator.choosePolytomyPrior();
    }   ///end_setTopologyPriorOptions
    
    inline double Updater::calcLogTopologyPrior() const {   ///begin_calcLogTopologyPrior
        Tree::SharedPtr tree = _tree_manipulator->getTree();
        assert(tree);
        if (tree->isRooted())
            _topo_prior_calculator.chooseRooted();
        else
            _topo_prior_calculator.chooseUnrooted();
        _topo_prior_calculator.setNTax(tree->numLeaves());
        unsigned m = tree->numInternals();
                
        double log_topology_prior = _topo_prior_calculator.getLogNormalizedTopologyPrior(m);

        return log_topology_prior;
    }   ///end_calcLogTopologyPrior

    inline double Updater::calcLogEdgeLengthPrior() const { ///begin_calcLogEdgeLengthPrior
        double log_prior = 0.0;
        Tree::SharedPtr tree = _tree_manipulator->getTree();
        assert(tree);

        double TL = _tree_manipulator->calcTreeLength();
        //double n = tree->numLeaves();                     ///!m
        double num_edges = _tree_manipulator->countEdges(); ///!n

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
        //    Gamma(c)^n / Gamma(n*c)
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
        return log_prior;
    } ///end_calcLogEdgeLengthPrior

    inline double Updater::getLogZero() {
        return _log_zero;
    }
    
} ///end
