/****************************************************************************
  FileName     [ v3VrfUMC.cpp ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Unbounded Model Checking on V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_UMC_C
#define V3_VRF_UMC_C

#include "v3Msg.h"
#include "v3VrfUMC.h"

#include <cmath>
#include <ctime>
#include <iomanip>

/* -------------------------------------------------- *\
 * Class V3VrfUMC Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3VrfUMC::V3VrfUMC(const V3NtkHandler* const handler) : V3VrfBase(handler) {
   _preDepth = 0; _incDepth = 1; _umcAttr = 0; _solver = 0;
}

V3VrfUMC::~V3VrfUMC() {
   if (_solver) delete _solver; _solver = 0;
}

// Private Verification Main Functions
void
V3VrfUMC::startVerify(const uint32_t& pIndex) {
   // Consistency Check
   if (_preDepth > _maxDepth) _maxDepth = _preDepth; assert (_maxDepth >= _preDepth);
   if (_incDepth && ((_maxDepth - _preDepth) % _incDepth)) _maxDepth -= (_maxDepth - _preDepth) % _incDepth;
   assert (!_incDepth || !((_maxDepth - _preDepth) % _incDepth)); assert (_maxDepth >= _preDepth);
   if ((_maxDepth > _preDepth) && !_incDepth) _maxDepth = _preDepth;

   // Initialize
   V3Ntk* const ntk = _handler->getNtk(); assert (ntk);
   if (_solver) delete _solver; _solver = allocSolver(getSolver(), ntk);
   assert (_solver); assert (_solver->totalSolves() == 0);
   assert (pIndex < _result.size()); assert (pIndex < ntk->getOutputSize());
   const V3NetId& pId = ntk->getOutput(pIndex); assert (V3NetUD != pId);
   const uint32_t logMaxWidth = (uint32_t)(ceil(log10(_maxDepth)));
   const string flushSpace = string(100, ' ');
   uint32_t proved = V3NtkUD, fired = V3NtkUD;
   double runtime = clock();
   uint32_t boundDepth = _preDepth ? _preDepth : _incDepth;
   
   // Solver Data
   V3SvrData pFormulaData; V3PtrVec pFormula;
   pFormula.reserve((_preDepth > _incDepth) ? _preDepth : _incDepth);

   // Start UMC Based Verification
   for (uint32_t i = 0, j = _maxDepth; i < j; ++i) {
      // Add time frame expanded circuit to SAT Solver
      _solver->addBoundedVerifyData(pId, i); pFormula.push_back(_solver->getFormula(pId, i));
      // Check if the bound is achieved
      if ((1 + i) < boundDepth) continue; assert ((1 + i) == boundDepth);
      assert ((1 + i) >= pFormula.size()); boundDepth += _incDepth;
      // Add assume for assumption solve only
      _solver->assumeRelease();
      if (1 == pFormula.size()) _solver->assumeProperty(pId, false, i);
      else {
         pFormulaData = _solver->setImplyUnion(pFormula);
         assert (pFormulaData); _solver->assumeProperty(pFormulaData);
      }
      _solver->simplify();
      // Report Verification Progress
      if (intactON()) {
         if (!endLineON()) Msg(MSG_IFO) << "\r" + flushSpace + "\r";
         Msg(MSG_IFO) << "Verification completed under depth = " << setw(logMaxWidth) << (i + 1);
         if (svrInfoON()) { Msg(MSG_IFO) << "  ("; _solver->printInfo(); Msg(MSG_IFO) << ")"; }
         if (endLineON()) Msg(MSG_IFO) << endl; else Msg(MSG_IFO) << flush;
      }
      // Assumption Solve : If UNSAT, proved!
      if (!isFireOnly() && !_solver->assump_solve()) {
         if (!isProveOnly()) {
            proved = i; break;
         }
      }
      // Assumption Solve : If SAT, disproved!
      if (!isProveOnly()) {
         _solver->assumeInit(); // Conjunction with initial condition
         if (_solver->assump_solve()) {
            for (uint32_t k = 0; k < pFormula.size(); ++k)
               if ('0' != _solver->getDataValue(pFormula[k])) {
                  fired = (1 + i + k - pFormula.size()); break;
               }
            assert (V3NtkUD != fired); break;
         }
      }
      // Add assert back to the property
      if (1 < pFormula.size()) { assert (pFormulaData); _solver->assertProperty(pFormulaData, true); }
      for (uint32_t k = i - pFormula.size(); k < i; ++k) _solver->assertProperty(pId, true, k);
      pFormula.clear(); pFormulaData = 0;
   }

   // Report Verification Result
   if (reportON()) {
      runtime = (clock() - runtime) / CLOCKS_PER_SEC;
      if (intactON()) {
         if (endLineON()) Msg(MSG_IFO) << endl;
         else Msg(MSG_IFO) << "\r" << flushSpace << "\r";
      }
      if (V3NtkUD != proved) Msg(MSG_IFO) << "Inductive Invariant found at depth = " << ++proved;
      else if (V3NtkUD != fired) Msg(MSG_IFO) << "Counter-example found at depth = " << ++fired;
      else Msg(MSG_IFO) << "UNDECIDED at depth = " << _maxDepth;
      if (usageON()) Msg(MSG_IFO) << "  (time = " << setprecision(5) << runtime << "  sec)" << endl;
      if (profileON()) { /* Report some profiling here ... */ }
   }
   else {
      if (V3NtkUD != proved) ++proved;
      else if (V3NtkUD != fired) ++fired;
   }

   // Record CounterExample Trace or Invariant
   if (V3NtkUD != fired) {  // Record Counter-Example
      V3CexTrace* const cex = new V3CexTrace(fired); assert (cex);
      _result[pIndex].setCexTrace(cex); assert (_result[pIndex].isCex());
      // Compute Pattern Size  (PI + PIO)
      uint32_t patternSize = 0;
      for (uint32_t i = 0; i < ntk->getInputSize(); ++i) patternSize += ntk->getNetWidth(ntk->getInput(i));
      for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) patternSize += ntk->getNetWidth(ntk->getInout(i));
      // Set Pattern Value  (PI + PIO)
      V3BitVecX dataValue, patternValue(patternSize ? patternSize : 1);
      for (uint32_t i = 0; i < fired; ++i) {
         patternSize = 0; patternValue.clear();
         for (uint32_t j = 0; j < ntk->getInputSize(); ++j) {
            if (_solver->existVerifyData(ntk->getInput(j), i)) {
               dataValue = _solver->getDataValue(ntk->getInput(j), i);
               assert (dataValue.size() == ntk->getNetWidth(ntk->getInput(j)));
               for (uint32_t k = 0; k < dataValue.size(); ++k, ++patternSize) {
                  if ('0' == dataValue[k]) patternValue.set0(patternSize);
                  else if ('1' == dataValue[k]) patternValue.set1(patternSize);
               }
            }
            else patternSize += ntk->getNetWidth(ntk->getInput(j));
         }
         for (uint32_t j = 0; j < ntk->getInoutSize(); ++j) {
            if (_solver->existVerifyData(ntk->getInout(j), i)) {
               dataValue = _solver->getDataValue(ntk->getInout(j), i);
               assert (dataValue.size() == ntk->getNetWidth(ntk->getInout(j)));
               for (uint32_t k = 0; k < dataValue.size(); ++k, ++patternSize) {
                  if ('0' == dataValue[k]) patternValue.set0(patternSize);
                  else if ('1' == dataValue[k]) patternValue.set1(patternSize);
               }
            }
            else patternSize += ntk->getNetWidth(ntk->getInout(j));
         }
         assert (_solver->existVerifyData(pId, i));
         assert (!patternSize || patternSize == patternValue.size()); cex->pushData(patternValue);
      }
   }
   else if (V3NtkUD != proved) {  // Record Inductive Invariant
      _result[pIndex].setIndInv(ntk); assert (_result[pIndex].isInv());
   }
}

#endif

