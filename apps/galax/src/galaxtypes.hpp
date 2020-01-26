#pragma once

#include <vector>
#include <map>
#include <set>
#include "split.hpp"

namespace strom {

typedef std::set< Split >                        split_set_t;
typedef std::vector< Split >                     split_vect_t;
typedef std::vector< double >                    count_vect_t;
typedef std::vector< unsigned >                  tree_counts_vect_t;
typedef std::vector< std::string >               name_vect_t;
typedef std::map< split_vect_t, count_vect_t >   ccd_map_t;
typedef std::vector< double >                    time_vect_t;
typedef std::vector< double >                    weight_vect_t;
typedef std::map< std::string, double >          clade_info_map_t;
typedef std::vector< split_vect_t >              tree_id_t;
typedef std::set< tree_id_t >                    tree_id_set_t;
typedef std::vector< tree_id_set_t >             subset_tree_set_t;
typedef std::map< tree_id_t, unsigned >          tree_map_t;
typedef std::vector< tree_map_t >                subset_tree_map_t;

}
