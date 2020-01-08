#pragma once

#include <memory>
#include <boost/format.hpp>
#include "lot.hpp"
#include "data.hpp"
#include "tree.hpp"
#include "model.hpp"
#include "likelihood.hpp"
#include "tree_manip.hpp"
#include "updater.hpp"
#include "gamma_ratevar_updater.hpp"
#include "omega_updater.hpp"
#include "pinvar_updater.hpp"
#include "statefreq_updater.hpp"
#include "exchangeability_updater.hpp"
#include "subset_relrate_updater.hpp"
#include "tree_updater.hpp"
#include "tree_length_updater.hpp"

namespace strom {

    class Chain {
    
        friend class Likelihood;

        public:
        
            typedef std::vector<Updater::SharedPtr> updater_vect_t;
            typedef std::shared_ptr<Chain>          SharedPtr;

                                                    Chain();
                                                    ~Chain();

            void                                    clear();

            void                                    startTuning();
            void                                    stopTuning();

            void                                    setTreeFromNewick(std::string & newick);
            unsigned                                createUpdaters(Model::SharedPtr model, Lot::SharedPtr lot, Likelihood::SharedPtr likelihood);

            TreeManip::SharedPtr                    getTreeManip();
            Model::SharedPtr                        getModel();
            double                                  getLogLikelihood() const;


            void                                    setHeatingPower(double p);
            double                                  getHeatingPower() const;

            void                                    setChainIndex(unsigned idx);
            double                                  getChainIndex() const;

            Updater::SharedPtr                      findUpdaterByName(std::string name);
            std::vector<std::string>                getUpdaterNames() const;
            std::vector<double>                     getAcceptPercentages() const;
            std::vector<unsigned>                   getNumUpdates() const;
            std::vector<double>                     getLambdas() const;
            void                                    setLambdas(std::vector<double> & v);

            double                                  calcLogLikelihood() const;
            double                                  calcLogJointPrior() const;

            void                                    start();
            void                                    stop();
            void                                    nextStep(int iteration);

        private:

            Model::SharedPtr                        _model;
            Lot::SharedPtr                          _lot;
            TreeManip::SharedPtr                    _tree_manipulator;

            updater_vect_t                          _updaters;

            unsigned                                _chain_index;
            double                                  _heating_power;
            double                                  _log_likelihood;
    };

    // member function bodies go here
    inline Chain::Chain() {
        //std::cout << "Chain being created" << std::endl;
        clear();
    }

    inline Chain::~Chain() {
        //std::cout << "Chain being destroyed" << std::endl;
    }

    inline void Chain::clear() {
        _log_likelihood = 0.0;
        _updaters.clear();
        _chain_index = 0;
        setHeatingPower(1.0);
        startTuning();
    }

    inline void Chain::startTuning() {
        for (auto u : _updaters)
            u->setTuning(true);
    }

    inline void Chain::stopTuning() {
        for (auto u : _updaters)
            u->setTuning(false);
    }

    inline void Chain::setTreeFromNewick(std::string & newick) {
        assert(_updaters.size() > 0);
        if (!_tree_manipulator)
            _tree_manipulator.reset(new TreeManip);
        _tree_manipulator->buildFromNewick(newick, false, false);
        for (auto u : _updaters)
            u->setTreeManip(_tree_manipulator);
    }

    inline unsigned Chain::createUpdaters(Model::SharedPtr model, Lot::SharedPtr lot, Likelihood::SharedPtr likelihood) {
        _model = model;
        _lot = lot;
        _updaters.clear();

        double wstd             = 1.0;
        double wtreelength      = 1.0;
        double wtreetopology    = 19.0;
        double sum_weights      = 0.0;
        
        // Add state frequency parameter updaters to _updaters
        Model::state_freq_params_t & statefreq_shptr_vect = _model->getStateFreqParams();
        for (auto statefreq_shptr : statefreq_shptr_vect) {
            Updater::SharedPtr u = StateFreqUpdater::SharedPtr(new StateFreqUpdater(statefreq_shptr));
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(1.0);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters(std::vector<double>(statefreq_shptr->getStateFreqsSharedPtr()->size(), 1.0));
            u->setWeight(wstd); sum_weights += wstd;
            _updaters.push_back(u);
        }

        // Add exchangeability parameter updaters to _updaters
        Model::exchangeability_params_t & exchangeability_shptr_vect = _model->getExchangeabilityParams();
        for (auto exchangeability_shptr : exchangeability_shptr_vect) {
            Updater::SharedPtr u = ExchangeabilityUpdater::SharedPtr(new ExchangeabilityUpdater(exchangeability_shptr));
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(1.0);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters({1.0, 1.0, 1.0, 1.0, 1.0, 1.0});
            u->setWeight(wstd); sum_weights += wstd;
            _updaters.push_back(u);
        }

        // Add rate variance parameter updaters to _updaters
        Model::ratevar_params_t & ratevar_shptr_vect = _model->getRateVarParams();
        for (auto ratevar_shptr : ratevar_shptr_vect) {
            Updater::SharedPtr u = GammaRateVarUpdater::SharedPtr(new GammaRateVarUpdater(ratevar_shptr));
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(1.0);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters({1.0, 1.0});
            u->setWeight(wstd); sum_weights += wstd;
            _updaters.push_back(u);
        }
        
        // Add pinvar parameter updaters to _updaters
        Model::pinvar_params_t & pinvar_shptr_vect = _model->getPinvarParams();
        for (auto pinvar_shptr : pinvar_shptr_vect) {
            Updater::SharedPtr u = PinvarUpdater::SharedPtr(new PinvarUpdater(pinvar_shptr));
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(0.5);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters({1.0, 1.0});
            u->setWeight(wstd); sum_weights += wstd;
            _updaters.push_back(u);
        }
        
        // Add omega parameter updaters to _updaters
        Model::omega_params_t & omega_shptr_vect = _model->getOmegaParams();
        for (auto omega_shptr : omega_shptr_vect) {
            Updater::SharedPtr u = OmegaUpdater::SharedPtr(new OmegaUpdater(omega_shptr));
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(1.0);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters({1.0, 1.0});
            u->setWeight(wstd); sum_weights += wstd;
            _updaters.push_back(u);
        }
        
        // Add subset relative rate parameter updater to _updaters
        if (!_model->isFixedSubsetRelRates()) {
            Updater::SharedPtr u = SubsetRelRateUpdater::SharedPtr(new SubsetRelRateUpdater(_model));
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(1.0);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters(std::vector<double>(_model->getNumSubsets(), 1.0));
            u->setWeight(wstd); sum_weights += wstd;
            _updaters.push_back(u);
        }
        
        // Add tree updater and tree length updater to _updaters
        if (!_model->isFixedTree()) {
            double tree_length_shape = 1.0;
            double tree_length_scale = 10.0;
            double dirichlet_param   = 1.0;
            
            Updater::SharedPtr u = TreeUpdater::SharedPtr(new TreeUpdater());
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(0.5);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters({tree_length_shape, tree_length_scale, dirichlet_param});
            u->setWeight(wtreetopology); sum_weights += wtreetopology;
            _updaters.push_back(u);

            u = TreeLengthUpdater::SharedPtr(new TreeLengthUpdater());
            u->setLikelihood(likelihood);
            u->setLot(lot);
            u->setLambda(0.2);
            u->setTargetAcceptanceRate(0.3);
            u->setPriorParameters({tree_length_shape, tree_length_scale, dirichlet_param});
            u->setWeight(wtreelength); sum_weights += wtreelength;
            _updaters.push_back(u);
        }
        
        for (auto u : _updaters) {
            u->calcProb(sum_weights);
        }
        
        return (unsigned)_updaters.size();
    }

    inline TreeManip::SharedPtr Chain::getTreeManip() {
        return _tree_manipulator;
    }

    inline Model::SharedPtr Chain::getModel() {
        return _model;
    }

    inline double Chain::getHeatingPower() const {
        return _heating_power;
    }

    inline void Chain::setHeatingPower(double p) {
        _heating_power = p;
        for (auto u : _updaters)
            u->setHeatingPower(p);
    }

    inline double Chain::getChainIndex() const {
        return _chain_index;
    }

    inline void Chain::setChainIndex(unsigned idx) {
        _chain_index = idx;
    }
        
    inline Updater::SharedPtr Chain::findUpdaterByName(std::string name) {
        Updater::SharedPtr retval = nullptr;
        for (auto u : _updaters) {
            if (u->getUpdaterName() == name) {
                retval = u;
                break;
            }
        }
        assert(retval != nullptr);
        return retval;
    }

    inline std::vector<std::string> Chain::getUpdaterNames() const {
        std::vector<std::string> v;
        for (auto u : _updaters)
            v.push_back(u->getUpdaterName());
        return v;
    }

    inline std::vector<double> Chain::getAcceptPercentages() const {
        std::vector<double> v;
        for (auto u : _updaters)
            v.push_back(u->getAcceptPct());
        return v;
    }

    inline std::vector<unsigned> Chain::getNumUpdates() const {
        std::vector<unsigned> v;
        for (auto u : _updaters)
            v.push_back(u->getNumUpdates());
        return v;
    }

    inline std::vector<double> Chain::getLambdas() const {
        std::vector<double> v;
        for (auto u : _updaters)
            v.push_back(u->getLambda());
        return v;
    }

    inline void Chain::setLambdas(std::vector<double> & v) {
        assert(v.size() == _updaters.size());
        unsigned index = 0;
        for (auto u : _updaters) {
            u->setLambda(v[index++]);
        }
    }

    inline double Chain::calcLogLikelihood() const {
        return _updaters[0]->calcLogLikelihood();
    }

    inline double Chain::calcLogJointPrior() const {
        double lnP = 0.0;
        for (auto u : _updaters) {
            if (u->_name != "Tree Length")
                lnP += u->calcLogPrior();
        }
        return lnP;
    }

    inline void Chain::start() {
        _tree_manipulator->selectAllPartials();
        _tree_manipulator->selectAllTMatrices();
        _log_likelihood = calcLogLikelihood();
        _tree_manipulator->deselectAllPartials();
        _tree_manipulator->deselectAllTMatrices();
    }

    inline void Chain::stop() {
    }

    inline void Chain::nextStep(int iteration) {
        assert(_lot);
        double u = _lot->uniform();
        double cumprob = 0.0;
        unsigned i = 0;
        for (auto updater : _updaters) {
            cumprob += updater->_prob;
            if (u < cumprob)
                break;
            i++;
        }
        assert(i < _updaters.size());
        _log_likelihood = _updaters[i]->update(_log_likelihood);
    }

    inline double Chain::getLogLikelihood() const {
        return _log_likelihood;
    }

}
