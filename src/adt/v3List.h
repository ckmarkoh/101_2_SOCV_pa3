/****************************************************************************
  FileName     [ v3List.h ]
  PackageName  [ v3/src/adt ]
  Synopsis     [ Abstract Data Type for List. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ADT_LIST_H
#define V3_ADT_LIST_H

// Defines
//#define V3_USE_BOOST_LIST
//#define V3_USE_GOOGLE_LIST

#ifdef V3_USE_BOOST_LIST
#elif V3_USE_GOOGLE_LIST
#else
#include <list>
#endif

template<class T>
struct V3List {
#ifdef V3_USE_BOOST_LIST
#elif V3_USE_GOOGLE_LIST
#else
   typedef std::list<T> List;
#endif
};

#endif

