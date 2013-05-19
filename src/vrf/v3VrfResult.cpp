/****************************************************************************
  FileName     [ v3VrfResult.cpp ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Verification Result Validation Checker. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_RESULT_C
#define V3_VRF_RESULT_C

#include "v3Msg.h"
#include "v3VrfUMC.h"
#include "v3VrfResult.h"
#include "v3AlgSimulate.h"

#include <iomanip>

// Counterexample Checker
const int simulationCheckFiredResult(const V3CexTrace* const cex, const V3NtkHandler* const handler) {
   return cycleSimulateResult(cex, handler);
}

// Simulation-Based Checker
const int cycleSimulateResult(const V3CexTrace* const cex, const V3NtkHandler* const handler) {
   // Check Counterexample Directly on Input Ntk
   assert (cex); assert (cex->getTraceSize()); assert (handler);
   V3Ntk* const ntk = handler->getNtk(); assert (ntk); assert (1 == ntk->getOutputSize());
   // Create Simulator
   V3AlgSimulate* simulator = 0;
   if (dynamic_cast<V3BvNtk*>(ntk)) simulator = new V3AlgBvSimulate(handler);
   else simulator = new V3AlgAigSimulate(handler); assert (simulator);
   // For each time-frame, set pattern from counter-example
   V3BitVecX pattern, value;
   for (uint32_t i = 0, k; i < cex->getTraceSize(); ++i) {
      if (cex->getTraceDataSize()) pattern = cex->getData(i); k = 0;
      // Update FF Next State Values
      simulator->updateNextStateValue();
      // Set PI Values
      for (uint32_t j = 0; j < ntk->getInputSize(); ++j) {
         simulator->setSource(ntk->getInput(j), pattern.bv_slice(k + ntk->getNetWidth(ntk->getInput(j)) - 1, k));
         k += ntk->getNetWidth(ntk->getInput(j)); assert (k <= pattern.size());
      }
      for (uint32_t j = 0; j < ntk->getInoutSize(); ++j) {
         simulator->setSource(ntk->getInout(j), pattern.bv_slice(k + ntk->getNetWidth(ntk->getInout(j)) - 1, k));
         k += ntk->getNetWidth(ntk->getInout(j)); assert (k <= pattern.size());
      }
      // Simulate Ntk for a Cycle
      simulator->simulate();
      // Check Property Assertion
      value = simulator->getSimValue(ntk->getOutput(0));
      if ((i < (cex->getTraceSize() - 1)) && ('1' == value[0])) reportShorterTrace(i, value[0]);
   }
   return ('1' == value[0]) ? 1 : -1;
}

// Counterexample Checker Report Functions
void reportUnexpectedState(const uint32_t& cycle, const uint32_t& index, const V3BitVecX& v1, const V3BitVecX& v2) {
   Msg(MSG_IFO) << "Cycle = " << cycle << ", DFF(" << index << ") has unexpected trace value = " << v1
                << ", expected simulation value = " << v2 << endl;
}

void reportShorterTrace(const uint32_t& cycle, const char& value) {
   Msg(MSG_IFO) << "A shorter trace found @ Cycle = " << cycle << ", assertion = " << value << endl;
}

#endif

