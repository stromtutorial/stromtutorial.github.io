#pragma once    ///start

#include "dirichlet_updater.hpp"

namespace strom {
    
    class StateFreqUpdater : public DirichletUpdater {
        public:
            typedef std::shared_ptr< StateFreqUpdater > SharedPtr;

                                            StateFreqUpdater(QMatrix::SharedPtr qmatrix);
                                            ~StateFreqUpdater();
                                            
            virtual double                  calcLogPrior(int & checklist);

        private:
        
            void                            pullFromModel();
            void                            pushToModel();

            QMatrix::SharedPtr              _qmatrix;
        };

    // member function bodies go here
    ///end_class_declaration
    inline StateFreqUpdater::StateFreqUpdater(QMatrix::SharedPtr qmatrix) {   ///begin_constructor
        //std::cout << "Creating a StateFreqUpdater" << std::endl;
        DirichletUpdater::clear();
        _name = "State Frequencies";
        assert(qmatrix);
        _qmatrix = qmatrix;
    }   ///end_constructor

    inline StateFreqUpdater::~StateFreqUpdater() {  ///begin_destructor
        //std::cout << "Destroying a StateFreqUpdater" << std::endl;
    }   ///end_destructor

    inline double StateFreqUpdater::calcLogPrior(int & checklist) {  ///begin_calcLogPrior
        if (checklist & Model::StateFreqs)
            return 0.0;
        checklist |= Model::StateFreqs;
        
        return DirichletUpdater::calcLogPrior(checklist);
    }  ///end_calcLogPrior
    
    inline void StateFreqUpdater::pullFromModel() {  ///begin_pullFromModel
        QMatrix::freq_xchg_ptr_t freqs = _qmatrix->getStateFreqsSharedPtr();
        _curr_point.assign(freqs->begin(), freqs->end());
    }   ///end_pullFromModel
    
    inline void StateFreqUpdater::pushToModel() {  ///begin_pushToModel
        _qmatrix->setStateFreqs(_curr_point);
    }   ///end_pushToModel
    
}   ///end
