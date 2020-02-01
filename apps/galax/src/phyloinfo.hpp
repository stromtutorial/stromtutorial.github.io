#pragma once

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
#include "galaxtypes.hpp"
#include "galaxdata.hpp"
#include "xstrom.hpp"

#include "ncl/nxsmultiformat.h"

namespace strom {

    class PhyloInfo {
        public:
                                        PhyloInfo(  std::string treefile,
                                                    std::string listfile,
                                                    std::string outfile,
                                                    unsigned skip,
                                                    bool rooted,
                                                    bool details,
                                                    unsigned outgroup,
                                                    std::string prog,
                                                    unsigned major,
                                                    unsigned minor);
                                        ~PhyloInfo();

            void                        clear();
            void                        reset();
            void                        run();
            
            void                        openOutputFile();
            void                        getTreeFileNames();
            void                        initTreeCCD(unsigned num_subsets);
            void                        readTreefile(   const std::string   filename,
                                                        ccd_map_t &         ccdmap,
                                                        unsigned            subset_index,
                                                        unsigned            num_subsets);
            void                        estimateInfo(   ccd_map_t &                 ccdmap,
                                                        std::string &               summaryinfostr,
                                                        std::string &               detailedinfostr,
                                                        std::vector<GalaxInfo> &    clade_info);

        protected:

            // Settings
            std::string                 _tree_file_name;
            std::string                 _list_file_name;
            std::string                 _output_file_prefix;
            std::string                 _program_name;
            unsigned                    _skip;
            unsigned                    _outgroup;
            unsigned                    _major;
            unsigned                    _minor;
            bool                        _rooted;
            bool                        _show_details;
            
            std::vector<unsigned>       _tree_counts;
            std::vector<std::string>    _treefile_names;
            std::vector<std::string>    _newicks;
            std::vector<std::string>    _merged_newicks;
            
            std::vector<std::string>    _taxon_labels;
            
            std::ofstream               _outf;

            boost::posix_time::ptime    _start_time;
            boost::posix_time::ptime    _end_time;
            double                      _total_seconds;
            
            bool                        _is_listfile;
            
            // _ccdlist[i][j] stores conditional clade info for clade i, subset j, for trees specified by --listfile
            ccd_map_t                   _ccdlist;
            
            // _ccdtree[i][0] stores conditional clade info for clade i for trees specified by --treefile
            ccd_map_t                   _ccdtree;

            // _treeCCD[i] is a set of tree IDs (list of cond. clades) appearing in the sample for subset i
            subset_tree_set_t           _treeCCD;

            // _treeMap[i] is a map relating tree IDs (keys) to counts (values) of each tree topology
            // sampled in subset i (only used if _show_details is true)
            subset_tree_map_t           _treeMap;

        public:

            typedef std::shared_ptr<PhyloInfo> SharedPtr;
    };

    // insert member function bodies here

    inline PhyloInfo::PhyloInfo(std::string treefile, std::string listfile, std::string outfile, unsigned skip, bool rooted, bool details, unsigned outgroup, std::string prog, unsigned major, unsigned minor) {
        _output_file_prefix = outfile;
        _tree_file_name     = treefile;
        _list_file_name     = listfile;
        _skip               = skip;
        _rooted             = rooted;
        _show_details       = details;
        _outgroup           = outgroup;
        _program_name       = prog;
        _major              = major;
        _minor              = minor;
        //std::cout << "Constructing a PhyloInfo" << std::endl;
    }

    inline PhyloInfo::~PhyloInfo() {
        //std::cout << "Destroying a PhyloInfo" << std::endl;
    }

    inline void PhyloInfo::clear() {
        _tree_file_name.clear();
        _list_file_name.clear();
        _output_file_prefix.clear();
        _program_name.clear();

        _skip           = 0;
        _outgroup       = 0;
        _major          = 0;
        _minor          = 0;

        _total_seconds  = 0.0;
        
        _rooted         = false;
        _show_details   = false;
        _is_listfile    = false;

        _taxon_labels.clear();
        _tree_counts.clear();
        _treefile_names.clear();
        _newicks.clear();
        _merged_newicks.clear();
        _ccdlist.clear();
        _ccdtree.clear();
        _treeCCD.clear();
        _treeMap.clear();
    }
    
    inline void PhyloInfo::reset() {
        _newicks.clear();
    }
    
    inline void PhyloInfo::openOutputFile() {
        std::string outfile_name = _output_file_prefix + ".txt";
        _outf.open(outfile_name.c_str());

        // Start by reporting settings used
        _outf << _program_name << " " << _major << "." << _minor << "\n\n";
        _outf << "Options specified:\n";
        _outf << "   --treefile: " << _tree_file_name << "\n";
        _outf << "   --listfile: " << _list_file_name << "\n";
        _outf << "       --skip: " << _skip << "\n";
        _outf << "     --rooted: " << (_rooted ? "true" : "false") << "\n";
        //_outf << "    --details: " << (save_details ? "true" : "false") << "\n";
        _outf << "   --outgroup: " << _outgroup << "\n";
        _outf << "    --outfile: " << outfile_name << "\n";
        //_outf << "      --mapto: " << maptofname << "\n";
        //_outf << "--maptorooted: " << (mapto_trees_rooted ? "true" : "false") << "\n";
        _outf << std::endl;
    }
    
    inline void PhyloInfo::getTreeFileNames() {
        _start_time = getCurrentTime();

        _treefile_names.clear();
        _is_listfile = (_list_file_name.length() > 0);
        if (_is_listfile) {
            std::string file_contents;
            getFileContents(file_contents, _list_file_name);
            extractAllWhitespaceDelimitedStrings(_treefile_names, file_contents);
            if (_treefile_names.size() == 0)
                throw XStrom("found no tree file names in specified listfile");
            else if (_treefile_names.size() == 1)
                throw XStrom("use --treefile (not --listfile) if there is only one tree file to process");
        }
        else
            _treefile_names.push_back(_tree_file_name);

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

    inline void PhyloInfo::initTreeCCD(unsigned num_subsets) {
        _treeCCD.clear();
        _treeMap.clear();
        for (unsigned i = 0; i <= num_subsets; ++i) {
            tree_id_set_t v;
            _treeCCD.push_back(v);
            tree_map_t m;
            _treeMap.push_back(m);
        }
    }

    inline void PhyloInfo::run() {
        openOutputFile();
        getTreeFileNames();
        
        std::string summaryinfostr;
        std::string detailedinfostr;
        if (!_is_listfile) {
            //
            // --treefile provided on command line
            //
            initTreeCCD(1);
            _ccdtree.clear();
            readTreefile(_tree_file_name, _ccdtree, 0, 1);
            _tree_counts.push_back((unsigned)_newicks.size());
            _merged_newicks.insert(_merged_newicks.end(), _newicks.begin(), _newicks.end());

            std::string msg = boost::str(boost::format("Read %d trees from tree file %s\n") % _newicks.size() % _tree_file_name);
            std::cout << msg;
            _outf << msg;

            GalaxInfo::_galaxinfo_vect_t majrule_clade_info;
            std::vector<Split> majrule_splits;
            std::string majrule_tree;
            estimateInfo(_ccdtree, summaryinfostr, detailedinfostr, majrule_clade_info);
            //buildMajorityRuleTree(majrule_clade_info, majrule_clade_info, majrule_tree);
            //writeMajruleTreefile("majrule", majrule_tree);
        }
        else {
            std::cerr << "--listfile option not yet working" << std::endl;
        }

        _outf << "\n" << summaryinfostr;
        _outf.close();

        if (_show_details) {
            std::string detailsfname = std::string(_output_file_prefix + "-details.txt");
            std::ofstream detailsf(detailsfname.c_str());
            detailsf << "\n" << detailedinfostr;
            detailsf.close();
        }

        std::cout << "\nRequired " << _total_seconds << " total seconds" << std::endl;

    }

    inline void PhyloInfo::readTreefile(const std::string filename, ccd_map_t & ccdmap, unsigned subset_index, unsigned num_subsets) {
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
            reset();
            NxsTaxaBlock * taxaBlock = nexusReader.GetTaxaBlock(i);
            std::string taxaBlockTitle = taxaBlock->GetTitle();
            _taxon_labels = taxaBlock->GetAllLabels();

            const unsigned nTreesBlocks = nexusReader.GetNumTreesBlocks(taxaBlock);
            for (unsigned j = 0; j < nTreesBlocks; ++j) {
                const NxsTreesBlock * treesBlock = nexusReader.GetTreesBlock(taxaBlock, j);
                unsigned ntrees = treesBlock->GetNumTrees();
                if (_skip < ntrees) {
                    //std::cout << "Trees block contains " << ntrees << " tree descriptions.\n";
                    for (unsigned t = _skip; t < ntrees; ++t) {
                        const NxsFullTreeDescription & d = treesBlock->GetFullTreeDescription(t);

                        // store the newick tree description
                        std::string newick = d.GetNewick();
                        _newicks.push_back(newick);

#if 1
                        // build the tree
                        tm.buildFromNewick(newick, _rooted, false); //POLY
                        if (!_rooted)
                            tm.rerootAtNodeNumber(_outgroup - 1);
                        tm.addToCCDMap(ccdmap, _treeCCD, _treeMap, _show_details, subset_index, num_subsets);
#else
                        unsigned tree_index = (unsigned)_newicks.size() - 1;

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
#endif
                    } // trees loop
                } // if _skip < ntrees
            } // TREES block loop
        } // TAXA block loop

        // No longer any need to store raw data from nexus file
        nexusReader.DeleteBlocksFromFactories();
    }

    inline void PhyloInfo::estimateInfo(ccd_map_t & ccdmap, std::string & summaryinfostr, std::string & detailedinfostr, GalaxInfo::_galaxinfo_vect_t & clade_info) {
        // ccdmap: key = SplitVector defining (un)conditional clade, value = CountVector (counts for each subset)
        // summaryinfostr: holds summary output over all clades
        // detailedinfostr: holds detailed output for each clade
        // clade_info: vector of GalaxInfo objects that is filled here and used to build and label majority rule tree later
        
        _start_time = getCurrentTime();

        assert(ccdmap.size() > 0);
        Split clade;
        clade_info.clear();
        bool first = true;
        std::vector<double> tmp;

        //std::cerr << "***** In Galax::estimateInfo, _outgroup = " << (_rooted ? 0 : _outgroup) << std::endl;

        // Create a GalaxData object to do most of the work
        unsigned ntaxa = (unsigned)_taxon_labels.size();
        GalaxData gd(_tree_counts, _treefile_names, (_rooted ? ntaxa : ntaxa - 1), (_rooted ? 0 : _outgroup));
        if (_show_details)
            gd.setShowDetails(true);
        else
            gd.setShowDetails(false);

        //unsigned total_trees = std::accumulate(_tree_counts.begin(), _tree_counts.end(), 0);

        // Provide a key to the taxa in split representations in both output files
        std::string tmpstr = "Order of taxa in split representations:\n";
        unsigned t = 1;
        for (auto & label : _taxon_labels) {
            tmpstr += boost::str(boost::format("%12d  %s\n") % (t++) % label);
        }
        summaryinfostr = tmpstr;
        detailedinfostr = tmpstr;
        detailedinfostr += "\nLindley Information\n";
        detailedinfostr += boost::str(boost::format("  Number of taxa in ingroup: %d\n") % ntaxa);
        detailedinfostr += boost::str(boost::format("  Total prior entropy: %.5f\n") % gd.getTotalEntropy());
        detailedinfostr += "\nKey to columns:\n\n";
        detailedinfostr += "  treefile = file containing sampled trees from one data subset\n";
        detailedinfostr += "  trees    = number of trees sampled\n";
        detailedinfostr += "  K        = Kullback-Leibler divergence for current clade\n";
        detailedinfostr += "  p*       = marginal posterior probability of current clade\n";
        detailedinfostr += "  I        = information for current clade (p_C*KL_c)\n";
        detailedinfostr += "  Ipct     = information for current clade as percent of maximum information\n";
        detailedinfostr += "  D        = component of dissonance specific to current clade\n\n";

        for (ccd_map_t::iterator it = ccdmap.begin(); it != ccdmap.end(); ++it) {
            // *it comprises a key (SplitVector) and a value (CountVector)
            // it->first is the key: it is a vector containing either
            //       a) 1 split (represents an unconditional clade probability) or
            //       b) 3 splits (represents a conditional clade probability, with
            //          first split being parent clade and remaining two splits
            //          representing the bipartition of that parent split)
            // it->second is the value: it is a vector of counts, with one element for each subset
            //const SplitVector   & v     = it->first;
            //std::vector<double> & count = it->second;
            //double sum_counts = std::accumulate(count.begin(), count.end(), 0.0);

            // The following depends on ccdmap keys (split vectors) being sorted such that an
            // entry for an unconditional clade, e.g. (ABC), precedes any conditional clade entries,
            // e.g. (ABC,A,BC) that have that clade as parent. Thus, when next unconditional
            // clade entry is encountered, we know that there are no more conditional clade entries
            // for the previous parent clade and we can at that point compute the information for
            // the previous parent.
            if (first) {
                first = false;
                clade = (it->first)[0];
                gd.newClade(it->first, it->second);
            }
            else {
                if ((it->first)[0] == clade) {
                    // Conditional clade entry whose parent is clade
                    gd.conditionalClade(it->first, it->second);
                }
                else {
                    // Just found next unconditional clade entry, so now is the time to finish computing
                    // information content for the previous clade

                    tmp = gd.finalizeClade(detailedinfostr);
                    gd.newClade(it->first, it->second);

                    // tmp[0] = Ipct
                    // tmp[1] = D (not Dpct)
                    // tmp[2] = P
                    // tmp[3] = I
                    clade_info.push_back(GalaxInfo(clade.createPatternRepresentation(), tmp));
                    clade = (it->first)[0];
                }
            }
        }

        // Compute I for final clade
        tmp = gd.finalizeClade(detailedinfostr);
        clade_info.push_back(GalaxInfo(clade.createPatternRepresentation(), tmp));

        gd.estimateCoverage(ccdmap, _treeCCD, _treeMap, detailedinfostr);
        gd.reportTotals(summaryinfostr, clade_info);

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

}
