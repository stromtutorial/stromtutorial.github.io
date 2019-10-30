#pragma once    ///start

#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include "split.hpp"
#include "tree_manip.hpp"
#include "xstrom.hpp"

#include "ncl/nxsmultiformat.h"

namespace strom {

    class TreeSummary {
        public:
                                        TreeSummary();
                                        ~TreeSummary();

            void                        readTreefile(const std::string filename, unsigned skip);
            void                        showSummary() const;
            typename Tree::SharedPtr    getTree(unsigned index);
            std::string                 getNewick(unsigned index);
            void                        clear();

        private:

            Split::treemap_t            _treeIDs;
            std::vector<std::string>    _newicks;

        public:

            typedef std::shared_ptr< TreeSummary > SharedPtr;
    };

    // insert member function bodies here
    ///end_class_declaration
    inline TreeSummary::TreeSummary() { ///begin_constructor
        //std::cout << "Constructing a TreeSummary" << std::endl;
    }   ///end_constructor

    inline TreeSummary::~TreeSummary() { ///begin_destructor
        //std::cout << "Destroying a TreeSummary" << std::endl;
    }   ///end_destructor

    inline Tree::SharedPtr TreeSummary::getTree(unsigned index) {   ///begin_getTree
        if (index >= _newicks.size())
            throw XStrom("getTree called with index >= number of stored trees");

        TreeManip tm;

        // build the tree
        tm.buildFromNewick(_newicks[index], false, false);

        return tm.getTree();
    }   ///end_getTree

    inline std::string TreeSummary::getNewick(unsigned index) { ///begin_getNewick
        if (index >= _newicks.size())
            throw XStrom("getNewick called with index >= number of stored trees");

        return _newicks[index];
    }   ///end_getNewick

    inline void TreeSummary::clear() {  ///begin_clear
        _treeIDs.clear();
        _newicks.clear();
    }   ///end_clear

    inline void TreeSummary::readTreefile(const std::string filename, unsigned skip) {  ///begin_readTreefile
        TreeManip tm;
        Split::treeid_t splitset;

        // See http://phylo.bio.ku.edu/ncldocs/v2.1/funcdocs/index.html for NCL documentation

        MultiFormatReader nexusReader(-1, NxsReader::WARNINGS_TO_STDERR);
        try {
            nexusReader.ReadFilepath(filename.c_str(), MultiFormatReader::NEXUS_FORMAT);
        }
        catch(...) {
            nexusReader.DeleteBlocksFromFactories();
            throw;
        }

        int numTaxaBlocks = nexusReader.GetNumTaxaBlocks();
        for (int i = 0; i < numTaxaBlocks; ++i) {
            clear();
            NxsTaxaBlock * taxaBlock = nexusReader.GetTaxaBlock(i);
            std::string taxaBlockTitle = taxaBlock->GetTitle();

            const unsigned nTreesBlocks = nexusReader.GetNumTreesBlocks(taxaBlock);
            for (unsigned j = 0; j < nTreesBlocks; ++j) {
                const NxsTreesBlock * treesBlock = nexusReader.GetTreesBlock(taxaBlock, j);
                unsigned ntrees = treesBlock->GetNumTrees();
                if (skip < ntrees) {
                    //std::cout << "Trees block contains " << ntrees << " tree descriptions.\n";
                    for (unsigned t = skip; t < ntrees; ++t) {
                        const NxsFullTreeDescription & d = treesBlock->GetFullTreeDescription(t);

                        // store the newick tree description
                        std::string newick = d.GetNewick();
                        _newicks.push_back(newick);
                        unsigned tree_index = (unsigned)_newicks.size() - 1;

                        // build the tree
                        tm.buildFromNewick(newick, false, false);

                        // store set of splits
                        splitset.clear();
                        tm.storeSplits(splitset);

                        // iterator iter will point to the value corresponding to key splitset
                        // or to end (if splitset is not already a key in the map)
                        Split::treemap_t::iterator iter = _treeIDs.lower_bound(splitset);

                        if (iter == _treeIDs.end() || iter->first != splitset) {
                            // splitset key not found in map, need to create an entry
                            std::vector<unsigned> v(1, tree_index);  // vector of length 1 with only element set to tree_index
                            _treeIDs.insert(iter, Split::treemap_t::value_type(splitset, v));
                        }
                        else {
                            // splitset key was found in map, need to add this tree's index to vector
                            iter->second.push_back(tree_index);
                        }
                    } // trees loop
                } // if skip < ntrees
            } // TREES block loop
        } // TAXA block loop

        // No longer any need to store raw data from nexus file
        nexusReader.DeleteBlocksFromFactories();
    }   ///end_readTreefile

    inline void TreeSummary::showSummary() const {  ///begin_showSummary
        // Produce some output to show that it works
        std::cout << boost::str(boost::format("\nRead %d trees from file") % _newicks.size()) << std::endl;

        // Show all unique topologies with a list of the trees that have that topology
        // Also create a map that can be used to sort topologies by their sample frequency
        typedef std::pair<unsigned, unsigned> sorted_pair_t;
        std::vector< sorted_pair_t > sorted;
        int t = 0;
        for (auto & key_value_pair : _treeIDs) {
            unsigned topology = ++t;
            unsigned ntrees = (unsigned)key_value_pair.second.size();
            sorted.push_back(std::pair<unsigned, unsigned>(ntrees,topology));
            std::cout << "Topology " << topology << " seen in these " << ntrees << " trees:" << std::endl << "  ";
            std::copy(key_value_pair.second.begin(), key_value_pair.second.end(), std::ostream_iterator<unsigned>(std::cout, " "));
            std::cout << std::endl;
        }

        // Show sorted histogram data
        std::sort(sorted.begin(), sorted.end());
        //unsigned npairs = (unsigned)sorted.size();
        std::cout << "\nTopologies sorted by sample frequency:" << std::endl;
        std::cout << boost::str(boost::format("%20s %20s") % "topology" % "frequency") << std::endl;
        for (auto & ntrees_topol_pair : boost::adaptors::reverse(sorted)) {
            unsigned n = ntrees_topol_pair.first;
            unsigned t = ntrees_topol_pair.second;
            std::cout << boost::str(boost::format("%20d %20d") % t % n) << std::endl;
        }
    }   ///end_showSummary

}   ///end
