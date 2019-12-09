#pragma once    ///start

#include "updater.hpp"

namespace strom {

    class Chain;

    class TreeUpdater : public Updater {

        friend class Chain;

        public:

            typedef std::shared_ptr< TreeUpdater > SharedPtr;

                                                TreeUpdater();
                                                ~TreeUpdater();

            virtual double                      calcLogPrior(int & checklist);
            //double                              calcLogTopologyPrior() const;

        private:

            virtual void                        revert();
            virtual void                        proposeNewState();
        
            void                                starTreeMove();
            Node *                              chooseRandomChild(Node * x, Node * avoid, bool parent_included);

            virtual void                        reset();

            double                              _orig_edgelen_top;
            double                              _orig_edgelen_middle;
            double                              _orig_edgelen_bottom;

            double                              _new_edgelen_top;
            double                              _new_edgelen_middle;
            double                              _new_edgelen_bottom;

            unsigned                            _case;
            bool                                _topology_changed;
            bool                                _star_tree_move;
            Node *                              _x;
            Node *                              _y;
            Node *                              _a;
            Node *                              _b;
            Node *                              _c;
            Node *                              _d;
    };

    // Member function bodies go here
    ///end_class_declaration
    inline TreeUpdater::TreeUpdater() { ///begin_constructor
        // std::cout << "Creating a TreeUpdater" << std::endl;
        Updater::clear();
        _name = "Tree and Edge Lengths";
        reset();
    }   ///end_constructor

    inline TreeUpdater::~TreeUpdater() {    ///begin_destructor
        // std::cout << "Destroying a TreeUpdater" << std::endl;
    }   ///end_destructor
    
    inline void TreeUpdater::reset() {  ///begin_reset
        _topology_changed       = false;
        _orig_edgelen_top       = 0.0;
        _orig_edgelen_middle    = 0.0;
        _orig_edgelen_bottom    = 0.0;
        _new_edgelen_top        = 0.0;
        _new_edgelen_middle     = 0.0;
        _new_edgelen_bottom     = 0.0;
        _log_hastings_ratio     = 0.0;
        _case                   = 0;
        _x                      = 0;
        _y                      = 0;
        _a                      = 0;
        _b                      = 0;
        _c                      = 0;
        _d                      = 0;
    }   ///end_reset

#if 0
    inline double TreeUpdater::calcLogTopologyPrior() const {   ///begin_calcLogTopologyPrior
        typename Tree::SharedPtr tree = _tree_manipulator->getTree();
        assert(tree);
        double n = tree->numLeaves();
        if (tree->isRooted())
            n += 1.0;
        double log_num_topologies = lgamma(2.0*n - 5.0 + 1.0) - (n - 3.0)*log(2.0) - lgamma(n - 3.0 + 1.0);
        return -log_num_topologies;
    }   ///end_calcLogTopologyPrior
#endif

    inline double TreeUpdater::calcLogPrior(int & checklist) {   ///begin_calcLogPrior
        double log_prior = 0.0;
        
        bool tree_topology_prior_calculated = (checklist & Model::TreeTopology);
        if (!tree_topology_prior_calculated) {
            log_prior += Updater::calcLogTopologyPrior();
            checklist |= Model::TreeTopology;
        }
            
        bool edge_lengths_prior_calculated  = (checklist & Model::EdgeLengths);
        if (!edge_lengths_prior_calculated) {
            log_prior += Updater::calcLogEdgeLengthPrior();
            checklist |= Model::EdgeLengths;
        }
        return log_prior;
    }   ///end_calcLogPrior

    inline Node * TreeUpdater::chooseRandomChild(Node * x, Node * avoid, bool parent_included) {
        // Count number of children of x
        unsigned n = 0;
        Node * child = x->getLeftChild();
        while (child) {
            if (child != avoid)
                n++;
            child = child->getRightSib();
        }
        
        // Choose random child index
        unsigned upper = n + (parent_included ? 1 : 0);
        unsigned chosen = _lot->randint(0,upper - 1);
        
        // If chosen < n, then find and return that particular child
        if (chosen < n) {
            child = x->getLeftChild();
            unsigned i = 0;
            while (child) {
                if (child != avoid && i == chosen)
                    return child;
                else if (child != avoid)
                    i++;
                child = child->getRightSib();
            }
        }

        // If chosen equals n, then the parent was chosen, indicated by returning NULL
        return NULL;
    }

    inline void TreeUpdater::starTreeMove() {    ///begin_starTreeMove
        // Choose expansion/contraction factor
#if 1   //POLTMP
        // try not having a Hastings ratio
        _log_hastings_ratio = 0.0;
#else
        double m = exp(_lambda*(_lot->uniform() - 0.5));
#endif

        // Choose focal 2-edge segment to shrink or grow (middle edge constrained to equal zero in star tree)
        _orig_edgelen_middle = 0.0;
        
        // Choose the first edge
        _a = chooseRandomChild(_x, 0, true);
        if (!_a)
            _a = _x;
        _orig_edgelen_top = _a->getEdgeLength();

        // Choose the second edge
        _b = chooseRandomChild(_x, _a, true);
        if (!_b)
            _b = _x;
        _orig_edgelen_bottom = _b->getEdgeLength();

        // Note that what (_a or _b) we're calling top and what we're calling bottom is arbitrary
#if 1   //POLTMP
        // try not having a Hastings ratio
        double u = _lot->uniform();
        _new_edgelen_top    = u*_orig_edgelen_top;
        double remainder = _orig_edgelen_top - _new_edgelen_top;
        _new_edgelen_middle = 0.0;
        _new_edgelen_bottom = _orig_edgelen_bottom + remainder;
#else
        _new_edgelen_top    = m*_orig_edgelen_top;
        _new_edgelen_middle = 0.0;
        _new_edgelen_bottom = m*_orig_edgelen_bottom;
#endif

        // Calculate Hastings ratio under GammaDir parameterization
#if 0
        double num_edges = (double)(_tree_manipulator->countEdges());
        double TL = _tree_manipulator->calcTreeLength();
        double fraction_of_tree_length = (_orig_edgelen_top + _orig_edgelen_bottom)/TL;
        _log_hastings_ratio = 2.0*log(m); //POLTMP - num_edges*log(fraction_of_tree_length*m + 1.0 - fraction_of_tree_length);
#endif
        // Change edge lengths and flag partials and transition matrices for recalculation
        _tree_manipulator->selectPartialsHereToRoot(_x);
        _a->setEdgeLength(_new_edgelen_top);
        _a->selectTMatrix();
        _b->setEdgeLength(_new_edgelen_bottom);
        _b->selectTMatrix();
    }   ///end_starTreeMove
    
    // This version uses a polytomy-aware NNI swap
    inline void TreeUpdater::proposeNewState() {    ///begin_proposeNewState
        _case = 0;
        _topology_changed = false;
        assert(!_tree_manipulator->getTree()->isRooted());

        // Choose random internal node x that is not the root and has parent y that is also not the root.
        // After choosing x (and thus y), choose a and b randomly to create a contiguous 3-edge segment.
        //
        //        a
        //   \ | /
        //    \|/
        //     x
        //      \ | /
        //       \|/
        //        y
        //        |
        //        |
        //        b
        //
        // For the star tree, there is only one internal node. In this case, only choose
        // two edges and modify them (no change in tree topology is possible)
        //
        //           a
        //      \ | /
        //       \|/
        //        x
        //        |
        //        |
        //        b
        //
        
        _x = _tree_manipulator->randomInternalEdge(_lot->uniform());
        
        // The only child of the root node will be chosen only if the tree equals the star tree
        // in which case we want to switch to doing a starTreeMove rather than Larget-Simon
        _star_tree_move = false;
        if (_x->getParent() && !_x->getParent()->getParent()) {
            _star_tree_move = true;
            starTreeMove();
            return;
        }
            
        _orig_edgelen_middle = _x->getEdgeLength();

        _y = _x->getParent();

        // Choose focal 3-edge segment to shrink or grow
        // Begin by randomy choosing one child of x to be node a
        _a = chooseRandomChild(_x, 0, false);
        _orig_edgelen_top = _a->getEdgeLength();

        _b = chooseRandomChild(_y, _x, true);
        bool b_is_child_of_y;
        if (_b) {
            b_is_child_of_y = true;
            _orig_edgelen_bottom = _b->getEdgeLength();
        }
        else {
            b_is_child_of_y = false;
            _b = _y->getParent();
            _orig_edgelen_bottom = _y->getEdgeLength();
        }
        
        // Choose expansion/contraction factor
#if 1   //POLTMP
        // try not having a Hastings ratio
        double m = 1.0; // no shrinkage or expansion
        _log_hastings_ratio = 0.0;
#else
        double m = exp(_lambda*(_lot->uniform() - 0.5));
#endif

        // Calculate Hastings ratio under GammaDir parameterization
#if 0   //original version
        double num_edges = (double)(_tree_manipulator->countEdges());
        double TL = _tree_manipulator->calcTreeLength();
        double fraction_of_tree_length = (_orig_edgelen_top + _orig_edgelen_middle + _orig_edgelen_bottom)/TL;
        _log_hastings_ratio = 3.0*log(m); //POLTMP - num_edges*log(fraction_of_tree_length*m + 1.0 - fraction_of_tree_length);
#endif

#if 0   //POLTMP
        Tree::SharedPtr tree = _tree_manipulator->getTree();
        std::vector<double> nonfocal_edgelens;
        std::vector<double> focal_edgelens;
        for (auto nd : tree->_preorder) {
            //std::cerr << "nd = " << nd << std::endl;
            double edgelen = nd->getEdgeLength();
            if (b_is_child_of_y) {
                if (nd == _a || nd == _x || nd == _b)
                    focal_edgelens.push_back(edgelen);
                else
                    nonfocal_edgelens.push_back(edgelen);
            }
            else {
                if (nd == _a || nd == _x || nd == _y)
                    focal_edgelens.push_back(edgelen);
                else
                    nonfocal_edgelens.push_back(edgelen);
            }
        }
        
        unsigned num_edges = (unsigned)focal_edgelens.size() + (unsigned)nonfocal_edgelens.size();
        if (num_edges != _tree_manipulator->countEdges()) {
            std::cerr << "oops: num_edges = " << num_edges << ", should be " << (_tree_manipulator->countEdges()) << std::endl;
            assert(false);
        }

        double sum_focal = std::accumulate(focal_edgelens.begin(),focal_edgelens.end(), 0.0);
//        if (std::fabs(sum_focal - _orig_edgelen_top - _orig_edgelen_middle - _orig_edgelen_bottom) > 1.e-8) {
//            std::cerr << "oops: sum_focal = " << sum_focal << ", should be " << (_orig_edgelen_top + _orig_edgelen_middle + _orig_edgelen_bottom) << std::endl;
//            assert(false);
//        }
        double sum_nonfocal = std::accumulate(nonfocal_edgelens.begin(),nonfocal_edgelens.end(), 0.0);
        double TL = sum_focal + sum_nonfocal;
//        if (std::fabs(TL - _tree_manipulator->calcTreeLength()) > 1.e-8) {
//            std::cerr << "oops: TL = " << TL << ", should be " << _tree_manipulator->calcTreeLength() << std::endl;
//            assert(false);
//        }
        double beta = (sum_focal*m + sum_nonfocal)/TL;
        double phi = sum_focal;
        std::transform(nonfocal_edgelens.begin(), nonfocal_edgelens.end(), nonfocal_edgelens.begin(), [](double x) -> double { return std::log(x); });
        double log_pi = std::accumulate(nonfocal_edgelens.begin(), nonfocal_edgelens.end(), 0.0);
        double pi = std::exp(log_pi);
        _log_hastings_ratio = 3.0*std::log(m) - log(beta)*(2.0*num_edges-2.0);
        double term = (1.0 - m)*( pi*m*(1-m)*phi - pi*beta*(1+m)-m*beta*phi*(1-phi) );
        _log_hastings_ratio += std::log(term);
#endif

        _new_edgelen_top    = m*_orig_edgelen_top;
        _new_edgelen_middle = m*_orig_edgelen_middle;
        _new_edgelen_bottom = m*_orig_edgelen_bottom;

        // Decide where along focal path (starting from top) to place moved node
        double new_focal_path_length = _new_edgelen_top + _new_edgelen_middle + _new_edgelen_bottom;
        double u = _lot->uniform();
        double new_attachment_point = u*new_focal_path_length;
        if (new_attachment_point <= Node::_smallest_edge_length)
            new_attachment_point = Node::_smallest_edge_length;
        else if (new_focal_path_length - new_attachment_point <= Node::_smallest_edge_length)
            new_attachment_point = new_focal_path_length - Node::_smallest_edge_length;

        // Decide which node(s) to move, and whether the move involves a topology change
        u = _lot->uniform();
        bool x_node_slides = (bool)(u < 0.5);
        if (x_node_slides) {
            // _x slides toward _y
            _topology_changed = (new_attachment_point > _new_edgelen_top + _new_edgelen_middle);
            if (_topology_changed) {
                _tree_manipulator->LargetSimonSwap(_a, _b);
                if (b_is_child_of_y) {
                    // LargetSimonSwap case 1: a swapped with b
                    _a->setEdgeLength(new_focal_path_length - new_attachment_point);
                    _x->setEdgeLength(new_attachment_point - _new_edgelen_top - _new_edgelen_middle);
                    _b->setEdgeLength(_new_edgelen_top + _new_edgelen_middle);
                    _case = 1;
                } else {
                    // LargetSimonSwap case 2: x's children (except a) swapped with y's children (except b)
                    _a->setEdgeLength(_new_edgelen_top + _new_edgelen_middle);
                    _x->setEdgeLength(new_attachment_point - _new_edgelen_top - _new_edgelen_middle);
                    _y->setEdgeLength(new_focal_path_length - new_attachment_point);
                    _case = 2;                }
            } else {
                _a->setEdgeLength(new_attachment_point);
                _x->setEdgeLength(_new_edgelen_top + _new_edgelen_middle - new_attachment_point);
                if (b_is_child_of_y) {
                    _b->setEdgeLength(_new_edgelen_bottom);
                    _case = 3;
                } else {
                    _y->setEdgeLength(_new_edgelen_bottom);
                    _case = 4;
                }
            }
        } else {
            // _y slides toward _x
            _topology_changed = (new_attachment_point < _new_edgelen_top);
            if (_topology_changed) {
                _tree_manipulator->LargetSimonSwap(_a, _b);
                if (b_is_child_of_y) {
                    // LargetSimonSwap case 1: a swapped with b
                    _a->setEdgeLength(_new_edgelen_middle + _new_edgelen_bottom);
                    _x->setEdgeLength(_new_edgelen_top - new_attachment_point);
                    _b->setEdgeLength(new_attachment_point);
                    _case = 5;
                } else {
                    // LargetSimonSwap case 2: x's children (except a) swapped with y's children (except b)
                    _a->setEdgeLength(new_attachment_point);
                    _x->setEdgeLength(_new_edgelen_top - new_attachment_point);
                    _y->setEdgeLength(_new_edgelen_middle + _new_edgelen_bottom);
                    _case = 6;
                }
            } else {
                _a->setEdgeLength(_new_edgelen_top);
                _x->setEdgeLength(new_attachment_point - _new_edgelen_top);
                if (b_is_child_of_y) {
                    _b->setEdgeLength(new_focal_path_length - new_attachment_point);
                    _case = 7;
                } else {
                    _y->setEdgeLength(new_focal_path_length - new_attachment_point);
                    _case = 8;
                }
            }
        }

        // flag partials and transition matrices for recalculation
        _tree_manipulator->selectPartialsHereToRoot(_x);
        _a->selectTMatrix();
        _x->selectTMatrix();
        if (_case == 2 || _case == 4 || _case == 6 || _case == 8) {
            // In these cases b is below y, so it is y's edge that is modified
            _y->selectTMatrix();
        } else {
            // In these cases b is above y, so it is b's edge that is modified
            _b->selectTMatrix();
        }
    }   ///end_proposeNewState
    
    inline void TreeUpdater::revert() { ///begin_revert
        if (_star_tree_move) {
            _a->setEdgeLength(_orig_edgelen_top);
            _b->setEdgeLength(_orig_edgelen_bottom);
        }
        else {
            assert(_case > 0 && _case < 9);
            if (_case == 2 || _case == 6)
                _tree_manipulator->LargetSimonSwap(_a, _b);
            else if (_case == 1 || _case == 5)
                _tree_manipulator->LargetSimonSwap(_b, _a);
            _a->setEdgeLength(_orig_edgelen_top);
            _x->setEdgeLength(_orig_edgelen_middle);
            if (_case == 1 || _case == 3 || _case == 5 || _case == 7)
                _b->setEdgeLength(_orig_edgelen_bottom);
            else
                _y->setEdgeLength(_orig_edgelen_bottom);
        }
    }   ///end_revert

}   ///end
