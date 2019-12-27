#pragma once    ///start

#include "updater.hpp"

#define POLNEWWAY 1

namespace strom {

    class Chain;

    class PolytomyUpdater : public Updater {

        friend class Chain;

        public:

            typedef std::vector<double>                         _partition_vect_t;
            typedef std::map<unsigned, _partition_vect_t >      _partition_map_t;
            typedef std::vector<Node *>                         _polytomy_vect_t;
            typedef std::shared_ptr< PolytomyUpdater >          SharedPtr;

                                                PolytomyUpdater();
                                                ~PolytomyUpdater();

            virtual double                      calcLogPrior();
            
        private:

            virtual void                        revert();
            virtual void                        proposeNewState();
            virtual void                        reset();
            
            void                                proposeAddEdgeMove(Node * nd);
            void                                proposeDeleteEdgeMove(Node * nd);
            
            _partition_vect_t &                 computePolytomyDistribution(unsigned nspokes);
            void                                refreshPolytomies();

            _partition_map_t                    _poly_prob;
            _polytomy_vect_t                    _polytomies;
            
            Node *                              _orig_par;
            Node *                              _orig_lchild;

            bool                                _add_edge_proposed;
            double                              _new_edge_proportion;
            double                              _orig_edge_proportion;
            double                              _tree_length;
            double                              _phi;
            unsigned                            _polytomy_size;
            unsigned                            _num_polytomies;
    };

    // Member function bodies go here
    ///end_class_declaration
    inline PolytomyUpdater::PolytomyUpdater() { ///begin_constructor
        // std::cout << "Creating a PolytomyUpdater" << std::endl;
        Updater::clear();
        _name = "Polytomies";
        reset();
    }   ///end_constructor

    inline PolytomyUpdater::~PolytomyUpdater() {    ///begin_destructor
        // std::cout << "Destroying a PolytomyUpdater" << std::endl;
    }   ///end_destructor
    
    inline void PolytomyUpdater::reset() {  ///begin_reset
        _tree_length          = 0.0;
        _new_edge_proportion  = 0.0;
        _orig_edge_proportion = 0.0;
        _phi                  = 0.5;
        _orig_par             = 0;
        _orig_lchild          = 0;
        _polytomy_size        = 0;
        _num_polytomies       = 0;
        _add_edge_proposed    = false;
        _polytomies.clear();
    }   ///end_reset

    inline double PolytomyUpdater::calcLogPrior() {   ///begin_calcLogPrior
        double log_prior = 0.0;
        log_prior += Updater::calcLogTopologyPrior();
        log_prior += Updater::calcLogEdgeLengthPrior();
        return log_prior;
    }   ///end_calcLogPrior

inline PolytomyUpdater::_partition_vect_t & PolytomyUpdater::computePolytomyDistribution(unsigned nspokes) {    ///begin_computePolytomyDistribution
    assert(nspokes > 2);
            
    // Only compute it if it isn't already stored in the _poly_prob map
    auto iter = _poly_prob.find(nspokes);
    if (iter == _poly_prob.end()) {
        // There is no existing probability distribution vector corresponding to nspokes
        double ln_denom = std::log(pow(2.0,nspokes-1) - nspokes - 1.0);
        _partition_vect_t v(nspokes - 3);
        unsigned first = 2;
        unsigned last = nspokes/2;
        bool nspokes_even = nspokes % 2 == 0;
        double total_prob = 0.0;
        for (unsigned x = first; x <= last; ++x) {
            double ln_numer = std::lgamma(nspokes + 1) - std::lgamma(x + 1) - std::lgamma(nspokes - x + 1);
            if (nspokes_even && x == last)
                ln_numer -= std::log(2);
            double prob_x = exp(ln_numer - ln_denom);
            if (prob_x > 1.0)
                prob_x = 1.0;
            total_prob += prob_x;
            v[x-first] = prob_x;
            }
        assert(std::fabs(total_prob - 1.0) < 1.e-8);
        _poly_prob[nspokes] = v;
    }
    return _poly_prob[nspokes];
} ///end_computePolytomyDistribution
        
    inline void PolytomyUpdater::proposeNewState() {    ///begin_proposeNewState
        Tree::SharedPtr tree = _tree_manipulator->getTree();
        
        // Translate tuning parameter _lambda into the maximum possible proportion
        // that a newly created edge could have
        if (_lambda > 1000.0)
            _lambda = 1000.0;
#if 0 //POLTMP
        else if (_lambda < 0.1)
            _lambda = 0.1;
#else
        else if (_lambda < 1.0)
            _lambda = 1.0;
#endif
        _phi = 1.0 - std::exp(-_lambda);
        
#if 1 //POLTMP
        double num_edges_in_fully_resolved_tree = 0;
        if (tree->isRooted())
            num_edges_in_fully_resolved_tree = 2*tree->numLeaves() - 2;
        else
            num_edges_in_fully_resolved_tree = 2*tree->numLeaves() - 3;
        _phi = 1.0/num_edges_in_fully_resolved_tree;
        const double pradd = 0.5; // probability of choosing and add-edge move if both add-edge and delete-edge are possible
#endif

        // Compute number of internal nodes in a fully resolved tree
        unsigned num_internals_in_fully_resolved_tree = 0;
        if (tree->isRooted())
            num_internals_in_fully_resolved_tree = tree->numLeaves(); // remember: root node is internal (has lchild)
        else
            num_internals_in_fully_resolved_tree = tree->numLeaves() - 2;
            
        // Compute tree length before proposed move
        _tree_length = _tree_manipulator->calcTreeLength();

        // Determine whether starting tree is fully resolved or the star tree
        unsigned num_internals_before = tree->numInternals();
        unsigned num_leaves_before = tree->numLeaves();
        unsigned num_internal_edges_before = num_internals_before - (tree->isRooted() ? 2 : 1);
        unsigned total_edges_before = num_leaves_before + num_internal_edges_before;
        bool fully_resolved_before = (num_internals_in_fully_resolved_tree == num_internals_before);
        bool star_tree_before = (tree->numInternals() == 1);
        
        // Rebuild _polytomies vector, which is a vector of pointers to Node objects having more than 2 children
        refreshPolytomies();
        _num_polytomies = (unsigned)_polytomies.size();
        
        // Determine whether an add edge move is proposed (alternative is a delete edge move)
        if (star_tree_before)
            _add_edge_proposed = true;
        else if (fully_resolved_before)
            _add_edge_proposed = false;
        else
#if 1 //POLTMP
            _add_edge_proposed = (_lot->uniform() < pradd);
#else
            _add_edge_proposed = (_lot->uniform() < 0.5);
#endif

        Node * nd = 0;
        if (_add_edge_proposed) {
            // Choose a polytomy at random to split
            unsigned i = _lot->randint(0, _num_polytomies-1);
            nd = _polytomies[i];
            _polytomy_size = 1 + _tree_manipulator->countChildren(nd);

            // Add an edge to split up polytomy at nd, moving a random subset
            // of the spokes to the (new) left child of nd
            proposeAddEdgeMove(nd);
            Node * new_nd = nd->getLeftChild();

            double TL_after_add_edge = _tree_manipulator->calcTreeLength();
            assert(std::fabs(_tree_length - TL_after_add_edge) < 1.e-8);

            // Compute the log of the Hastings ratio
            _log_hastings_ratio  = 0.0;
            _log_hastings_ratio += std::log(_num_polytomies);
            _log_hastings_ratio += std::log(pow(2.0, _polytomy_size - 1) - _polytomy_size - 1);
            _log_hastings_ratio -= std::log(num_internal_edges_before + 1);
            
            // Now multiply by the value of the quantity labeled gamma_b in the Lewis-Holder-Holsinger (2005) paper
            unsigned num_internals_after = tree->numInternals();
            assert(num_internals_after == num_internals_before + 1);
            const bool fully_resolved_after = (num_internals_after == num_internals_in_fully_resolved_tree);
            double tmp = 0.0;
#if 1 //POLTMP
            if (star_tree_before && !fully_resolved_after)
                tmp += log(1.0 - pradd);
            else if (fully_resolved_after && !star_tree_before)
                tmp -= log(pradd);
            else if (!star_tree_before && !fully_resolved_after) {
                tmp += log(1.0 - pradd);
                tmp -= log(pradd);
            }
#else
            if (star_tree_before && !fully_resolved_after)
                tmp -= log(2.0);
            else if (fully_resolved_after && !star_tree_before)
                tmp += log(2.0);
#endif
            //Updater::_om->outputParameterDebugInfo(boost::str(boost::format("tmp (add) = %.5f") % tmp));
            _log_hastings_ratio += tmp;

            // Compute the log of the Jacobian
            _log_jacobian = 0.0;
            _log_jacobian += std::log(_phi);
            _log_jacobian += (total_edges_before - 1)*std::log(1.0 - _new_edge_proportion);

            // flag partials and transition matrices for recalculation
#if POLNEWWAY
            _tree_manipulator->selectAllPartials();
            _tree_manipulator->selectAllTMatrices();
#else
            _tree_manipulator->selectPartialsHereToRoot(new_nd);
            new_nd->selectTMatrix();
#endif
        }
        else {
            // Choose an internal edge at random and delete it to create a polytomy
            // (or a bigger polytomy if there is already a polytomy)
            nd = _tree_manipulator->randomInternalEdge(_lot->uniform());
            proposeDeleteEdgeMove(nd);

            double TL_after_del_edge = _tree_manipulator->calcTreeLength();
            assert(std::fabs(_tree_length - TL_after_del_edge) < 1.e-8);

            // Compute the log of the Hastings ratio
            _log_hastings_ratio  = 0.0;
            _log_hastings_ratio += std::log(num_internal_edges_before);
            _log_hastings_ratio -= std::log(_num_polytomies);
            _log_hastings_ratio -= std::log(pow(2.0, _polytomy_size - 1) - _polytomy_size - 1);

            // Now multiply by the value of the quantity labeled gamma_b in the Lewis-Holder-Holsinger (2005) paper
            // Now multiply by the value of the quantity labeled gamma_d in the paper
            unsigned num_internals_after = tree->numInternals();
            assert(num_internals_after == num_internals_before - 1);
            const bool star_tree_after = (num_internals_after == (tree->isRooted() ? 2 : 1));
            double tmp = 0.0;
#if 1 //POLTMP
            if (fully_resolved_before && !star_tree_after)
                tmp = log(pradd);
            else if (star_tree_after && !fully_resolved_before)
                tmp -= log(1.0 - pradd);
            else if (!fully_resolved_before && !star_tree_after) {
                tmp += log(pradd);
                tmp -= log(1.0 - pradd);
            }
#else
            if (fully_resolved_before && !star_tree_after)
                tmp -= log(2.0);
            else if (star_tree_after && !fully_resolved_before)
                tmp += log(2.0);
#endif
            //Updater::_om->outputParameterDebugInfo(boost::str(boost::format("tmp (del) = %.5f") % tmp));
            _log_hastings_ratio += tmp;

            // Compute the log Jacobian
            _log_jacobian = 0.0;
            _log_jacobian -= std::log(_phi);
            _log_jacobian -= (total_edges_before - 2)*std::log(1.0 - _orig_edge_proportion);

            // flag partials and transition matrices for recalculation
#if POLNEWWAY
            _tree_manipulator->selectAllPartials();
            _tree_manipulator->selectAllTMatrices();
#else
            _tree_manipulator->selectPartialsHereToRoot(_orig_par);
#endif
        }
    }   ///end_proposeNewState
    
    inline void PolytomyUpdater::proposeAddEdgeMove(Node * u) {    ///begin_proposeAddEdgeMove
        Tree::SharedPtr tree = _tree_manipulator->getTree();

        // Split up the polytomy at `u' by creating a new internal node v and a new edge
        // connecting u with v. Node u is saved as _orig_par and node v is saved
        // as _orig_lchild in case we need to revert the proposed move.
        assert(u);

        // Calculate (if necessary) the probability of each possible partitioning of the chosen polytomy
        // Select number of spokes to move over to new node
        // Note that 0 and 1 are not allowed because they
        // would leave the tree in an invalid state
        const _partition_vect_t & prob_n = computePolytomyDistribution(_polytomy_size);
        double p = _lot->uniform();
        double cum = 0.0;
        unsigned k = 0;
        bool found = false;
        for (; k < prob_n.size(); ++k) {
            double prob_k_given_n = prob_n[k];
            cum += prob_k_given_n;
            if (p < cum) {
                found = true;
                break;
            }
        }
        assert(found);
        k += 2;

        // Create the new node that will receive the k randomly-chosen spokes
        Node * v = _tree_manipulator->getUnusedNode();
        _new_edge_proportion = _lot->uniform()*_phi;
        _tree_manipulator->scaleAllEdgeLengths(1.0 - _new_edge_proportion);
        v->setEdgeLength(_new_edge_proportion*_tree_length);
        _tree_manipulator->rectifyNumInternals(+1);
        _tree_manipulator->insertSubtreeOnLeft(v, u);
        assert(u->getLeftChild() == v);

        // Save u and v. If revert is necessary, all of orig_lchild's nodes will be returned
        // to orig_par, and orig_lchild will be deleted.
        _orig_par    = u;
        _orig_lchild = v;

        // After the move, either v should have k spokes and the
        // other node _polytomy_size - k spokes (u and v will each
        // have 1 additional connector spoke).
        //
        // Choose k spokes randomly out of the _polytomy_size available.
        // If u->par is included, let u retain the k spokes and move
        // _polytomy_size - k spokes to v. Otherwise, move the
        // k spokes to v leaving _polytomy_size - k spokes behind.
        
        // Create vector of valid spokes (parent and all children of u except v)
        std::vector<Node *> uspokes;
        uspokes.push_back(u->getParent());
        for (Node * uchild = u->getLeftChild(); uchild; uchild = uchild->getRightSib()) {
            if (uchild != v)
                uspokes.push_back(uchild);
        }
        assert (uspokes.size() == _polytomy_size);

        bool reverse_polarity = false;
        std::vector<Node *> vspokes;
        typedef std::vector<Node *>::iterator::difference_type vec_it_diff;
        for (unsigned i = 0; i < k; ++i) {
            unsigned num_u_spokes = (unsigned)uspokes.size();
            assert(num_u_spokes > 0);
            unsigned j = _lot->randint(0, num_u_spokes-1);
            Node * s = uspokes[j];
            if (s == u->getParent())
                reverse_polarity = true;
            vspokes.push_back(s);
            uspokes.erase(uspokes.begin() + (vec_it_diff)j);
            }
        assert(uspokes.size() + vspokes.size() == _polytomy_size);

        if (reverse_polarity) {
            // transfer nodes in uspokes to v
            for (auto s = uspokes.begin(); s != uspokes.end(); ++s) {
                _tree_manipulator->detachSubtree(*s);
                _tree_manipulator->insertSubtreeOnRight(*s, v);
            }
        }
        else {
            // transfer nodes in vspokes to v
            for (auto s = vspokes.begin(); s != vspokes.end(); ++s) {
                _tree_manipulator->detachSubtree(*s);
                _tree_manipulator->insertSubtreeOnRight(*s, v);
            }
        }
        
        _tree_manipulator->refreshNavigationPointers();
    }   ///end_proposeAddEdgeMove
    
    inline void PolytomyUpdater::proposeDeleteEdgeMove(Node * u) {    ///begin_proposeDeleteEdgeMove
        // Delete the edge associated with `u' to create a polytomy (or a bigger polytomy if `u->par' was already a polytomy).
        // The supplied node u should not be the only child of the root node.
        //
        //       b       c
        //        \     /
        //         \   /
        //          \ /
        //   a       u           a   b   c
        //    \     /            \  |  /
        //     \   /              \ | /
        //      \ /                \|/
        //       v                  v
        //      /                  /
        //
        //     Before           After
        //
        // Returns the number of polytomies in the tree after the proposed delete-edge move.
        // The return value will be incorrect if the polytomies vector is not up-to-date.
        
        // Save u's edge length in case we need to revert
        _orig_edge_proportion = u->getEdgeLength()/_tree_length;

        // This operation should not leave the root node (which is a tip) with more than one
        // child, so check to make sure that the supplied node is not the root nor a child of root
        _orig_par = u->getParent();
        assert(_orig_par);
        assert(_orig_par->getParent());

        // Compute size of polytomy after the delete-edge move, a quantity that is needed for computing the Hastings ratio.
        // Note that one of v's children (i.e. u) is deleted but this is made up for by considering v->par, which is
        // also a spoke that counts.
        unsigned u_children = _tree_manipulator->countChildren(u);
        unsigned v_children = _tree_manipulator->countChildren(_orig_par);
        _polytomy_size = v_children + u_children;

        bool u_polytomy_before = (u_children > 2);
        bool v_polytomy_before = (v_children > 2);
        if (u_polytomy_before && v_polytomy_before) {
            // No. polytomies will decrease by one as a result of this delete-edge move
            --_num_polytomies;
        }
        else if (!u_polytomy_before && !v_polytomy_before) {
            // No. polytomies will increase by one as a result of this delete-edge move
            ++_num_polytomies;
        }

        // Make all of u's children siblings of u (i.e. children of u->par)
        _orig_lchild = u->getLeftChild();
        while (u->getLeftChild() != NULL) {
            Node * tmp = u->getLeftChild();
            _tree_manipulator->detachSubtree(tmp);
            _tree_manipulator->insertSubtreeOnRight(tmp, _orig_par);
        }

        _tree_manipulator->detachSubtree(u);
        _tree_manipulator->putUnusedNode(u);
        _tree_manipulator->rectifyNumInternals(-1);
        
        _tree_manipulator->refreshNavigationPointers();
        assert(_orig_edge_proportion < 1.0);
        double scaler = 1.0/(1.0 - _orig_edge_proportion);
        _tree_manipulator->scaleAllEdgeLengths(scaler);
    }   ///end_proposeDeleteEdgeMove
    
    inline void PolytomyUpdater::revert() { ///begin_revert
        if (_add_edge_proposed) {
            // Return all of _orig_lchild's child nodes to _orig_par, then return orig_lchild to storage
            Node * child = _orig_lchild->getLeftChild();
            while (child) {
                Node * rsib = child->getRightSib();
                _tree_manipulator->detachSubtree(child);
                _tree_manipulator->insertSubtreeOnRight(child, _orig_par);
                child = rsib;
            }
            _tree_manipulator->detachSubtree(_orig_lchild);
            _tree_manipulator->putUnusedNode(_orig_lchild);
            _tree_manipulator->rectifyNumInternals(-1);
            
            _tree_manipulator->refreshNavigationPointers();
            assert(_new_edge_proportion < 1.0);
            double scaler = 1.0/(1.0 - _new_edge_proportion);
            _tree_manipulator->scaleAllEdgeLengths(scaler);
            double TL_after_revert_add_edge = _tree_manipulator->calcTreeLength();
            assert(std::fabs(_tree_length -TL_after_revert_add_edge) < 1.e-8);
        }
        else {
            Node * v = _tree_manipulator->getUnusedNode();
            _tree_manipulator->rectifyNumInternals(+1);
            _tree_manipulator->scaleAllEdgeLengths(1.0 - _orig_edge_proportion);
            v->setEdgeLength(_orig_edge_proportion*_tree_length);
            for (Node * child = _orig_lchild; child;) {
                Node * child_rsib = child->getRightSib();
                _tree_manipulator->detachSubtree(child);
                _tree_manipulator->insertSubtreeOnRight(child, v);
                child = child_rsib;
            }
            _tree_manipulator->insertSubtreeOnRight(v, _orig_par);

            _tree_manipulator->refreshNavigationPointers();
            double TL_after_revert_del_edge = _tree_manipulator->calcTreeLength();
            assert(std::fabs(_tree_length - TL_after_revert_del_edge) < 1.e-8);
        }
    }   ///end_revert

    inline void PolytomyUpdater::refreshPolytomies() {  ///begin_refreshPolytomies
        Tree::SharedPtr tree = _tree_manipulator->getTree();
        _polytomies.clear();
        for (auto nd : tree->_preorder) {
            unsigned s = _tree_manipulator->countChildren(nd);
            if (s > 2)
                _polytomies.push_back(nd);
        }
    }   ///end_refreshPolytomies

}   ///end
