/****************************************************************************
  FileName     [ v3SvrType.h ]
  PackageName  [ v3/src/svr ]
  Synopsis     [ Types in V3 Solver. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_SVR_TYPE_H
#define V3_SVR_TYPE_H

#include "v3Misc.h"

#include "Solver.h"
extern "C" {
#include "boolector.h"
}

// Defines for Common Representation
typedef size_t                   V3SvrData;
typedef V3Vec<V3SvrData>::Vec    V3SvrDataVec;
typedef V3Set<V3SvrData>::Set    V3SvrDataSet;

// Defines for MiniSAT
typedef V3Vec<Var>::Vec          V3SvrMVarData;
typedef V3Vec<Lit>::Vec          V3SvrMLitData;

// Defines for Boolector
typedef struct BtorNode          BtorExp;
typedef V3Vec<BtorExp*>::Vec     V3BtorExpVec;

#endif

