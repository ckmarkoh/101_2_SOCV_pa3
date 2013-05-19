/****************************************************************************
  FileName     [ v3VrfSIM.cpp ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Constrained Random Simulation on V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_SIM_C
#define V3_VRF_SIM_C

#include "v3Msg.h"
#include "v3VrfSIM.h"
#include "v3AlgSimulate.h"

#include <cmath>
#include <ctime>
#include <iomanip>

/* -------------------------------------------------- *\
 * Class V3VrfUMC Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3VrfSIM::V3VrfSIM(const V3NtkHandler* const handler) : V3VrfBase(handler) {
   _maxTime = 10;
}

V3VrfSIM::~V3VrfSIM() {
}

// Verification Main Functions
void
V3VrfSIM::startVerify(const uint32_t& pIndex) {
   // Initialize
   V3Ntk* const ntk = _handler->getNtk(); assert (ntk);
   V3AlgSimulate* simulator = 0;
   if (dynamic_cast<V3BvNtk*>(ntk)) simulator = new V3AlgBvSimulate(_handler);
   else simulator = new V3AlgAigSimulate(_handler); assert (simulator);
   const V3NetId& pId = ntk->getOutput(pIndex); assert (V3NetUD != pId);
   const uint32_t logMaxWidth = (uint32_t)(ceil(log10(_maxDepth)));
   const string flushSpace = string(100, ' ');
   uint32_t cycle = V3NtkUD, cycleReached = 0;
   double runtime = clock(), timeBound = clock() + (CLOCKS_PER_SEC * _maxTime);
   
   // Start SIM Based Verification
   const uint32_t Cycle = 10000;
   uint32_t i = 0, j = 0;
   for (uint32_t k = 0, n = (uint32_t)(ceil(_maxDepth / Cycle)); k < n; ++k) {
      j = Cycle + i; if (j > _maxDepth) j = _maxDepth;
      for (i = Cycle * k; i < j; ++i) {
         // Update FF Next State Values
         simulator->updateNextStateValue();
         // Purely Random Simulation
         simulator->setSourceFree(V3_PI, true);
         simulator->setSourceFree(V3_PIO, true);
         // Simulate for One Cycle
         simulator->simulate();
         // Record Simulation Data
         simulator->recordSimValue();
         // Check if Property Asserted
         if (simulator->getSimValue(pId).exist1()) { cycle = i; break; }
      }
      if (V3NtkUD != cycle) break;
      // Report Verification Progress
      if (intactON()) {
         if (!endLineON()) Msg(MSG_IFO) << "\r" + flushSpace + "\r";
         Msg(MSG_IFO) << "Simulation completed under cycle = " << setw(logMaxWidth) << i;
         if (endLineON()) Msg(MSG_IFO) << endl; else Msg(MSG_IFO) << flush;
         // Check if Time Bound Reached
         if (clock() >= timeBound) { cycleReached = i; break; }
      }
   }

   // Report Verification Result
   if (reportON()) {
      runtime = (clock()- runtime) / CLOCKS_PER_SEC;
      if (intactON()) {
         if (endLineON()) Msg(MSG_IFO) << endl;
         else Msg(MSG_IFO) << "\r" << flushSpace << "\r";
      }
      if (V3NtkUD != cycle) Msg(MSG_IFO) << "Counter-example found at cycle = " << ++cycle;
      else Msg(MSG_IFO) << "UNDECIDED at cycle = " << ((clock() >= timeBound) ? cycleReached : _maxDepth);
      if (usageON()) Msg(MSG_IFO) << "  (time = " << setprecision(5) << runtime << "  sec)" << endl;
      if (profileON()) { /* Report some profiling here ... */ }
   }

   // Record CounterExample Trace
   if (V3NtkUD == cycle) return;
   // Compute Pattern Size and Initialize Trace
   V3CexTrace* const cex = new V3CexTrace(cycle); assert (cex);
   _result[pIndex].setCexTrace(cex); assert (_result[pIndex].isCex());
   uint32_t patternSize = 0;
   for (uint32_t i = 0; i < ntk->getInputSize(); ++i) patternSize += ntk->getNetWidth(ntk->getInput(i));
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) patternSize += ntk->getNetWidth(ntk->getInout(i));
   if (!patternSize) return;
   // Record Counter-Example
   V3SimTrace pattern(ntk->getInputSize()); V3BitVecX trace(patternSize);
   if (dynamic_cast<V3BvNtk*>(ntk)) {
      for (uint32_t i = 0; i < cycle; ++i) {
         simulator->getSimRecordData(i, pattern);
         uint32_t k = 0;
         for (uint32_t j = 0; j < ntk->getInputSize(); ++j) {
            assert (pattern[j].size() == ntk->getNetWidth(ntk->getInput(j)));
            for (uint32_t x = 0; x < pattern[j].size(); ++x, ++k)
               if ('1' == pattern[j][x]) trace.set1(k);
               else if ('0' == pattern[j][x]) trace.set0(k);
         }
         for (uint32_t j = 0; j < ntk->getInoutSize(); ++j) {
            assert (pattern[j].size() == ntk->getNetWidth(ntk->getInout(j)));
            for (uint32_t x = 0; x < pattern[j].size(); ++x, ++k)
               if ('1' == pattern[j][x]) trace.set1(k);
               else if ('0' == pattern[j][x]) trace.set0(k);
         }
         cex->pushData(trace); trace.clear();
      }
   }
   else {
      const uint32_t p = simulator->getSimValue(pId).first1(); assert (p < simulator->getSimValue(pId).size());
      assert (pattern.size() == trace.size());
      for (uint32_t i = 0; i < cycle; ++i) {
         simulator->getSimRecordData(i, pattern);
         for (uint32_t j = 0; j < pattern.size(); ++j)
            if ('1' == pattern[j][p]) trace.set1(j);
            else if ('0' == pattern[j][p]) trace.set0(j);
         cex->pushData(trace); trace.clear();
      }
   }
}

// SIM Specific Functions
void
V3VrfSIM::printSettings() const {
   Msg(MSG_IFO) << "Current Settings: Time Bound = " << _maxTime << ", Cycle Bound = " << _maxDepth << endl;
}

#endif

