#pragma once

#include <vector>
#include <memory>
#include <set>
#include <map>
#include <climits>
#include <cassert>
#include "xstrom.hpp"

namespace strom {

    class Split {
        public:
                                                                Split();
                                                                Split(const Split & other);
                                                                ~Split();

            Split &                                             operator=(const Split & other);
            bool                                                operator==(const Split & other) const;
            bool                                                operator<(const Split & other) const;

            void                                                clear();
            void                                                resize(unsigned nleaves);
            void                                                createFromPattern(std::string s, char on_symbol = '*', char off_symbol = '-');  //POLNEW added

            typedef unsigned long                               split_unit_t;
            typedef std::vector<split_unit_t>                   split_t;
            typedef std::set<Split>                             treeid_t;
            typedef std::map< treeid_t, std::vector<unsigned> > treemap_t;
            //typedef std::tuple<unsigned,unsigned,unsigned>      split_metrics_t;                      //POLNEW deleted

            const split_t &                                     getBits() const;                        //POLNEW added
            split_unit_t                                        getMask() const;                        //POLNEW added
            unsigned                                            getNLeaves() const;                     //POLNEW added
            unsigned                                            getBitsPerUnit() const;                 //POLNEW added
            //split_unit_t                                        getBits(unsigned unit_index) const;   //POLNEW deleted
            
            void                                                addSplit(const Split & other);

            bool                                                getBitAt(unsigned leaf_index) const;
            void                                                setBitAt(unsigned leaf_index);
            
            unsigned                                            countOnBits() const;                    //POLNEW added
            unsigned                                            countOffBits() const;                   //POLNEW added

            bool                                                isEquivalent(const Split & other) const;
            bool                                                isCompatible(const Split & other) const;
            bool                                                conflictsWith(const Split & other) const;
            bool                                                subsumedIn(const Split & other) const;  //POLNEW added

            std::string                                         createPatternRepresentation() const;
            //split_metrics_t                                     getSplitMetrics() const;              //POLNEW deleted
            
            void                                                setInfo(double info);       //POLNEW added
            double                                              getInfo() const;            //POLNEW added

            void                                                setWeight(double w);        //POLNEW added
            double                                              getWeight() const;          //POLNEW added

            void                                                setCertainty(double ic);    //POLNEW added
            double                                              getCertainty() const;       //POLNEW added

            void                                                setDisparity(double d);     //POLNEW added
            double                                              getDisparity() const;       //POLNEW added

        private:

            split_unit_t                                        _mask;
            split_t                                             _bits;
            unsigned                                            _bits_per_unit;
            unsigned                                            _nleaves;

            double                                              _info;              //POLNEW added
            double                                              _weight;            //POLNEW added
            double                                              _certainty;         //POLNEW added
            double                                              _disparity;         //POLNEW added

        public:

            typedef std::shared_ptr< Split >                    SharedPtr;
    };

    // member function bodies go here

    inline Split::Split() {
        _mask = 0L;
        _nleaves = 0;
        _bits_per_unit = (CHAR_BIT)*sizeof(Split::split_unit_t);
        clear();
        //std::cout << "Constructing a Split" << std::endl;
    }

    inline Split::Split(const Split & other) {
        _mask = other._mask;
        _nleaves = other._nleaves;
        _bits_per_unit = (CHAR_BIT)*sizeof(Split::split_unit_t);
        _bits = other._bits;
        //std::cout << "Constructing a Split by copying an existing split" << std::endl;
    }

    inline Split::~Split() {
        //std::cout << "Destroying a Split" << std::endl;
    }

    inline void Split::clear() {
        for (auto & u : _bits) {
            u = 0L;
        }
    }

    inline void Split::createFromPattern(std::string s, char on_symbol, char off_symbol) {
        // Sets non-constant data members using a pattern supplied in the form of the string s
        // consisting of a sequence of on and off symbols. For example, assuming _bits_per_unit = 8,
        // calling the function with the pattern "--*---***-" sets _nleaves to 10, _nunits to 2,
        // _bits[0] to 142 (binary 10001110), and _bits[1] to 0.
        unsigned slen = (unsigned)s.size();
        std::vector<unsigned> on_bits;
        for (unsigned k = 0; k < slen; ++k) {
            if (s[k] == on_symbol)
                on_bits.push_back(k);
            else if (s[k] != off_symbol)
                throw XStrom(str(boost::format("character in pattern (%c) not recognized as either the on symbol (%c) or the off symbol (%c)") % s[k] % on_symbol % off_symbol));
            }

        // No funny characters were found in the supplied pattern string, so we have a green light to build the split
        resize(slen);
        split_unit_t unity = 1;
        for (auto k : on_bits) {
            unsigned i = k/_bits_per_unit;
            unsigned j = k % _bits_per_unit;
            _bits[i] |= (unity << j);
            }
        }

    inline Split & Split::operator=(const Split & other) {
        _nleaves = other._nleaves;
        _bits = other._bits;
        return *this;
    }

    inline bool Split::operator==(const Split & other) const {
        return (_bits == other._bits);
    }

    inline bool Split::operator<(const Split & other) const {
        assert(_bits.size() == other._bits.size());
        return (_bits < other._bits);
    }

    inline void Split::resize(unsigned nleaves) {
        _nleaves = nleaves;
        unsigned nunits = 1 + ((nleaves - 1)/_bits_per_unit);
        _bits.resize(nunits);

        // create mask used to select only those bits used in final unit
        unsigned num_unused_bits = nunits*_bits_per_unit - _nleaves;
        unsigned num_used_bits = _bits_per_unit - num_unused_bits;
        _mask = 0L;
        split_unit_t unity = 1;
        for (unsigned i = 0; i < num_used_bits; i++)
            _mask |= (unity << i);

        clear();
    }

    inline void Split::setBitAt(unsigned leaf_index) {
        unsigned unit_index = leaf_index/_bits_per_unit;
        unsigned bit_index = leaf_index - unit_index*_bits_per_unit;
        split_unit_t unity = 1;
        split_unit_t bit_to_set = unity << bit_index;
        _bits[unit_index] |= bit_to_set;
    }

    inline const Split::split_t & Split::getBits() const {
        return _bits;
    }

    inline Split::split_unit_t Split::getMask() const {
        return _mask;
    }

    inline unsigned Split::getNLeaves() const {
        return _nleaves;
    }

    inline unsigned Split::getBitsPerUnit() const {
        return _bits_per_unit;
    }

    //inline Split::split_unit_t Split::getBits(unsigned unit_index) const {
    //    assert(unit_index < _bits.size());
    //    return _bits[unit_index];
    //}

    inline bool Split::getBitAt(unsigned leaf_index) const {
        unsigned unit_index = leaf_index/_bits_per_unit;
        unsigned bit_index = leaf_index - unit_index*_bits_per_unit;
        split_unit_t unity = 1;
        split_unit_t bit_to_set = unity << bit_index;
        return (bool)(_bits[unit_index] & bit_to_set);
    }

    inline void Split::addSplit(const Split & other) {
        unsigned nunits = (unsigned)_bits.size();
        assert(nunits == other._bits.size());
        for (unsigned i = 0; i < nunits; ++i) {
            _bits[i] |= other._bits[i];
        }
    }

    inline std::string Split::createPatternRepresentation() const {
        std::string s;
        unsigned ntax_added = 0;
        for (unsigned i = 0; i < _bits.size(); ++i) {
            for (unsigned j = 0; j < _bits_per_unit; ++j) {
                split_unit_t bitmask = ((split_unit_t)1 << j);
                bool bit_is_set = ((_bits[i] & bitmask) > (split_unit_t)0);
                if (bit_is_set)
                    s += '*';
                else
                    s += '-';
                if (++ntax_added == _nleaves)
                    break;
            }
        }
        return s;
    }

    inline bool Split::isEquivalent(const Split & other) const {
        unsigned nunits = (unsigned)_bits.size();
        assert(nunits > 0);

        // polarity 1 means root is on the same side of both splits
        // polarity 2 means they are inverted relative to one another
        unsigned polarity = 0;
        for (unsigned i = 0; i < nunits; ++i) {
            split_unit_t a = _bits[i];
            split_unit_t b = other._bits[i];
            bool a_equals_b = (a == b);
            bool a_equals_inverse_b = (a == ~b);
            if (i == nunits - 1) {
                a_equals_inverse_b = (a == (~b & _mask));
            }
            bool ok = (a_equals_b || a_equals_inverse_b);
            if (ok) {
                if (polarity == 0) {
                    // First unit examined determines polarity
                    if (a_equals_b)
                        polarity = 1;
                    else
                        polarity = 2;
                }
                else {
                    // Polarity determined by first unit used for all subsequent units
                    if (polarity == 1 && !a_equals_b )
                        return false;
                    else if (polarity == 2 && !a_equals_inverse_b )
                        return false;
                }
            }
            else {
                return false;
            }
        }

        // All of the units were equivalent, so that means the splits are equivalent
        return true;
    }

    inline bool Split::isCompatible(const Split & other) const {
        for (unsigned i = 0; i < _bits.size(); ++i) {
            split_unit_t a = _bits[i];
            split_unit_t b = other._bits[i];
            split_unit_t a_and_b = (a & b);
            bool equals_a   = (a_and_b == a);
            bool equals_b   = (a_and_b == b);
            if (a_and_b && !(equals_a || equals_b)) {
                // A failure of any unit to be compatible makes the entire split incompatible
                return false;
            }
        }

        // None of the units were incompatible, so that means the splits are compatible
        return true;
    }

    inline bool Split::conflictsWith(const Split & other) const {
        return !isCompatible(other);
    }
    
    inline unsigned Split::countOnBits() const {
        // This assumes that the unused bits in the last unit are all 0.
        unsigned num_bits_set = 0;
        for (auto b : _bits) {
            // The  Kernighan method of counting bits is used below: see exercise 2-9 in the Kernighan and Ritchie book.
            // As an example, consider v = 10100010
            // c = 0:
            //   v     = 10100010
            //   v - 1 = 10100001
            //   ----------------
            //   new v = 10100000
            // c = 1:
            //   v     = 10100000
            //   v - 1 = 10011111
            //   ----------------
            //   new v = 10000000
            // c = 2:
            //   v     = 10000000
            //   v - 1 = 01111111
            //   ----------------
            //   new v = 00000000
            // c = 3:
            //   break out of loop because v = 0
            //
            auto v = b;
            unsigned c = 0;
            for (; v; ++c) {
                v &= v - 1;
            }
            num_bits_set += c;
        }
        return num_bits_set;
    }

    inline unsigned Split::countOffBits() const {
        return _nleaves - countOnBits();
    }

    //   If this split is subsumed in other, then a bitwise AND for any unit should equal the unit from this split. If this
    //   tis not the case, it means there was a 0 in other for a bit that is set in this split. For example:
    //
    //        **-*--**-*-  <-- this split
    //        *****-****-  <-- other split
    //        ----------------------------
    //        **-*--**-*-  <-- bitwise AND
    //
    //    In the above example, this split is subsumed in the other split because the bitwise AND equals this split.
    //    Note that every split is, by definition, subsumed in itself.
    inline bool Split::subsumedIn(const Split & other) const {
        for (unsigned i = 0; i < _bits.size(); ++i) {
            if ((_bits[i] & other._bits[i]) != _bits[i])
                return false;
            }
        return true;
    }
    
    inline void Split::setInfo(double info) {
        _info = info;
    }

    inline double Split::getInfo() const {
        return _info;
    }

    inline void Split::setWeight(double w) {
        _weight = w;
    }

    inline double Split::getWeight() const {
        return _weight;
    }

    inline void Split::setCertainty(double ic) {
        _certainty = ic;
    }

    inline double Split::getCertainty() const {
        return (_weight < 1.0 ? _certainty : 1.0);
    }

    inline void Split::setDisparity(double d) {
        _disparity = d;
    }

    inline double Split::getDisparity() const {
        return _disparity;
    }

}
