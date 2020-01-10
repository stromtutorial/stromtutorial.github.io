#pragma once    ///start

#include "updater.hpp"

namespace strom {
    
    class Chain;
    
    class DirichletUpdater : public Updater {

        friend class Chain;
        
        public:
            typedef std::vector<double>                 point_t;
            typedef std::shared_ptr< DirichletUpdater > SharedPtr;

                                                DirichletUpdater();
            virtual                             ~DirichletUpdater();
        
            void                                clear();
            virtual double                      calcLogPrior();
        
        protected:
        
            virtual void                        pullFromModel() = 0;    ///pullFromModel
            virtual void                        pushToModel() = 0;    ///pushToModel

            void                                proposeNewState();
            void                                revert();
        
            point_t                             _curr_point;
            point_t                             _prev_point;
    };
    
    // member function bodies go here
    ///end_class_declaration
    inline DirichletUpdater::DirichletUpdater() {   ///begin_constructor
        // std::cout << "Creating DirichletUpdater object" << std::endl;
        clear();
    }

    inline DirichletUpdater::~DirichletUpdater() {
        // std::cout << "Destroying DirichletUpdater object" << std::endl;
    }   ///end_destructor
    
    inline void DirichletUpdater::clear() { ///begin_clear
        Updater::clear();
        _prev_point.clear();
    }   ///end_clear
    
    inline double DirichletUpdater::calcLogPrior() {  ///begin_calcLogPrior
        pullFromModel();
        assert(_curr_point.size() > 0);
        assert(_curr_point.size() == _prior_parameters.size());
        bool flat_prior = true;
        bool bad_point = false;
        double log_prior = 0.0;
        double prior_param_sum = 0.0;
        for (unsigned i = 0; i < _curr_point.size(); ++i) {
            if (_prior_parameters[i] != 1.0)
                flat_prior = false;
            if (_curr_point[i] == 0.0)
                bad_point = true;
            log_prior += (_prior_parameters[i] - 1.0)*std::log(_curr_point[i]);
            log_prior -= std::lgamma(_prior_parameters[i]);
            prior_param_sum += _prior_parameters[i];
        }
        if (flat_prior)
            return std::lgamma(prior_param_sum);
        else if (bad_point)
            return Updater::_log_zero;
        else
            log_prior += std::lgamma(prior_param_sum);
        return log_prior;
    }  ///end_calcLogPrior

    inline void DirichletUpdater::proposeNewState() {   ///begin_proposeNewState
        // Save length of _curr_point.
        pullFromModel();
        unsigned dim = (unsigned)_curr_point.size();
        
        // Save copy of _curr_point in case revert is necessary.
        _prev_point.assign(_curr_point.begin(), _curr_point.end());
        
        // Determine parameters of Dirichlet forward proposal distribution and, at the same time,
        // draw gamma deviates that will be used to form the proposed point.
        std::vector<double> forward_params(dim, 0.0);
        for (unsigned i = 0; i < dim; ++i) {
            // Calculate ith forward parameter
            double alpha_i = 1.0 + _prev_point[i]/_lambda;
            if (alpha_i < 1.e-12)
                alpha_i = 1.e-12;
            forward_params[i] = alpha_i;
            
            // Draw ith gamma deviate
            _curr_point[i] = 0.0;
            if (alpha_i > 0.0)
                _curr_point[i] = _lot->gamma(alpha_i, 1.0);
        }
        
        double sum_gamma_deviates     = std::accumulate(_curr_point.begin(), _curr_point.end(), 0.0);
        double sum_forward_parameters = std::accumulate(forward_params.begin(), forward_params.end(), 0.0);

        // Choose new state by sampling from forward proposal distribution.
        // We've already stored gamma deviates in _curr_point, now just need to normalize them.
        for (unsigned i = 0; i < dim; ++i) {
            _curr_point[i] /= sum_gamma_deviates;
        }
        
        // Determine probability density of the forward proposal
        double log_forward_density = 0.0;
        for (unsigned i = 0; i < dim; ++i) {
            log_forward_density += (forward_params[i] - 1.0)*std::log(_prev_point[i]);
            log_forward_density -= std::lgamma(forward_params[i]);
        }
        log_forward_density += std::lgamma(sum_forward_parameters);
        
        // Determine parameters of Dirichlet reverse proposal distribution
        std::vector<double> reverse_params(dim, 0.0);
        for (unsigned i = 0; i < dim; ++i) {
            reverse_params[i] = 1.0 + _curr_point[i]/_lambda;
        }
        
        double sum_reverse_parameters = std::accumulate(reverse_params.begin(), reverse_params.end(), 0.0);

        // determine probability density of the reverse proposal
        double log_reverse_density = 0.0;
        for (unsigned i = 0; i < dim; ++i) {
            log_reverse_density += (reverse_params[i] - 1.0)*std::log(_curr_point[i]);
            log_reverse_density -= std::lgamma(reverse_params[i]);
        }
        log_reverse_density += std::lgamma(sum_reverse_parameters);
        
        // calculate the logarithm of the Hastings ratio
        _log_hastings_ratio = log_reverse_density - log_forward_density;
        
        pushToModel();

        // This proposal invalidates all transition matrices and partials
        _tree_manipulator->selectAllPartials();
        _tree_manipulator->selectAllTMatrices();
    }   ///end_proposeNewState
    
    inline void DirichletUpdater::revert() {    ///begin_revert
        std::copy(_prev_point.begin(), _prev_point.end(), _curr_point.begin());
        pushToModel();
    }   ///end_revert

}   ///end
