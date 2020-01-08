#pragma once    ///start

#include "genetic_code.hpp"
#include <boost/format.hpp>

namespace strom {

    class DataType {
        public:
                                            DataType();
                                            ~DataType();

            void                            setNucleotide();
            void                            setCodon();
            void                            setProtein();
            void                            setStandard();
        
            bool                            isNucleotide() const;
            bool                            isCodon() const;
            bool                            isProtein() const;
            bool                            isStandard() const;

            void                            setStandardNumStates(unsigned nstates);
            void                            setGeneticCodeFromName(std::string genetic_code_name);
            void                            setGeneticCode(GeneticCode::SharedPtr gcode);

            unsigned                        getDataType() const;
            unsigned                        getNumStates() const;
            std::string                     getDataTypeAsString() const;
            const GeneticCode::SharedPtr    getGeneticCode() const;
            
            static std::string              translateDataTypeToString(unsigned datatype);

        private:

            unsigned                        _datatype;
            unsigned                        _num_states;
            GeneticCode::SharedPtr          _genetic_code;
    };
    
    // member function bodies go here
    ///end_class_declaration
    inline DataType::DataType() : _datatype(0), _num_states(0) {    ///begin_constructor
        //std::cout << "Creating a DataType object" << std::endl;
        setNucleotide();
    }
    
    inline DataType::~DataType() {
        //std::cout << "Destroying a DataType object" << std::endl;
    }    ///end_destructor
    
    inline void DataType::setNucleotide() {    ///begin_setNucleotide
        _datatype = 1;
        _num_states = 4;
        _genetic_code = nullptr;
    }
    
    inline void DataType::setCodon() {
        _datatype = 2;
        _genetic_code = GeneticCode::SharedPtr(new GeneticCode("standard"));
        _num_states = _genetic_code->getNumNonStopCodons();
    }
    
    inline void DataType::setProtein() {
        _datatype = 3;
        _num_states = 20;
        _genetic_code = nullptr;
    }
    
    inline void DataType::setStandard() {
        _datatype = 4;
        _num_states = 2;
        _genetic_code = nullptr;
    }   ///end_setStandard

    inline bool DataType::isNucleotide() const {    ///begin_isNucleotide
        return (_datatype == 1);
    }

    inline bool DataType::isCodon() const {
        return (_datatype == 2);
    }

    inline bool DataType::isProtein() const {
        return (_datatype == 3);
    }

    inline bool DataType::isStandard() const {
        return (_datatype == 4);
    }   ///end_isStandard

    inline void DataType::setGeneticCodeFromName(std::string genetic_code_name) {   ///begin_setGeneticCodeFromName
        assert(isCodon());
        _genetic_code = GeneticCode::SharedPtr(new GeneticCode(genetic_code_name));
    }
    
    inline void DataType::setGeneticCode(GeneticCode::SharedPtr gcode) {
        assert(isCodon());
        assert(gcode);
        _genetic_code = gcode;
    }

    inline void DataType::setStandardNumStates(unsigned nstates) {
        _datatype = 4;
        _num_states = nstates;
        _genetic_code = nullptr;
    }   ///end_setStandardNumStates

    inline unsigned DataType::getDataType() const {   ///begin_getDataType
        return _datatype;
    }
    
    inline unsigned DataType::getNumStates() const {
        return _num_states;
    }
    
    inline const GeneticCode::SharedPtr DataType::getGeneticCode() const {
        assert(isCodon());
        return _genetic_code;
    }   ///end_getGeneticCode
    
    inline std::string DataType::getDataTypeAsString() const {   ///begin_getDataTypeAsString
        std::string s = translateDataTypeToString(_datatype);
        if (isCodon())
            s += boost::str(boost::format(",%s") % _genetic_code->getGeneticCodeName());
        return s;
    }
    
    inline std::string DataType::translateDataTypeToString(unsigned datatype) {
        assert(datatype == 1 || datatype == 2 || datatype == 3 || datatype == 4);
        if (datatype == 1)
            return std::string("nucleotide");
        else if (datatype == 2)
            return std::string("codon");
        else if (datatype == 3)
            return std::string("protein");
        else
            return std::string("standard");
    }   ///end_translateDataTypeToString
    
}   ///end
