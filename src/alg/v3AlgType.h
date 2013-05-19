/****************************************************************************
  FileName     [ v3AlgType.h ]
  PackageName  [ v3/src/alg ]
  Synopsis     [ Types for V3 Algorithms. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ALG_TYPE_H
#define V3_ALG_TYPE_H

#include "v3Ntk.h"

// Defines for Simulation Data
typedef V3Vec<V3BitVecX>::Vec             V3SimTrace;
typedef V3Vec<V3BitVecX>::Vec             V3BvSimDataVec;
typedef V3Vec<V3BitVecS>::Vec             V3AigSimDataVec;
typedef V3Vec<V3SimTrace>::Vec            V3SimTraceVec;
typedef V3Vec<V3BvSimDataVec>::Vec        V3BvSimTraceVec;
typedef V3Vec<V3AigSimDataVec>::Vec       V3AigSimTraceVec;

#endif

