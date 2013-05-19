/****************************************************************************
  FileName     [ v3VrfResult.h ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Verification Result Validation Checker. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_RESULT_H
#define V3_VRF_RESULT_H

#include "v3NtkHandler.h"

// Counterexample Checker
const int simulationCheckFiredResult(const V3CexTrace* const, const V3NtkHandler* const);

// Simulation-Based Checker
const int cycleSimulateResult(const V3CexTrace* const, const V3NtkHandler* const);

// Counterexample Checker Report Functions
void reportUnexpectedState(const uint32_t&, const uint32_t&, const V3BitVecX&, const V3BitVecX&);
void reportShorterTrace(const uint32_t&, const char&);

#endif

