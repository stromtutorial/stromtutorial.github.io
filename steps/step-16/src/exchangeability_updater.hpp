#pragma once    ///start

#include "dirichlet_updater.hpp"

namespace strom {
    
    class ExchangeabilityUpdater : public DirichletUpdater {
        public:
            typedef std::shared_ptr< ExchangeabilityUpdater > SharedPtr;

                                            ExchangeabilityUpdater(QMatrix::SharedPtr qmatrix);
                                            ~ExchangeabilityUpdater();
        
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
    }

    inline ExchangeabilityUpdater::~ExchangeabilityUpdater() {
        // std::cout << "Destroying an ExchangeabilityUpdater" << std::endl;
    }   ///end_destructor
    
    inline void ExchangeabilityUpdater::pullFromModel() {  ///begin_pullFromModel
        QMatrix::freq_xchg_ptr_t xchg = _qmatrix->getExchangeabilitiesSharedPtr();
        _curr_point.assign(xchg->begin(), xchg->end());
    }
    
    inline void ExchangeabilityUpdater::pushToModel() {
        _qmatrix->setExchangeabilities(_curr_point);
    }   ///end_pushToModel

}   ///end
