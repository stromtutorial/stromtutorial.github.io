#pragma once

#include <iostream>
#include "tree_summary.hpp"
#include "phyloinfo.hpp"
#include <boost/program_options.hpp>

namespace strom {

    class Strom {
        public:
                                    Strom();
                                    ~Strom();

            void                    clear();
            void                    showSettings() const;
            void                    processCommandLineOptions(int argc, const char * argv[]);
            void                    run();

        private:

            bool                    _save_details;
            bool                    _trees_rooted;
            bool                    _mapto_trees_rooted;
            unsigned                _skipped_newicks;
            unsigned                _outgroup_taxon;
            std::string             _output_file_name;
            std::string             _tree_file_name;
            std::string             _list_file_name;
            std::string             _mapto_file_name;

            static std::string      _program_name;
            static unsigned         _major_version;
            static unsigned         _minor_version;

    };

    inline Strom::Strom() {
        //std::cout << "Constructing a Strom" << std::endl;
        clear();
    }

    inline Strom::~Strom() {
        //std::cout << "Destroying a Strom" << std::endl;
    }

    inline void Strom::clear() {
        _save_details       = false;
        _trees_rooted       = false;
        _mapto_trees_rooted = false;
        _skipped_newicks    = 0;
        _outgroup_taxon     = 1;
        _output_file_name   = "";
        _tree_file_name     = "";
        _list_file_name     = "";
        _mapto_file_name    = "";
    }

    inline void Strom::processCommandLineOptions(int argc, const char * argv[]) {
        boost::program_options::variables_map vm;
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("version,v", "show program version")
            ("rooted,r", boost::program_options::value(&_trees_rooted),
                "expect trees to be rooted (leave out this option to assume unrooted)")
            ("details,d", boost::program_options::value(&_save_details),
                "save information content details for each clade (can generate a lot of output)")
            ("skip,s", boost::program_options::value(&_skipped_newicks),
                "number of tree descriptions to skip at beginning of tree file")
            ("treefile,t", boost::program_options::value(&_tree_file_name),
                "name of tree file in NEXUS format (used to generate a majority-rule consensus tree)")
            ("listfile,l", boost::program_options::value(&_list_file_name),
                "name of file listing whitespace-separated, NEXUS-formatted tree file names to be processed")
            ("outfile,o", boost::program_options::value(&_output_file_name),
                "file name prefix of output file to create (.txt extension will be added)")
            ("outgroup,g", boost::program_options::value(&_outgroup_taxon),
                "number of taxon to use as the outgroup (greater than or equal to 1)")
            ("mapto,m", boost::program_options::value(&_mapto_file_name),
                "information estimates will be mapped onto this tree in addition to the merged consensus tree")
            ("maptorooted,e", boost::program_options::value(&_mapto_trees_rooted),
                "mapto tree file contains rooted trees (leave out this option to assume unrooted)")
        ;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        try {
            const boost::program_options::parsed_options & parsed = boost::program_options::parse_config_file< char >("galax.conf", desc, false);
            boost::program_options::store(parsed, vm);
        }
        catch(boost::program_options::reading_file & x) {
            std::cout << "Note: configuration file (strom.conf) not found" << std::endl;
        }
        boost::program_options::notify(vm);

        // If user specified --help on command line, output usage summary and quit
        if (vm.count("help") > 0) {
            std::cout << desc << "\n";
            std::exit(1);
        }

        // If user specified --version on command line, output version and quit
        if (vm.count("version") > 0) {
            std::cout << boost::str(boost::format("This is %s version %d.%d") % _program_name % _major_version % _minor_version) << std::endl;
            std::exit(1);
        }
        
        // Check to make sure user specified either --treefile or --listfile
        if (_list_file_name.size() == 0 && _tree_file_name.size() == 0) {
            std::cout << "Must specify either --treefile or --listfile (or both) on command line or in config file\n";
            std::cout << desc << std::endl;
            std::exit(1);
        }
    }
    
    inline void Strom::showSettings() const {
        if (_list_file_name.length() > 0 && _tree_file_name.length() > 0) {
            std::cout << "Trees will be read from tree files specified in file " << _list_file_name << std::endl;
            std::cout << "Majority-rule consensus will be constructed from trees in the file " << _tree_file_name << std::endl;
        }
        else if (_list_file_name.length() > 0) {
            std::cout << "Trees will be read from tree files specified in file " << _list_file_name << std::endl;
            std::cout << "Majority-rule consensus will be constructed from the merged tree set" << std::endl;
        }
        else {
            std::cout << "Trees will be read from file " << _tree_file_name << std::endl;
            std::cout << "Majority-rule consensus will be constructed from these trees" << std::endl;
        }

        if (_output_file_name.size() == 0)
            std::cout << "Output will not be saved as no output file was specified.\n";
        else
            std::cout << "Output will be stored in file " << _output_file_name << std::endl;

        if (_save_details)
            std::cout << "Detailed information about all clades will be saved" << std::endl;
        else
            std::cout << "Only summary information will be saved" << std::endl;
        
        if (_mapto_file_name.length() > 0)
            std::cout << "Information will be mapped to " << _mapto_file_name << " in addition to the merged consensus tree" << std::endl;
        else
            std::cout << "Information will be mapped only to the merged consensus tree because no --mapto tree file was specified" << std::endl;

        if (_trees_rooted)
            std::cout << "Trees assumed to be rooted" << std::endl;
        else {
            std::cout << "Trees assumed to be unrooted" << std::endl;
            std::cout << "Outgroup for polarizing splits will be taxon number " << _outgroup_taxon << std::endl;
        }

        if (_skipped_newicks == 0)
            std::cout << "No trees will be skipped" << std::endl;
        else if (_skipped_newicks == 1)
            std::cout << "The first tree will be skipped" << std::endl;
        else
            std::cout << "The first " << _skipped_newicks << " trees will be skipped" << std::endl;
    }

    inline void Strom::run() {
        std::cout << "Starting..." << std::endl;
        showSettings();
        
        try {
            //Galax galax(_output_file_name, boost::str(boost::format("%d.%d") % _major_version % _minor_version));
            //galax.run(_tree_file_name, _list_file_name, _skipped_newicks, _trees_rooted, _mapto_file_name, _mapto_trees_rooted, _save_details, _outgroup_taxon);
            PhyloInfo info(_tree_file_name, _list_file_name, _output_file_name, _skipped_newicks, _trees_rooted, _save_details, _outgroup_taxon, _program_name, _major_version, _minor_version);
            info.run();
        }
        catch (XStrom & x) {
            std::cerr << "Strom encountered a problem:\n  " << x.what() << std::endl;
        }

        std::cout << "\nFinished!" << std::endl;
    }

}
