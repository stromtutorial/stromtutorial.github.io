#pragma once

#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <numeric>
#include <limits>
#include <map>
#include <boost/format.hpp>
#include "genetic_code.hpp"
#include "datatype.hpp"
#include "partition.hpp"
#include "xstrom.hpp"
#include "ncl/nxsmultiformat.h"
#include <boost/algorithm/string/join.hpp>

namespace strom {

    class Data {    ///begin_data
        public:
            typedef std::vector<std::string>            taxon_names_t;
            typedef unsigned long long                  state_t;
            typedef std::vector<state_t>                pattern_vect_t;
            typedef std::vector<state_t>                monomorphic_vect_t;
            typedef std::vector<int>                    partition_key_t;
            typedef std::map<pattern_vect_t,unsigned>   pattern_map_t;
            typedef std::vector<pattern_vect_t>         data_matrix_t;
            typedef std::vector<pattern_map_t>          pattern_map_vect_t;
            typedef std::vector<double>                 pattern_counts_t;
            typedef std::vector<unsigned>               subset_end_t;
            typedef std::vector<unsigned>               npatterns_vect_t;
            typedef std::pair<unsigned, unsigned>       begin_end_pair_t;
            typedef std::shared_ptr<Data>               SharedPtr;

                                                        Data();
                                                        ~Data();
        
            Partition::SharedPtr                        getPartition();
            void                                        setPartition(Partition::SharedPtr partition);

            void                                        getDataFromFile(const std::string filename);

            unsigned                                    getNumSubsets() const;
            std::string                                 getSubsetName(unsigned subset) const;

            unsigned                                    getNumTaxa() const;
            const taxon_names_t &                       getTaxonNames() const;

            unsigned                                    getNumPatterns() const;
            npatterns_vect_t                            calcNumPatternsVect() const;
            unsigned                                    getNumPatternsInSubset(unsigned subset) const;
            unsigned                                    getNumStatesForSubset(unsigned subset) const;
            unsigned                                    calcSeqLen() const;
            unsigned                                    calcSeqLenInSubset(unsigned subset) const;
            const data_matrix_t &                       getDataMatrix() const;
            begin_end_pair_t                            getSubsetBeginEnd(unsigned subset) const;
            const pattern_counts_t &                    getPatternCounts() const;
            const monomorphic_vect_t &                  getMonomorphic() const;
            const partition_key_t &                     getPartitionKey() const;

            std::string                                 createTaxaBlock() const;            ///!a
            std::string                                 createTranslateStatement() const;   ///!b

            void                                        clear();

        private:

            unsigned                                    storeTaxonNames(NxsTaxaBlock * taxaBlock, unsigned taxa_block_index);
            unsigned                                    storeData(unsigned ntax, unsigned nchar, NxsCharactersBlock * charBlock, NxsCharactersBlock::DataTypesEnum datatype);
            unsigned                                    buildSubsetSpecificMaps(unsigned ntaxa, unsigned seqlen, unsigned nsubsets);
            void                                        updatePatternMap(Data::pattern_vect_t & pattern, unsigned subset);
            void                                        compressPatterns();

            Partition::SharedPtr                        _partition;
            pattern_counts_t                            _pattern_counts;
            monomorphic_vect_t                          _monomorphic;
            partition_key_t                             _partition_key;
            pattern_map_vect_t                          _pattern_map_vect;
            taxon_names_t                               _taxon_names;
            data_matrix_t                               _data_matrix;
            subset_end_t                                _subset_end;
    };  ///end_data

    // Member function bodies go below here but above the right curly bracket that ends the namespace block

    inline Data::Data() {
        //std::cout << "Creating a Data object" << std::endl;
        clear();
    }

    inline Data::~Data() {
        //std::cout << "Destroying a Data object" << std::endl;
    }
    
    inline void Data::setPartition(Partition::SharedPtr partition) {
        _partition = partition;
    }

    inline Partition::SharedPtr Data::getPartition() {
        return _partition;
    }

    inline unsigned Data::getNumSubsets() const {
        return (_partition ? _partition->getNumSubsets() : 1);
    }
    
    inline std::string Data::getSubsetName(unsigned subset) const {
        return _partition ? _partition->getSubsetName(subset) : std::string("default");
    }

    inline const Data::partition_key_t & Data::getPartitionKey() const {
        return _partition_key;
    }
    
    inline const Data::pattern_counts_t & Data::getPatternCounts() const {
        return _pattern_counts;
    }
    
    inline const Data::monomorphic_vect_t & Data::getMonomorphic() const {
        return _monomorphic;
    }

    inline const Data::taxon_names_t & Data::getTaxonNames() const {
        return _taxon_names;
    }

    inline const Data::data_matrix_t & Data::getDataMatrix() const {
        return _data_matrix;
    }

    inline Data::begin_end_pair_t Data::getSubsetBeginEnd(unsigned subset) const {
        assert(_subset_end.size() > subset);
        if (subset == 0)
            return std::make_pair(0, _subset_end[0]);
        else
            return std::make_pair(_subset_end[subset-1], _subset_end[subset]);
    }

    inline void Data::clear() {
        _partition_key.clear();
        _pattern_counts.clear();
        _monomorphic.clear();
        _pattern_map_vect.clear();
        _taxon_names.clear();
        _data_matrix.clear();
        _subset_end.clear();
    }

    inline unsigned Data::getNumPatterns() const {
        if (_data_matrix.size() > 0)
            return (unsigned)_data_matrix[0].size();
        else
            return 0;
    }

    inline Data::npatterns_vect_t Data::calcNumPatternsVect() const {
        unsigned nsubsets = (unsigned)_subset_end.size();
        std::vector<unsigned> num_patterns_vect(nsubsets, 0);
        for (unsigned s = 0; s < nsubsets; s++)
            num_patterns_vect[s] = getNumPatternsInSubset(s);
        return num_patterns_vect;
    }
    
    inline unsigned Data::getNumStatesForSubset(unsigned subset) const {
        DataType data_type = _partition->getDataTypeForSubset(subset);
        return data_type.getNumStates();
    }

    inline unsigned Data::getNumPatternsInSubset(unsigned subset) const {
        assert(_subset_end.size() > subset);
        return (unsigned)_subset_end[subset] - (subset == 0 ? 0 : _subset_end[subset-1]);
    }
    
    inline unsigned Data::getNumTaxa() const {
        return (unsigned)_taxon_names.size();
    }

    inline unsigned Data::calcSeqLen() const {
        return std::accumulate(_pattern_counts.begin(), _pattern_counts.end(), 0);
    }

    inline unsigned Data::calcSeqLenInSubset(unsigned subset) const {
        begin_end_pair_t s = getSubsetBeginEnd(subset);
        return std::accumulate(_pattern_counts.begin() + s.first, _pattern_counts.begin() + s.second, 0);
    }
    
    inline unsigned Data::buildSubsetSpecificMaps(unsigned ntaxa, unsigned seqlen, unsigned nsubsets) {
        pattern_vect_t pattern(ntaxa);

        _pattern_map_vect.clear();
        _pattern_map_vect.resize(nsubsets);
        
        const Partition::partition_t & tuples = _partition->getSubsetRangeVect();
        for (auto & t : tuples) {
            unsigned site_begin  = std::get<0>(t);
            unsigned site_end    = std::get<1>(t);
            unsigned site_skip   = std::get<2>(t);
            unsigned site_subset = std::get<3>(t);
            for (unsigned site = site_begin; site <= site_end; site += site_skip) {
                // Copy site into pattern
                for (unsigned taxon = 0; taxon < ntaxa; ++taxon) {
                    pattern[taxon] = _data_matrix[taxon][site-1];
                }
                
                // Add this pattern to _pattern_map_vect element corresponding to subset site_subset
                updatePatternMap(pattern, site_subset);
            }
        }
        
        // Tally total number of patterns across all subsets
        unsigned npatterns = 0;
        for (auto & map : _pattern_map_vect) {
            npatterns += (unsigned)map.size();
        }
        
        return npatterns;
    }

    inline void Data::updatePatternMap(Data::pattern_vect_t & pattern, unsigned subset) {
        // If pattern is not already in pattern_map, insert it and set value to 1.
        // If it does exist, increment its current value.
        // (see item 24, p. 110, in Meyers' Efficient STL for more info on the technique used here)
        pattern_map_t::iterator lowb = _pattern_map_vect[subset].lower_bound(pattern);
        if (lowb != _pattern_map_vect[subset].end() && !(_pattern_map_vect[subset].key_comp()(pattern, lowb->first))) {
            // this pattern has already been seen
            lowb->second += 1;
        }
        else
            {
            // this pattern has not yet been seen
            _pattern_map_vect[subset].insert(lowb, pattern_map_t::value_type(pattern, 1));
        }
    }

    inline void Data::compressPatterns() {
        // Perform sanity checks
        if (_data_matrix.empty())
            throw XStrom("Attempted to compress an empty data matrix");
        
        unsigned ntaxa = (unsigned)_data_matrix.size();
        unsigned seqlen = (unsigned)_data_matrix[0].size();
        
        // Finalize partition
        unsigned nsubsets = getNumSubsets();
        _subset_end.resize(nsubsets);
        _partition->finalize(seqlen);

        // Compact the data, storing it in _pattern_map_vect
        unsigned npatterns = buildSubsetSpecificMaps(ntaxa, seqlen, nsubsets);
        _pattern_counts.assign(npatterns, 0);
        _monomorphic.assign(npatterns, 0);
        _partition_key.assign(npatterns, -1);

        // Rebuild _data_matrix to hold compact data, storing counts in _pattern_counts
        _data_matrix.resize(ntaxa);
        for (auto & row : _data_matrix) {
            row.resize(npatterns);
        }

        unsigned p = 0;
        for (unsigned subset = 0; subset < nsubsets; subset++) {
            for (auto & pc : _pattern_map_vect[subset]) {
                _pattern_counts[p] = pc.second; // record how many sites have pattern p
                _partition_key[p] = subset;     // record the subset to which pattern p belongs
                
                state_t constant_state = pc.first[0];
                unsigned t = 0;
                for (auto sc : pc.first) {
                    assert(sc > 0);
                    constant_state &= sc;
                    _data_matrix[t][p] = sc;
                    ++t;
                }
                // constant_state equals 0 if polymorphic or state code of state present if monomorphic
                _monomorphic[p] = constant_state;
                ++p;
            }
            
            _subset_end[subset] = p;

            // Everything for this subset has been transferred to _data_matrix and _pattern_counts,
            // so we can now free this memory
            _pattern_map_vect[subset].clear();
        }
    }

    inline unsigned Data::storeTaxonNames(NxsTaxaBlock * taxaBlock, unsigned taxa_block_index) {
        unsigned ntax = 0;
        if (taxa_block_index == 0) {
            // First taxa block encountered in the file
            _taxon_names.clear();
            for (auto s : taxaBlock->GetAllLabels())
                _taxon_names.push_back(s);
            ntax = (unsigned)_taxon_names.size();
            _data_matrix.resize(ntax);
        }
        else {
            // Second (or later) taxa block encountered in the file
            // Check to ensure taxa block is identical to the first one
            for (auto s : taxaBlock->GetAllLabels()) {
                if (_taxon_names[ntax++] != s)
                    throw XStrom(boost::format("Taxa block %d in data file is not identical to first taxa block read") % (taxa_block_index+1));
            }
        }
        
        return ntax;
    }
            
    inline unsigned Data::storeData(unsigned ntax, unsigned nchar_before, NxsCharactersBlock * charBlock, NxsCharactersBlock::DataTypesEnum datatype) {
        unsigned seqlen = 0;
        
        // Find the data type for the partition subset containing the first site in this NxsCharactersBlock
        // Assumes that all sites in any given NxsCharactersBlock have the same type (i.e. mixed not allowed)
        assert(_partition);
        unsigned subset_index = _partition->findSubsetForSite(nchar_before + 1); // remember that sites begin at 1, not 0, in partition definitions
        DataType dt = _partition->getDataTypeForSubset(subset_index);

        // Determine number of states and bail out if data type not handled
        // 1 = standard, 2 = dna, 3 = rna, 4 = nucleotide, 5 = protein, 6 = continuous, 7 = codon, 8 = mixed
        NxsCharactersBlock * block = charBlock;
        if (datatype == NxsCharactersBlock::dna || datatype == NxsCharactersBlock::rna || datatype == NxsCharactersBlock::nucleotide) {
            if (dt.isCodon()) {
                // Create a NxsCharactersBlock containing codons rather than nucleotides
                block = NxsCharactersBlock::NewCodonsCharactersBlock(
                    charBlock,
                    true,   // map partial ambiguities to completely missing (note: false is not yet implemented in NCL)
                    true,   // gaps to missing
                    true,   // inactive characters treated as missing
                    NULL,   // if non-NULL, specifies the indices of the positions in the gene
                    NULL);  // if non-NULL, specifies a pointer to a NxsCharactersBlock that contains all non-coding positions in gene
            }
            else {
                if (!dt.isNucleotide())
                    throw XStrom(boost::format("Partition subset has data type \"%s\" but data read from file has data type \"nucleotide\"") % dt.getDataTypeAsString());
            }
        }
        else if (datatype == NxsCharactersBlock::protein) {
            if (!dt.isProtein())
                throw XStrom(boost::format("Partition subset has data type \"%s\" but data read from file has data type \"protein\"") % dt.getDataTypeAsString());
        }
        else if (datatype == NxsCharactersBlock::standard) {
            if (!dt.isStandard())
                throw XStrom(boost::format("Partition subset has data type \"%s\" but data read from file has data type \"standard\"") % dt.getDataTypeAsString());
            assert(charBlock->GetSymbols());
            std::string symbols = std::string(charBlock->GetSymbols());
            dt.setStandardNumStates((unsigned)symbols.size());
        }
        else {
            // ignore block because data type is not one that is supported
            return nchar_before;
        }
        
        unsigned num_states = dt.getNumStates();
        
        // Make sure all states can be accommodated in a variable of type state_t
        unsigned bits_in_state_t = 8*sizeof(state_t);
        if (num_states > bits_in_state_t)
            throw XStrom(boost::format("This program can only process data types with fewer than %d states") % bits_in_state_t);
        
        // Copy data matrix from NxsCharactersBlock object to _data_matrix
        // Loop through all taxa, processing one row from block for each taxon
        for (unsigned t = 0; t < ntax; ++t) {

            const NxsDiscreteStateRow & row = block->GetDiscreteMatrixRow(t);
            if (seqlen == 0)
                seqlen = (unsigned)row.size();
            _data_matrix[t].resize(nchar_before + seqlen);
            
            // Loop through all sites/characters in row corresponding to taxon t
            unsigned k = nchar_before;
            for (int raw_state_code : row) {
                // For codon model, raw_state_code ranges from 0-63, but deletion of stop codons means fewer state codes
                state_t state = std::numeric_limits<state_t>::max(); // complete ambiguity, all bits set
                bool complete_ambiguity = (!dt.isCodon() && raw_state_code == (int)num_states);
                bool all_missing_or_gaps = (raw_state_code < 0);
                if ((!complete_ambiguity) && (!all_missing_or_gaps)) {
                    int state_code = raw_state_code;
                    if (dt.isCodon())
                        state_code = dt.getGeneticCode()->getStateCode(raw_state_code);

                    if (state_code < (int)num_states) {
                        state = (state_t)1 << state_code;
                    }
                    else {
                        // incomplete ambiguity (NCL state code > num_states)
                        const NxsDiscreteDatatypeMapper      * mapper = block->GetDatatypeMapperForChar(k - nchar_before);
                        const std::set<NxsDiscreteStateCell> & state_set = mapper->GetStateSetForCode(raw_state_code);
                        state = 0;
                        for (auto s : state_set) {
                             state |= (state_t)1 << s;
                        }
                    }
                }
                _data_matrix[t][k++] = state;
            }
        }
        
        return seqlen;
    }

    inline void Data::getDataFromFile(const std::string filename) {
        // See http://phylo.bio.ku.edu/ncldocs/v2.1/funcdocs/index.html for documentation
        //
        // -1 means "process all blocks found" (this is a bit field and -1 fills the bit field with 1s)
        // Here are the bits (and nexus blocks) that are defined:
        //     enum NexusBlocksToRead
        //     {
        //         NEXUS_TAXA_BLOCK_BIT = 0x01,
        //         NEXUS_TREES_BLOCK_BIT = 0x02,
        //         NEXUS_CHARACTERS_BLOCK_BIT = 0x04,
        //         NEXUS_ASSUMPTIONS_BLOCK_BIT = 0x08,
        //         NEXUS_SETS_BLOCK_BIT = 0x10,
        //         NEXUS_UNALIGNED_BLOCK_BIT = 0x20,
        //         NEXUS_DISTANCES_BLOCK_BIT = 0x40,
        //         NEXUS_UNKNOWN_BLOCK_BIT = 0x80
        //     };
        MultiFormatReader nexusReader(-1, NxsReader::WARNINGS_TO_STDERR);
        try {
            nexusReader.ReadFilepath(filename.c_str(), MultiFormatReader::NEXUS_FORMAT);
        }
        catch(...) {
            nexusReader.DeleteBlocksFromFactories();
            throw;
        }

        // Commit to storing new data
        clear();

        // Ensure that Data::setPartition was called before reading data
        assert(_partition);

        int numTaxaBlocks = nexusReader.GetNumTaxaBlocks();
        if (numTaxaBlocks == 0)
            throw XStrom("No taxa blocks were found in the data file");
            
        unsigned cum_nchar = 0;
        for (int i = 0; i < numTaxaBlocks; ++i) {
            NxsTaxaBlock * taxaBlock = nexusReader.GetTaxaBlock(i);
            unsigned ntax = storeTaxonNames(taxaBlock, i);
            const unsigned numCharBlocks = nexusReader.GetNumCharactersBlocks(taxaBlock);
            for (unsigned j = 0; j < numCharBlocks; ++j) {
                NxsCharactersBlock * charBlock = nexusReader.GetCharactersBlock(taxaBlock, j);
                NxsCharactersBlock::DataTypesEnum datatype = charBlock->GetOriginalDataType();
                cum_nchar += storeData(ntax, cum_nchar, charBlock, datatype);
            }
        }

        // No longer any need to store raw data from nexus file
        nexusReader.DeleteBlocksFromFactories();

        // Compress _data_matrix so that it holds only unique patterns (counts stored in _pattern_counts)
        if (_data_matrix.empty()) {
            std::cout << "No data were stored from the file \"" << filename << "\"" << std::endl;
            clear();
        }
        else {
            compressPatterns();
        }
    }
    
    inline std::string Data::createTaxaBlock() const {    ///begin_createTaxaBlock
        std::string s = "";
        s += "begin taxa;\n";
        s += boost::str(boost::format("  dimensions ntax=%d;\n") % _taxon_names.size());
        s += "  taxlabels\n";
        for (auto nm : _taxon_names) {
            std::string taxon_name = std::regex_replace(nm, std::regex(" "), "_");
            s += "    " + taxon_name + "\n";
            }
        s += "    ;\n";
        s += "end;\n";
        return s;
    } 
    
    inline std::string Data::createTranslateStatement() const {
        std::string s = "";
        s += "  translate\n";
        unsigned t = 1;
        for (auto nm : _taxon_names) {
            std::string taxon_name = std::regex_replace(nm, std::regex(" "), "_");
            s += boost::str(boost::format("    %d %s%s\n") % t % taxon_name % (t < _taxon_names.size() ? "," : ""));
            t++;
            }
        s += "  ;\n";
        return s;
    }    ///end_createTranslateStatement
    
}
