/****************************************************************************
  FileName     [ v3Queue.h ]
  PackageName  [ v3/src/adt ]
  Synopsis     [ Abstract Data Type for Queue. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ADT_QUEUE_H
#define V3_ADT_QUEUE_H

// Defines
//#define V3_USE_BOOST_QUEUE
//#define V3_USE_GOOGLE_QUEUE

#ifdef V3_USE_BOOST_QUEUE
#elif V3_USE_GOOGLE_QUEUE
#else
#include <queue>
#endif

template<class T>
struct V3Queue {
#ifdef V3_USE_BOOST_QUEUE
#elif V3_USE_GOOGLE_QUEUE
#else
   typedef std::queue<T> Queue;
#endif
};

#endif

