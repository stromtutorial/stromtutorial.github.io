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

            virtual double                      calcLogPrior();
            //double                              calcLogTopologyPrior() const; ///!a

        private:

            virtual void                        revert();
            virtual void                        proposeNewState();
        
            void                                starTreeMove(); ///!b
            Node *                              chooseRandomChild(Node * x, Node * avoid, bool parent_included);

            virtual void                        reset();

            double                              _orig_edgelen_top;
            double                              _orig_edgelen_middle;
            double                              _orig_edgelen_bottom;

            unsigned                            _case;
            bool                                _topology_changed;
            bool                                _star_tree_move; ///!c
            Node *                              _x;
            Node *                              _y;
            Node *                              _a;
            Node *                              _b;
    };
    ///end_class_declaration

    inline TreeUpdater::TreeUpdater() { ///begin_constructor
        // std::cout << "Creating a TreeUpdater" << std::endl;
        Updater::clear();
        _name = "Tree Topology and Edge Proportions";
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
        _log_hastings_ratio     = 0.0;
        _case                   = 0;
        _x                      = 0;
        _y                      = 0;
        _a                      = 0;
        _b                      = 0;
    }   ///end_reset

    inline double TreeUpdater::calcLogPrior() {   ///begin_calcLogPrior
        double log_topology_prior    = Updater::calcLogTopologyPrior();
        double log_edge_length_prior = Updater::calcLogEdgeLengthPrior();
        return log_topology_prior + log_edge_length_prior;
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
        unsigned chosen = (unsigned)_lot->randint(0,upper - 1);
        
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
        // Choose focal 2-edge segment to modify
        _orig_edgelen_middle = 0.0;
        
        // Choose the first edge
        _a = chooseRandomChild(_x, 0, false);
        _orig_edgelen_top = _a->getEdgeLength();

        // Choose the second edge
        _b = chooseRandomChild(_x, _a, true);
        if (!_b)
            _b = _x;
        _orig_edgelen_bottom = _b->getEdgeLength();

        // Note that _a must be a child of _x, but _b may either be a different child of _x or _x itself
        double u = _lot->uniform();
        double new_edgelen_top    = u*(_orig_edgelen_top + _orig_edgelen_bottom);
        double new_edgelen_bottom = (1.0 - u)*(_orig_edgelen_top + _orig_edgelen_bottom);

        // Hastings ratio and Jacobian are both 1 under Gamma-Dirichlet parameterization
        _log_hastings_ratio = 0.0;
        _log_jacobian = 0.0;

        // Change edge lengths and flag partials and transition matrices for recalculation
        _tree_manipulator->selectPartialsHereToRoot(_x);
        _a->setEdgeLength(new_edgelen_top);
        _a->selectTMatrix();
        _b->setEdgeLength(new_edgelen_bottom);
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
        // For the star tree, there is only one internal node. In this case, only choose    ///!d
        // two edges and modify them (no change in tree topology is possible)
        //
        //           a
        //      \ | /
        //       \|/
        //        x
        //        |
        //        |
        //        b
        //  ///!e
        
        _x = _tree_manipulator->randomInternalEdge(_lot->uniform());
        _orig_edgelen_middle = _x->getEdgeLength();
        
        // The only child of the root node will be chosen only if the tree equals the star tree ///!f
        // in which case we want to perform a starTreeMove rather than Larget-Simon
        _star_tree_move = false;
        if (_x->getParent() && !_x->getParent()->getParent()) {
            _star_tree_move = true;
            starTreeMove();
            return;
        }   ///!g

        _y = _x->getParent();
        //...
        ///end_star_tree_modifications

        // Choose focal 3-edge segment to modify
        // Begin by randomly choosing one child of x to be node _a
        _a = chooseRandomChild(_x, 0, false);
        _orig_edgelen_top = _a->getEdgeLength();

        // Now choose a different child of x (or the parent) to be node _b
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
        
        // Symmetric move: Hastings ratio = 1
        _log_hastings_ratio = 0.0;

        // Decide where along focal path (starting from top) to place moved node
        double focal_path_length = _orig_edgelen_top + _orig_edgelen_middle + _orig_edgelen_bottom;
        double u = _lot->uniform();
        double new_attachment_point = u*focal_path_length;
        if (new_attachment_point <= Node::_smallest_edge_length)
            new_attachment_point = Node::_smallest_edge_length;
        else if (focal_path_length - new_attachment_point <= Node::_smallest_edge_length)
            new_attachment_point = focal_path_length - Node::_smallest_edge_length;

        // Decide which node(s) to move, and whether the move involves a topology change
        u = _lot->uniform();
        bool x_node_slides = (bool)(u < 0.5);
        if (x_node_slides) {
            // _x slides toward _y
            _topology_changed = (new_attachment_point > _orig_edgelen_top + _orig_edgelen_middle);
            if (_topology_changed) {
                _tree_manipulator->LargetSimonSwap(_a, _b);
                if (b_is_child_of_y) {
                    // LargetSimonSwap case 1: a swapped with b
#if 1
                    _a->setEdgeLength(_orig_edgelen_top + _orig_edgelen_middle);
                    _x->setEdgeLength(new_attachment_point - _orig_edgelen_top - _orig_edgelen_middle);
                    _b->setEdgeLength(focal_path_length - new_attachment_point);
#else
                    _a->setEdgeLength(focal_path_length - new_attachment_point);
                    _x->setEdgeLength(new_attachment_point - _orig_edgelen_top - _orig_edgelen_middle);
                    _b->setEdgeLength(_orig_edgelen_top + _orig_edgelen_middle);
#endif
                    _case = 1;
                } else {
                    // LargetSimonSwap case 2: x's children (except a) swapped with y's children (except b)
                    _a->setEdgeLength(_orig_edgelen_top + _orig_edgelen_middle);
                    _x->setEdgeLength(new_attachment_point - _orig_edgelen_top - _orig_edgelen_middle);
                    _y->setEdgeLength(focal_path_length - new_attachment_point);
                    _case = 2;
                }
            } else {
                _a->setEdgeLength(new_attachment_point);
                _x->setEdgeLength(_orig_edgelen_top + _orig_edgelen_middle - new_attachment_point);
                if (b_is_child_of_y) {
                    _b->setEdgeLength(_orig_edgelen_bottom);    // not really necessary
                    _case = 3;
                } else {
                    _y->setEdgeLength(_orig_edgelen_bottom);    // not really necessary
                    _case = 4;
                }
            }
        } else {
            // _y slides toward _x
            _topology_changed = (new_attachment_point < _orig_edgelen_top);
            if (_topology_changed) {
                _tree_manipulator->LargetSimonSwap(_a, _b);
                if (b_is_child_of_y) {
                    // LargetSimonSwap case 1: a swapped with b
#if 1
                    _a->setEdgeLength(new_attachment_point);
                    _x->setEdgeLength(_orig_edgelen_top - new_attachment_point);
                    _b->setEdgeLength(_orig_edgelen_middle + _orig_edgelen_bottom);
#else
                    _a->setEdgeLength(_orig_edgelen_middle + _orig_edgelen_bottom);
                    _x->setEdgeLength(_orig_edgelen_top - new_attachment_point);
                    _b->setEdgeLength(new_attachment_point);
#endif
                    _case = 5;
                } else {
                    // LargetSimonSwap case 2: x's children (except a) swapped with y's children (except b)
                    _a->setEdgeLength(new_attachment_point);
                    _x->setEdgeLength(_orig_edgelen_top - new_attachment_point);
                    _y->setEdgeLength(_orig_edgelen_middle + _orig_edgelen_bottom);
                    _case = 6;
                }
            } else {
                _a->setEdgeLength(_orig_edgelen_top);
                _x->setEdgeLength(new_attachment_point - _orig_edgelen_top);
                if (b_is_child_of_y) {
                    _b->setEdgeLength(focal_path_length - new_attachment_point);
                    _case = 7;
                } else {
                    _y->setEdgeLength(focal_path_length - new_attachment_point);
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
        if (_star_tree_move) {  ///!h
            _a->setEdgeLength(_orig_edgelen_top);
            _b->setEdgeLength(_orig_edgelen_bottom);
        }
        else {  ///!i
            assert(_case > 0 && _case < 9);
            if (_case == 2 || _case == 6)
                _tree_manipulator->LargetSimonSwap(_a, _b);
            else if (_case == 1 || _case == 5)
                _tree_manipulator->LargetSimonSwap(_b, _a);
            _a->setEdgeLength(_orig_edgelen_top);
            _x->setEdgeLength(_orig_edgelen_middle);
            if (_case == 1 || _case == 3 || _case == 5 || _case == 7)
                _b->setEdgeLength(_orig_edgelen_bottom);    // not necessary for case 3
            else
                _y->setEdgeLength(_orig_edgelen_bottom);    // not necessary for case 4
        }   ///!j
    }   ///end_revert

}   ///end
