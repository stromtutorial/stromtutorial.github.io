#pragma once

#include <cmath>
#include <string>
#include <set>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "xstrom.hpp"

namespace strom {

    // From http://stackoverflow.com/questions/6097927/is-there-a-way-to-implement-analog-of-pythons-separator-join-in-c
    template <typename Iter>
    std::string join(Iter curr, Iter stop, std::string const & separator) {
        std::ostringstream result;
        if (curr != stop)
            result << *curr++;
        while (curr != stop)
            result << separator << *curr++;
        return result.str();
    }

    inline double logfactorial(unsigned n) {
        // Returns log(n!)
        if (n == 0 || n == 1)
            return 0.0;
        return lgamma(n+1.0);
    }

    inline double choose(unsigned n, unsigned y) {
        // Returns n choose y
        double tmp = logfactorial(n) - logfactorial(y) - logfactorial(n-y);
        if (n-y == y)
            tmp -= log(2.0);
        return exp(tmp);
    }

    inline double lognrooted(unsigned y) {
        // Returns number of rooted trees of y taxa
        double fy = (double)y;
        double log_num_rooted_trees = 0.0;
        if (y > 2)
            log_num_rooted_trees = logfactorial(2*y - 3) - (fy - 2.0)*log(2.0) - logfactorial(y - 2);
        return log_num_rooted_trees;
    }

    // See item 24, p. 110, in Meyers, Scott. 2001. Effective STL: 50 specific ways to improve your
    // use of the Standard Template Library. Addison-Wesley, Boston.
    template< typename MapType, typename KeyArgType, typename ValueArgType >
    typename MapType::iterator efficientAddOrCheck(MapType & m, const KeyArgType & k, const ValueArgType & v) {
        // Find where k is, or should be
        typename MapType::iterator lb = m.lower_bound(k);

        // If lb points to a pair whose key is equivalent to k,
        if (lb != m.end() && !(m.key_comp()(k, lb->first))) {
            // check the pair's value and return m.end() if v is not equal to currently-store value or lb if they are the same
            if (lb->second == v)
                return lb;
            else
                return m.end();
        }
        else {
            // else add pair(k,v) to m and return an iterator to the new map element.
            typedef typename MapType::value_type MVT;
            return m.insert(lb, MVT(k, v));
        }
    }

    // This function provides an efficient, general way to increment the value m[k] where map values are num_subsets-length vectors of doubles.
    template< typename MapType, typename KeyArgType >
    typename MapType::iterator efficientIncrementSubset(MapType & m, const KeyArgType & k, unsigned subset_index, unsigned num_subsets) {
        // Modified version of item 24, p. 110, in Meyers, Scott. 2001. Effective STL: 50 specific ways to improve
        // your use of the Standard Template Library. Addison-Wesley, Boston.

        // Find where k is, or should be
        typename MapType::iterator lb = m.lower_bound(k);

        // If lb points to a pair whose key is equivalent to k,
        if (lb != m.end() && !(m.key_comp()(k, lb->first))) {
            // update the pair's value and return an iterator to that pair,
            lb->second[subset_index] += 1.0;
            return lb;
        }
        else {
            // else add pair(k,v) to m and return an iterator to the new map element.
            typedef typename MapType::value_type MVT;
            std::vector<double> v(num_subsets, 0.0);
            v[subset_index] = 1.0;
            return m.insert(lb, MVT(k, v));
        }
    }

    // This function provides an efficient, general way to increment the value m[k] where map values are unsigned integers.
    template< typename MapType, typename KeyArgType >
    typename MapType::iterator efficientAddTo(MapType & m, const KeyArgType & k, unsigned amount_to_add) {
        // Modified version of item 24, p. 110, in Meyers, Scott. 2001. Effective STL: 50 specific ways to improve
        // your use of the Standard Template Library. Addison-Wesley, Boston.

        // Find where k is, or should be
        typename MapType::iterator lb = m.lower_bound(k);

        // If lb points to a pair whose key is equivalent to k,
        if (lb != m.end() && !(m.key_comp()(k, lb->first))) {
            // update the pair's value and return an iterator to that pair,
            lb->second += amount_to_add;
            return lb;
        }
        else {
            // else add pair(k,v) to m and return an iterator to the new map element.
            typedef typename MapType::value_type MVT;
            return m.insert(lb, MVT(k, amount_to_add));
        }
    }

    inline boost::posix_time::ptime getCurrentTime() {
        return boost::posix_time::microsec_clock::local_time();
    }

    inline double secondsElapsed(boost::posix_time::ptime a, boost::posix_time::ptime b) {
        boost::posix_time::time_period tp(a, b);
        boost::posix_time::time_duration td = tp.length();
        return 0.001*td.total_milliseconds();
    }

    inline void getFileContents(std::string & file_contents, std::string filename) {
        // Read contents of treefname into string
        std::ifstream f(filename.c_str());

        if (f.good()) {
            f.seekg(0, std::ios::end);
            //long unsigned nbytes = f.tellg();
            std::streamoff nbytes = f.tellg();
            if (nbytes == 0)
                throw XStrom(boost::str(boost::format("File specified (%s) is empty") % filename));
            f.seekg(0, std::ios::beg);

            file_contents.insert( file_contents.begin(), std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>() );
        }
        else {
            throw XStrom(boost::str(boost::format("Could not read file %s") % filename));
        }
    }

    inline void extractAllWhitespaceDelimitedStrings(std::vector<std::string> & receiving_vector, const std::string & text_to_process) {
        receiving_vector.clear();

        // Use regular expression to divide text_to_process into whitespace-delimited strings, which are stored in receiving_vector
        std::regex re("\\s+");
        std::sregex_token_iterator i(text_to_process.begin(), text_to_process.end(), re, -1);
        std::sregex_token_iterator j;

        while(i != j) {
            receiving_vector.push_back(*i++);
        }
    }

    inline std::string stripComments(const std::string & commented_nexus_text) {
        std::regex re("\\[.+?\\]");
        return std::regex_replace(commented_nexus_text, re, std::string());
    }

    inline std::string stripTreeName(const std::string & name_equals_tree) {
        std::string tree_description;
        std::smatch what;
        std::regex pattern("^.+?=\\s*(.+)");
        bool regex_ok = std::regex_search(name_equals_tree, what, pattern);
        if (regex_ok) {
            // what[0] contains the whole string
            // what[1] contains only what comes after the first =
            // Construct a string using characters in contents from what[1].first to what[1].second
            tree_description.insert(tree_description.begin(), what[1].first, what[1].second);
        }
        else {
            throw XStrom("regex failed to find = separating tree name from tree description in tree definition");
        }
        return tree_description;
    }

    inline std::pair<bool, unsigned> convertStringToUnsignedInteger(const std::string & theString) {
        // attempt to convert theString to an unsigned integer
        std::string s = theString;
        bool worked = true;
        boost::trim(s);
        unsigned x = 0;
        try {
            x = boost::lexical_cast<unsigned>(s);
        }
        catch(boost::bad_lexical_cast &) {
            // node name could not be converted to an integer value
            worked = false;
        }
        return std::pair<bool,unsigned>(worked, x);
    }

    inline bool numberAlreadyUsed(unsigned x, std::set<unsigned> & used) {
        std::pair<std::set<unsigned>::iterator, bool> insert_result = used.insert(x);
        if (insert_result.second) {
            // insertion was made, so x has NOT already been used
            return false;
        }
        else {
            // insertion was not made, so set already contained x
            return true;
        }
    }

}
