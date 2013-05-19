/****************************************************************************
  FileName     [ v3Map.h ]
  PackageName  [ v3/src/adt ]
  Synopsis     [ Abstract Data Type for Map. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ADT_MAP_H
#define V3_ADT_MAP_H

// Defines
//#define V3_USE_BOOST_MAP
//#define V3_USE_GOOGLE_MAP

#ifdef V3_USE_BOOST_MAP
#elif V3_USE_GOOGLE_MAP
#else
#include <map>
#endif

template<class Key, class Data, class Compare = std::less<Key>, class Alloc = std::allocator<std::pair<const Key, Data> > >
struct V3Map {
#ifdef V3_USE_BOOST_MAP
#elif V3_USE_GOOGLE_MAP
#else
   typedef std::map<Key, Data, Compare, Alloc> Map;
#endif
};

#endif

