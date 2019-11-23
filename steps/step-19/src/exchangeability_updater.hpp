#pragma once    ///start

#include "dirichlet_updater.hpp"

namespace strom {
    
    class ExchangeabilityUpdater : public DirichletUpdater {
        public:
            typedef std::shared_ptr< ExchangeabilityUpdater > SharedPtr;

                                            ExchangeabilityUpdater(QMatrix::SharedPtr qmatrix);
                                            ~ExchangeabilityUpdater();
                                        
            virtual double                  calcLogPrior(int & checklist);

        private:
        
            void                            pullFromModel();
            void                            pushToModel();

            QMatrix::SharedPtr              _qmatrix;
        };

    // member function bodies go here
    ///end_class_declaration
    inline ExchangeabilityUpdater::ExchangeabilityUpdater(QMatrix::SharedPtr qmatrix) { ///begin_constructor 
        // std::cout << "Creating an ExchangeabilityUpdater" << std::endl;
        DirichletUpdater::clear();
        _name = "Exchangeabilities";
        assert(qmatrix);
        _qmatrix = qmatrix;
    }   ///end_constructor

    inline ExchangeabilityUpdater::~ExchangeabilityUpdater() {  ///begin_destructor
        // std::cout << "Destroying an ExchangeabilityUpdater" << std::endl;
    }   ///end_destructor
    
    inline double ExchangeabilityUpdater::calcLogPrior(int & checklist) {  ///begin_calcLogPrior
        if (checklist & Model::Exchangeabilities)
            return 0.0;
        checklist |= Model::Exchangeabilities;
        
        return DirichletUpdater::calcLogPrior(checklist);
    }  ///end_calcLogPrior
    
    inline void ExchangeabilityUpdater::pullFromModel() {  ///begin_pullFromModel
        QMatrix::freq_xchg_ptr_t xchg = _qmatrix->getExchangeabilitiesSharedPtr();
        _curr_point.assign(xchg->begin(), xchg->end());
    }   ///end_pullFromModel
    
    inline void ExchangeabilityUpdater::pushToModel() {  ///begin_pushToModel
        _qmatrix->setExchangeabilities(_curr_point);
    }   ///end_pushToModel

}   ///end
