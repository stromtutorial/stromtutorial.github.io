#pragma once

#include <string>
#include <vector>
#include  <iostream>
//#include "split.hpp"

namespace strom {

    class Tree;
    class TreeManip;
    //class Likelihood;
    //class Updater;

    class Node {
            friend class Tree;
            friend class TreeManip;
            //friend class Likelihood;
            //friend class Updater;

        public:
                                        Node();
                                        ~Node();

                    Node *              getParent()     {return _parent;}
                    Node *              getLeftChild()  {return _left_child;}
                    Node *              getRightSib()   {return _right_sib;}
                    int                 getNumber()     {return _number;}
                    std::string         getName()       {return _name;}
                    //Split               getSplit()      {return _split;}

                    double              getEdgeLength() {return _edge_length;}
                    void                setEdgeLength(double v);

            static const double _smallest_edge_length;

            typedef std::vector<Node>    Vector;
            typedef std::vector<Node *>  PtrVector;

        private:

            void                clear();

            Node *              _left_child;
            Node *              _right_sib;
            Node *              _parent;
            int                 _number;
            std::string         _name;
            double              _edge_length;
            //Split               _split;
    };

    inline Node::Node() {
        std::cout << "Creating Node object" << std::endl;
        clear();
    }

    inline Node::~Node() {
        std::cout << "Destroying Node object" << std::endl;
    }

    inline void Node::clear() {
        _left_child = 0;
        _right_sib = 0;
        _parent = 0;
        _number = -1;
        _name = "";
        _edge_length = _smallest_edge_length;
    }

    inline void Node::setEdgeLength(double v) {
        _edge_length = (v < _smallest_edge_length ? _smallest_edge_length : v);
    }

}
