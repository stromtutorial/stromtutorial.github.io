#pragma once

#include <algorithm>
#include <vector>
#include <Eigen/Dense>
#include "genetic_code.hpp"
#include "xstrom.hpp"

namespace strom {

    class QMatrix {

        public:
            typedef std::vector<double>             freq_xchg_t;
            typedef std::shared_ptr<freq_xchg_t>    freq_xchg_ptr_t;
            typedef double                          omega_t;
            typedef std::shared_ptr<omega_t>        omega_ptr_t;
            typedef boost::shared_ptr<QMatrix>      SharedPtr;

                                                    QMatrix();
            virtual                                 ~QMatrix();
        
            virtual void                            clear() = 0;

            virtual void                            setEqualStateFreqs(freq_xchg_ptr_t freq_ptr) = 0;
            virtual void                            setStateFreqsSharedPtr(freq_xchg_ptr_t freq_ptr) = 0;
            virtual void                            setStateFreqs(freq_xchg_t & freq) = 0;
            virtual freq_xchg_ptr_t                 getStateFreqsSharedPtr() = 0;
            virtual const double *                  getStateFreqs() const = 0;
            void                                    fixStateFreqs(bool is_fixed);
            bool                                    isFixedStateFreqs() const;

            virtual void                            setEqualExchangeabilities(freq_xchg_ptr_t xchg_ptr) = 0;
            virtual void                            setExchangeabilitiesSharedPtr(freq_xchg_ptr_t xchg) = 0;
            virtual void                            setExchangeabilities(freq_xchg_t & xchg) = 0;
            virtual freq_xchg_ptr_t                 getExchangeabilitiesSharedPtr() = 0;
            virtual const double *                  getExchangeabilities() const = 0;
            void                                    fixExchangeabilities(bool is_fixed);
            bool                                    isFixedExchangeabilities() const;

            virtual void                            setOmegaSharedPtr(omega_ptr_t omega) = 0;
            virtual void                            setOmega(omega_t omega) = 0;
            virtual omega_ptr_t                     getOmegaSharedPtr() = 0;
            virtual double                          getOmega() const = 0;
            void                                    fixOmega(bool is_fixed);
            bool                                    isFixedOmega() const;

            virtual const double *                  getEigenvectors() const = 0;
            virtual const double *                  getInverseEigenvectors() const = 0;
            virtual const double *                  getEigenvalues() const = 0;

            void                                    setActive(bool activate);
            
            virtual unsigned                        getNumStates() const = 0;
            virtual unsigned                        getNumExchangeabilities() const = 0;
        
        protected:
        
            virtual void                            recalcRateMatrix() = 0;
            void                                    normalizeFreqsOrExchangeabilities(freq_xchg_ptr_t v);

            unsigned                                _nstates;
            bool                                    _is_active;
            bool                                    _state_freqs_fixed;
            bool                                    _exchangeabilities_fixed;
            bool                                    _omega_fixed;
    };
    
    inline QMatrix::QMatrix() {
        //std::cout << "Creating a QMatrix object" << std::endl;
    }
    
    inline QMatrix::~QMatrix() {
        //std::cout << "Destroying a QMatrix object" << std::endl;
    }
    
    inline void QMatrix::setActive(bool activate) {
        _is_active = activate;
        recalcRateMatrix();
    }

    inline void QMatrix::clear() {
        _nstates = 4;
        _is_active = false;
        _state_freqs_fixed = false;
        _exchangeabilities_fixed = false;
        _omega_fixed = false;
    }

    inline void QMatrix::fixStateFreqs(bool is_fixed) {
        _state_freqs_fixed = is_fixed;
    }
    
    inline void QMatrix::fixExchangeabilities(bool is_fixed) {
        _exchangeabilities_fixed = is_fixed;
    }
    
    inline void QMatrix::fixOmega(bool is_fixed) {
        _omega_fixed = is_fixed;
    }
    
    inline bool QMatrix::isFixedStateFreqs() const {
        return _state_freqs_fixed;
    }
    
    inline bool QMatrix::isFixedExchangeabilities() const {
        return _exchangeabilities_fixed;
    }
    
    inline bool QMatrix::isFixedOmega() const {
        return _omega_fixed;
    }
    
    inline void QMatrix::normalizeFreqsOrExchangeabilities(QMatrix::freq_xchg_ptr_t v) {
        // Be sure elements of v sum to 1.0 and assert that they are all positive
        double sum_v = std::accumulate(v->begin(), v->end(), 0.0);
        for (auto & x : *v) {
            assert(x > 0.0);
            x /= sum_v;
        }
    }
    
    //******************************************************************************
    //******************************************************************************
    //*****************************                     ****************************
    //*****************************  QMatrixNucleotide  ****************************
    //*****************************                     ****************************
    //******************************************************************************
    //******************************************************************************
    
    class QMatrixNucleotide : public QMatrix {

        public:
            typedef Eigen::Matrix<double, 4, 4, Eigen::RowMajor>    eigenMatrix4d_t;
            typedef Eigen::Vector4d                                 eigenVector4d_t;
        
                                        QMatrixNucleotide();
                                        ~QMatrixNucleotide();
        
            void                        clear();

            void                        setEqualStateFreqs(freq_xchg_ptr_t freq_ptr);
            void                        setStateFreqsSharedPtr(freq_xchg_ptr_t freq_ptr);
            void                        setStateFreqs(freq_xchg_t & freqs);
            freq_xchg_ptr_t             getStateFreqsSharedPtr();
            const double *              getStateFreqs() const;

            void                        setEqualExchangeabilities(freq_xchg_ptr_t xchg_ptr);
            void                        setExchangeabilitiesSharedPtr(freq_xchg_ptr_t xchg_ptr);
            void                        setExchangeabilities(freq_xchg_t & xchg);
            freq_xchg_ptr_t             getExchangeabilitiesSharedPtr();
            const double *              getExchangeabilities() const;

            void                        setOmegaSharedPtr(omega_ptr_t omega_ptr);
            void                        setOmega(omega_t omega);
            omega_ptr_t                 getOmegaSharedPtr();
            double                      getOmega() const;

            const double *              getEigenvectors() const;
            const double *              getInverseEigenvectors() const;
            const double *              getEigenvalues() const;
        
            unsigned                    getNumStates() const;
            unsigned                    getNumExchangeabilities() const;

            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        
        protected:
        
            virtual void                recalcRateMatrix();

        private:
        
            // workspaces for computing eigenvectors/eigenvalues
            eigenMatrix4d_t             _sqrtPi;
            eigenMatrix4d_t             _sqrtPiInv;
            eigenMatrix4d_t             _Q;
            eigenMatrix4d_t             _eigenvectors;
            eigenMatrix4d_t             _inverse_eigenvectors;
            eigenVector4d_t             _eigenvalues;

            freq_xchg_ptr_t             _state_freqs;
            freq_xchg_ptr_t             _exchangeabilities;
    };
    
    inline QMatrixNucleotide::QMatrixNucleotide() {
        //std::cout << "Constructing a QMatrixNucleotide object" << std::endl;
        clear();
    }

    inline QMatrixNucleotide::~QMatrixNucleotide() {
        //std::cout << "Destroying a QMatrixNucleotide object" << std::endl;
    }

    inline void QMatrixNucleotide::clear() {
        QMatrix::clear();

        _nstates = 4;

        QMatrix::freq_xchg_t xchg = {1,1,1,1,1,1};
        _exchangeabilities = std::make_shared<QMatrix::freq_xchg_t>(xchg);

        QMatrix::freq_xchg_t freq_vect = {0.25, 0.25, 0.25, 0.25};
        _state_freqs = std::make_shared<QMatrix::freq_xchg_t>(freq_vect);
        
        recalcRateMatrix();
    }

    inline unsigned QMatrixNucleotide::getNumStates() const {
        return 4;
    }

    inline unsigned QMatrixNucleotide::getNumExchangeabilities() const {
        return 6;
    }

    inline void QMatrixNucleotide::setEqualStateFreqs(QMatrix::freq_xchg_ptr_t freq_ptr) {
        _state_freqs = freq_ptr;
        _state_freqs->assign(4, 0.25);
        recalcRateMatrix();
    }
    
    inline void QMatrixNucleotide::setStateFreqsSharedPtr(QMatrix::freq_xchg_ptr_t freq_ptr) {
        if (freq_ptr->size() != 4)
            throw XStrom(boost::format("Expecting 4 state frequencies and got %d: perhaps you meant to specify a subset data type other than nucleotide") % freq_ptr->size());
        double sum_of_freqs = std::accumulate(freq_ptr->begin(), freq_ptr->end(), 0.0);
        if (std::fabs(sum_of_freqs - 1.0) > 0.001)
            throw XStrom(boost::format("Expecting sum of 4 state frequencies to be 1, but instead got %g") % sum_of_freqs);
        _state_freqs = freq_ptr;
        recalcRateMatrix();
    }
    
    inline void QMatrixNucleotide::setStateFreqs(QMatrix::freq_xchg_t & freqs) {
        if (freqs.size() != 4)
            throw XStrom(boost::format("Expecting 4 state frequencies and got %d: perhaps you meant to specify a subset data type other than nucleotide") % freqs.size());
        std::copy(freqs.begin(), freqs.end(), _state_freqs->begin());
        recalcRateMatrix();
    }
    
    inline QMatrix::freq_xchg_ptr_t QMatrixNucleotide::getStateFreqsSharedPtr() {
        return _state_freqs;
    }

    inline const double * QMatrixNucleotide::getStateFreqs() const {
        return &(*_state_freqs)[0];
    }

    inline void QMatrixNucleotide::setEqualExchangeabilities(QMatrix::freq_xchg_ptr_t xchg_ptr) {
        _exchangeabilities = xchg_ptr;
        _exchangeabilities->assign(6, 1.0/6.0);
        recalcRateMatrix();
    }
    
    inline void QMatrixNucleotide::setExchangeabilitiesSharedPtr(QMatrix::freq_xchg_ptr_t xchg_ptr) {
        if (xchg_ptr->size() != 6)
            throw XStrom(boost::format("Expecting 6 exchangeabilities and got %d: perhaps you meant to specify a subset data type other than nucleotide") % xchg_ptr->size());
        _exchangeabilities = xchg_ptr;
        normalizeFreqsOrExchangeabilities(_exchangeabilities);
        recalcRateMatrix();
    }
            
    inline void QMatrixNucleotide::setExchangeabilities(QMatrix::freq_xchg_t & xchg) {
        if (xchg.size() != 6)
            throw XStrom(boost::format("Expecting 6 exchangeabilities and got %d: perhaps you meant to specify a subset data type other than nucleotide") % xchg.size());
        std::copy(xchg.begin(), xchg.end(), _exchangeabilities->begin());
        recalcRateMatrix();
    }
    
    inline QMatrix::freq_xchg_ptr_t QMatrixNucleotide::getExchangeabilitiesSharedPtr() {
        return _exchangeabilities;
    }
    
    inline const double * QMatrixNucleotide::getExchangeabilities() const {
        return &(*_exchangeabilities)[0];
    }

    inline void QMatrixNucleotide::setOmegaSharedPtr(QMatrix::omega_ptr_t omega_ptr) {
        assert(false);
    }

    inline void QMatrixNucleotide::setOmega(QMatrix::omega_t omega) {
        assert(false);
    }

    inline QMatrix::omega_ptr_t QMatrixNucleotide::getOmegaSharedPtr() {
        assert(false);
        return nullptr;
    }
    
    inline double QMatrixNucleotide::getOmega() const {
        assert(false);
        return 0.0;
    }

    inline const double * QMatrixNucleotide::getEigenvectors() const {
        return _eigenvectors.data();
    }
    
    inline const double * QMatrixNucleotide::getInverseEigenvectors() const {
        return _inverse_eigenvectors.data();
    }
    
    inline const double * QMatrixNucleotide::getEigenvalues() const {
        return _eigenvalues.data();
    }
    
    inline void QMatrixNucleotide::recalcRateMatrix() {
        // Must have assigned both _state_freqs and _exchangeabilities to recalculate rate matrix
        if (!_is_active || !(_state_freqs && _exchangeabilities))
            return;
        
        double piA = (*_state_freqs)[0];
        double piC = (*_state_freqs)[1];
        double piG = (*_state_freqs)[2];
        double piT = (*_state_freqs)[3];
        
        Eigen::Map<const Eigen::Array4d> tmp(_state_freqs->data());
        _sqrtPi = tmp.sqrt().matrix().asDiagonal();
        _sqrtPiInv = _sqrtPi.inverse();

        assert(_exchangeabilities->size() == 6);
        double rAC = (*_exchangeabilities)[0];
        double rAG = (*_exchangeabilities)[1];
        double rAT = (*_exchangeabilities)[2];
        double rCG = (*_exchangeabilities)[3];
        double rCT = (*_exchangeabilities)[4];
        double rGT = (*_exchangeabilities)[5];

        double inverse_scaling_factor = piA*(rAC*piC + rAG*piG + rAT*piT) + piC*(rAC*piA + rCG*piG + rCT*piT) + piG*(rAG*piA + rCG*piC + rGT*piT) + piT*(rAT*piA + rCT*piC + rGT*piG);
        double scaling_factor = 1.0/inverse_scaling_factor;

        _Q(0,0) = -scaling_factor*(rAC*piC + rAG*piG + rAT*piT);
        _Q(0,1) = scaling_factor*rAC*piC;
        _Q(0,2) = scaling_factor*rAG*piG;
        _Q(0,3) = scaling_factor*rAT*piT;

        _Q(1,0) = scaling_factor*rAC*piA;
        _Q(1,1) = -scaling_factor*(rAC*piA + rCG*piG + rCT*piT);
        _Q(1,2) = scaling_factor*rCG*piG;
        _Q(1,3) = scaling_factor*rCT*piT;

        _Q(2,0) = scaling_factor*rAG*piA;
        _Q(2,1) = scaling_factor*rCG*piC;
        _Q(2,2) = -scaling_factor*(rAG*piA + rCG*piC + rGT*piT);
        _Q(2,3) = scaling_factor*rGT*piT;

        _Q(3,0) = scaling_factor*rAT*piA;
        _Q(3,1) = scaling_factor*rCT*piC;
        _Q(3,2) = scaling_factor*rGT*piG;
        _Q(3,3) = -scaling_factor*(rAT*piA + rCT*piC + rGT*piG);

        // S is a symmetric matrix
        eigenMatrix4d_t S = eigenMatrix4d_t(_sqrtPi*_Q*_sqrtPiInv);

        // Can use efficient eigensystem solver because S is symmetric
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix4d> solver(S);
        if (solver.info() != Eigen::Success) {
            throw XStrom("Error in the calculation of eigenvectors and eigenvalues of the GTR rate matrix");
        }

        _eigenvectors           = _sqrtPiInv*solver.eigenvectors();
        _inverse_eigenvectors   = solver.eigenvectors().transpose()*_sqrtPi;
        _eigenvalues            = solver.eigenvalues();
    }
  
    //******************************************************************************
    //******************************************************************************
    //******************************                   *****************************
    //******************************  QMatrixStandard  *****************************
    //******************************                   *****************************
    //******************************************************************************
    //******************************************************************************
    
    class QMatrixStandard : public QMatrix {

        public:
            typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>      eigenMatrixXd_t;
            typedef Eigen::VectorXd                                                             eigenVectorXd_t;
        
                                        QMatrixStandard();
                                        ~QMatrixStandard();
        
            void                        clear();

            void                        setEqualStateFreqs(freq_xchg_ptr_t freq_ptr);
            void                        setStateFreqsSharedPtr(freq_xchg_ptr_t freq_ptr);
            void                        setStateFreqs(freq_xchg_t & freq);
            freq_xchg_ptr_t             getStateFreqsSharedPtr();
            const double *              getStateFreqs() const;
            
            void                        setEqualExchangeabilities(freq_xchg_ptr_t xchg_ptr);
            void                        setExchangeabilitiesSharedPtr(freq_xchg_ptr_t xchg_ptr);
            void                        setExchangeabilities(freq_xchg_t & xchg);
            freq_xchg_ptr_t             getExchangeabilitiesSharedPtr();
            const double *              getExchangeabilities() const;

            void                        setOmegaSharedPtr(omega_ptr_t omega);
            void                        setOmega(omega_t omega);
            omega_ptr_t                 getOmegaSharedPtr();
            double                      getOmega() const;

            const double *              getEigenvectors() const;
            const double *              getInverseEigenvectors() const;
            const double *              getEigenvalues() const;
        
            unsigned                    getNumStates() const;
            unsigned                    getNumExchangeabilities() const;

        protected:
        
            virtual void                recalcRateMatrix();

        private:
        
            // workspaces for computing eigenvectors/eigenvalues
            eigenMatrixXd_t             _sqrtPi;
            eigenMatrixXd_t             _sqrtPiInv;
            eigenMatrixXd_t             _Q;
            eigenMatrixXd_t             _eigenvectors;
            eigenMatrixXd_t             _inverse_eigenvectors;
            eigenVectorXd_t             _eigenvalues;

            freq_xchg_ptr_t             _state_freqs;
            freq_xchg_ptr_t             _exchangeabilities;
    };

    inline QMatrixStandard::QMatrixStandard() {
        //std::cout << "Constructing a QMatrixStandard object" << std::endl;
        clear();
    }

    inline QMatrixStandard::~QMatrixStandard() {
        //std::cout << "Destroying a QMatrixStandard object" << std::endl;
    }

    inline void QMatrixStandard::clear() {
        QMatrix::clear();
        
        _nstates = 2;               //TODO relax this to allow any number of states
        
        // state frequencies in this model are completely determined by the
        // relative rates (exchangeabilities)
        _state_freqs_fixed = true;
        
        // ---    r1   r2   r3   r4
        //  r5   ---   r6   r7   r8
        //  r9   r10  ---  r11  r12
        // r13   r14  r15  ---  r16
        // r17   r18  r19  r20  ---
        //
        // nstates = 5
        // number of rates = nstates*(nstates - 1) = 5*4 = 20
        
        unsigned nrates = _nstates*(_nstates - 1);
        QMatrix::freq_xchg_t xchg(nrates, 1.);
        _exchangeabilities = std::make_shared<QMatrix::freq_xchg_t>(xchg);

        QMatrix::freq_xchg_t freq_vect(_nstates, 1./_nstates);
        _state_freqs = std::make_shared<QMatrix::freq_xchg_t>(freq_vect);
        
        _Q.resize(_nstates, _nstates);
        _eigenvectors.resize(_nstates, _nstates);
        _inverse_eigenvectors.resize(_nstates, _nstates);
        _eigenvalues.resize(_nstates);
        
        recalcRateMatrix();
    }
    
    inline unsigned QMatrixStandard::getNumStates() const {
        return _nstates;
    }

    inline unsigned QMatrixStandard::getNumExchangeabilities() const {
        return _nstates*(_nstates-1);
    }

    inline void QMatrixStandard::setEqualStateFreqs(QMatrix::freq_xchg_ptr_t freq_ptr) {
        assert(false);
    }
    
    inline void QMatrixStandard::setStateFreqsSharedPtr(QMatrix::freq_xchg_ptr_t freq_ptr) {
        assert(false);
    }

    inline void QMatrixStandard::setStateFreqs(QMatrix::freq_xchg_t & freqs) {
        assert(false);
    }

    inline QMatrix::freq_xchg_ptr_t QMatrixStandard::getStateFreqsSharedPtr() {
        return _state_freqs;
    }

    inline const double * QMatrixStandard::getStateFreqs() const {
        return &(*_state_freqs)[0];
    }

    inline void QMatrixStandard::setEqualExchangeabilities(QMatrix::freq_xchg_ptr_t xchg_ptr) {
        _exchangeabilities = xchg_ptr;
        unsigned nrates = _nstates*(_nstates - 1);
        _exchangeabilities->assign(nrates, 1.);
        recalcRateMatrix();
    }
    
    inline void QMatrixStandard::setExchangeabilitiesSharedPtr(QMatrix::freq_xchg_ptr_t xchg_ptr) {
        unsigned nrates = _nstates*(_nstates - 1);
        if (xchg_ptr->size() != nrates)
            throw XStrom(boost::format("Expecting %d rates and got %d") % nrates % xchg_ptr->size());
        _exchangeabilities = xchg_ptr;
        recalcRateMatrix();
    }
    
    inline void QMatrixStandard::setExchangeabilities(QMatrix::freq_xchg_t & xchg) {
        unsigned nrates = _nstates*(_nstates - 1);
        if (xchg.size() != nrates)
            throw XStrom(boost::format("Expecting %d rates and got %d") % nrates % xchg.size());
        std::copy(xchg.begin(), xchg.end(), _exchangeabilities->begin());
        recalcRateMatrix();
    }
    
    inline QMatrix::freq_xchg_ptr_t QMatrixStandard::getExchangeabilitiesSharedPtr() {
        return _exchangeabilities;
    }
    
    inline const double * QMatrixStandard::getExchangeabilities() const {
        return &(*_exchangeabilities)[0];
    }
    
    inline void QMatrixStandard::setOmegaSharedPtr(QMatrix::omega_ptr_t omega_ptr) {
        assert(false);
    }

    inline void QMatrixStandard::setOmega(QMatrix::omega_t omega) {
        assert(false);
    }

    inline QMatrix::omega_ptr_t QMatrixStandard::getOmegaSharedPtr() {
        assert(false);
        return nullptr;
    }
    
    inline double QMatrixStandard::getOmega() const {
        assert(false);
        return 0.0;
    }

    inline const double * QMatrixStandard::getEigenvectors() const {
        return _eigenvectors.data();
    }
    
    inline const double * QMatrixStandard::getInverseEigenvectors() const {
        return _inverse_eigenvectors.data();
    }
    
    inline const double * QMatrixStandard::getEigenvalues() const {
        return _eigenvalues.data();
    }
    
    inline void QMatrixStandard::recalcRateMatrix() {
        unsigned nrates = _nstates*(_nstates - 1);
        assert(_exchangeabilities->size() == nrates);
        assert(_state_freqs->size() == _nstates);
        const double * r = getExchangeabilities();
        
        // Calculate (unscaled) instantaneous rate matrix
        _Q = Eigen::MatrixXd::Zero(_nstates,_nstates);
        unsigned k = 0;
        for (unsigned i = 0; i < _nstates; i++) {
            double row_sum = 0.0;
            for (unsigned j = 0; j < _nstates; j++) {
                if (i != j) {
                    row_sum += r[k];
                    _Q(i,j) = r[k++];
                }
            _Q(i,i) = -row_sum;
            }
        }
        
        // Calculate equilibrium frequencies
        //
        // Definition of equilibrium frequencies:
        //
        //    P^T pi = pi
        //
        // where P is the transition probability matrix (over any time period, including
        // infinitesimal dt), P^T is P transposed, and pi is the stationary distribution.
        //
        // For 2 states, this becomes
        //
        //  /            \/     \   /                   \   /     \
        //  | P00   P10  || pi0 |   | pi0 P00 + pi1 P10 |   | pi0 |
        //  |            ||     | = |                   | = |     |
        //  | P01   P11  || pi1 |   | pi0 P01 + pi1 P11 |   | pi1 |
        //  \            /\     /   \                   /   \     /
        //        P^T       pi
        //
        // Noting that P(dt) = I + Q dt, we can write
        //
        //  (I + Q^T dt) pi = I pi + Q^T pi dt
        //
        // The right-hand side equals pi iff Q^T pi = zero vector.
        //
        // Let Q equal this 3x3 matrix:
        //
        //     /                 \        /                 \
        //     | -a-b   a    b   |        | -a-b   c    e   |
        //     |                 |        |                 |
        // Q = |   c  -c-d   d   |  Q^T = |   a  -c-d   f   |
        //     |                 |        |                 |
        //     |   e    f  -e-f  |        |   b    e  -e-f  |
        //     \                 /        \                 /
        //
        // The linear equation we need to solve is this:
        //
        //  /              \ /     \   /   \
        //  | -a-b  c   e  | | pi1 |   | 0 |
        //  |              | |     |   |   |
        //  |  a  -c-d  f  | | pi2 | = | 0 |
        //  |              | |     |   |   |
        //  |  b   d  -e-f | | pi3 |   | 0 |
        //  \              / \     /   \   /
        //
        // This does not really represent 3 equations/3 unknowns because
        // of the fact that the pi elements sum to 1. The reduced (2 dimensional)
        // linear equation is thus (subtracting the third element, arbitrarily, in
        // each row from each of the other elements and from the corresponding row
        // on the right side):
        //
        //  /              \ /     \   /    \
        //  | -a-b-e  c-e  | | pi1 |   | -e |
        //  |              | |     |   |    |
        //  |  a-f  -c-d-f | | pi2 | = | -f |
        //  \              / \     /   \    /
        //
        //
        // Construct reduced matrix A and vector b
        Eigen::MatrixXd A(_nstates-1, _nstates-1);
        Eigen::VectorXd b(_nstates-1);
        for (unsigned i = 0; i < _nstates-1; i++) {
            b(i) = -_Q(_nstates-1,i);
            for (unsigned j = 0; j < _nstates-1; j++) {
                A(i,j) = _Q(j,i) - _Q(_nstates-1,i);
            }
        }
        Eigen::VectorXd f(_nstates-1);
        f = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
        
        // Compute vector of equilibrium frequencies pi
        for (unsigned i = 0; i < _nstates-1; i++) {
            (*_state_freqs)[i] = f(i);
        }
        (*_state_freqs)[_nstates-1] = 1.0 - f.sum();
        
        double average_rate = 0.0;
        for (unsigned i = 0; i < nrates; i++)
            average_rate -= (*_state_freqs)[i]*_Q(i,i);
        double scaling_factor = 1.0/average_rate;

        _Q *= scaling_factor;

        // S is a symmetric matrix
        Eigen::Map<const Eigen::ArrayXd> tmp(_state_freqs->data(), _nstates);
        eigenMatrixXd_t sqrtPi = tmp.sqrt().matrix().asDiagonal();
        eigenMatrixXd_t sqrtPiInv = sqrtPi.inverse();
        eigenMatrixXd_t S = eigenMatrixXd_t(sqrtPi*_Q*sqrtPiInv);

        // Can use efficient eigensystem solver because S is symmetric
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(S);
        if (solver.info() != Eigen::Success) {
            throw XStrom("Error in the calculation of eigenvectors and eigenvalues of the codon model rate matrix");
        }

        _eigenvectors           = sqrtPiInv*solver.eigenvectors();
        _inverse_eigenvectors   = solver.eigenvectors().transpose()*sqrtPi;
        _eigenvalues            = solver.eigenvalues();
    }
    
    //******************************************************************************
    //******************************************************************************
    //******************************                   *****************************
    //******************************   QMatrixCodon    *****************************
    //******************************                   *****************************
    //******************************************************************************
    //******************************************************************************
    
    class QMatrixCodon : public QMatrix {

        public:
            typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>      eigenMatrixXd_t;
            typedef Eigen::VectorXd                                                             eigenVectorXd_t;
        
                                        QMatrixCodon(GeneticCode::SharedPtr gcode);
                                        ~QMatrixCodon();
        
            void                        clear();

            void                        setEqualStateFreqs(freq_xchg_ptr_t freq_ptr);
            void                        setStateFreqsSharedPtr(freq_xchg_ptr_t freq_ptr);
            void                        setStateFreqs(freq_xchg_t & freqs);
            freq_xchg_ptr_t             getStateFreqsSharedPtr();
            const double *              getStateFreqs() const;

            void                        setEqualExchangeabilities(freq_xchg_ptr_t xchg_ptr);
            void                        setExchangeabilitiesSharedPtr(freq_xchg_ptr_t xchg_ptr);
            void                        setExchangeabilities(freq_xchg_t & xchg);
            freq_xchg_ptr_t             getExchangeabilitiesSharedPtr();
            const double *              getExchangeabilities() const;

            void                        setOmegaSharedPtr(omega_ptr_t omega_ptr);
            void                        setOmega(omega_t omega);
            omega_ptr_t                 getOmegaSharedPtr();
            double                      getOmega() const;

            const double *              getEigenvectors() const;
            const double *              getInverseEigenvectors() const;
            const double *              getEigenvalues() const;
        
            unsigned                    getNumStates() const;
            unsigned                    getNumExchangeabilities() const;

        protected:
        
            virtual void                recalcRateMatrix();

        private:
        
            // workspaces for computing eigenvectors/eigenvalues
            eigenMatrixXd_t             _sqrtPi;
            eigenMatrixXd_t             _sqrtPiInv;
            eigenMatrixXd_t             _Q;
            eigenMatrixXd_t             _eigenvectors;
            eigenMatrixXd_t             _inverse_eigenvectors;
            eigenVectorXd_t             _eigenvalues;

            freq_xchg_ptr_t             _state_freqs;
            omega_ptr_t                 _omega;

            std::vector<std::string>    _codons;
            std::vector<unsigned>       _amino_acids;
        
            GeneticCode::SharedPtr      _genetic_code;
    };

    inline QMatrixCodon::QMatrixCodon(GeneticCode::SharedPtr gcode) {
        //std::cout << "Constructing a QMatrixCodon object" << std::endl;
        assert(gcode);
        _genetic_code = gcode;
        clear();
    }

    inline QMatrixCodon::~QMatrixCodon() {
        //std::cout << "Destroying a QMatrixCodon object" << std::endl;
    }

    inline void QMatrixCodon::clear() {
        QMatrix::clear();

        _nstates = 61;
        
        unsigned nstates = _genetic_code->getNumNonStopCodons();
        _genetic_code->copyCodons(_codons);
        _genetic_code->copyAminoAcids(_amino_acids);
        
        QMatrix::omega_t omega = 0.1;
        _omega = std::make_shared<QMatrix::omega_t>(omega);
        
        QMatrix::freq_xchg_t freq_vect(nstates, 1./nstates);
        _state_freqs = std::make_shared<QMatrix::freq_xchg_t>(freq_vect);
        
        _sqrtPi.resize(nstates, nstates);
        _sqrtPiInv.resize(nstates, nstates);
        _Q.resize(nstates, nstates);
        _eigenvectors.resize(nstates, nstates);
        _inverse_eigenvectors.resize(nstates, nstates);
        _eigenvalues.resize(nstates);
        
        recalcRateMatrix();
    }

    inline unsigned QMatrixCodon::getNumStates() const {
        return 61;
    }

    inline unsigned QMatrixCodon::getNumExchangeabilities() const {
        assert(false);
        return 0;
    }

    inline QMatrix::freq_xchg_ptr_t QMatrixCodon::getExchangeabilitiesSharedPtr() {
        assert(false);
        return nullptr;
    }
    
    inline QMatrix::freq_xchg_ptr_t QMatrixCodon::getStateFreqsSharedPtr() {
        return _state_freqs;
    }

    inline QMatrix::omega_ptr_t QMatrixCodon::getOmegaSharedPtr() {
        return _omega;
    }
    
    inline const double * QMatrixCodon::getEigenvectors() const {
        return _eigenvectors.data();
    }
    
    inline const double * QMatrixCodon::getInverseEigenvectors() const {
        return _inverse_eigenvectors.data();
    }
    
    inline const double * QMatrixCodon::getEigenvalues() const {
        return _eigenvalues.data();
    }
    
    inline const double * QMatrixCodon::getExchangeabilities() const {
        assert(false);
        return 0;
    }

    inline const double * QMatrixCodon::getStateFreqs() const {
        return &(*_state_freqs)[0];
    }

    inline double QMatrixCodon::getOmega() const {
        return *_omega;
    }

    inline void QMatrixCodon::setEqualExchangeabilities(QMatrix::freq_xchg_ptr_t xchg_ptr) {
        assert(false);
    }
    
    inline void QMatrixCodon::setExchangeabilitiesSharedPtr(QMatrix::freq_xchg_ptr_t xchg_ptr) {
        assert(false);
    }
    
    inline void QMatrixCodon::setExchangeabilities(QMatrix::freq_xchg_t & xchg) {
        assert(false);
    }
    
    inline void QMatrixCodon::setEqualStateFreqs(QMatrix::freq_xchg_ptr_t freq_ptr) {
        _state_freqs = freq_ptr;
        unsigned nstates = _genetic_code->getNumNonStopCodons();
        _state_freqs->assign(nstates, 1./nstates);
        recalcRateMatrix();
    }
    
    inline void QMatrixCodon::setStateFreqsSharedPtr(QMatrix::freq_xchg_ptr_t freq_ptr) {
        unsigned nstates = _genetic_code->getNumNonStopCodons();
        if (freq_ptr->size() != nstates)
            throw XStrom(boost::format("Expecting %d state frequencies and got %d: perhaps you meant to specify a subset data type other than codon") % nstates % freq_ptr->size());
        double sum_of_freqs = std::accumulate(freq_ptr->begin(), freq_ptr->end(), 0.0);
        if (std::fabs(sum_of_freqs - 1.0) > 0.001)
            throw XStrom(boost::format("Expecting sum of codon frequencies to be 1, but instead got %g") % sum_of_freqs);
        _state_freqs = freq_ptr;
        normalizeFreqsOrExchangeabilities(_state_freqs);
        recalcRateMatrix();
    }
    
    inline void QMatrixCodon::setStateFreqs(QMatrix::freq_xchg_t & freqs) {
        unsigned nstates = _genetic_code->getNumNonStopCodons();
        if (freqs.size() != nstates)
            throw XStrom(boost::format("Expecting %d state frequencies and got %d: perhaps you meant to specify a subset data type other than codon") % nstates % freqs.size());
        std::copy(freqs.begin(), freqs.end(), _state_freqs->begin());
        recalcRateMatrix();
    }
    
    inline void QMatrixCodon::setOmegaSharedPtr(QMatrix::omega_ptr_t omega_ptr) {
        _omega = omega_ptr;
        recalcRateMatrix();
    }

    inline void QMatrixCodon::setOmega(QMatrix::omega_t omega) {
        *_omega = omega;
        recalcRateMatrix();
    }

    inline void QMatrixCodon::recalcRateMatrix() {
        // Must have assigned both _state_freqs and _omega to recalculate rate matrix
        if (!_is_active || !(_state_freqs && _omega))
            return;
        
        unsigned nstates = _genetic_code->getNumNonStopCodons();
        assert(_state_freqs->size() == nstates);
        const double * pi = getStateFreqs();
        double omega = getOmega();
        
        Eigen::Map<const Eigen::ArrayXd> tmp(_state_freqs->data(), nstates);
        _sqrtPi = tmp.sqrt().matrix().asDiagonal();
        _sqrtPiInv = _sqrtPi.inverse();

        // Calculate (unscaled) instantaneous rate matrix
        _Q = Eigen::MatrixXd::Zero(nstates,nstates);

        for (unsigned i = 0; i < nstates-1; i++) {
            for (unsigned j = i+1; j < nstates; j++) {
                unsigned diffs = 0;
                if (_codons[i][0] != _codons[j][0])
                    diffs++;
                if (_codons[i][1] != _codons[j][1])
                    diffs++;
                if (_codons[i][2] != _codons[j][2])
                    diffs++;
                if (diffs == 1) {
                    bool synonymous = _amino_acids[i] == _amino_acids[j];
                    _Q(i,j) = (synonymous ? 1.0 : omega)*pi[j];
                    _Q(j,i) = (synonymous ? 1.0 : omega)*pi[i];
                    _Q(i,i) -= _Q(i,j);
                    _Q(j,j) -= _Q(j,i);
                }
            }
        }

        double average_rate = 0.0;
        for (unsigned i = 0; i < nstates; i++)
            average_rate -= pi[i]*_Q(i,i);
        double scaling_factor = 3.0/average_rate;

        _Q *= scaling_factor;

        // S is a symmetric matrix
        eigenMatrixXd_t S = eigenMatrixXd_t(_sqrtPi*_Q*_sqrtPiInv);

        // Can use efficient eigensystem solver because S is symmetric
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(S);
        if (solver.info() != Eigen::Success) {
            throw XStrom("Error in the calculation of eigenvectors and eigenvalues of the codon model rate matrix");
        }

        _eigenvectors           = _sqrtPiInv*solver.eigenvectors();
        _inverse_eigenvectors   = solver.eigenvectors().transpose()*_sqrtPi;
        _eigenvalues            = solver.eigenvalues();
    }
    
} // namespace strom
