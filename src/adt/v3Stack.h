/****************************************************************************
  FileName     [ v3Stack.h ]
  PackageName  [ v3/src/adt ]
  Synopsis     [ Abstract Data Type for Stack. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ADT_STACK_H
#define V3_ADT_STACK_H

// Defines
//#define V3_USE_BOOST_STACK
//#define V3_USE_GOOGLE_STACK

#ifdef V3_USE_BOOST_STACK
#elif V3_USE_GOOGLE_STACK
#else
#include <stack>
#endif

template<class T>
struct V3Stack {
#ifdef V3_USE_BOOST_STACK
#elif V3_USE_GOOGLE_STACK
#else
   typedef std::stack<T> Stack;
#endif
};

#endif

