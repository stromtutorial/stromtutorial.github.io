namespace strom {

#if 0
class Galax {
    public:
        typedef std::vector< Split >                     split_vect_t;
        typedef std::vector< double >                    count_vect_t;
        typedef std::map< split_vect_t, count_vect_t >   ccd_map_t;
        typedef std::vector< double >                    time_vect_t;
        typedef std::vector< double >                    weight_vect_t;
        typedef std::map< std::string, double >          clade_info_map_t;

        typedef std::vector< split_vect_t >              tree_id_t;
        typedef std::set< tree_id_t >                    tree_id_set_t;
        
        // used to store vector of sets of tree IDs, one vector for each subset
        typedef std::vector< tree_id_set_t >             subset_tree_set_t;

        typedef std::map< tree_id_t, unsigned >          tree_map_t;

        // used to store vector of maps relating tree IDs to counts of sampled trees
        typedef std::vector< tree_map_t >                subset_tree_map_t;

        typedef std::vector<GalaxInfo>                   galax_info_vect_t;

                                            Galax(const std::string outfile_prefix, const std::string version);
                                            ~Galax();

        void                                run(std::string treefname, std::string listfname, unsigned skip, bool trees_rooted, std::string maptofname, bool mapto_trees_rooted, bool save_details, unsigned outgroup_taxon);

    private:

        void                                debugShowCCDMap(ccd_map_t & ccdmap, unsigned subset_index);
        void                                saveDetailedInfoForClade(std::string & detailedinfostr, std::string pattern, unsigned num_subsets, unsigned total_trees, std::vector<double> & clade_Hp, std::vector<double> & clade_H, std::vector<double> & w, std::vector<double> & I, std::vector<double> & Ipct, double D);
        void                                estimateInfo(ccd_map_t & ccdmap, std::string & summaryinfostr, std::string & detailedinfostr, std::vector<GalaxInfo> & clade_info);
        void                                buildMajorityRuleTree(std::vector<GalaxInfo> & majrule_info, std::vector<GalaxInfo> & annotate_info, std::string & majrule_newick);
        void                                writeMajruleTreefile(std::string fnprefix, std::string & majrule_newick);
        void                                mapToTree(std::string & maptofname, std::vector<GalaxInfo> & annotate_info, std::string & mapto_newick);
        void                                initTreeCCD(unsigned num_subsets);
        void                                processTrees(TreeManip<Node>::TreeManipShPtr tm, ccd_map_t & ccdmap, unsigned subset_index, unsigned num_subsets);
        void                                storeTrees(std::string file_contents, unsigned skip, std::vector< std::string > & tree_descriptions);
        //void                              getTrees(std::string file_contents, unsigned skip);
        bool                                isNexusFile(const std::string & file_contents);
        unsigned                            taxonNumberFromName(const std::string taxon_name, bool add_if_missing);
        bool                                replaceTaxonNames(const std::string & newick_with_taxon_names, std::string & newick_with_taxon_numbers);
        void                                parseTranslate(const std::string & file_contents);
        void                                getPhyloBayesNewicks(std::vector< std::string > & tree_descriptions, const std::string & file_contents, unsigned skip);
        std::string                         standardizeNodeNumber(boost::smatch const & what);
        std::string                         standardizeTreeDescription(std::string & newick);
        void                                getNewicks(std::vector< std::string > & tree_descriptions, const std::string & file_contents, unsigned skip);
        std::vector<std::string>            getTreeFileList(std::string listfname);

        // profile
        void                                writeInfoProfile(TreeManip<Node>::TreeManipShPtr tm, std::vector<GalaxInfo> & clade_info);

    private:
        static const unsigned               _ALLSUBSETS;
        std::vector< std::string >          _treefile_names;
        std::vector< std::string >          _newicks;
        std::vector< std::string >          _merged_newicks;
        std::vector< unsigned >             _tree_counts;
        std::map< unsigned, std::string >   _translate;
        std::map< std::string, unsigned >   _taxon_map;
        boost::posix_time::ptime            _start_time;
        boost::posix_time::ptime            _end_time;
        double                              _total_seconds;
        bool                                _rooted;
        bool                                _input_rooted;
        bool                                _mapto_rooted;
        std::string                         _outfprefix;
        std::string                         _version;
        bool                                _show_details;

        std::ofstream                       _treef;     // file for storing majority rule consensus tree
        std::ofstream                       _outf;      // file used for output summary
        std::ofstream                       _detailsf;  // file used for details of clades

        unsigned                            _outgroup;  // 1-based index of outgroup taxon to use for rooting unrooted trees (1 assumed if not specified)

        ccd_map_t                           _ccdlist;   // _ccdlist[i][j] stores conditional clade info for clade i, subset j, for trees specified by --listfile
        ccd_map_t                           _ccdtree;   // _ccdtree[i][0] stores conditional clade info for clade i for trees specified by --treefile

        subset_tree_set_t                   _treeCCD;  // _treeCCD[i] is a set of tree IDs (list of cond. clades) appearing in the sample for subset i
        subset_tree_map_t                   _treeMap;  // _treeMap[i] is a map relating tree IDs (keys) to counts (values) of each tree topology sampled in subset i (only used if _show_details is true)
    };

    void Galax::run(std::string treefname,
                    std::string listfname,
                    unsigned skip,
                    bool trees_rooted,
                    std::string maptofname,
                    bool mapto_trees_rooted,
                    bool save_details,
                    unsigned outgroup_taxon) {
        assert (_ALLSUBSETS == (unsigned)(-1));
        try {
            std::string outfname = std::string(_outfprefix + ".txt");
            _outf.open(outfname.c_str());

            // Start by reporting settings used
            _outf << "Galax " << _version << "\n\n";
            _outf << "Options specified:\n";
            _outf << "   --treefile: " << treefname << "\n";
            _outf << "   --listfile: " << listfname << "\n";
            _outf << "       --skip: " << skip << "\n";
            _outf << "     --rooted: " << (trees_rooted ? "true" : "false") << "\n";
            _outf << "    --details: " << (save_details ? "true" : "false") << "\n";
            _outf << "   --outgroup: " << outgroup_taxon << "\n";
            _outf << "    --outfile: " << _outfprefix << "\n";
            _outf << "      --mapto: " << maptofname << "\n";
            _outf << "--maptorooted: " << (mapto_trees_rooted ? "true" : "false") << "\n";
            _outf << std::endl;

            _show_details   = save_details;
            _outgroup       = outgroup_taxon;
            _input_rooted   = trees_rooted;
            _mapto_rooted   = mapto_trees_rooted;
            _rooted         = _input_rooted;
            if (!_rooted && _outgroup == 0)
                throw XStrom("outgroup taxon specified must be a number greater than zero unless trees are rooted (in which case outgroup specification is ignored)");

            _treefile_names.clear();
            bool is_treefile = (treefname.length() > 0);
            bool is_listfile = (listfname.length() > 0);
            if (is_listfile) {
                _treefile_names = getTreeFileList(listfname);
                if (_treefile_names.size() == 0)
                    throw XStrom("found no tree file names in specified listfile");
                else if (_treefile_names.size() == 1)
                    throw XStrom("use --treefile (not --listfile) if there is only one tree file to process");
            }
            else
                _treefile_names.push_back(treefname);

            std::string file_contents;
            std::string summaryinfostr;
            std::string detailedinfostr;
            TreeManip::SharedPtr tm(new TreeManip());

            if (is_treefile) {
                //
                // --treefile provided on command line
                //
                initTreeCCD(1);
                _ccdtree.clear();
                getFileContents(file_contents, treefname);
                std::vector< std::string > tree_descriptions;
                storeTrees(file_contents, skip, tree_descriptions);
                _tree_counts.push_back((unsigned)tree_descriptions.size());
                _newicks.clear();
                _newicks.insert(_newicks.end(), tree_descriptions.begin(), tree_descriptions.end());
                _merged_newicks.insert(_merged_newicks.end(), tree_descriptions.begin(), tree_descriptions.end());
                processTrees(tm, _ccdtree, 0, 1);

                std::string msg = boost::str(boost::format("Read %d trees from tree file %s\n") % _newicks.size() % treefname);
                std::cout << msg;
                _outf << msg;

                GalaxInfoVector majrule_clade_info;
                std::vector<Split> majrule_splits;
                std::string majrule_tree;
                estimateInfo(_ccdtree, summaryinfostr, detailedinfostr, majrule_clade_info);
                buildMajorityRuleTree(majrule_clade_info, majrule_clade_info, majrule_tree);
                writeMajruleTreefile("majrule", majrule_tree);
            }
            else {
                //
                // --listfile provided on command line
                //
                std::cout << boost::str(boost::format("Reading tree file names from list file %s\n") % listfname);

                unsigned subset_index = 0;
                unsigned num_subsets = (unsigned)_treefile_names.size();
                initTreeCCD(num_subsets);
                _ccdlist.clear();
                for (auto & tree_file_name : _treefile_names) {
                    file_contents.clear();
                    getFileContents(file_contents, tree_file_name);
                    std::vector< std::string > tree_descriptions;
                    storeTrees(file_contents, skip, tree_descriptions);
                    _tree_counts.push_back((unsigned)tree_descriptions.size());
                    _newicks.clear();
                    _newicks.insert(_newicks.end(), tree_descriptions.begin(), tree_descriptions.end());
                    _merged_newicks.insert(_merged_newicks.end(), tree_descriptions.begin(), tree_descriptions.end());
                    processTrees(tm, _ccdlist, subset_index++, (unsigned)_treefile_names.size());
                    std::string msg = boost::str(boost::format("Read %d trees from tree file %s\n") % _newicks.size() % tree_file_name);
                    std::cout << msg;
                    _outf << msg;
                }
                GalaxInfoVector merged_clade_info;
                std::vector<Split> merged_splits;
                std::string merged_tree;
                estimateInfo(_ccdlist, summaryinfostr, detailedinfostr, merged_clade_info);
                buildMajorityRuleTree(merged_clade_info, merged_clade_info, merged_tree);
                writeMajruleTreefile("merged", merged_tree);

                if (maptofname.size() > 0) {
                    std::string mapto_tree;
                    mapToTree(maptofname, merged_clade_info, mapto_tree);
                    writeMajruleTreefile("mapped", mapto_tree);
                }
            }

            if (_rooted)
                std::cout << "Input trees assumed to be rooted\n";
            else {
                std::cout << "Input trees assumed to be unrooted\n";
                std::cout << boost::str(boost::format("Each input tree was rooted at outgroup taxon %d (\"%s\")\n") % _outgroup % _translate[_outgroup]);
            }

            _outf << "\n" << summaryinfostr;
            _outf.close();

            if (save_details) {
                std::string detailsfname = std::string(_outfprefix + "-details.txt");
                _detailsf.open(detailsfname.c_str());
                _detailsf << "\n" << detailedinfostr;
                _detailsf.close();
            }

            std::cout << "\nRequired " << _total_seconds << " total seconds" << std::endl;
        }
        catch(XStrom x) {
            std::cerr << "ERROR: " << x.what() << std::endl;
        }
    }

#endif
}
