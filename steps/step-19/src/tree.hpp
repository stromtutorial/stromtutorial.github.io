#pragma once    ///start

#include <stack>    ///!a
#include <memory>
#include <iostream>
#include "node.hpp"

namespace strom {

    class TreeManip;
    class Likelihood;
    class Updater;
    class TreeUpdater;
    class PolytomyUpdater;  ///!b
#if 1 //POLTMP
    class Strom;
#endif

    class Tree {

            friend class TreeManip;
            friend class Likelihood;
            friend class Updater;
            friend class TreeUpdater;
            friend class PolytomyUpdater;   ///!c
            
#if 1 //POLTMP
            friend class Strom;
#endif
        
        public:

                                        Tree();
                                        ~Tree();

            bool                        isRooted() const;
            unsigned                    numLeaves() const;
            unsigned                    numInternals() const;
            unsigned                    numNodes() const;
        
        private:

            void                        clear();

            bool                        _is_rooted;
            Node *                      _root;
            unsigned                    _nleaves;
            unsigned                    _ninternals;
            Node::PtrVector             _preorder;
            Node::PtrVector             _levelorder;
            Node::Vector                _nodes;
            std::stack<Node *>          _unused_nodes;  ///!d

        public:

            typedef std::shared_ptr<Tree> SharedPtr;
    };
    ///end_class_declaration

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

}
