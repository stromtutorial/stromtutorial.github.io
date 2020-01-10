#pragma once    ///start

#include <string>
#include <vector>
#include  <iostream>
#include "split.hpp"

namespace strom {

    class Tree;
    class TreeManip;
    class Likelihood;
    class Updater;

    class Node {
        friend class Tree;
        friend class TreeManip;
        friend class Likelihood;
        friend class Updater;

        public:
                                        Node();
                                        ~Node();

                    Node *              getParent()                 {return _parent;}
                    Node *              getLeftChild()              {return _left_child;}
                    Node *              getRightSib()               {return _right_sib;}
                    int                 getNumber()                 {return _number;}
                    std::string         getName()                   {return _name;}
                    Split               getSplit()                  {return _split;}
        
                    bool                isSelected()                {return _flags & Flag::Selected;}
                    void                select()                    {_flags |= Flag::Selected;}
                    void                deselect()                  {_flags &= ~Flag::Selected;}

                    bool                isSelPartial()              {return _flags & Flag::SelPartial;}
                    void                selectPartial()             {_flags |= Flag::SelPartial;}
                    void                deselectPartial()           {_flags &= ~Flag::SelPartial;}

                    bool                isSelTMatrix()              {return _flags & Flag::SelTMatrix;}
                    void                selectTMatrix()             {_flags |= Flag::SelTMatrix;}
                    void                deselectTMatrix()           {_flags &= ~Flag::SelTMatrix;}

                    bool                isAltPartial()              {return _flags & Flag::AltPartial;}
                    void                setAltPartial()             {_flags |= Flag::AltPartial;}
                    void                clearAltPartial()           {_flags &= ~Flag::AltPartial;}

                    bool                isAltTMatrix()              {return _flags & Flag::AltTMatrix;}
                    void                setAltTMatrix()             {_flags |= Flag::AltTMatrix;}
                    void                clearAltTMatrix()           {_flags &= ~Flag::AltTMatrix;}
                    
                    void                flipTMatrix()               {isAltTMatrix() ? clearAltTMatrix() : setAltTMatrix();}
                    void                flipPartial()               {isAltPartial() ? clearAltPartial() : setAltPartial();}

                    double              getEdgeLength()             {return _edge_length;}
                    void                setEdgeLength(double v);

                    void                clearPointers()             {_left_child = _right_sib = _parent = 0;}   ///!a
                                        
            static const double _smallest_edge_length;

            typedef std::vector<Node>    Vector;
            typedef std::vector<Node *>  PtrVector;
        
        private:
        
            enum Flag {
                Selected   = (1 << 0),
                SelPartial = (1 << 1),
                SelTMatrix = (1 << 2),
                AltPartial = (1 << 3),
                AltTMatrix = (1 << 4)
            };

            void                clear();

            Node *              _left_child;
            Node *              _right_sib;
            Node *              _parent;
            int                 _number;
            std::string         _name;
            double              _edge_length;
            Split               _split;
            int                 _flags;
    };
    ///end_class_declaration
    
    inline Node::Node() {
        //std::cout << "Creating Node object" << std::endl;
        clear();
    }

    inline Node::~Node() {
        //std::cout << "Destroying Node object" << std::endl;
    }

    inline void Node::clear() { ///begin_clear
        _flags = 0;
        clearPointers();    ///!b
        //_left_child = 0;
        //_right_sib = 0;
        //_parent = 0;      ///!bb
        _number = -1;
        _name = "";
        _edge_length = _smallest_edge_length;
    }   ///end_clear

    inline void Node::setEdgeLength(double v) {
        _edge_length = (v < _smallest_edge_length ? _smallest_edge_length : v);
    }

}
