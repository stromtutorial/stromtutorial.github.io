#pragma once

//#include <stack>
//#include <memory>
//#include <iostream>
#include "tree.hpp"
#include "lot.hpp"

namespace strom {

    class PolytomyTopoPriorCalculator {
    
        public:
            typedef std::shared_ptr<PolytomyTopoPriorCalculator> SharedPtr;
                                            
                                            PolytomyTopoPriorCalculator();
                                            ~PolytomyTopoPriorCalculator();

            bool                            isResolutionClassPrior() const;
            bool                            isPolytomyPrior() const;

            bool                            isRooted() const;
            bool                            isUnrooted() const;

            void                            setNTax(unsigned n);
            unsigned                        getNTax() const;

            void                            chooseRooted();
            void                            chooseUnrooted();

            double                          getLogCount(unsigned n, unsigned m);
            double                          getLogSaturatedCount(unsigned n);
            double                          getLogTotalCount(unsigned n);
            std::vector<double>             getLogCounts();

            std::vector<double>             getCountsVect();
            std::vector<int>                getNFactorsVect();

            void                            chooseResolutionClassPrior();
            void                            choosePolytomyPrior();

            void                            setC(double c);
            double                          getC() const;

            void                            setLogScalingFactor(double lnf);
            double                          getLogScalingFactor() const;

            virtual double                  getLogTopoProb(Tree::SharedPtr t);

            double                          getLogTopologyPrior(unsigned m);
            double                          getLogNormalizedTopologyPrior(unsigned m);
            double                          getLogNormConstant();

            std::vector<double>             getTopoPriorVect();
            std::vector<double>             getRealizedResClassPriorsVect();

            unsigned                        sample(Lot::SharedPtr rng);

            void                            reset();

        private:

            void                            recalcCountsAndPriorsImpl(unsigned n);
            void                            recalcPriorsImpl();

            unsigned                        _ntax;
            bool                            _is_rooted;
            bool                            _is_resolution_class_prior;
            double                          _C;

            bool                            _topo_priors_dirty;
            bool                            _counts_dirty;

            double                          _log_scaling_factor;
            std::vector<int>                _nfactors;
            std::vector<double>             _counts;
            double                          _log_total_count;
            std::vector<double>             _topology_prior;
    };

/*----------------------------------------------------------------------------------------------------------------------
|   Recomputes `_counts' vector for the supplied number of internal nodes (`n') using the method outlined by Joe
|   Felsenstein in his 2004 book and also in Felsenstein (1978) and Felsenstein (1981).
|
|   Felsenstein, J. 1978. The number of evolutionary trees. Syst. Zool. 27: 27-33.
|   Felsenstein, J. 1981. Syst. Zool. 30: 122.
|   Felsenstein, J. 2004. Inferring Phylogeny. Sinauer, Sunderland, Massachusetts.
|
|   Below I've reproduced the table from Felsenstein 2004 illustrating how to calculate the number of possible rooted
|   tree topologies for any number of internal nodes:
|>
|                      number of taxa in rooted tree            _counts    _nfactors
|             2       3       4       5        6        7         8
|    n     +-------+-------+-------+--------+--------+--------+---------+   +---+  This table shows results for 8
|    u  1  |   1   |   1   |   1   |   1    |    1   |    1   |    1    |   | 0 |  taxa and rooted trees. Columns
|    m     +-------+-------+-------+--------+--------+--------+---------+   +---+  corresponding to 2-7 taxa would have
|    b  2          |   3   |   10  |   25   |   56   |   119  |   246   |   | 0 |  been replaced by the column
|    e             +-------+-------+--------+--------+--------+---------+   +---+  corresponding to 8 taxa. This column
|    r  3                  |   15  |  105   |   490  |   1918 |   6825  |   | 0 |  is now stored in the `_counts' vector.
|                          +-------+--------+--------+--------+---------+   +---+  The scaling factor used in this case
|    o  4                          | 0.0105 | 0.1260 |  0.945 |  5.6980 |   | 1 |  was 10,000. The number of times this
|    f                             +--------+--------+--------+---------+   +---+  factor was applied is stored in the
|       5                                   | 0.0945 | 1.7325 | 19.0575 |   | 1 |  _nfactors vector. Thus, the "count"
|    n                                      +--------+--------+---------+   +---+  for 5 internal nodes is not really
|    o  6                                            | 1.0395 | 27.0270 |   | 1 |  19.0575, but instead 190575:
|    d                                               +--------+---------+   +---+  _counts[4]*(scaling_factor)^_nfactors[4]
|    e  7                                                     | 13.5135 |   | 1 |
|    s                                                        +---------+   +---+
|>
|   The recalcCountsAndPriorsImpl function works from left to right, calculating each column in turn. Within a column,
|   it works down. Each cell except the first in a given column requires knowledge of two cells from the column to the
|   left (the cell to its immediate left as well as the cell above the cell to its immediate left). This is because
|   in order to know how many tree topologies there are for N taxa, one needs to know how many places a taxon could be
|   added to a tree with one fewer taxa. The N-1 taxon tree could have the same number of internal nodes as the new
|   tree (the new taxon was added to an existing node, either creating a new polytomy or enlarging an existing one), or
|   the N-1 taxon tree could have one fewer internal nodes (in which case the new taxon inserts a new node).
*/
void PolytomyTopoPriorCalculator::recalcCountsAndPriorsImpl(unsigned n) {
    if (_is_resolution_class_prior)
        _counts_dirty = true;
    if (_counts_dirty) {
        double scaling_factor = exp(_log_scaling_factor);

        _counts.clear();
        _counts.push_back(1.0); // _counts are always 1 for m = 1

        int last_factor = 0;
        _nfactors.clear();
        _nfactors.push_back(last_factor); // never need to scale this one

        // temporary variables
        double a, b, c;
        double max_log_double = log(DBL_MAX);
        double epsilon = scaling_factor/10.0; // value arbitrary, but must be larger than zero and less than scaling_factor

        // Compute the vector of _counts for the number of internal nodes specified
        // This is the main loop over columns. z is the number of taxa minus 1 for rooted trees, and is the number
        // of taxa minus 2 for unrooted trees
        for (unsigned z = 2; z <= n; ++z) {
            _counts.push_back(0.0);  // this column is one element longer than the column to its left
            _nfactors.push_back(last_factor);
            b = _counts[0];  // _counts[0] is always 1.0 because there is only one star tree topology

            // This is the loop over rows within the current column. m + 1 is the number of internal nodes.
            for (unsigned m = 1; m < z; ++m) {
                unsigned num_internal_nodes = m + 1;
                double diff = (double)(_nfactors[m - 1] - _nfactors[m]);
                double log_factor = diff*_log_scaling_factor;
                if (log_factor >= max_log_double) {
                    std::cerr << "Oops! log_factor >= max_log_double" << std::endl;
                }
                assert(log_factor < max_log_double);
                a = b*exp(log_factor);
                b = _counts[m];
                c = a*((double)(z + num_internal_nodes - 1));
                if (num_internal_nodes < z) {
                    c += b*(double)num_internal_nodes;
                }
                if (c > scaling_factor) {
                    //unsigned prev_nfactors = _nfactors[m];
                    double incr = floor(log(c - epsilon)/_log_scaling_factor);
                    _nfactors[m] += (unsigned)incr;
                    last_factor = _nfactors[m];
                    _counts[m] = exp(log(c) - incr*_log_scaling_factor);
                    b = exp(log(b) - incr*_log_scaling_factor);
                }
                else
                    _counts[m] = c;
            }
        }

        // Now compute the log of the total number of tree topologies over all possible resolution classes
        // (i.e. number of internal nodes)
        // Begin by creating a vector of log _counts and finding the largest value (this will be factored out
        // to avoid overflow)
        std::vector<double> v;
        unsigned sz = (unsigned)_nfactors.size();
        assert(sz == _counts.size());
        double max_log_count = 0.0;
        for (unsigned i = 0; i < sz; ++i) {
            double num_factors = (double)_nfactors[i];
            double log_count = num_factors*_log_scaling_factor + log(_counts[i]);
            if (log_count > max_log_count)
                max_log_count = log_count;
            v.push_back(log_count);
        }

        // Compute log sum of _counts by factoring out the largest count. Underflow will occur, but only for
        // _counts that are so much smaller than the dominant _counts that the underflow can be ignored for
        // all practical purposes
        double sum = 0.0;
        for (std::vector<double>::const_iterator it = v.begin(); it != v.end(); ++it) {
            double diff = (*it) - max_log_count;
            sum += exp(diff);
        }
        assert(sum > 0.0);
        _log_total_count = log(sum) + max_log_count;
        _counts_dirty = false;
    }
    else {
        _nfactors.clear();
        _counts.clear();
        _counts_dirty = true;    // ensures that _counts will be calcualated if someone asks for one, say by calling GetCount
    }

    // Recalculate the _topology_prior vector too
    recalcPriorsImpl();

    _topo_priors_dirty = false;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Recomputes _topology_prior vector for the supplied number of internal nodes (`n'). The element _topology_prior[m]
|    is the natural logarithm of the unnormalized prior probability of a (rooted/unrooted) tree with `_ntax' taxa and m
|    internal nodes. The rooted/unrooted status is determined by the state of the data member `_is_rooted'. The element
|    _topology_prior[0] is the natural log of the normalizing constant. Normally, only unnormalized values are needed
|    because MCMC deals in prior ratios, but if for some reason the normalized prior probability is needed, it can be
|    computed as exp{_topology_prior[m] - _topology_prior[0]}. This function requires `_counts' to be correct if using
|   the resolution class prior. Thus, never call recalcPriorsImpl directly, only invoke it indirectly by calling the
|   function recalcCountsAndPriorsImpl.
|
|    Consider an unrooted tree with _ntax = 6. Such a tree has 4 internal nodes, and calling recalcPriorsImpl
|    would yield the following if `_is_resolution_class_prior' is false:
|>
|         unnormalized
|    m    polytomy prior     _C = 2
|    ----------------------------------
|    1        _C^3              8             _topology_prior[1] = ln(8)  = 2.079
|    2        _C^2              4             _topology_prior[2] = ln(4)  = 1.386
|    3        _C^1              2             _topology_prior[3] = ln(2)  = 0.693
|    4        _C^0              1             _topology_prior[4] = ln(1)  = 0.000
|    ----------------------------------
|           _C^4 - 1         16 - 1
|           -------         ------ = 15   _topology_prior[0] = ln(15) = 2.708
|            _C - 1           2 - 1
|>
|    If instead `_is_resolution_class_prior' is true, we have:
|>
|                  unnormalized
|                  resolution
|    m    _counts   class prior        _C = 2
|    ---------------------------------------------
|    1         1     (_C^3)/1         8/1   = 8.000   _topology_prior[1] = ln(8.000) =  2.079
|    2        25     (_C^2)/25        4/25  = 0.160   _topology_prior[2] = ln(0.160) = -1.833
|    3       105     (_C^1)/105       2/105 = 0.019   _topology_prior[3] = ln(0.019) = -3.963
|    4       105     (_C^0)/105       1/105 = 0.010   _topology_prior[4] = ln(0.010) = -4.605
|    ---------------------------------------------
|                (no easy formula)           8.189   _topology_prior[0] = ln(8.189) =  2.103
|>
*/
void PolytomyTopoPriorCalculator::recalcPriorsImpl() {
    _topology_prior.clear();
    _topology_prior.push_back(0.0);    // This will hold the normalizing constant in the end

    // Figure out the maximum possible value for m, the number of internal nodes
    unsigned maxm = _ntax - (_is_rooted ? 1 : 2);

    if (_is_resolution_class_prior) {
        // _counts vector should have length equal to maxm if everything is ok
        assert(maxm == (unsigned)_counts.size());

        double total = 0.0;
        double logC = std::log(_C);
        for (unsigned m = 1; m <= maxm; ++m)
            {
            double logCterm = (double)(maxm - m)*logC;
            double log_count_m = std::log(_counts[m - 1]) + _log_scaling_factor*(double)_nfactors[m - 1];
            double log_v = logCterm - log_count_m;
            total += std::exp(log_v);
            _topology_prior.push_back(log_v);
        }
        _topology_prior[0] = std::log(total);
    }
    else {
        double total = 0.0;
        double logC = std::log(_C);
        for (unsigned m = 1; m <= maxm; ++m) {
            double logCterm = (double)(maxm - m)*logC;
            total += std::exp(logCterm);
            _topology_prior.push_back(logCterm);
        }
        _topology_prior[0] = std::log(total);
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the natural log of the number of trees having `n' taxa and `m' internal nodes. Calls RecalcCountsAndPriors
|   function if `n' is not equal to `_ntax'. Assumes `m' is greater than 0. If `_is_rooted' is true, assumes `m' is less
|   than `_ntax'. If `_is_rooted' is false, assumes `m' less than `_ntax' - 1.
*/
double PolytomyTopoPriorCalculator::getLogCount(unsigned n, unsigned m) {
    assert((_is_rooted && (m < n)) || (!_is_rooted && (m < n - 1)));
    if (n != _ntax)
        setNTax(n);
    if (_counts_dirty)
        reset();
    double nf = (double)(_nfactors[m - 1]);
    double log_count = nf*_log_scaling_factor + log(_counts[m - 1]);
    return log_count;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the number of saturated (i.e. fully-resolved and thus having as many internal nodes as possible) trees
|    of `n' taxa. Calls RecalcCountsAndPriors function if `n' is not equal to `_ntax'.
*/
double PolytomyTopoPriorCalculator::getLogSaturatedCount(unsigned n) {
    if (n != _ntax)
        setNTax(n);
    if (_counts_dirty)
        reset();
    unsigned last = (unsigned)(_counts.size() - 1);
    double nf = (double)(_nfactors[last]);
    double log_count = nf*_log_scaling_factor + log(_counts[last]);
    return log_count;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the natural log of the total number of trees for `n' taxa, including all resolution classes from the star
|   tree to fully resolved (saturated) trees. Calls RecalcCountsAndPriors function if `n' is not equal to `_ntax' or if
|   not using the resolution class prior (in which case _counts have not been calculated).
*/
double PolytomyTopoPriorCalculator::getLogTotalCount(unsigned n) {
    if (n != _ntax)
        setNTax(n);
    if (_counts_dirty)
        reset();
    return _log_total_count;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Constructs a vector of realized resolution class priors from the values in the _topology_prior vector. If
|    `_topo_priors_dirty' is true, it recomputes the _topology_prior vectors first. The mth element of the
|    returned vector is set to T_{n,m}*_topology_prior[m] for m > 0. The 0th element of the returned vector holds the
|    normalization constant (sum of all other elements). This function is not efficient because it is intended only to
|    be used for providing information to the user on request. Table 2, p. 248, in the "Polytomies and Bayesian
|    Phylogenetic Inference" paper (Lewis, P. O., M. T. Holder and K. E. Holsinger. 2005. Systematic Biology 54(2):
|    241-253) presented (normalized) values from this vector.
*/
std::vector<double> PolytomyTopoPriorCalculator::getRealizedResClassPriorsVect() {
    if (!_is_resolution_class_prior)
        _counts_dirty = true;
    if (_topo_priors_dirty || _counts_dirty)
        reset();

    std::vector<double> v;
    v.reserve(_topology_prior.size());
    v.push_back(0.0);

    unsigned sz = (unsigned)_topology_prior.size();

    // First loop will be to determine largest value, which will be factored out
    // the second time through so that the total does not overflow
    double log_factored_out = 0.0;
    for (unsigned i = 1; i < sz; ++i) {
        double c = _counts[i - 1];
        double nf = (double)_nfactors[i - 1];
        double log_Tnm = log(c) + _log_scaling_factor*nf;
        double log_prior = log_Tnm + _topology_prior[i];
        v.push_back(log_prior);
        if (log_prior > log_factored_out)
            log_factored_out = log_prior;
    }

    // Now we can compute the total
    double total = 0.0;
    std::vector<double>::const_iterator it = v.begin();
    for (++it; it != v.end(); ++it) {
        total += exp((*it) - log_factored_out);
    }
    v[0] = log(total) + log_factored_out;

    return v;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Constructs a vector in which the element having index m (i = 0, 1, ..., max. num. internal nodes) represents the
|   natural logarithm of the number of tree topologies having m internal nodes. If `_counts_dirty' is true, it recomputes
|   the `_counts' vectors first. The 0th element of the returned vector holds the natural log of the total number of tree
|   topologies (log of the sum of all other elements).
*/
std::vector<double> PolytomyTopoPriorCalculator::getLogCounts() {
    if (_is_resolution_class_prior)
        _counts_dirty = true;
    if (_counts_dirty)
        reset();

    std::vector<double> v;
    unsigned sz = _ntax - (_is_rooted ? 0 : 1);
    v.reserve(sz);
    v.push_back(_log_total_count);

    //@POL could use a version of the transform algorithm here
    for (unsigned i = 1; i < sz; ++i) {
        double log_Tnm = log(_counts[i - 1]) + _log_scaling_factor*(double)(_nfactors[i - 1]);
        v.push_back(log_Tnm);
    }

    return v;
}

/*----------------------------------------------------------------------------------------------------------------------
|   Samples a resolution class (i.e. number of internal nodes) from the realized resolution class distribution. This
|   function is not very efficient because it calls PolytomyTopoPriorCalculator::getRealizedResClassPriorsVect, resulting in an
|   unnecessary vector copy operation.
*/
unsigned PolytomyTopoPriorCalculator::sample(Lot::SharedPtr rng) {
    std::vector<double> v = getRealizedResClassPriorsVect();
    double u = rng->uniform();
    double z = v[0];
    double cum = 0.0;
    for (unsigned i = 1; i < v.size(); ++i) {
        cum += exp(v[i] - z);
        if (u <= cum)
            return i;
    }
    assert(0);
    return (unsigned)(v.size() - 1);
}

/*----------------------------------------------------------------------------------------------------------------------
|    Constructor sets `_is_rooted' to false, `_is_resolution_class_prior' to true, `_C' to 1.0, `_ntax' to 4, and
|   `_topo_priors_dirty' to true.
*/
PolytomyTopoPriorCalculator::PolytomyTopoPriorCalculator() {
    _topo_priors_dirty            = true;
    _is_rooted                    = false;
    _is_resolution_class_prior    = true;
    _C                            = 1.0;
    _ntax                        = 4;
    _counts_dirty                = true;
    _log_scaling_factor          = 10.0;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Destructor clears the vectors `_counts', `_nfactors' and _topology_prior.
*/
PolytomyTopoPriorCalculator::~PolytomyTopoPriorCalculator() {
    _counts.clear();
    _topology_prior.clear();
    _nfactors.clear();
}

/*----------------------------------------------------------------------------------------------------------------------
|    If `new_ntax' differs from `_ntax', sets `_ntax' to `new_ntax' and sets `_topo_priors_dirty'. Returns immediately
|    without taking any action if `_ntax' equals `new_ntax'. Assumes that `new_ntax' is greater than 1 if `_is_rooted' is
|    true, or that `new_ntax' is greater than 2 if `_is_rooted' is false.
*/
void PolytomyTopoPriorCalculator::setNTax(unsigned new_ntax) {
    if (_ntax != new_ntax) {
        // Set _ntax to the new value
        assert(new_ntax > (unsigned)(_is_rooted ? 1 : 2));
        _ntax = new_ntax;

        _counts_dirty = true;
        _topo_priors_dirty = true;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Forces recalculation of `polytomy_prior' if `_is_resolution_class_prior' is false, and both `_counts' and
|   `polytomy_prior' if `_is_resolution_class_prior' is true (or if `_counts_dirty' is true).
*/
void PolytomyTopoPriorCalculator::reset() {
    unsigned num_internal_nodes = (_is_rooted ? (_ntax - 1) : (_ntax - 2));
    recalcCountsAndPriorsImpl(num_internal_nodes);
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the value of the data member `_ntax'.
*/
unsigned PolytomyTopoPriorCalculator::getNTax() const {
    return _ntax;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Sets `_is_rooted' data member to true. There are more rooted than unrooted trees for the same value of `_ntax', so
|    this setting is important when asking questions that require knowledge of the numbers of possible trees.
*/
void PolytomyTopoPriorCalculator::chooseRooted() {
    if (!_is_rooted) {
        _is_rooted = true;
        _topo_priors_dirty = true;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Sets `_is_rooted' data member to false. There are more rooted than unrooted trees for the same value of `_ntax', so
|    this setting is important when asking questions that require knowledge of the numbers of possible trees.
*/
void PolytomyTopoPriorCalculator::chooseUnrooted() {
    if (_is_rooted) {
        _is_rooted = false;
        _topo_priors_dirty = true;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns copy of the `_counts' vector, which contains in its (m-1)th element the number of tree topologies having
|   exactly m internal nodes. Note that you will need to also call getNFactorsVect if there is any chance that some of
|   the _counts are larger than exp(_log_scaling_factor). In such cases, the actual log count is
|   log count = `_nfactors[m]'*`_log_scaling_factor' + log(`count[m - 1]')
*/
std::vector<double> PolytomyTopoPriorCalculator::getCountsVect() {
    //@POL this function could be const were it not for lazy evaluation
    if (_counts_dirty)
        reset();
    return _counts;
    }

/*----------------------------------------------------------------------------------------------------------------------
|    Returns copy of the `_nfactors' vector, which contains in its (m-1)th element the number of times _counts[m] has been
|   rescaled by dividing by the scaling factor (the log of which is `_log_scaling_factor').
*/
std::vector<int> PolytomyTopoPriorCalculator::getNFactorsVect() {
    //@POL this function could be const were it not for lazy evaluation
    if (_counts_dirty)
        reset();
    return _nfactors;
    }

/*----------------------------------------------------------------------------------------------------------------------
|    Sets `_is_resolution_class_prior' data member to true.
*/
void PolytomyTopoPriorCalculator::chooseResolutionClassPrior() {
    if (!_is_resolution_class_prior) {
        _is_resolution_class_prior = true;
        _topo_priors_dirty = true;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Sets `_is_resolution_class_prior' data member to false.
*/
void PolytomyTopoPriorCalculator::choosePolytomyPrior() {
    if (_is_resolution_class_prior) {
        _is_resolution_class_prior = false;
        _topo_priors_dirty = true;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Sets `_C' data member to `c'. Assumes `c' is greater than 0.0.
*/
void PolytomyTopoPriorCalculator::setC(double c) {
    assert(c > 0.0);
    if (c != _C) {
        _C = c;
        _topo_priors_dirty = true;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns current value of the `_C' data member.
*/
double PolytomyTopoPriorCalculator::getC() const {
    return _C;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Sets `_log_scaling_factor' data member to the supplied value `lnf'.
*/
void PolytomyTopoPriorCalculator::setLogScalingFactor(double lnf) {
    assert(lnf > 0.0);
    if (lnf != _log_scaling_factor) {
        _log_scaling_factor = lnf;
        _counts_dirty = true;
        _topo_priors_dirty = true;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns current value of the `_log_scaling_factor' data member.
*/
double PolytomyTopoPriorCalculator::getLogScalingFactor() const {
    return _log_scaling_factor;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns copy of the _topology_prior vector, which contains in its mth element the unnormalized prior for tree
|    topologies having exactly m internal nodes. The 0th element of _topology_prior holds the normalizing constant.
*/
std::vector<double> PolytomyTopoPriorCalculator::getTopoPriorVect() {
    //@POL this function could be const were it not for lazy evaluation
    if (_topo_priors_dirty)
        reset();
    return _topology_prior;
}

/*----------------------------------------------------------------------------------------------------------------------
|   Returns result of call to getLogTopologyPrior(m), where m is the number of internal nodes in `t'.
*/
double PolytomyTopoPriorCalculator::getLogTopoProb(Tree::SharedPtr t) {
    //std::cerr << ">>>>>>>> in PolytomyTopoPriorCalculator::getLnTopoProb" << std::endl;  //POL_BOOKMARK 13-July-2017
    unsigned n = t->numLeaves();
    assert(n > 3);
    if (n != getNTax())
        setNTax(n);
    unsigned m = t->numInternals();
    double topo_prior = getLogTopologyPrior(m);
    return topo_prior;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the natural logarithm of the unnormalized topology prior. This represents the resolution class prior if
|    `_is_resolution_class_prior' is true, otherwise it represents the polytomy prior. Assumes `m' is less than the
|    length of the _topology_prior vector.
*/
double PolytomyTopoPriorCalculator::getLogTopologyPrior(unsigned m) {
    if (_topo_priors_dirty)
        reset();
    assert(m < _topology_prior.size());
    return _topology_prior[m];
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the natural logarithm of the normalized topology prior. This represents the resolution class prior if
|    `_is_resolution_class_prior' is true, otherwise it represents the polytomy prior. Assumes `m' is less than the length
|    of the _topology_prior vector. The log of the normalized topology prior is obtained as _topology_prior[m] minus
|    _topology_prior[0] (the 0th element of _topology_prior holds the log of the normalization constant).
*/
double PolytomyTopoPriorCalculator::getLogNormalizedTopologyPrior(unsigned m) {
    if (_topo_priors_dirty)
        reset();
    assert(m < _topology_prior.size());
    return (_topology_prior[m] - _topology_prior[0]);
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the natural logarithm of the normalizing constant for the topology prior. This value is stored in
|    _topology_prior[0].
*/
double PolytomyTopoPriorCalculator::getLogNormConstant() {
    if (_topo_priors_dirty)
        reset();
    return _topology_prior[0];
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the value of `_is_resolution_class_prior'.
*/
bool PolytomyTopoPriorCalculator::isResolutionClassPrior() const {
    return _is_resolution_class_prior;
}

/*----------------------------------------------------------------------------------------------------------------------
|    If `_is_resolution_class_prior' is true, returns false; if `_is_resolution_class_prior' is false, returns true.
*/
bool PolytomyTopoPriorCalculator::isPolytomyPrior() const {
    return !_is_resolution_class_prior;
}

/*----------------------------------------------------------------------------------------------------------------------
|    Returns the value of `_is_rooted'.
*/
bool PolytomyTopoPriorCalculator::isRooted() const {
    return _is_rooted;
}

/*----------------------------------------------------------------------------------------------------------------------
|    If `_is_rooted' is true, returns false; if `_is_rooted' is false, returns true.
*/
bool PolytomyTopoPriorCalculator::isUnrooted() const {
    return !_is_rooted;
}

}

