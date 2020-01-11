#pragma once

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

        private:

            virtual void                        revert();
            virtual void                        proposeNewState();
        
            virtual void                        reset();

            double                              _orig_edgelen_top;
            double                              _orig_edgelen_middle;
            double                              _orig_edgelen_bottom;

            unsigned                            _case;
            bool                                _topology_changed;
            Node *                              _x;
            Node *                              _y;
            Node *                              _a;
            Node *                              _b;
    };

    inline TreeUpdater::TreeUpdater() {
        // std::cout << "Creating a TreeUpdater" << std::endl;
        Updater::clear();
        _name = "Tree Topol. and Edge Prop.";
        reset();
    }

    inline TreeUpdater::~TreeUpdater() {
        // std::cout << "Destroying a TreeUpdater" << std::endl;
    }
    
    inline void TreeUpdater::reset() {
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
    }

    inline double TreeUpdater::calcLogPrior() {
        double log_topology_prior    = Updater::calcLogTopologyPrior();
        double log_edge_length_prior = Updater::calcLogEdgeLengthPrior();
        return log_topology_prior + log_edge_length_prior;
    }

    // This version uses a polytomy-aware NNI swap
    inline void TreeUpdater::proposeNewState() {
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
        
        _x = _tree_manipulator->randomInternalEdge(_lot);
        _orig_edgelen_middle = _x->getEdgeLength();

        _y = _x->getParent();

        // Choose focal 3-edge segment to modify
        // Begin by randomly choosing one child of x to be node _a
        _a = _tree_manipulator->randomChild(_lot, _x, 0, false);
        _orig_edgelen_top = _a->getEdgeLength();

        // Now choose a different child of x (or the parent) to be node _b
        _b = _tree_manipulator->randomChild(_lot, _y, _x, true);
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
                    _a->setEdgeLength(_orig_edgelen_top + _orig_edgelen_middle);
                    _x->setEdgeLength(new_attachment_point - _orig_edgelen_top - _orig_edgelen_middle);
                    _b->setEdgeLength(focal_path_length - new_attachment_point);
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
                    _a->setEdgeLength(new_attachment_point);
                    _x->setEdgeLength(_orig_edgelen_top - new_attachment_point);
                    _b->setEdgeLength(_orig_edgelen_middle + _orig_edgelen_bottom);
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
    }
    
    inline void TreeUpdater::revert() {
        assert(_case > 0 && _case < 9);
        if (_case == 2 || _case == 6)
            _tree_manipulator->LargetSimonSwap(_a, _b);
        else if (_case == 1 || _case == 5)
            _tree_manipulator->LargetSimonSwap(_b, _a);
        _a->setEdgeLength(_orig_edgelen_top);
        _x->setEdgeLength(_orig_edgelen_middle);
        if (_case == 1 || _case == 3 || _case == 5 || _case == 7)
            _b->setEdgeLength(_orig_edgelen_bottom);    // not actually necessary for case 3
        else
            _y->setEdgeLength(_orig_edgelen_bottom);    // not actually necessary for case 4
        }
    } 

}
