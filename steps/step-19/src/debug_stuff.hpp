#pragma once

#include <stack>
#include <boost/format.hpp>
#include "tree.hpp"
#include "node.hpp"

namespace strom {

    struct DebugStuff {
            static unsigned    _debug_add_tries;    //POLTMP
            static unsigned    _debug_add_accepts;  //POLTMP
            static unsigned    _debug_del_tries;    //POLTMP
            static unsigned    _debug_del_accepts;  //POLTMP

            static bool        _ignore;
            static unsigned    _partial_offset;
            static unsigned    _tmatrix_offset;
            static unsigned    _which_iter;
            static unsigned    _tree_index;
            static void        debugOpenTreeFile();
            static void        debugCloseTreeFile();
            static void        debugSaveTree(std::string treename, std::string description);
            static std::string debugMakeNewick(Tree::SharedPtr tree, unsigned precision, bool js = true);
            static void        debugShowNodeSummary(Node * a, std::string label);
    };

    inline void DebugStuff::debugOpenTreeFile() {
        if (!_ignore) {
            std::ofstream treef("newicks.js");
            treef << "var newicks = [" << std::endl;
            treef.close();
        }
    }
    
    inline void DebugStuff::debugCloseTreeFile() {
        if (!_ignore) {
            std::ofstream treef("newicks.js", std::ios::out | std::ios::app);
            treef << "]" << std::endl;
            treef.close();
        }
    }
    
    inline void DebugStuff::debugSaveTree(std::string treename, std::string description) {
        if (!_ignore) {
            std::ofstream treef("newicks.js", std::ios::out | std::ios::app);
            
            // this version displays _tree_index
            //treef << boost::str(boost::format("     {name:\"%s_%d\", relrate:1.0, newick:\"%s\"},") % treename % _tree_index % description) << std::endl;
            
            // this version ignores _tree_index
            treef << boost::str(boost::format("     {name:\"%s\", relrate:1.0, newick:\"%s\"},") % treename  % description) << std::endl;
            
            treef.close();
            _tree_index++;
        }
    }
    
    inline std::string DebugStuff::debugMakeNewick(Tree::SharedPtr tree, unsigned precision, bool js) {
        std::string newick;
        if (_ignore)
            return newick;
            
#if 0
        const boost::format tip_node_format( boost::str(boost::format("{%%s}:%%.%df") % precision) );
        const boost::format internal_node_format( boost::str(boost::format("){%%s}:%%.%df") % precision) );
#else
        const boost::format js_tip_node_format( boost::str(boost::format("{%%s}:%%.%df") % precision) );
        const boost::format js_internal_node_format( boost::str(boost::format("){%%s}:%%.%df") % precision) );
        const boost::format plain_tip_node_format( boost::str(boost::format("%%s:%%.%df") % precision) );
        const boost::format plain_internal_node_format( boost::str(boost::format(")%%s:%%.%df") % precision) );
#endif
        std::stack<Node *> node_stack;

        //Node * root_tip = (tree->_is_rooted ? 0 : tree->_root);
        newick += "(";
        node_stack.push(tree->_root);
        for (auto nd : tree->_preorder) {
            //...
            if (nd->_left_child) {
                newick += "(";
                node_stack.push(nd);
            }
            else {
                unsigned node_number = nd->_number;
                unsigned tmatrix = node_number + (nd->isAltTMatrix() ? DebugStuff::_tmatrix_offset : 0);
                unsigned pselected = nd->isSelPartial() ? 1 : 0;
                unsigned tselected = nd->isSelTMatrix() ? 1 : 0;
#if 0
                std::string infostr = boost::str(boost::format("%d,null,%d,%d,%d") % node_number % pselected % tmatrix % tselected);
                newick += boost::str(boost::format(tip_node_format) % infostr % nd->_edge_length);
#else
                if (js) {
                    std::string infostr = boost::str(boost::format("%d,null,%d,%d,%d") % node_number % pselected % tmatrix % tselected);
                    newick += boost::str(boost::format(js_tip_node_format) % infostr % nd->_edge_length);
                }
                else {
                    std::string infostr = boost::str(boost::format("%d") % node_number);
                    newick += boost::str(boost::format(plain_tip_node_format) % infostr % nd->_edge_length);
                }
#endif
                if (nd->_right_sib)
                    newick += ",";
                else {
                    Node * popped = (node_stack.empty() ? 0 : node_stack.top());
                    while (popped && !popped->_right_sib) {
                        node_stack.pop();
                        unsigned node_number = popped->_number;
                        unsigned partial = node_number + (popped->isAltPartial() ? DebugStuff::_partial_offset : 0);
                        unsigned tmatrix = node_number + (popped->isAltTMatrix() ? DebugStuff::_tmatrix_offset : 0);
                        unsigned pselected = popped->isSelPartial() ? 1 : 0;
                        unsigned tselected = popped->isSelTMatrix() ? 1 : 0;
#if 0
                        std::string infostr = boost::str(boost::format("%d,%d,%d,%d,%d") % node_number % partial % pselected % tmatrix % tselected);
                        newick += boost::str(boost::format(internal_node_format) % infostr % popped->_edge_length);
#else
                        if (js) {
                            std::string infostr = boost::str(boost::format("%d,%d,%d,%d,%d") % node_number % partial % pselected % tmatrix % tselected);
                            newick += boost::str(boost::format(js_internal_node_format) % infostr % popped->_edge_length);
                        }
                        else {
                            std::string infostr = boost::str(boost::format("%d") % node_number);
                            newick += boost::str(boost::format(plain_internal_node_format) % infostr % popped->_edge_length);
                        }
#endif
                        popped = (node_stack.empty() ? 0 : node_stack.top());
                    }
                    if (popped && popped->_right_sib) {
                        node_stack.pop();
                        unsigned node_number = popped->_number;
                        unsigned partial = node_number + (popped->isAltPartial() ? DebugStuff::_partial_offset : 0);
                        unsigned tmatrix = node_number + (popped->isAltTMatrix() ? DebugStuff::_tmatrix_offset : 0);
                        unsigned pselected = popped->isSelPartial() ? 1 : 0;
                        unsigned tselected = popped->isSelTMatrix() ? 1 : 0;
#if 0
                        std::string infostr = boost::str(boost::format("%d,%d,%d,%d,%d") % node_number % partial % pselected % tmatrix % tselected);
                        newick += boost::str(boost::format(internal_node_format) % infostr % popped->_edge_length);
#else
                        if (js) {
                            std::string infostr = boost::str(boost::format("%d,%d,%d,%d,%d") % node_number % partial % pselected % tmatrix % tselected);
                            newick += boost::str(boost::format(js_internal_node_format) % infostr % popped->_edge_length);
                        }
                        else {
                            std::string infostr = boost::str(boost::format("%d") % node_number);
                            newick += boost::str(boost::format(plain_internal_node_format) % infostr % popped->_edge_length);
                        }
#endif
                        newick += ",";
                    }
                }
            }
        }

        return newick;
    }
    
    inline void DebugStuff::debugShowNodeSummary(Node * a, std::string label) {
        if (!_ignore) {
            std::cerr << boost::str(boost::format("Node %d (\"%s\")") % a->getNumber() % label) << std::endl;

            if (a->getLeftChild())
                std::cerr << boost::str(boost::format("--- lchild = %d") % a->getLeftChild()->getNumber()) << std::endl;
            else
                std::cerr << boost::str(boost::format("--- lchild = %s") % "NULL") << std::endl;
            
            if (a->getRightSib())
                std::cerr << boost::str(boost::format("--- rsib   = %d") % a->getRightSib()->getNumber()) << std::endl;
            else
                std::cerr << boost::str(boost::format("--- rsib   = %s") % "NULL") << std::endl;
            
            if (a->getParent())
                std::cerr << boost::str(boost::format("--- parent = %d") % a->getParent()->getNumber()) << std::endl;
            else
                std::cerr << boost::str(boost::format("--- parent = %s") % "NULL") << std::endl;
        }
    }

}
