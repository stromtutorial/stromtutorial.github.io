#pragma once    ///start

#include "updater.hpp"

namespace strom {

    class Chain;

    class PolytomyUpdater : public Updater {

        friend class Chain;

        public:

            typedef std::map<unsigned, std::vector<double> >    _polytomy_distr_map_t;
            typedef std::vector<Node *>                         _polytomy_vect_t;
            typedef std::vector<double>                         _move_prob_vect_t;
            typedef std::shared_ptr< PolytomyUpdater >          SharedPtr;

                                                PolytomyUpdater();
                                                ~PolytomyUpdater();

            virtual double                      calcLogPrior(int & checklist);

        private:

            virtual void                        revert();
            virtual void                        proposeNewState();
            virtual void                        reset();
            
            void                                proposeAddEdgeMove(Node * nd);
            void                                proposeDeleteEdgeMove(Node * nd);
            
            void                                computePolytomyDistribution(unsigned nspokes);
            void                                refreshPolytomies();
            void                                refreshBirthDeathMoveProbs(unsigned num_taxa, unsigned num_internals_in_fully_resolved_tree);
            double                              sampleEdgeLength() const;
            
            _polytomy_distr_map_t               _poly_prob;
            _polytomy_vect_t                    _poly_distr;
            _polytomy_vect_t                    _polytomies;
            
            //unsigned                            _added_node_index;
            Node *                              _orig_par;
            Node *                              _orig_lchild;
            //Node *                              _orig_rchild;

            bool                                _add_edge_proposed;
            double                              _new_edgelen;
            unsigned                            _polytomy_size;
            unsigned                            _num_polytomies;
            double                              _orig_edgelen;
            double                              _new_edgelen_mean;
            _move_prob_vect_t                   _b;
            _move_prob_vect_t                   _d;
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
        _new_edgelen_mean  = 0.01;
        _orig_par          = 0;
        _orig_lchild       = 0;
        _new_edgelen       = 0.0;
        _polytomy_size     = 0;
        _num_polytomies    = 0;
        _orig_edgelen      = 0.0;
        _add_edge_proposed = false;
        _polytomies.clear();
        _b.clear();
        _d.clear();
    }   ///end_reset

    inline double PolytomyUpdater::calcLogPrior(int & checklist) {   ///begin_calcLogPrior
        double log_prior = 0.0;
        
        bool tree_topology_prior_calculated = (checklist & Model::TreeTopology);
        if (!tree_topology_prior_calculated) {
            log_prior += calcLogTopologyPrior();
            checklist |= Model::TreeTopology;
        }
            
        bool edge_lengths_prior_calculated  = (checklist & Model::EdgeLengths);
        if (!edge_lengths_prior_calculated) {
            log_prior += Updater::calcLogEdgeLengthPrior();
            checklist |= Model::EdgeLengths;
        }
            
        return log_prior;
    }   ///end_calcLogPrior

    inline void PolytomyUpdater::computePolytomyDistribution(unsigned nspokes) {    ///begin_computePolytomyDistribution
        // Determines distribution of x given nspokes, where x is the number spokes
        // assigned to the newly created node in an add-edge move. The number of ways
        // of choosing x spokes to move out of nspokes total is {nspokes \choose x}.
        // We are not interested in the values x = 0, x = 1, x = nspokes - 1, and x = nspokes
        // because these lead either to a non-move (x = 0 and x = nspokes) or to a tree
        // that has an invalid structure (x = 1 and x = nspokes - 1). Thus, the total
        // number of possible trees considered is
        //  {nspokes \choose 2} + {nspokes \choose 3} + ... + {n \choose nspokes - 2}
        //    = 2^nspokes - 2*(nspokes + 1).
        // The 2^nspokes comes from the fact that 2^nspokes is the sum of all binomial
        // coefficients for a sample size of nspokes. The subtracted term 2(nspokes + 1)
        // comes from the fact that the first and last binomial coefficients -
        // {nspokes \choose 0} and {nspokes \choose nspokes} - are always 1 and the
        // second and penultimate binomial coefficients - {nspokes \choose 1} and
        // {nspokes \choose nspokes - 1} - always equal nspokes. Thus, if one wishes to
        // choose randomly from all possible ways of splitting the polytomy into two
        // groups of spokes, select x with probability:
        //
        //                     (nspokes \choose x}
        //   Pr(X = x) = -----------------------------, x = 2, 3, ..., nspokes - 2
        //                 2^nspokes - 2*(nspokes + 1)
        //
        assert(nspokes > 2);
                
        // Only compute it if it isn't already stored in the _poly_prob map
        auto i = _poly_prob.find(nspokes);
        if (i == _poly_prob.end()) {
            // There is no existing probability distribution vector corresponding to nspokes
            double ln_denom = std::log(pow(2.0,nspokes) - 2.0*nspokes - 2.0); //std::log(2)*nspokes + std::log(1 - std::exp(std::log(nspokes + 1) - std::log(2)*(nspokes-1)));
            std::vector<double> v(nspokes - 3);
            for (unsigned x = 2; x <= nspokes - 2; ++x) {
                double ln_numer = std::lgamma(nspokes + 1) - std::lgamma(x + 1) - std::lgamma(nspokes - x + 1);
                double prob_x = exp(ln_numer - ln_denom);
                if (prob_x > 1.0)
                    prob_x = 1.0;
                v[x-2] = prob_x;
                }
            _poly_prob[nspokes] = v;
        }
    } ///end_computePolytomyDistribution
        
    inline void PolytomyUpdater::proposeNewState() {    ///begin_proposeNewState
        Tree::SharedPtr tree = _tree_manipulator->getTree();
        
        // Compute number of internal nodes in a fully resolved tree
        unsigned num_internals_in_fully_resolved_tree = 0;
        if (tree->isRooted())
            num_internals_in_fully_resolved_tree = tree->numLeaves();
        else
            num_internals_in_fully_resolved_tree = tree->numLeaves() - 2;
            
        // Compute tree length before proposed move
        double tree_length_before = _tree_manipulator->calcTreeLength();
            
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
        
        //refreshBirthDeathMoveProbs(tree->numLeaves(), num_internals_in_fully_resolved_tree);

        // Determine whether an add edge move is proposed (alternative is a delete edge move)
        if (star_tree_before)
            _add_edge_proposed = true;
        else if (fully_resolved_before)
            _add_edge_proposed = false;
        else
            //_add_edge_proposed = (_lot->uniform() < _b[num_internal_edges_before]);
            _add_edge_proposed = (_lot->uniform() < 0.5);
            
#if DEBUG_POLY //POLTMP //POLY
        if (_add_edge_proposed) {
            std::cerr << "\n**** add edge *****" << std::endl;
            DebugStuff::_debug_add_tries++;
            DebugStuff::_debug_add_accepts++;
        }
        else {
            std::cerr << "\n**** delete edge *****" << std::endl;
            DebugStuff::_debug_del_tries++;
            DebugStuff::_debug_del_accepts++;
        }
#endif

        Node * nd = 0;
        if (_add_edge_proposed) {
            // Choose a polytomy at random to split
            unsigned i = _lot->randint(0, _num_polytomies-1);
            nd = _polytomies[i];
            _polytomy_size = 1 + _tree_manipulator->countChildren(nd);

            DebugStuff::debugSaveTree("pre-addedge", DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
            //std::cerr << "DebugStuff::debugSaveTree: pre-addedge" << std::endl; //POLTMP

            // Add an edge to split up polytomy at nd, moving a random subset
            // of the spokes to the (new) left child of nd
            proposeAddEdgeMove(nd);
            Node * new_nd = nd->getLeftChild();

            // Compute the log of the Hastings ratio
            _log_hastings_ratio  = 0.0;
            _log_hastings_ratio += std::log(_num_polytomies);
            _log_hastings_ratio += std::log(pow(2.0, _polytomy_size - 1) - _polytomy_size - 1);
            _log_hastings_ratio -= std::log(num_internal_edges_before + 1);
            
            // Now multiply by the value of the quantity labeled gamma_b in the Lewis-Holder-Holsinger (2005) paper
            unsigned num_internals_after = tree->numInternals();
            assert(num_internals_after == num_internals_before + 1);
#if 0
            _log_hastings_ratio += std::log(_d[num_internals_after]);
            _log_hastings_ratio -= std::log(_b[num_internals_before]);
            //if (num_internals_before == 1)
            //    std::cerr << "star tree before add edge move" << std::endl;
            //else if (num_internals_after == num_internals_in_fully_resolved_tree)
            //    std::cerr << "resolved tree after add edge move" << std::endl;
#else
            const bool fully_resolved_after = (num_internals_after == num_internals_in_fully_resolved_tree);
            if (star_tree_before && !fully_resolved_after)
                _log_hastings_ratio -= log(2.0);
            else if (fully_resolved_after && !star_tree_before)
                _log_hastings_ratio += log(2.0);
#endif
                
            // Compute the log of the Jacobian
            _log_jacobian = 0.0;
            _log_jacobian += std::log(_new_edgelen_mean);
            _log_jacobian += total_edges_before*std::log(tree_length_before);
            _log_jacobian += _new_edgelen/_new_edgelen_mean;
            _log_jacobian -= (total_edges_before + 1)*std::log(tree_length_before + _new_edgelen);

            // flag partials and transition matrices for recalculation
            _tree_manipulator->selectPartialsHereToRoot(new_nd);
            new_nd->selectTMatrix();

            _tree_manipulator->refreshNavigationPointers();
            DebugStuff::debugSaveTree("post-addedge", DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
            //std::cerr << "DebugStuff::debugSaveTree: post-addedge" << std::endl; //POLTMP
        }
        else {
            DebugStuff::debugSaveTree("pre-deleteedge", DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
            //std::cerr << "DebugStuff::debugSaveTree: pre-deleteedge" << std::endl; //POLTMP

            // Choose an internal edge at random and delete it to create a polytomy
            // (or a bigger polytomy if there is already a polytomy)
            nd = _tree_manipulator->randomInternalEdge(_lot->uniform());
            proposeDeleteEdgeMove(nd);

            // Compute the log of the Hastings ratio
            _log_hastings_ratio  = 0.0;
            _log_hastings_ratio += std::log(num_internal_edges_before);
            _log_hastings_ratio -= std::log(_num_polytomies);
            _log_hastings_ratio -= std::log(pow(2.0, _polytomy_size - 1) - _polytomy_size - 1);

            // Now multiply by the value of the quantity labeled gamma_b in the Lewis-Holder-Holsinger (2005) paper
            // Now multiply by the value of the quantity labeled gamma_d in the paper
            unsigned num_internals_after = tree->numInternals();
            assert(num_internals_after == num_internals_before - 1);
#if 0
            _log_hastings_ratio += std::log(_b[num_internals_after]);
            _log_hastings_ratio -= std::log(_d[num_internals_before]);
            //if (num_internals_after == 1)
            //    std::cerr << "star tree after delete edge move" << std::endl;
            //else if (num_internals_before == num_internals_in_fully_resolved_tree)
            //    std::cerr << "resolved tree before delete edge move" << std::endl;

#else
            const bool star_tree_after = (num_internals_after == (tree->isRooted() ? 2 : 1));
            if (fully_resolved_before && !star_tree_after) {
                _log_hastings_ratio -= log(2.0);
            }
            else if (star_tree_after && !fully_resolved_before) {
                _log_hastings_ratio += log(2.0);
            }
#endif

            // Compute the log Jacobian
            _log_jacobian = 0.0;
            _log_jacobian -= std::log(_new_edgelen_mean);
            _log_jacobian -= (total_edges_before - 1)*std::log(tree_length_before - _orig_edgelen);
            _log_jacobian -= _orig_edgelen/_new_edgelen_mean;
            _log_jacobian += total_edges_before*std::log(tree_length_before);

            // flag partials and transition matrices for recalculation
            _tree_manipulator->selectPartialsHereToRoot(nd);

            _tree_manipulator->refreshNavigationPointers();
            DebugStuff::debugSaveTree("post-deleteedge", DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
            //std::cerr << "DebugStuff::debugSaveTree: post-deleteedge" << std::endl; //POLTMP
}
    }   ///end_proposeNewState
    
    inline void PolytomyUpdater::proposeAddEdgeMove(Node * u) {    ///begin_proposeAddEdgeMove
        Tree::SharedPtr tree = _tree_manipulator->getTree();

        // Split up the polytomy at `u' by creating a new internal node v and a new edge
        // connecting u with v. Node u is saved as _orig_par and node v is saved
        // as _orig_lchild in case we need to revert the proposed move.
        assert(u);

        // Calculate (if necessary) the probability of each possible partitioning of the chosen polytomy
        computePolytomyDistribution(_polytomy_size);
        const std::vector<double> & prob_n = _poly_prob[_polytomy_size];

        // Select number of spokes to move over to new node
        // Note that 0, 1 and _polytomy_size, _polytomy_size-1 are not allowed
        // because any of these would leave the tree in an incorrect state
        double p = _lot->uniform();
        double cum = 0.0;
        unsigned k = 2;
        for (; k <= _polytomy_size - 2; ++k) {
            double prob_k_given_n = prob_n[k-2];
            cum += prob_k_given_n;
            if (p < cum)
                break;
        }
        assert(k < _polytomy_size - 1);

        // Create the new node that will receive the x randomly-chosen spokes
        Node * v = _tree_manipulator->getUnusedNode();
        _new_edgelen = sampleEdgeLength();
        v->setEdgeLength(_new_edgelen);
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
        // Returns the number of polytomies in the tree after the proposed delete-edge move. The return value will be incorrect if
        // the polytomies vector is not up-to-date.
        
        // Save u's edge length in case we need to revert
        _orig_edgelen = u->getEdgeLength();

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

        // Make all of u's children left siblings (i.e. children of u->par)
        _orig_lchild = u->getLeftChild();
        while (u->getLeftChild() != NULL) {
            Node * tmp = u->getLeftChild();
            _tree_manipulator->detachSubtree(tmp);
            _tree_manipulator->insertSubtreeOnRight(tmp, _orig_par);
        }

        //std::cerr << "*** DELETE EDGE ***" << std::endl;  //POLTMP
        _tree_manipulator->detachSubtree(u);
        //Tree::SharedPtr tree = _tree_manipulator->getTree();
        _tree_manipulator->putUnusedNode(u);
        _tree_manipulator->rectifyNumInternals(-1);
    }   ///end_proposeDeleteEdgeMove
    
    inline void PolytomyUpdater::revert() { ///begin_revert
        //Tree::SharedPtr tree = _tree_manipulator->getTree();
#if DEBUG_POLY //POLTMP //POLY
        if (_add_edge_proposed)
            DebugStuff::_debug_add_accepts--;
        else
            DebugStuff::_debug_del_accepts--;
#endif
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
            //std::cerr << "*** REVERT ADD EDGE ***" << std::endl;  //POLTMP
            _tree_manipulator->putUnusedNode(_orig_lchild);
            _tree_manipulator->rectifyNumInternals(-1);

            _tree_manipulator->refreshNavigationPointers();
            DebugStuff::debugSaveTree("revert-addedge", DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
            //std::cerr << "DebugStuff::debugSaveTree: revert-addedge" << std::endl; //POLTMP
        }
        else {
            //std::cerr << "*** REVERT DELETE EDGE ***" << std::endl;  //POLTMP
            Node * v = _tree_manipulator->getUnusedNode();
            _tree_manipulator->rectifyNumInternals(+1);
            v->setEdgeLength(_orig_edgelen);
            for (Node * child = _orig_lchild; child;) {
                Node * child_rsib = child->getRightSib();
                _tree_manipulator->detachSubtree(child);
                _tree_manipulator->insertSubtreeOnRight(child, v);
                child = child_rsib;
            }
            _tree_manipulator->insertSubtreeOnRight(v, _orig_par);

            _tree_manipulator->refreshNavigationPointers();
            DebugStuff::debugSaveTree("revert-deleteedge", DebugStuff::debugMakeNewick(_tree_manipulator->getTree(), 5));  //DEBUGSTUFF
            //std::cerr << "DebugStuff::debugSaveTree: revert-deleteedge" << std::endl; //POLTMP
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

    inline void PolytomyUpdater::refreshBirthDeathMoveProbs(unsigned num_taxa, unsigned num_internals_in_fully_resolved_tree) {  ///begin_refreshPolytomies
        // This funtion is currently unused: probs turned out to be not that different from 50:50 when not at the ends
        //
        //  m        count    log(prob)         b[m]         d[m]         c[m]
        //  1            1     -2.07944      1.00000      0.00000      0.00200
        //  2          500     -8.29605      0.02138      0.97862      1.02184
        //  3        22935    -12.11986      0.07037      0.92963      1.07569
        //  4       302994    -14.70091      0.15621      0.84379      1.18513
        //  5      1636634    -16.38759      0.28534      0.71466      1.39927
        //  6      4099094    -17.30572      0.46429      0.53571      1.86667
        //  7      4729725    -17.44882      0.50000      0.50000      2.00000
        //  8      2027024    -16.60152      0.00000      1.00000      0.42857

        Tree::SharedPtr tree = _tree_manipulator->getTree();
        //double total_count = _topo_prior_calculator.getLogTotalCount(num_taxa);
        _b.resize(num_internals_in_fully_resolved_tree+1, 0.0);
        _d.resize(num_internals_in_fully_resolved_tree+1, 0.0);
        std::vector<double> c(num_internals_in_fully_resolved_tree+1, 1.0);
        _d[1] = 0.0;
        _b[num_internals_in_fully_resolved_tree] = 0.0;
        unsigned m = 1;
        for (; m < num_internals_in_fully_resolved_tree; m++) {
            double pm  = _topo_prior_calculator.getLogNormalizedTopologyPrior(m);
            double pm1 = _topo_prior_calculator.getLogNormalizedTopologyPrior(m+1);
            if (pm1 > pm) {
                _b[m] = 1.0;
                _d[m+1] = std::exp(pm - pm1);
            }
            else {
                _b[m] = std::exp(pm1 - pm);
                _d[m+1] = 1.0;
            }
            c[m] = _b[m] + _d[m];
        }
        c[m] = _b[m] + _d[m];
        //std::cerr << boost::str(boost::format("%6s %12s %12s %12s %12s %12s") % "m" % "count" % "log(prob)" % "b[m]" % "d[m]" % "c[m]") << std::endl;
        for (m = 1; m <= num_internals_in_fully_resolved_tree; m++) {
            _b[m] /= c[m];
            _d[m] /= c[m];
            //std::cerr << boost::str(boost::format("%6d %12d %12.5f %12.5f %12.5f %12.5f")
            //    % m
            //    % int(std::exp(_topo_prior_calculator.getLogCount(num_taxa,m)))
            //    % _topo_prior_calculator.getLogNormalizedTopologyPrior(m)
            //    % _b[m]  % _d[m] % c[m]) << std::endl;
        }
        //std::cerr << std::endl;
        }   ///end_refreshPolytomies

    inline double PolytomyUpdater::sampleEdgeLength() const {  ///begin_sampleEdgeLength
        double new_edgelen = -_new_edgelen_mean*std::log(_lot->uniform());
        return new_edgelen;
    }   ///end_sampleEdgeLength

}   ///end
