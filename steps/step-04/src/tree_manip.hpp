#pragma once	///start

#include <cassert>
#include <memory>
#include "tree.hpp"

namespace strom {

    class TreeManip {
        public:
                                        TreeManip();
                                        TreeManip(Tree::SharedPtr t);
                                        ~TreeManip();

            void                        setTree(Tree::SharedPtr t);
            Tree::SharedPtr             getTree();
            double                      calcTreeLength() const;
            unsigned                    countEdges() const;
            void                        scaleAllEdgeLengths(double scaler);
            void                        createTestTree();
            void                        clear();

        private:

            Tree::SharedPtr             _tree;

        public:

            typedef std::shared_ptr< TreeManip > SharedPtr;
    };

    // This is where function bodies go

///bodies
    inline TreeManip::TreeManip() {  ///begin_constructor
        std::cerr << "Constructing a TreeManip" << std::endl;
        clear();
    }

    inline TreeManip::TreeManip(Tree::SharedPtr t) {
        std::cerr << "Constructing a TreeManip with a supplied tree" << std::endl;
        clear();
        setTree(t);
    } 

    inline TreeManip::~TreeManip() {
        std::cerr << "Destroying a TreeManip" << std::endl;
    }  ///end_destructor

    inline void TreeManip::clear() {  ///begin_clear
        _tree.reset();
    }  ///end_clear

    inline void TreeManip::setTree(Tree::SharedPtr t) {  ///begin_setTree
        assert(t);
        _tree = t;
    }  ///end_setTree

    inline Tree::SharedPtr TreeManip::getTree() {  ///begin_getTree
        return _tree;
    }  ///end_getTree

    inline double TreeManip::calcTreeLength() const {  ///begin_calcTreeLength
        double TL = 0.0;
        for (auto nd : _tree->_preorder) {
            TL += nd->_edge_length;
        } 
        return TL;
    }  ///end_calcTreeLength

    inline unsigned TreeManip::countEdges() const { ///begin_countEdges
        return (unsigned)_tree->_preorder.size();
    } ///end_countEdges

    inline void TreeManip::scaleAllEdgeLengths(double scaler) {  ///begin_scaleAllEdgeLengths
        for (auto nd : _tree->_preorder) {
            nd->setEdgeLength(scaler*nd->_edge_length);
        }
    }  ///end_scaleAllEdgeLengths

    inline void TreeManip::createTestTree() {  ///begin_createTestTree
        clear();
        _tree = Tree::SharedPtr(new Tree());
        _tree->_nodes.resize(6);

        Node * root_node       = &_tree->_nodes[0];
        Node * first_internal  = &_tree->_nodes[1];
        Node * second_internal = &_tree->_nodes[2];
        Node * first_leaf      = &_tree->_nodes[3];
        Node * second_leaf     = &_tree->_nodes[4];
        Node * third_leaf      = &_tree->_nodes[5];

        // Here is the structure of the tree (numbers in
        // parentheses are node numbers, other numbers
        // are edge lengths):
        //
        // first_leaf (0)   second_leaf (1)   third_leaf (2)
        //      \              /                  /
        //       \ 0.1        / 0.1              /
        //        \          /                  /
        //     second_internal (3)             / 0.2
        //             \                      /
        //              \ 0.1                /
        //               \                  /
        //                first_internal (4)
        //                        |
        //                        | 0.1
        //                        |
        //                    root_node (5)
        //
        root_node->_parent = 0;
        root_node->_left_child = first_internal;
        root_node->_right_sib = 0;
        root_node->_number = 5;
        root_node->_name = "root_node";
        root_node->_edge_length = 0.0;

        first_internal->_parent = root_node;
        first_internal->_left_child = second_internal;
        first_internal->_right_sib = 0;
        first_internal->_number = 4;
        first_internal->_name = "first_internal_node";
        first_internal->_edge_length = 0.1;

        second_internal->_parent = first_internal;
        second_internal->_left_child = first_leaf;
        second_internal->_right_sib = third_leaf;
        second_internal->_number = 3;
        second_internal->_name = "second_internal_node";
        second_internal->_edge_length = 0.1;

        first_leaf->_parent = second_internal;
        first_leaf->_left_child = 0;
        first_leaf->_right_sib = second_leaf;
        first_leaf->_number = 0;
        first_leaf->_name = "first_leaf";
        first_leaf->_edge_length = 0.1;

        second_leaf->_parent = second_internal;
        second_leaf->_left_child = 0;
        second_leaf->_right_sib = 0;
        second_leaf->_number = 1;
        second_leaf->_name = "second_leaf";
        second_leaf->_edge_length = 0.1;

        third_leaf->_parent = first_internal;
        third_leaf->_left_child = 0;
        third_leaf->_right_sib = 0;
        third_leaf->_number = 2;
        third_leaf->_name = "third_leaf";
        third_leaf->_edge_length = 0.2;

        _tree->_is_rooted = true;
        _tree->_root = root_node;
        _tree->_nleaves = 3;

        // Note that root node is not included in _preorder
        _tree->_preorder.push_back(first_internal);
        _tree->_preorder.push_back(second_internal);
        _tree->_preorder.push_back(first_leaf);
        _tree->_preorder.push_back(second_leaf);
        _tree->_preorder.push_back(third_leaf);

        _tree->_levelorder.push_back(first_internal);
        _tree->_levelorder.push_back(second_internal);
        _tree->_levelorder.push_back(third_leaf);
        _tree->_levelorder.push_back(first_leaf);
        _tree->_levelorder.push_back(second_leaf);
    }  ///end_createTestTree

} ///end
