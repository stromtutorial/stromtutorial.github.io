#pragma once    ///start

#include "updater.hpp"

namespace strom {

    class TreeLengthUpdater : public Updater {
    
        public:
        
            typedef std::shared_ptr< TreeLengthUpdater > SharedPtr;

                                        TreeLengthUpdater();
                                        ~TreeLengthUpdater();

            virtual void                clear();
            virtual void                proposeNewState();
            virtual void                revert();

            virtual double              calcLogPrior(int & checklist);

            void                        pullFromModel();
            void                        pushToModel() const;
        
        private:

            double                      _prev_point;
            double                      _curr_point;
    };

    // Member function bodies go here
    ///end_class_declaration
    
    inline TreeLengthUpdater::TreeLengthUpdater() { ///begin_constructor
        // std::cout << "Creating a TreeLengthUpdater..." << std::endl;
        clear();
        _name = "Tree Length";
    }   ///end_constructor

    inline TreeLengthUpdater::~TreeLengthUpdater() {    ///begin_destructor
        // std::cout << "Destroying a TreeLengthUpdater..." << std::endl;
    }   ///end_destructor

    inline void TreeLengthUpdater::clear() {    ///begin_clear
        Updater::clear();
        _prev_point     = 0.0;
        _curr_point     = 0.0;
        reset();
    }   ///end_clear
    
    inline void TreeLengthUpdater::proposeNewState() {  ///begin_proposeNewState
        // Save copy of _curr_point in case revert is necessary.
        pullFromModel();
        _prev_point = _curr_point;

        // Let _curr_point be proposed value
        double m = exp(_lambda*(_lot->uniform() - 0.5));
        _curr_point = m*_prev_point;
        pushToModel();

        // calculate log of Hastings ratio under GammaDir parameterization
        _log_hastings_ratio = log(m);

        // This proposal invalidates all transition matrices and partials
        _tree_manipulator->selectAllPartials();
        _tree_manipulator->selectAllTMatrices();
    }   ///end_proposeNewState

    inline void TreeLengthUpdater::revert() {   ///begin_revert
        // swap _curr_point and _prev_point so that edge length scaler
        // in pushCurrentStateToModel will be correctly calculated
        double tmp = _curr_point;
        _curr_point = _prev_point;
        _prev_point = tmp;
        pushToModel();
    }   ///end_revert

    inline double TreeLengthUpdater::calcLogPrior(int & checklist) {   ///begin_calcLogPrior
        if (checklist & Model::EdgeLengths)
            return 0.0;
        checklist |= Model::EdgeLengths;

        return Updater::calcLogEdgeLengthPrior();
    }   ///end_calcLogPrior

    inline void TreeLengthUpdater::pullFromModel() {    ///begin_pullFromModel
        _curr_point = _tree_manipulator->calcTreeLength();
    }   ///end_pullFromModel

    inline void TreeLengthUpdater::pushToModel() const {    ///begin_pushToModel
        double scaler = _curr_point/_prev_point;
        _tree_manipulator->scaleAllEdgeLengths(scaler);
    }   ///end_pushToModel

}   ///end
