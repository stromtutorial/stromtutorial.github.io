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
    }    ///end_constructor
    
    inline DataType::~DataType() {    ///begin_destructor
        //std::cout << "Destroying a DataType object" << std::endl;
    }    ///end_destructor
    
    inline void DataType::setNucleotide() {    ///begin_setNucleotide
        _datatype = 1;
        _num_states = 4;
        _genetic_code = nullptr;
    }   ///end_setNucleotide
    
    inline void DataType::setCodon() {  ///begin_setCodon
        _datatype = 2;
        _genetic_code = GeneticCode::SharedPtr(new GeneticCode("standard"));
        _num_states = _genetic_code->getNumNonStopCodons();
    }   ///end_setCodon
    
    inline void DataType::setProtein() {    ///begin_setProtein
        _datatype = 3;
        _num_states = 20;
        _genetic_code = nullptr;
    }   ///end_setProtein
    
    inline void DataType::setStandard() {   ///begin_setStandard
        _datatype = 4;
        _num_states = 2;
        _genetic_code = nullptr;
    }   ///end_setStandard

    inline bool DataType::isNucleotide() const {    ///begin_isNucleotide
        return (_datatype == 1);
    }   ///end_isNucleotide

    inline bool DataType::isCodon() const { ///begin_isCodon
        return (_datatype == 2);
    }   ///end_isCodon

    inline bool DataType::isProtein() const {   ///begin_isProtein
        return (_datatype == 3);
    }   ///end_isProtein

    inline bool DataType::isStandard() const {  ///begin_isStandard
        return (_datatype == 4);
    }   ///end_isStandard

    inline void DataType::setGeneticCodeFromName(std::string genetic_code_name) {   ///begin_setGeneticCodeFromName
        assert(isCodon());
        _genetic_code = GeneticCode::SharedPtr(new GeneticCode(genetic_code_name));
    }   ///end_setGeneticCodeFromName
    
    inline void DataType::setGeneticCode(GeneticCode::SharedPtr gcode) {   ///begin_setGeneticCode
        assert(isCodon());
        assert(gcode);
        _genetic_code = gcode;
    }   ///end_setGeneticCode

    inline void DataType::setStandardNumStates(unsigned nstates) {   ///begin_setStandardNumStates
        _datatype = 4;
        _num_states = nstates;
        _genetic_code = nullptr;
    }   ///end_setStandardNumStates

    inline unsigned DataType::getDataType() const {   ///begin_getDataType
        return _datatype;
    }   ///end_getDataType
    
    inline unsigned DataType::getNumStates() const {   ///begin_getNumStates
        return _num_states;
    }   ///end_getNumStates
    
    inline const GeneticCode::SharedPtr DataType::getGeneticCode() const {   ///begin_getGeneticCode
        assert(isCodon());
        return _genetic_code;
    }   ///end_getGeneticCode
    
    inline std::string DataType::getDataTypeAsString() const {   ///begin_getDataTypeAsString
        std::string s = translateDataTypeToString(_datatype);
        if (isCodon())
            s += boost::str(boost::format(",%s") % _genetic_code->getGeneticCodeName());
        return s;
    }   ///end_getDataTypeAsString
    
    inline std::string DataType::translateDataTypeToString(unsigned datatype) {   ///begin_translateDataTypeToString
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
