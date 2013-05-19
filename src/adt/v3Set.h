/****************************************************************************
  FileName     [ v3Set.h ]
  PackageName  [ v3/src/adt ]
  Synopsis     [ Abstract Data Type for Set. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ADT_SET_H
#define V3_ADT_SET_H

// Defines
//#define V3_USE_BOOST_SET
//#define V3_USE_GOOGLE_SET

#ifdef V3_USE_BOOST_SET
#elif V3_USE_GOOGLE_SET
#else
#include <set>
#endif

template<class Key, class Compare = std::less<Key>, class Alloc = std::allocator<Key> >
struct V3Set {
#ifdef V3_USE_BOOST_SET
#elif V3_USE_GOOGLE_SET
#else
   typedef std::set<Key, Compare, Alloc> Set;
#endif
};

#endif

