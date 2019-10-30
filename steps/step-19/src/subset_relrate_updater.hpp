#pragma once    ///start

#include "dirichlet_updater.hpp"

namespace strom {
    
    class SubsetRelRateUpdater : public DirichletUpdater {
        public:
            typedef std::shared_ptr< SubsetRelRateUpdater > SharedPtr;

                                            SubsetRelRateUpdater(Model::SharedPtr model);
                                            ~SubsetRelRateUpdater();
        
            double                          calcLogPrior();

        private:
        
            void                            pullFromModel();
            void                            pushToModel();

            Model::SharedPtr                _model;
        };

    // member function bodies go here
    ///end_class_declaration
    inline SubsetRelRateUpdater::SubsetRelRateUpdater(Model::SharedPtr model) {   ///begin_constructor
        //std::cout << "Creating a SubsetRelRateUpdater" << std::endl;
        DirichletUpdater::clear();
        _name = "Subset Relative Rates";
        _model = model;
    }   ///end_constructor

    inline SubsetRelRateUpdater::~SubsetRelRateUpdater() {  ///begin_destructor
        //std::cout << "Destroying a SubsetRelRateUpdater" << std::endl;
    }   ///end_destructor

    inline double SubsetRelRateUpdater::calcLogPrior() {  ///begin_calcLogPrior
        Model::subset_sizes_t & subset_sizes = _model->getSubsetSizes();
        double log_num_sites = std::log(_model->getNumSites());
        unsigned num_subsets = _model->getNumSubsets();
        double log_prior = DirichletUpdater::calcLogPrior();
        for (unsigned i = 0; i < num_subsets-1; i++) {
            log_prior += std::log(subset_sizes[i]) - log_num_sites;
        }
        return log_prior;
    }  ///end_calcLogPrior

    inline void SubsetRelRateUpdater::pullFromModel() {  ///begin_pullFromModel
        Model::subset_relrate_vect_t & relative_rates = _model->getSubsetRelRates();
        Model::subset_sizes_t &        subset_sizes   = _model->getSubsetSizes();
        unsigned num_sites   = _model->getNumSites();
        unsigned num_subsets = _model->getNumSubsets();
        assert(subset_sizes.size() == num_subsets);
        assert(relative_rates.size() == num_subsets);
        _curr_point.resize(num_subsets);
        for (unsigned i = 0; i < num_subsets; i++) {
            _curr_point[i] = relative_rates[i]*subset_sizes[i]/num_sites;
        }
    }   ///end_pullFromModel
    
    inline void SubsetRelRateUpdater::pushToModel() {  ///begin_pushToModel
        Model::subset_sizes_t & subset_sizes = _model->getSubsetSizes();
        unsigned num_sites   = _model->getNumSites();
        unsigned num_subsets = _model->getNumSubsets();
        point_t tmp(num_subsets);
        for (unsigned i = 0; i < num_subsets; i++) {
            tmp[i] = _curr_point[i]*num_sites/subset_sizes[i];
        }
        _model->setSubsetRelRates(tmp, false);
    }   ///end_pushToModel
    
}   ///end
