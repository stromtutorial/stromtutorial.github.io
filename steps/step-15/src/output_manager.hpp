#pragma once    ///start

#include "data.hpp"
#include "tree_manip.hpp"
#include "model.hpp"
#include "xstrom.hpp"
#include <fstream>

namespace strom {

    class OutputManager {
        public:
            typedef std::shared_ptr< OutputManager > SharedPtr;

                                       OutputManager();
                                       ~OutputManager();
            
            void                       setModel(Model::SharedPtr model) {_model = model;}
            void                       setTreeManip(TreeManip::SharedPtr tm) {_tree_manip = tm;}
            
            void                       openTreeFile(std::string filename, Data::SharedPtr data);
            void                       openParameterFile(std::string filename, Model::SharedPtr model);
            
            void                       closeTreeFile();
            void                       closeParameterFile();

            void                       outputConsole(std::string s);
            void                       outputTree(unsigned iter, TreeManip::SharedPtr tm);
            void                       outputParameters(unsigned iter, double lnL, double lnP, double TL, Model::SharedPtr model);
            

        private:

            TreeManip::SharedPtr       _tree_manip;
            Model::SharedPtr           _model;
            std::ofstream              _treefile;
            std::ofstream              _parameterfile;
            std::string                _tree_file_name;
            std::string                _param_file_name;
    };

    // member function bodies go here
    ///end_class_declaration
    inline OutputManager::OutputManager() { ///begin_constructor
        //std::cout << "Constructing an OutputManager" << std::endl;
        _tree_file_name = "trees.t";
        _param_file_name = "params.p";
    }

    inline OutputManager::~OutputManager() {
        //std::cout << "Destroying an OutputManager" << std::endl;
    } ///end_destructor

    inline void OutputManager::openTreeFile(std::string filename, Data::SharedPtr data) {   ///begin_openTreeFile
        assert(!_treefile.is_open());
        _tree_file_name = filename;
        _treefile.open(_tree_file_name.c_str());
        if (!_treefile.is_open())
            throw XStrom(boost::str(boost::format("Could not open tree file \"%s\"") % _tree_file_name));

        _treefile << "#nexus\n\n";
        _treefile << data->createTaxaBlock() << std::endl;
       
        _treefile << "begin trees;\n";
        _treefile << data->createTranslateStatement() << std::endl;
    }

    inline void OutputManager::closeTreeFile() {
        assert(_treefile.is_open());
        _treefile << "end;\n";
        _treefile.close();
    }   ///end_closeTreeFile

    inline void OutputManager::openParameterFile(std::string filename, Model::SharedPtr model) {    ///begin_openParameterFile
        assert(model);
        assert(!_parameterfile.is_open());
        _param_file_name = filename;
        _parameterfile.open(_param_file_name.c_str());
        if (!_parameterfile.is_open())
            throw XStrom(boost::str(boost::format("Could not open parameter file \"%s\"") % _param_file_name));
        _parameterfile << boost::str(boost::format("%s\t%s\t%s\t%s\t%s") % "iter" % "lnL" % "lnPr" % "TL" % model->paramNamesAsString("\t")) << std::endl;
    }

    inline void OutputManager::closeParameterFile() {
        assert(_parameterfile.is_open());
        _parameterfile.close();
    }   ///end_closeParameterFile

    inline void OutputManager::outputConsole(std::string s) {   ///begin_outputConsole
        std::cout << s << std::endl;
    }
    
    inline void OutputManager::outputTree(unsigned iter, TreeManip::SharedPtr tm) {
        assert(_treefile.is_open());
        assert(tm);
        _treefile << boost::str(boost::format("  tree iter_%d = %s;") % iter % tm->makeNewick(5)) << std::endl;
    }
    
    inline void OutputManager::outputParameters(unsigned iter, double lnL, double lnP, double TL, Model::SharedPtr model) {
        assert(model);
        assert(_parameterfile.is_open());
        _parameterfile << boost::str(boost::format("%d\t%.5f\t%.5f\t%.5f\t%s") % iter % lnL % lnP % TL % model->paramValuesAsString("\t")) << std::endl;
    }///end_outputParameters

}   ///end
