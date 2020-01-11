#pragma once	///start

#include <memory>
#include <iostream>
#include "node.hpp"

namespace strom {

    class TreeManip;	///!a
    //class Likelihood;
    //class Updater;

    class Tree {

            friend class TreeManip;		///!b
            //friend class Likelihood;
            //friend class Updater;

        public:

                                        Tree();
                                        ~Tree();

            bool                        isRooted() const;
            unsigned                    numLeaves() const;
            unsigned                    numInternals() const;
            unsigned                    numNodes() const;

            //void                        createTestTree();	///!g

        private:

            void                        clear();

            bool                        _is_rooted;
            Node *                      _root;
            unsigned                    _nleaves;
            unsigned                    _ninternals;
            Node::PtrVector             _preorder;
            Node::PtrVector             _levelorder;
            Node::Vector                _nodes;

        public:

            typedef std::shared_ptr<Tree> SharedPtr;
    };

    inline Tree::Tree() {
        std::cout << "Constructing a Tree" << std::endl;
        clear();	///!c
        //createTestTree();	///!d
    }

    inline Tree::~Tree() {
        std::cout << "Destroying a Tree" << std::endl;
    }

    inline void Tree::clear() {
        _is_rooted = false;
        _root = 0;
        _nodes.clear();
        _preorder.clear();
        _levelorder.clear();
    }

    inline bool Tree::isRooted() const {
        return _is_rooted;
    }

    inline unsigned Tree::numLeaves() const {
        return _nleaves;
    }

    inline unsigned Tree::numInternals() const {
        return _ninternals;
    }

    inline unsigned Tree::numNodes() const {
        return (unsigned)_nodes.size();
    }

    // inline void Tree::createTestTree() {	///!e
    // 	clear();
    // 	_nodes.resize(6);	
    // 
    // 	Node * root_node       = &_nodes[0];	
    // 	Node * first_internal  = &_nodes[1];
    // 	Node * second_internal = &_nodes[2];
    // 	Node * first_leaf      = &_nodes[3];
    // 	Node * second_leaf     = &_nodes[4];
    // 	Node * third_leaf      = &_nodes[5];	
    // 
    // 	// Here is the structure of the tree (numbers in
    // 	// parentheses are node numbers, other numbers
    // 	// are edge lengths):
    // 	//
    // 	// first_leaf (0)   second_leaf (1)   third_leaf (2)
    // 	//      \              /                  /
    // 	//       \ 0.1        / 0.1              /
    // 	//        \          /                  /
    // 	//     second_internal (3)             / 0.2
    // 	//             \                      /
    // 	//              \ 0.1                /
    // 	//               \                  /
    // 	//                first_internal (4)
    // 	//                        |
    // 	//                        | 0.1
    // 	//                        |
    // 	//                    root_node (5)
    // 	//
    // 	root_node->_parent            = 0;					
    // 	root_node->_left_child        = first_internal;
    // 	root_node->_right_sib         = 0;
    // 	root_node->_number            = 5;
    // 	root_node->_name              = "root node";
    // 	root_node->_edge_length       = 0.0;			
    // 
    // 	first_internal->_parent       = root_node;
    // 	first_internal->_left_child   = second_internal;
    // 	first_internal->_right_sib    = 0;
    // 	first_internal->_number       = 4;
    // 	first_internal->_name         = "first internal node";
    // 	first_internal->_edge_length  = 0.1;
    // 
    // 	second_internal->_parent      = first_internal;
    // 	second_internal->_left_child  = first_leaf;
    // 	second_internal->_right_sib   = third_leaf;
    // 	second_internal->_number      = 3;
    // 	second_internal->_name        = "second internal node";
    // 	second_internal->_edge_length = 0.1;
    // 
    // 	first_leaf->_parent           = second_internal;
    // 	first_leaf->_left_child       = 0;
    // 	first_leaf->_right_sib        = second_leaf;
    // 	first_leaf->_number           = 0;
    // 	first_leaf->_name             = "first leaf";
    // 	first_leaf->_edge_length      = 0.1;
    // 
    // 	second_leaf->_parent          = second_internal;
    // 	second_leaf->_left_child      = 0;
    // 	second_leaf->_right_sib       = 0;
    // 	second_leaf->_number          = 1;
    // 	second_leaf->_name            = "second leaf";
    // 	second_leaf->_edge_length     = 0.1;
    // 
    // 	third_leaf->_parent           = first_internal;
    // 	third_leaf->_left_child       = 0;
    // 	third_leaf->_right_sib        = 0;
    // 	third_leaf->_number           = 2;
    // 	third_leaf->_name             = "third leaf";
    // 	third_leaf->_edge_length      = 0.2;
    // 
    // 	_is_rooted             = true;		
    // 	_root                  = root_node;
    // 	_nleaves               = 3;			
    // 
    // 	// Note that root node is not included in _preorder
    // 	_preorder.push_back(first_internal);	
    // 	_preorder.push_back(second_internal);
    // 	_preorder.push_back(first_leaf);
    // 	_preorder.push_back(second_leaf);
    // 	_preorder.push_back(third_leaf);		
    // 
    // 	_levelorder.push_back(first_internal);	
    // 	_levelorder.push_back(second_internal);
    // 	_levelorder.push_back(third_leaf);
    // 	_levelorder.push_back(first_leaf);
    // 	_levelorder.push_back(second_leaf);		
    // }	///!f

} ///end
