#pragma once

#include <stack>
#include <memory>
#include <iostream>
#include "node.hpp"

namespace strom {

    class TreeManip;
    class Likelihood;
    class Updater;
    class PolytomyUpdater;

    class DebugStuff;   //DEBUGSTUFF

    class Tree {

            friend class TreeManip;
            friend class Likelihood;
            friend class Updater;
            friend class PolytomyUpdater;

            friend class DebugStuff;   //DEBUGSTUFF
        
        public:

                                        Tree();
                                        ~Tree();

            bool                        isRooted() const;
            unsigned                    numLeaves() const;
            unsigned                    numInternals() const;
            unsigned                    numNodes() const;
        
            Node *                      getNodeWithIndex(unsigned i);
            unsigned                    getUnusedNode();
            void                        putUnusedNodeIndex(unsigned num);
            void                        putUnusedNodePtr(Node * nd);

        private:

            void                        clear();

            bool                        _is_rooted;
            Node *                      _root;
            unsigned                    _nleaves;
            unsigned                    _ninternals;
            Node::PtrVector             _preorder;
            Node::PtrVector             _levelorder;
            Node::Vector                _nodes;
            std::stack<unsigned>        _unused_nodes;

        public:

            typedef std::shared_ptr<Tree> SharedPtr;
    };

    inline Tree::Tree() {
        //std::cout << "Constructing a Tree" << std::endl;
        clear();
    }

    inline Tree::~Tree() {
        //std::cout << "Destroying a Tree" << std::endl;
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
    
    inline unsigned Tree::getUnusedNode() {
        unsigned i = _unused_nodes.top();
        _unused_nodes.pop();
        _nodes[i].clearPointers();
        return i;
    }
    
    inline void Tree::putUnusedNodeIndex(unsigned num) {
        _unused_nodes.push(num);
    }
    
    inline void Tree::putUnusedNodePtr(Node * nd) {
        unsigned i = 0;
        for (i = 0; i < _nodes.size(); ++i) {
            if (nd == &_nodes[i]) {
                _unused_nodes.push(i);
                break;
            }
        }
        assert(i < _nodes.size());
    }
    
    inline Node * Tree::getNodeWithIndex(unsigned i) {
        assert(i < _nodes.size());
        Node * nd = &_nodes[i];
        return nd;
    }
    
}
