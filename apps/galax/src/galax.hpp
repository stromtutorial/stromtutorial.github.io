#pragma once

#include <regex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "tree_manip.hpp"
#include "galaxdata.hpp"
#include "galaxinfo.hpp"
#include "galaxutil.hpp"

namespace strom {

    class Galax {
        public:
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
            void                                processTrees(TreeManip::SharedPtr tm, ccd_map_t & ccdmap, unsigned subset_index, unsigned num_subsets);
            void                                storeTrees(std::string file_contents, unsigned skip, std::vector< std::string > & tree_descriptions);
            //void                              getTrees(std::string file_contents, unsigned skip);
            bool                                isNexusFile(const std::string & file_contents);
            unsigned                            taxonNumberFromName(const std::string taxon_name, bool add_if_missing);
            bool                                replaceTaxonNames(const std::string & newick_with_taxon_names, std::string & newick_with_taxon_numbers);
            void                                parseTranslate(const std::string & file_contents);
            void                                getPhyloBayesNewicks(std::vector< std::string > & tree_descriptions, const std::string & file_contents, unsigned skip);
            std::string                         standardizeNodeNumber(std::smatch const & what);
            std::string                         standardizeTreeDescription(std::string & newick);
            void                                getNewicks(std::vector< std::string > & tree_descriptions, const std::string & file_contents, unsigned skip);
            std::vector<std::string>            getTreeFileList(std::string listfname);

            // profile
            void                                writeInfoProfile(TreeManip::SharedPtr tm, std::vector<GalaxInfo> & clade_info);

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

            ccd_map_t                _ccdlist;   // _ccdlist[i][j] stores conditional clade info for clade i, subset j, for trees specified by --listfile
            ccd_map_t                _ccdtree;   // _ccdtree[i][0] stores conditional clade info for clade i for trees specified by --treefile

            subset_tree_set_t                   _treeCCD;  // _treeCCD[i] is a set of tree IDs (list of cond. clades) appearing in the sample for subset i
            subset_tree_map_t                   _treeMap;  // _treeMap[i] is a map relating tree IDs (keys) to counts (values) of each tree topology sampled in subset i (only used if _show_details is true)
        };

    inline Galax::Galax(const std::string outfile_prefix, const std::string version) {
        _outfprefix = outfile_prefix;
        _version = version;
    }

    inline Galax::~Galax() {
    }

    inline void Galax::storeTrees(std::string file_contents, unsigned skip, std::vector< std::string > & tree_descriptions) {
        _start_time = getCurrentTime();

        if (isNexusFile(file_contents))
            {
            parseTranslate(file_contents);
            getNewicks(tree_descriptions, file_contents, skip);
            }
        else
            {
            throw XStrom("Galax currently requires tree files to be in NEXUS format.");
            //getPhyloBayesNewicks(tree_descriptions, file_contents, skip);

            //TODO: this is ugly - try to get rid of _reverse_translate by converting _translate to map(string, int) rather than map(int, string)
            //_translate.clear();
            //for (std::map< std::string, unsigned>::iterator it = _reverse_translate.begin(); it != _reverse_translate.end(); ++it)
            //    {
            //    _translate[it->second] = it->first;
            //    }
            }

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

    //void Galax::getTrees(std::string file_contents, unsigned skip)
    //    {
    //    _start_time = getCurrentTime();
    //
    //    std::vector< std::string > tree_descriptions;
    //    if (isNexusFile(file_contents))
    //        {
    //        parseTranslate(file_contents);
    //        getNewicks(tree_descriptions, file_contents, skip);
    //        }
    //    else
    //        {
    //        throw XStrom("Galax currently requires tree files to be in NEXUS format.");
    //        //getPhyloBayesNewicks(tree_descriptions, file_contents, skip);
    //
    //        //TODO: this is ugly - try to get rid of _reverse_translate by converting _translate to map(string, int) rather than map(int, string)
    //        //_translate.clear();
    //        //for (std::map< std::string, unsigned>::iterator it = _reverse_translate.begin(); it != _reverse_translate.end(); ++it)
    //        //    {
    //        //    _translate[it->second] = it->first;
    //        //    }
    //        }
    //    _tree_counts.push_back((unsigned)tree_descriptions.size());
    //    _newicks.clear();
    //    _newicks.insert(_newicks.end(), tree_descriptions.begin(), tree_descriptions.end());
    //    _merged_newicks.insert(_merged_newicks.end(), tree_descriptions.begin(), tree_descriptions.end());
    //
    //    _end_time = getCurrentTime();
    //    _total_seconds += secondsElapsed(_start_time, _end_time);
    //    }

    inline std::vector<std::string> Galax::getTreeFileList(std::string listfname) {
        _start_time = getCurrentTime();

        std::string file_contents;
        getFileContents(file_contents, listfname);

        std::vector<std::string> tree_file_names;
        extractAllWhitespaceDelimitedStrings(tree_file_names, file_contents);

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);

        return tree_file_names;
    }

    inline bool Galax::isNexusFile(const std::string & file_contents) {
        std::regex pattern("#nexus|#Nexus|#NEXUS");
        std::smatch what;
        std::string first_six_characters = file_contents.substr(0,6);
        bool is_nexus_file = (bool)std::regex_match(first_six_characters, what, pattern);
        return is_nexus_file;
    }

    inline unsigned Galax::taxonNumberFromName(const std::string taxon_name, bool add_if_missing) {
        unsigned taxon_number = 0;
        std::map< std::string, unsigned >::iterator it = _taxon_map.find(taxon_name);
        if (it != _taxon_map.end()) {
            // taxon_name is an existing key, so return its value
            taxon_number = it->second;
        }
        else {
            if (add_if_missing) {
                // taxon_name is not an existing key, so create an entry for it
                taxon_number = (unsigned)(_taxon_map.size() + 1);
                _taxon_map[taxon_name] = taxon_number;
            }
            else
                throw XStrom(boost::str(boost::format("Could not find %s in _taxon_map keys") % taxon_name));
        }

        return taxon_number;
    }

    inline bool Galax::replaceTaxonNames(const std::string & newick_with_taxon_names, std::string & newick_with_taxon_numbers) {
        newick_with_taxon_numbers.clear();
        std::regex taxonexpr("[(,]+\\s*([^,):]+)");
        std::sregex_iterator m1(newick_with_taxon_names.begin(), newick_with_taxon_names.end(), taxonexpr);
        std::sregex_iterator m2;
        std::sregex_iterator last;
        for (; m1 != m2; ++m1) {
            std::smatch what = *m1;
            newick_with_taxon_numbers.append(what.prefix());
            newick_with_taxon_numbers.append(what[0].first, what[1].first) ;
            unsigned n = taxonNumberFromName(what[1].str(), true);
            if (n > 0) {
                std::ostringstream oss;
                oss << n;
                newick_with_taxon_numbers.append(oss.str());
            }
            else {
                throw XStrom(boost::str(boost::format("Could not convert taxon name (%s) into a valid taxon number") % what[1].str()));
            }
            last = m1;
        }
        std::smatch what = *last;
        newick_with_taxon_numbers.append(what.suffix());
        return true;
    }

    inline void Galax::parseTranslate(const std::string & file_contents) {
        _translate.clear();

        // The first time this function is called, the _translate command will be used to establish a _taxon_map
        // such that _taxon_map[<taxon name>] = <taxon index>. Subsequent calls will create _translate anew but
        // will not modify _taxon_map; however, an exception is thrown if a taxon is about to be added to _translate
        // that is not a key in _taxon_map.

        // First, separate out the contents of the translate statement from the rest of the tree file
        std::string translate_contents;
        std::regex pattern("[Tt]ranslate([\\S\\s]+?);");
        std::smatch what;
        bool regex_ok = std::regex_search(file_contents, what, pattern);
        if (regex_ok) {
            // what[0] contains the whole string
            // what[1] contains the translate statement contents
            // Construct a string using characters in contents from what[1].first to what[1].second
            translate_contents.insert(translate_contents.begin(), what[1].first, what[1].second);
        }
        else {
            throw XStrom("regex failed to find translate statement in tree file");
        }

        // Now create the map by iterating through each element of the translate statement
        std::regex re("(\\d+)\\s+'?([\\S\\s]+?)'?,?");
        std::sregex_iterator m1(translate_contents.begin(), translate_contents.end(), re);
        std::sregex_iterator m2;
        for (; m1 != m2; ++m1) {
            const std::match_results<std::string::const_iterator>& what = *m1;
            unsigned taxon_index = 0;
            try {
                taxon_index = boost::lexical_cast<unsigned>(what[1].str());
            }
            catch(const boost::bad_lexical_cast &) {
                throw XStrom("Could not interpret taxon index in translate statement as a number");
            }

            // efficientAddOrCheck returns valid iterator on first insertion or if identical association already previously made
            if (efficientAddOrCheck(_translate, taxon_index, what[2].str()) == _translate.end())
                throw XStrom(boost::str(boost::format("Taxon name (%s) does not match name already associated (%s) with taxon %d") % what[2].str() % _translate[taxon_index] % taxon_index));
        }

        typedef std::map< unsigned, std::string > translate_t;
        typedef std::map< std::string, unsigned > taxonmap_t;
        if (_taxon_map.empty()) {
            // Copy _translate to _taxon_map
            for (auto & p : _translate) {
                _taxon_map[p.second] = p.first;
            }
        }
        else {
            // Check to make sure this file does not have more or fewer taxa than files previously processed
            if (_translate.size() != _taxon_map.size())
               throw XStrom("Galax requires taxa to be the same for all tree files processed.");

            // Check to make sure all taxa in _translate are keys in _taxon_map
            for (auto & p : _translate) {
                taxonmap_t::iterator it = _taxon_map.lower_bound(p.second);
                if (it == _taxon_map.end())
                    throw XStrom(boost::str(boost::format("Taxon name (%s) does not match any name stored for tree files read previously") % p.second));
            }
        }
    }

    inline void Galax::getPhyloBayesNewicks(std::vector< std::string > & tree_descriptions, const std::string & file_contents, unsigned skip) {
        tree_descriptions.clear();
        std::regex re("^(.+?);");
        std::sregex_iterator m1(file_contents.begin(), file_contents.end(), re);
        std::sregex_iterator m2;  // empty iterator used only to detect when we are done
        unsigned n = 0;
        for (; m1 != m2; ++m1) {
            const std::match_results<std::string::const_iterator>& what = *m1;
            if (n >= skip) {
                std::string newick_only;
                replaceTaxonNames(what[1].str(), newick_only);
                tree_descriptions.push_back(newick_only);
            }
            n += 1;
            std::cerr << n << "\n";
        }
    }

    inline std::string Galax::standardizeNodeNumber(std::smatch const & what) {
        int x = atoi(what[2].str().c_str());

        // ensure that node numbers correspond to taxon indices stored in taxon_map,
        // which are not necessarily the same as those used in the translate command that
        // was in the tree file
        std::string & stored_taxon_name = _translate[x];
        unsigned stored_taxon_index = _taxon_map[stored_taxon_name];
        std::string s = boost::str(boost::format("%s%d") % what[1] % stored_taxon_index);
        return s;
    }

    inline std::string Galax::standardizeTreeDescription(std::string & newick_in) {
        // Search for node numbers in newick_in and replace them with standard node numbers stored in _taxon_map
        std::string newick_out;

        // regex pattern stores '(' or ',' just preceding a node number as group 1
        // and the node number itself as group 2
        std::regex re("([(,])(\\d+)");
        std::smatch match;  // match results for string
        auto iter = newick_in.cbegin();
        auto end = newick_in.cend();
        for (; std::regex_search(iter, end, match, re); iter = match[0].second) {
            // loop through matches, stopping when iter equals the past-end iterator (match[0].second)
            newick_out += match.prefix();
            newick_out += standardizeNodeNumber(match);
        }
        newick_out.append(iter,end);
        return newick_out;
    }

    inline void Galax::getNewicks(std::vector< std::string > & tree_descriptions, const std::string & file_contents, unsigned skip) {
        // tree STATE_0 [&lnP=-4493.80476846934,posterior=-4493.80476846934] = [&R] (9:[&rate=0.49971158909783764]1851.4724462198697,((1:[&rate=0.5965730394621352]292.73199858783727,(10:[&rate=0.6588031360335018]30.21172743645451,5:[&rate=0.7098036299867017]30.21172743645451):[&rate=1.0146941544458208]262.52027115138276):[&rate=1.0649642758561977]510.452441519872,((8:[&rate=0.7554924641162211]145.1076605992074,(7:[&rate=0.7984750329147966]64.0435017480143,6:[&rate=0.8402528958963882]64.0435017480143):[&rate=1.1206854064651213]81.06415885119311):[&rate=1.1844450597679457]522.823827314411,((3:[&rate=0.8818808237868384]60.2962343089954,4:[&rate=0.9242396697890951]60.2962343089954):[&rate=1.260685743226102]12.793802911399744,2:[&rate=0.9681896872253556]73.09003722039515):[&rate=1.3582802932633053]594.8414506932232):[&rate=1.4999660689010508]135.25295219409088):[&rate=1.7907115550989796]1048.2880061121605);
        // tree STATE_0 [&lnP=-222468.46405040708,posterior=-222468.46405040708] = [&R] (((((((13:[&rate=0.41948130885774293]6.2959114346539975,((7:[&rate=0.4835763823640333]0.13550093579331035,1:[&rate=0.5209684767630073]0.13550093579331035):[&rate=0.9635652545109973]0.5551042325454463,15:[&rate=0.5492827727849298]0.6906051683387566):[&rate=0.9755113666282312]5.605306266315241):[&rate=0.9876472559631831]10.322795910985544,(17:[&rate=0.5728193948810578]5.685111439869553,2:[&rate=0.5933667775296758]5.685111439869553):[&rate=0.9999937594617617]10.93359590576999):[&rate=1.0125732516497858]5.142489242495607,(5:[&rate=0.6118563212473699]0.15949221153379095,28:[&rate=0.6288406629410516]0.15949221153379095):[&rate=1.0254099216508532]21.601704376601358):[&rate=1.038530094409559]116.24314628707417,(((29:[&rate=0.6446773336086477]0.762443175497069,8:[&rate=0.6596124676296977]0.762443175497069):[&rate=1.051962607302118]22.93607986063065,(23:[&rate=0.6738236879118028]0.5047866135985948,21:[&rate=0.6874440397483901]0.5047866135985948):[&rate=1.0657392562843637]23.193736422529124):[&rate=1.0798953297143943]16.280424093309122,20:[&rate=0.7005762553101323]39.97894712943684):[&rate=1.0944702533868567]98.02539574577247):[&rate=1.1095083777014945]20.87666238489902,18:[&rate=0.713301711000359]158.88100526010834):[&rate=1.1250599481079797]25.369142391234874,(((((6:[&rate=0.7256862972167103]4.157046671443346,27:[&rate=0.7377844041897827]4.157046671443346):[&rate=1.1411823142956972]0.4216380394703174,31:[&rate=0.749641711811296]4.578684710913663):[&rate=1.157941453976706]2.7888871791074665,((14:[&rate=0.7612971942888511]0.4820008033421106,26:[&rate=0.7727845943689399]0.4820008033421106):[&rate=1.175413916572047]2.1006906770419045,33:[&rate=0.7841335302910268]2.582691480384015):[&rate=1.1936893354723344]4.784880409637115):[&rate=1.2128737226386077]28.320425658481167,(4:[&rate=0.7953703429930343]9.047384492551547,19:[&rate=0.8065187562334204]9.047384492551547):[&rate=1.2330938592149618]26.640613055950748):[&rate=1.254503252991522]144.44188595962413,(34:[&rate=0.8176003998695023]49.84530458500085,(((35:[&rate=0.8286352317599452]4.64783851774415,9:[&rate=0.8396418838264186]4.64783851774415):[&rate=1.2772903877797566]17.7189245645138,(12:[&rate=0.8506379510097704]9.214275524579357,(3:[&rate=0.8616402371306506]6.8925402326469944,32:[&rate=0.8726649683413712]6.8925402326469944):[&rate=1.301690414163583]2.321735291932362):[&rate=1.3280021656518173]13.152487557678594):[&rate=1.356613709910537]7.407288154720803,((30:[&rate=0.8837279825004343]3.4163490119965814,25:[&rate=0.8948449011281925]3.4163490119965814):[&rate=1.3880421571600987]13.604316647910641,24:[&rate=0.9060312894219462]17.020665659907223):[&rate=1.422998494768541]12.75338557707153):[&rate=1.4624990961768445]20.071253348022093):[&rate=1.5080711470564487]130.2845789231256):[&rate=1.5621665830644125]4.120264143216787):[&rate=1.6291050095698845]125.24494247082069,(16:[&rate=0.9173028089947355]35.446341908327426,((22:[&rate=0.9286753674700471]0.013463304338836088,11:[&rate=0.940165268760073]0.013463304338836088):[&rate=1.7176458014779916]7.67054782472139,10:[&rate=0.9517893784722757]7.6840111290602255):[&rate=1.8504611669408024]27.762330779267202):[&rate=2.133204264216236]274.04874821383646);
        // tree STATE_0 = ((((1:0.015159374158485823,6:0.015159374158485823):0.0064804882886747035,2:0.021639862447160527):0.104324463428508,5:0.12596432587566853):0.30645174794996116,(3:0.4084347373105321,4:0.4084347373105321):0.023981336515097595):0.0;
        tree_descriptions.clear();
        std::regex re("[\\n]\\s*[Tt]ree([\\S\\s]+?);");
        std::sregex_iterator m1(file_contents.begin(), file_contents.end(), re);
        std::sregex_iterator m2;  // empty iterator used only to detect when we are done
        unsigned n = 0;
        for (; m1 != m2; ++m1) {
            const std::match_results<std::string::const_iterator>& what = *m1;
            if (n >= skip) {
                std::string stripped = stripComments( what[1].str() );
                std::string newick_only = stripTreeName(stripped);
                std::string std_newick = standardizeTreeDescription(newick_only);
                tree_descriptions.push_back(std_newick);
            }
            n += 1;
        }
    }

    inline void Galax::processTrees(TreeManip::SharedPtr tm, ccd_map_t & ccdmap, unsigned subset_index, unsigned num_subsets) {
        _start_time = getCurrentTime();

        for (std::vector< std::string >::const_iterator sit = _newicks.begin(); sit != _newicks.end(); ++sit) {
            try {
                tm->buildFromNewick(*sit, _rooted, false);
                if (!_rooted)
                    tm->rerootAtNodeNumber(_outgroup - 1);
                tm->addToCCDMap(ccdmap, _treeCCD, _treeMap, _show_details, subset_index, num_subsets);
            }
            catch(XStrom & x) {
                std::cerr << "ERROR: " << x.what() << std::endl;
                std::exit(1);
            }
        }

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

    inline void Galax::writeInfoProfile(TreeManip::SharedPtr tm, GalaxInfo::_galaxinfo_vect_t & clade_info) {
        _start_time = getCurrentTime();

        double sample_size = (double)_newicks.size();

        std::vector< double > profile_times;
        std::vector< double > profile_weights;

        // Create a map from clade_info that provides the information content for any given clade
        std::map< std::string, double > clade_map;
        for (GalaxInfo::_galaxinfo_vect_t::const_iterator gi = clade_info.begin(); gi != clade_info.end(); ++gi) {
            const GalaxInfo & ginfo = *gi;
            double Iprop =ginfo._value[0]/100.0;
            double cladeprob = ginfo._value[2];
            double wt = Iprop/(cladeprob*sample_size);
            clade_map[ginfo._name] = wt;
        }
        std::cerr << "clade_map.size() = " << clade_map.size() << std::endl;

        // Build all trees, storing the time and information content (proportion of maximum possible) for each clade in profile_times and profile_weights, respectively
        for (std::vector< std::string >::const_iterator sit = _newicks.begin(); sit != _newicks.end(); ++sit) {
            try {
                tm->buildFromNewick(*sit, _rooted, false);
                if (!_rooted)
                    tm->rerootAtNodeNumber(_outgroup - 1);
                tm->addToProfile(profile_times, profile_weights, clade_map);
            }
            catch(XStrom & x) {
                std::cerr << "ERROR: " << x.what() << std::endl;
                std::exit(1);
            }
            //break;  //temporary!
        }

        // normalize profile weights
        double sum_weights = std::accumulate(profile_weights.begin(), profile_weights.end(), 0.0);
        std::cerr << "sum_weights = " << sum_weights << std::endl;
        std::transform(profile_weights.begin(), profile_weights.end(), profile_weights.begin(),
            [sum_weights](double x) {return x/sum_weights;});

        /*
        This is an example in R of weighted kernel density estimation:

        x <- seq(-1,6,.01)  # this is a list of 7*100=700 points along the x axis to use for plotting lines
        y1 <- 1             # first data point
        y2 <- 4             # second data point
        y3 <- 5             # third data point
        w1 <- 2/3           # weight for first data point
        w2 <- 1/6           # weight for second data point
        w3 <- 1/6           # weight for third data point
        h <- 2.0            # the bandwidth (standard deviation of component kernel densities)

        # plot the kernel density estimate using the black-box function "density"
        plot(density(c(y1,y2,y3), bw=h, kernel="gaussian", weights=c(w1,w2,w3)), xlim=c(-1,6), ylim=c(0,0.5), lwd=10, col="gray", lty="solid")

        # plot the three component kernel densities using blue, dotted lines
        curve(dnorm((x-y1)/h), col="blue", lty="dotted", add=T)
        curve(dnorm((x-y2)/h), col="blue", lty="dotted", add=T)
        curve(dnorm((x-y3)/h), col="blue", lty="dotted", add=T)

        # plot the kernel density estimate again (but do not use the "density" function this time) using a thicker, red, dotted line
        #curve(w1*dnorm((x-y1)/h)/h + w2*dnorm((x-y2)/h)/h + w3*dnorm((x-y3)/h)/h, add=T, lty="dotted", col="red", lwd=2)
        curve(w1*dnorm(x,y1,h) + w2*dnorm(x,y2,h) + w3*dnorm(x,y3,h), add=T, lty="dotted", col="red", lwd=2)
        */

        std::ofstream profile("profile.R");
        unsigned n = (unsigned)profile_times.size();
        assert(n == (unsigned)profile_weights.size());
        profile << "t <- c(" << join(profile_times.begin(), profile_times.end(), ",") << ")" << std::endl;
        profile << "w <- c(" << join(profile_weights.begin(), profile_weights.end(), ",") << ")" << std::endl;
        profile << "density(t, weights=w, from=0, give.Rkern=T)" << std::endl;
        profile << "plot(density(t, weights=w, from=0, adjust=1), main=\"Information Profile\", xlab=\"Node Height\", ylab=\"\")" << std::endl;
        profile << "rug(t)" << std::endl;
        profile.close();

        std::ofstream pro_file("profile.txt");
        for (unsigned i = 0; i < n; ++i) {
            pro_file << profile_times[i] << '\t' << profile_weights[i] << std::endl;
        }
        profile.close();

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

    inline void Galax::debugShowCCDMap(ccd_map_t & ccdmap, unsigned subset_index) {
        std::cerr << "CCD map has " << ccdmap.size() << " elements." << std::endl;
        for (ccd_map_t::iterator it = ccdmap.begin(); it != ccdmap.end(); ++it) {
            const split_vect_t & v = it->first;
            double count = 0.0;
            if (subset_index < _ALLSUBSETS)
                count = it->second[subset_index];
            else
                count = std::accumulate(it->second.begin(), it->second.end(), 0.0);
            std::cerr << "\n+--------------------------" << std::endl;
            for (auto & x : v) {
                _outf << "| " << x.createPatternRepresentation() << std::endl;
            }
            _outf << "| " << count << std::endl;
        }
    }

    inline void Galax::estimateInfo(ccd_map_t & ccdmap, std::string & summaryinfostr, std::string & detailedinfostr, GalaxInfo::_galaxinfo_vect_t & clade_info) {
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
        unsigned ntaxa = (unsigned)_translate.size();
        GalaxData gd(_tree_counts, _treefile_names, (_rooted ? ntaxa : ntaxa - 1), (_rooted ? 0 : _outgroup));
        if (_show_details)
            gd.setShowDetails(true);
        else
            gd.setShowDetails(false);

        //unsigned total_trees = std::accumulate(_tree_counts.begin(), _tree_counts.end(), 0);

        // Provide a key to the taxa in split representations in both output files
        std::string tmpstr = "Order of taxa in split representations:\n";
        typedef std::pair<unsigned, std::string> translate_map_type;
        for (auto & translate_key_value : _translate) {
            tmpstr += boost::str(boost::format("%12d  %s\n") % translate_key_value.first % translate_key_value.second);
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

    inline void Galax::mapToTree(std::string & maptofname, GalaxInfo::_galaxinfo_vect_t & annotate_info, std::string & mapto_newick) {
        _start_time = getCurrentTime();

        _rooted = _mapto_rooted;

        // Read tree file maptofname and store newick tree descriptions therein
        mapto_newick.clear();
        std::string file_contents;
        getFileContents(file_contents, maptofname);
        std::vector< std::string > newicks;
        storeTrees(file_contents, 0, newicks);

        // Build the first newick tree description
        TreeManip::SharedPtr tm(new TreeManip());
        tm->buildFromNewick(newicks[0], _rooted, false);
        if (!_rooted)
            tm->rerootAtNodeNumber(_outgroup - 1);

        // Ensure that annotate_info is sorted so that the clades with highest posterior are first
        // This is important only for calculation of IC
        GalaxInfo::_sortby_index = 2;   // 0=Ipct, 1=D, 2=w, 3=I
        std::sort(annotate_info.begin(), annotate_info.end(), std::greater<GalaxInfo>());

        // Walk through tree calling setInfo, setWeight, setDisparity, and setCertainty for each internal node
        tm->annotateTree(annotate_info);

        // Create annotated newick tree description
        mapto_newick = tm->makeNewick(5, true);

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

    inline void Galax::buildMajorityRuleTree(GalaxInfo::_galaxinfo_vect_t & majrule_info, GalaxInfo::_galaxinfo_vect_t & annotate_info, std::string & majrule_newick) {
        _start_time = getCurrentTime();

        majrule_newick.clear();

        // We can save a lot of computation if both info vectors are the same
        bool same_info = (&majrule_info == &annotate_info);

        // Ensure that both majrule_info and annotate_info are sorted so that the clades with highest posterior are first
        GalaxInfo::_sortby_index = 2;   // 0=Ipct, 1=D, 2=w, 3=I
        std::sort(majrule_info.begin(), majrule_info.end(), std::greater<GalaxInfo>());
        if (!same_info)
            std::sort(annotate_info.begin(), annotate_info.end(), std::greater<GalaxInfo>());

        // Go through all clades in majrule_info and store splits with w > 0.5
        double log2 = log(2.0);
        std::vector<Split> majrule_splits;
        for (GalaxInfo & c : majrule_info) {
            double w    = c._value[2];

            Split split;
            split.createFromPattern(c._name);

            if (w <= 0.5) {
                // majrule_info is sorted by w, so we can stop once we pass the 0.5 point
                // Note the less-than-or-equals sign above: using just less-than will get you into trouble
                // if there are muliple conflicting clades that have exactly 50% support.
                break;
            }
            else {
                // Add split to vector used to construct majority-rule tree
                majrule_splits.push_back(split);
            }
        }

        // Have now found all the splits comprising the majority rule tree but still
        // need to annotate each of these splits.
        if (majrule_splits.size() == 0)
            return;

        // identify most probable conflicting splits (from annotate_info) for those in majority rule tree
        for (Split & split : majrule_splits) {
            bool split_found = false;
            bool ic_computed = false;
            for (GalaxInfo & c : annotate_info) {
                Split asplit;
                asplit.createFromPattern(c._name);
                asplit.setWeight(c._value[2]);
                if (split == asplit) {
                    double info = c._value[0];
                    double d    = c._value[1];
                    double w    = c._value[2];

                    split.setInfo(info);
                    split.setWeight(w);
                    split.setDisparity(d);

                    split_found = true;
                }
                else if (split_found && !asplit.isCompatible(split)) {
                    // calculate internode certainty
                    //TODO: this can be done more efficiently using the conditional clade distribution
                    double p = split.getWeight();
                    double q = asplit.getWeight();
                    double ic = 1.0 + (p/(p+q))*log(p/(p+q))/log2 + (q/(p+q))*log(q/(p+q))/log2;
                    split.setCertainty(ic);
                    ic_computed = true;
                    break;
                }
            }
            assert(split_found);
            if (!ic_computed) {
                split.setCertainty(1.0);    // no conflicting splits found
            }
        }

        TreeManip::SharedPtr tm(new TreeManip());
        tm->buildFromSplitVector(majrule_splits, _rooted);
        if (!_rooted)
            tm->rerootAtNodeNumber(_outgroup - 1);
        majrule_newick = tm->makeNewick(5, true);

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

    inline void Galax::writeMajruleTreefile(std::string fnprefix, std::string & majrule_newick) {
        _start_time = getCurrentTime();

        if (majrule_newick.length() == 0) {
            _outf << "Majority rule tree is the star tree: no tree file written." << std::endl;
            return;
        }

        std::string treefname = boost::str(boost::format("%s-%s.tre") % _outfprefix % fnprefix);
        _treef.open(treefname.c_str());

        // Output the translate command
        _treef << "#nexus\n\nbegin trees;\n  translate\n";
        typedef std::map<std::string, unsigned > taxon_map_type;
        std::vector<std::string> translate_elements(_taxon_map.size());
        for (auto & p : _taxon_map) {
            translate_elements[p.second - 1] = boost::str(boost::format("    %d '%s'") % p.second % p.first);
        }
        _treef << boost::algorithm::join(translate_elements, ",\n") << ";" << std::endl;

        // Output the tree command
        _treef << "tree majrule = " << majrule_newick << ";\nend;" << std::endl;
        _treef.close();

        _end_time = getCurrentTime();
        _total_seconds += secondsElapsed(_start_time, _end_time);
    }

    inline void Galax::initTreeCCD(unsigned num_subsets) {
        // initialize _treeCCD, which will hold tree IDs for all unique tree topologies in each subset
        _treeCCD.clear();
        _treeMap.clear();
        for (unsigned i = 0; i <= num_subsets; ++i) {
            tree_id_set_t v;
            _treeCCD.push_back(v);
            tree_map_t m;
            _treeMap.push_back(m);
        }
    }

#if 0
    inline void Galax::run(std::string treefname, std::string listfname, unsigned skip, bool trees_rooted, std::string maptofname, bool mapto_trees_rooted, bool save_details, unsigned outgroup_taxon) {
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

            _show_details = save_details;
            _outgroup = outgroup_taxon;
            _input_rooted = trees_rooted;
            _mapto_rooted = mapto_trees_rooted;
            _rooted = _input_rooted;
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

                GalaxInfo::_galaxinfo_vect_t majrule_clade_info;
                std::vector<Split> majrule_splits;
                std::string majrule_tree;
                estimateInfo(_ccdtree, summaryinfostr, detailedinfostr, majrule_clade_info);
                buildMajorityRuleTree(majrule_clade_info, majrule_clade_info, majrule_tree);
                writeMajruleTreefile("majrule", majrule_tree);

                //std::string mapto_tree;
                //mapToTree(majrule_clade_info, majrule_clade_info, mapto_tree);
                //writeMajruleTreefile("@@@", mapto_tree);

                //if (_rooted)
                //    {
                //    // profiling informativeness only makes sense for ultrametric trees
                //    writeInfoProfile(tm, majrule_clade_info);
                //    }
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
                for (std::string & tree_file_name : _treefile_names) {
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
                GalaxInfo::_galaxinfo_vect_t merged_clade_info;
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
#else
    inline void Galax::run(std::string treefname,
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

                GalaxInfo::_galaxinfo_vect_t majrule_clade_info;
                std::vector<Split> majrule_splits;
                std::string majrule_tree;
                estimateInfo(_ccdtree, summaryinfostr, detailedinfostr, majrule_clade_info);
                //buildMajorityRuleTree(majrule_clade_info, majrule_clade_info, majrule_tree);
                //writeMajruleTreefile("majrule", majrule_tree);
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
                GalaxInfo::_galaxinfo_vect_t merged_clade_info;
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
