#pragma once    ///start

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
    ///end_class_declaration
    inline Chain::Chain() { ///begin_constructor
        //std::cout << "Chain being created" << std::endl;
        clear();
    }

    inline Chain::~Chain() {
        //std::cout << "Chain being destroyed" << std::endl;
    } ///end_destructor

    inline void Chain::clear() { ///begin_clear
        _log_likelihood = 0.0;
        _updaters.clear();
        _chain_index = 0;
        setHeatingPower(1.0);
        startTuning();
    } ///end_clear

    inline void Chain::startTuning() { ///begin_startTuning
        for (auto u : _updaters)
            u->setTuning(true);
    }

    inline void Chain::stopTuning() {
        for (auto u : _updaters)
            u->setTuning(false);
    } ///end_stopTuning

    inline void Chain::setTreeFromNewick(std::string & newick) { ///begin_setTreeFromNewick
        assert(_updaters.size() > 0);
        if (!_tree_manipulator)
            _tree_manipulator.reset(new TreeManip);
        _tree_manipulator->buildFromNewick(newick, false, false);
        for (auto u : _updaters)
            u->setTreeManip(_tree_manipulator);
    } ///end_setTreeFromNewick

    inline unsigned Chain::createUpdaters(Model::SharedPtr model, Lot::SharedPtr lot, Likelihood::SharedPtr likelihood) { ///begin_createUpdaters
        _model = model;
        _lot = lot;
        _updaters.clear();

        double wstd             = 1.0;
        double sum_weights      = 0.0;
        
        // Add rate variance parameter updater to _updaters
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

        for (auto u : _updaters) {
            u->calcProb(sum_weights);
        }

        return (unsigned)_updaters.size();
    } ///end_createUpdaters

    inline TreeManip::SharedPtr Chain::getTreeManip() { ///begin_getTreeManip
        return _tree_manipulator;
    } ///end_getTreeManip

    inline Model::SharedPtr Chain::getModel() { ///begin_getModel
        return _model;
    } ///end_getModel

    inline double Chain::getHeatingPower() const { ///begin_getHeatingPower
        return _heating_power;
    }

    inline void Chain::setHeatingPower(double p) {
        _heating_power = p;
        for (auto u : _updaters)
            u->setHeatingPower(p);
    } ///end_setHeatingPower

    inline double Chain::getChainIndex() const { ///begin_getChainIndex
        return _chain_index;
    }

    inline void Chain::setChainIndex(unsigned idx) {
        _chain_index = idx;
    } ///end_setChainIndex
        
    inline Updater::SharedPtr Chain::findUpdaterByName(std::string name) { ///begin_findUpdaterByName
        Updater::SharedPtr retval = nullptr;
        for (auto u : _updaters) {
            if (u->getUpdaterName() == name) {
                retval = u;
                break;
            }
        }
        assert(retval != nullptr);
        return retval;
    } ///end_findUpdaterByName

    inline std::vector<std::string> Chain::getUpdaterNames() const { ///begin_getUpdaterNames
        std::vector<std::string> v;
        for (auto u : _updaters)
            v.push_back(u->getUpdaterName());
        return v;
    } ///end_getUpdaterNames

    inline std::vector<double> Chain::getAcceptPercentages() const { ///begin_getAcceptPercentages
        std::vector<double> v;
        for (auto u : _updaters)
            v.push_back(u->getAcceptPct());
        return v;
    } ///end_getAcceptPercentages

    inline std::vector<unsigned> Chain::getNumUpdates() const { ///begin_getNumUpdates
        std::vector<unsigned> v;
        for (auto u : _updaters)
            v.push_back(u->getNumUpdates());
        return v;
    } ///end_getNumUpdates

    inline std::vector<double> Chain::getLambdas() const { ///begin_getLambdas
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
    } ///end_setLambdas

    inline double Chain::calcLogLikelihood() const { ///begin_calcLogLikelihood
        return _updaters[0]->calcLogLikelihood();
    } ///end_calcLogLikelihood

    inline double Chain::calcLogJointPrior() const { ///begin_calcLogJointPrior
        double lnP = 0.0;
        for (auto u : _updaters) {
            lnP += u->calcLogPrior();
        }
        return lnP;
    } ///end_calcLogJointPrior

    inline void Chain::start() { ///begin_start
        _tree_manipulator->selectAllPartials();
        _tree_manipulator->selectAllTMatrices();
        _log_likelihood = calcLogLikelihood();
        _tree_manipulator->deselectAllPartials();
        _tree_manipulator->deselectAllTMatrices();
    }

    inline void Chain::stop() {
    } ///end_stop

    inline void Chain::nextStep(int iteration) { ///begin_nextStep
        assert(_lot);
        double u = _lot->uniform();
        double cumprob = 0.0;
        unsigned i = 0;
        for (auto updater : _updaters) {
            cumprob += updater->_prob;
            if (u <= cumprob)
                break;
            i++;
        }
        assert(i < _updaters.size());
        _log_likelihood = _updaters[i]->update(_log_likelihood);
    } ///end_nextStep

    inline double Chain::getLogLikelihood() const {///begin_getLogLikelihood
        return _log_likelihood;
    } ///end_getLogLikelihood

}   ///end
