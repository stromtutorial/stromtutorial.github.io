#pragma once    ///start

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
        
        protected:
        
            virtual void                            recalcRateMatrix() = 0;
            void                                    normalizeFreqsOrExchangeabilities(freq_xchg_ptr_t v);

            bool                                    _is_active;
            bool                                    _state_freqs_fixed;
            bool                                    _exchangeabilities_fixed;
            bool                                    _omega_fixed;
    };
    
    // member function bodies go here
    ///end_class_declaration
    inline QMatrix::QMatrix() { ///begin_qmatrix_constructor
        //std::cout << "Creating a QMatrix object" << std::endl;
    } ///end_qmatrix_constructor
    
    inline QMatrix::~QMatrix() { ///begin_qmatrix_destructor
        //std::cout << "Destroying a QMatrix object" << std::endl;
    } ///end_qmatrix_destructor
    
    inline void QMatrix::setActive(bool activate) { ///begin_setActive
        _is_active = activate;
        recalcRateMatrix();
    } ///end_setActive

    inline void QMatrix::clear() { ///begin_clear
        _is_active = false;
        _state_freqs_fixed = false;
        _exchangeabilities_fixed = false;
        _omega_fixed = false;
    } ///end_clear

    inline void QMatrix::fixStateFreqs(bool is_fixed) { ///begin_fixStateFreqs
        _state_freqs_fixed = is_fixed;
    }   ///end_fixStateFreqs
    
    inline void QMatrix::fixExchangeabilities(bool is_fixed) {  ///begin_fixExchangeabilities
        _exchangeabilities_fixed = is_fixed;
    }   ///end_fixExchangeabilities
    
    inline void QMatrix::fixOmega(bool is_fixed) {  ///begin_fixOmega
        _omega_fixed = is_fixed;
    }   ///end_fixOmega

    inline bool QMatrix::isFixedStateFreqs() const { ///begin_isFixedStateFreqs
        return _state_freqs_fixed;
    }   ///end_isFixedStateFreqs
    
    inline bool QMatrix::isFixedExchangeabilities() const { ///begin_isFixedExchangeabilities
        return _exchangeabilities_fixed;
    }   ///end_isFixedExchangeabilities
    
    inline bool QMatrix::isFixedOmega() const { ///begin_isFixedOmega
        return _omega_fixed;
    }   ///end_isFixedOmega
    
    inline void QMatrix::normalizeFreqsOrExchangeabilities(QMatrix::freq_xchg_ptr_t v) {    ///begin_normalizeFreqsOrExchangeabilities
        // Be sure elements of v sum to 1.0 and assert that they are all positive
        double sum_v = std::accumulate(v->begin(), v->end(), 0.0);
        for (auto & x : *v) {
            assert(x > 0.0);
            x /= sum_v;
        }
    }   ///end_normalizeFreqsOrExchangeabilities

    class QMatrixNucleotide : public QMatrix { ///begin_QMatrixNucleotide 

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
    
    // QMatrixNucleotide member function bodies go here
    ///end_QMatrixNucleotide_class_declaration
    inline QMatrixNucleotide::QMatrixNucleotide() { ///begin_QMatrixNucleotide_constructor
        //std::cout << "Constructing a QMatrixNucleotide object" << std::endl;
        clear();
    } ///end_QMatrixNucleotide_constructor

    inline QMatrixNucleotide::~QMatrixNucleotide() { ///begin_QMatrixNucleotide_destructor
        //std::cout << "Destroying a QMatrixNucleotide object" << std::endl;
    } ///end_QMatrixNucleotide_destructor

    inline void QMatrixNucleotide::clear() { ///begin_QMatrixNucleotide_clear
        QMatrix::clear();

        QMatrix::freq_xchg_t xchg = {1,1,1,1,1,1};
        _exchangeabilities = std::make_shared<QMatrix::freq_xchg_t>(xchg);

        QMatrix::freq_xchg_t freq_vect = {0.25, 0.25, 0.25, 0.25};
        _state_freqs = std::make_shared<QMatrix::freq_xchg_t>(freq_vect);
        
        recalcRateMatrix();
    } ///end_QMatrixNucleotide_clear

    inline QMatrix::freq_xchg_ptr_t QMatrixNucleotide::getExchangeabilitiesSharedPtr() { ///begin_QMatrixNucleotide_getExchangeabilitiesSharedPtr
        return _exchangeabilities;
    } ///end_QMatrixNucleotide_getExchangeabilitiesSharedPtr
    
    inline QMatrix::freq_xchg_ptr_t QMatrixNucleotide::getStateFreqsSharedPtr() { ///begin_QMatrixNucleotide_getStateFreqsSharedPtr
        return _state_freqs;
    } ///end_QMatrixNucleotide_StateFreqsSharedPtr

    inline QMatrix::omega_ptr_t QMatrixNucleotide::getOmegaSharedPtr() { ///begin_QMatrixNucleotide_getOmegaSharedPtr
        assert(false);
        return nullptr;
    } ///end_QMatrixNucleotide_getOmegaSharedPtr
    
    inline const double * QMatrixNucleotide::getEigenvectors() const { ///begin_QMatrixNucleotide_getEigenvectors
        return _eigenvectors.data();
    } ///end_QMatrixNucleotide_getEigenvectors
    
    inline const double * QMatrixNucleotide::getInverseEigenvectors() const { ///begin_QMatrixNucleotide_getInverseEigenvectors
        return _inverse_eigenvectors.data();
    } ///end_QMatrixNucleotide_getInverseEigenvectors
    
    inline const double * QMatrixNucleotide::getEigenvalues() const { ///begin_QMatrixNucleotide_getEigenvalues
        return _eigenvalues.data();
    } ///end_QMatrixNucleotide_getEigenvalues
    
    inline const double * QMatrixNucleotide::getExchangeabilities() const { ///begin_QMatrixNucleotide_getExchangeabilities
        return &(*_exchangeabilities)[0];
    } ///end_QMatrixNucleotide_getExchangeabilities

    inline const double * QMatrixNucleotide::getStateFreqs() const { ///begin_QMatrixNucleotide_getStateFreqs
        return &(*_state_freqs)[0];
    } ///end_QMatrixNucleotide_getStateFreqs

    inline double QMatrixNucleotide::getOmega() const { ///begin_QMatrixNucleotide_getOmega
        assert(false);
        return 0.0;
    } ///end_QMatrixNucleotide_getOmega

    inline void QMatrixNucleotide::setEqualExchangeabilities(QMatrix::freq_xchg_ptr_t xchg_ptr) {   ///begin_QMatrixNucleotide_setEqualExchangeabilities
        _exchangeabilities = xchg_ptr;
        _exchangeabilities->assign(6, 1.0/6.0);
        recalcRateMatrix();
    } ///end_QMatrixNucleotide_setEqualExchangeabilities
    
    inline void QMatrixNucleotide::setExchangeabilitiesSharedPtr(QMatrix::freq_xchg_ptr_t xchg_ptr) { ///begin_QMatrixNucleotide_setExchangeabilitiesSharedPtr
        if (xchg_ptr->size() != 6)
            throw XStrom(boost::format("Expecting 6 exchangeabilities and got %d: perhaps you meant to specify a subset data type other than nucleotide") % xchg_ptr->size());
        _exchangeabilities = xchg_ptr;
        normalizeFreqsOrExchangeabilities(_exchangeabilities);
        recalcRateMatrix();
    } ///end_QMatrixNucleotide_setExchangeabilitiesSharedPtr
            
    inline void QMatrixNucleotide::setExchangeabilities(QMatrix::freq_xchg_t & xchg) { ///begin_QMatrixNucleotide_setExchangeabilities
        if (xchg.size() != 6)
            throw XStrom(boost::format("Expecting 6 exchangeabilities and got %d: perhaps you meant to specify a subset data type other than nucleotide") % xchg.size());
        std::copy(xchg.begin(), xchg.end(), _exchangeabilities->begin());
        recalcRateMatrix();
    } ///end_QMatrixNucleotide_setExchangeabilities
    
    inline void QMatrixNucleotide::setEqualStateFreqs(QMatrix::freq_xchg_ptr_t freq_ptr) { ///begin_QMatrixNucleotide_setEqualStateFreqs
        _state_freqs = freq_ptr;
        _state_freqs->assign(4, 0.25);
        recalcRateMatrix();
    } ///end_QMatrixNucleotide_setEqualStateFreqs
    
    inline void QMatrixNucleotide::setStateFreqsSharedPtr(QMatrix::freq_xchg_ptr_t freq_ptr) { ///begin_QMatrixNucleotide_setStateFreqsSharedPtr
        if (freq_ptr->size() != 4)
            throw XStrom(boost::format("Expecting 4 state frequencies and got %d: perhaps you meant to specify a subset data type other than nucleotide") % freq_ptr->size());
        double sum_of_freqs = std::accumulate(freq_ptr->begin(), freq_ptr->end(), 0.0);
        if (std::fabs(sum_of_freqs - 1.0) > 0.001)
            throw XStrom(boost::format("Expecting sum of 4 state frequencies to be 1, but instead got %g") % sum_of_freqs);
        _state_freqs = freq_ptr;
        recalcRateMatrix();
    } ///end_QMatrixNucleotide_setStateFreqsSharedPtr
    
    inline void QMatrixNucleotide::setStateFreqs(QMatrix::freq_xchg_t & freqs) { ///begin_QMatrixNucleotide_setStateFreqs
        if (freqs.size() != 4)
            throw XStrom(boost::format("Expecting 4 state frequencies and got %d: perhaps you meant to specify a subset data type other than nucleotide") % freqs.size());
        std::copy(freqs.begin(), freqs.end(), _state_freqs->begin());
        recalcRateMatrix();
    } ///end_QMatrixNucleotide_setStateFreqs
    
    inline void QMatrixNucleotide::setOmegaSharedPtr(QMatrix::omega_ptr_t omega_ptr) { ///begin_QMatrixNucleotide_setOmegaSharedPtr
        assert(false);
    } ///end_QMatrixNucleotide_setOmegaSharedPtr

    inline void QMatrixNucleotide::setOmega(QMatrix::omega_t omega) { ///begin_QMatrixNucleotide_setOmega
        assert(false);
    } ///end_QMatrixNucleotide_setOmega

    inline void QMatrixNucleotide::recalcRateMatrix() { ///begin_QMatrixNucleotide_recalcRateMatrix
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
    } ///end_QMatrixNucleotide_recalcRateMatrix
   
    class QMatrixCodon : public QMatrix { ///begin_QMatrixCodon

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
    ///end_QMatrixCodon_class_declaration
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
    }   ///end_QMatrixCodon_recalcRateMatrix

    // QMatrixNucleotide class goes here      ///begin_derived_class_placeholders
    // QMatrixCodon class goes here

}   ///end

#if 0
        // Debugging code to print out rate matrix for universal code
        // include boost format header at top:
        //    #include <boost/format.hpp>
        // insert just before this line in recalcRateMatrix:
        //    _Q *= scaling_factor;
        std::ofstream tmpf("qmatrixdump.txt");
        tmpf << boost::str(boost::format("scaling_factor = %g\n\n") % scaling_factor);
        tmpf << boost::str(boost::format("omega = %g\n\n") % omega);
        tmpf << "frequencies:\n";
        for (unsigned i = 0; i < 61; i++) {
            tmpf << boost::str(boost::format("%10.5lf ") % pi[i]);
        }
        tmpf << "\n\nQ:\n";
        std::string triplets[] = {
            "AAA",
            "AAC",
            "AAG",
            "AAT",
            "ACA",
            "ACC",
            "ACG",
            "ACT",
            "AGA",
            "AGC",
            "AGG",
            "AGT",
            "ATA",
            "ATC",
            "ATG",
            "ATT",
            "CAA",
            "CAC",
            "CAG",
            "CAT",
            "CCA",
            "CCC",
            "CCG",
            "CCT",
            "CGA",
            "CGC",
            "CGG",
            "CGT",
            "CTA",
            "CTC",
            "CTG",
            "CTT",
            "GAA",
            "GAC",
            "GAG",
            "GAT",
            "GCA",
            "GCC",
            "GCG",
            "GCT",
            "GGA",
            "GGC",
            "GGG",
            "GGT",
            "GTA",
            "GTC",
            "GTG",
            "GTT",
            //"TAA",
            "TAC",
            //"TAG",
            "TAT",
            "TCA",
            "TCC",
            "TCG",
            "TCT",
            //"TGA",
            "TGC",
            "TGG",
            "TGT",
            "TTA",
            "TTC",
            "TTG",
            "TTT"
        };
        std::string aminoacids[] = {
            "Lys", //AAA",
            "Asn", //AAC",
            "Lys", //"AAG",
            "Asn", //"AAT",
            "Thr", //"ACA",
            "Thr", //"ACC",
            "Thr", //"ACG",
            "Thr", //"ACT",
            "Arg", //"AGA",
            "Ser", //"AGC",
            "Arg", //"AGG",
            "Ser", //"AGT",
            "Ile", //"ATA",
            "Ile", //"ATC",
            "Met", //"ATG",
            "Ile", //"ATT",
            "Gln", //"CAA",
            "His", //"CAC",
            "Gln", //"CAG",
            "His", //"CAT",
            "Pro", //"CCA",
            "Pro", //"CCC",
            "Pro", //"CCG",
            "Pro", //"CCT",
            "Arg", //"CGA",
            "Arg", //"CGC",
            "Arg", //"CGG",
            "Arg", //"CGT",
            "Leu", //"CTA",
            "Leu", //"CTC",
            "Leu", //"CTG",
            "Leu", //"CTT",
            "Glu", //"GAA",
            "Asp", //"GAC",
            "Glu", //"GAG",
            "Asp", //"GAT",
            "Ala", //"GCA",
            "Ala", //"GCC",
            "Ala", //"GCG",
            "Ala", //"GCT",
            "Gly", //"GGA",
            "Gly", //"GGC",
            "Gly", //"GGG",
            "Gly", //"GGT",
            "Val", //"GTA",
            "Val", //"GTC",
            "Val", //"GTG",
            "Val", //"GTT",
            //Stop "TAA",
            "Tyr", //"TAC",
            //Stop "TAG",
            "Tyr", //"TAT",
            "Ser", //"TCA",
            "Ser", //"TCC",
            "Ser", //"TCG",
            "Ser", //"TCT",
            //Stop "TGA",
            "Cys", //"TGC",
            "Trp", //"TGG",
            "Cys", //"TGT",
            "Leu", //"TTA",
            "Phe", //"TTC",
            "Leu", //"TTG",
            "Phe" //"TTT"
        };
        
        tmpf << boost::str(boost::format("%10s %10s ") % " " % " ");
        for (unsigned i = 0; i < 61; i++) {
            tmpf << boost::str(boost::format("%10s ") % aminoacids[i]);
        }
        tmpf << "\n";
        
        tmpf << boost::str(boost::format("%10s %10s ") % " " % " ");
        for (unsigned i = 0; i < 61; i++) {
            tmpf << boost::str(boost::format("%10s ") % triplets[i]);
        }
        tmpf << "\n";
        
        for (unsigned i = 0; i < 61; i++) {
            tmpf << boost::str(boost::format("%10s %10s ") % aminoacids[i] % triplets[i]);
            for (unsigned j = 0; j < 61; j++) {
                tmpf << boost::str(boost::format("%10.5lf ") % _Q(i,j));
            }
            tmpf << "\n";
        }
        tmpf.close();
#endif
