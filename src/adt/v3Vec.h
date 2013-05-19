/****************************************************************************
  FileName     [ v3Vec.h ]
  PackageName  [ v3/src/adt ]
  Synopsis     [ Abstract Data Type for Vec. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ADT_VEC_H
#define V3_ADT_VEC_H

// Defines
//#define V3_USE_BOOST_VEC
//#define V3_USE_GOOGLE_VEC

#ifdef V3_USE_BOOST_VEC
#elif V3_USE_GOOGLE_VEC
#else
#include <vector>
#endif

template<class T>
struct V3Vec {
#ifdef V3_USE_BOOST_VEC
#elif V3_USE_GOOGLE_VEC
#else
   typedef std::vector<T> Vec;
#endif
};

#endif

