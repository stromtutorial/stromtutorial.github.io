#pragma once

namespace strom {

    class GalaxInfo {
        public:
            typedef std::vector<GalaxInfo>  _galaxinfo_vect_t;

                                            GalaxInfo() {}
                                            GalaxInfo(const std::string & s, std::vector<double> & v);
                                            ~GalaxInfo() {}

            bool                            operator<(const GalaxInfo & other) const;
            bool                            operator>(const GalaxInfo & other) const;

            std::string                     _name;
            
            // _value for clades:  0=Ipct, 1=D, 2=w, 3=I
            // _value for subsets: 0=Ipct, 1=unique, 2=coverage
            std::vector<double>             _value;

            static unsigned                 _sortby_index;
    };

    inline GalaxInfo::GalaxInfo(const std::string & s, std::vector<double> & v) : _name(s), _value(v) {
        //std::cout << "Creating a GalaxInfo object" << std::endl;
    }
    
    inline bool GalaxInfo::operator<(const GalaxInfo & other) const {
        return (bool)(_value[_sortby_index] < other._value[_sortby_index]);
    }
    
    inline bool GalaxInfo::operator>(const GalaxInfo & other) const {
        return (bool)(_value[_sortby_index] > other._value[_sortby_index]);
    }

}


