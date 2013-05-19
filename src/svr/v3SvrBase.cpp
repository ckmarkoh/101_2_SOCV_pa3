/****************************************************************************
  FileName     [ v3SvrBase.cpp ]
  PackageName  [ v3/src/svr ]
  Synopsis     [ V3 Engine-Encapsulated Base Solver. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2011 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_SVR_BASE_C
#define V3_SVR_BASE_C

#include "v3Msg.h"
#include "v3SvrBase.h"

/* -------------------------------------------------- *\
 * Class V3SvrBase Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3SvrBase::V3SvrBase(const V3Ntk* const ntk, const bool& freeBound) : _ntk(ntk), _freeBound(freeBound) {
   _solves = 0; _runTime = 0;
}

V3SvrBase::~V3SvrBase() {
}

// Basic Operation Functions
void
V3SvrBase::reset() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::reset() !!" << endl;
}

void
V3SvrBase::assumeInit() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assumeInit() !!" << endl;
}

void
V3SvrBase::assertInit() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assertInit() !!" << endl;
}

void
V3SvrBase::initRelease() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::initRelease() !!" << endl;
}

void
V3SvrBase::assumeRelease() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assumeRelease() !!" << endl;
}

void
V3SvrBase::assumeProperty(const size_t& var_exp, const bool& invert) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assumeProperty() !!" << endl;
}

void
V3SvrBase::assertProperty(const size_t& var_exp, const bool& invert) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assertProperty() !!" << endl;
}

void
V3SvrBase::assumeProperty(const V3NetId& id, const bool& invert, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assumeProperty() !!" << endl;
}

void
V3SvrBase::assertProperty(const V3NetId& id, const bool& invert, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assertProperty() !!" << endl;
}

const bool
V3SvrBase::simplify() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::simplify() !!" << endl; return false;
}

const bool
V3SvrBase::solve() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::solve() !!" << endl; return false;
}

const bool
V3SvrBase::assump_solve() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assump_solve() !!" << endl; return false;
}

// Manipulation Helper Functions
void
V3SvrBase::setTargetValue(const V3NetId& id, const V3BitVecX& value, const uint32_t& depth, V3SvrDataVec& formula) {
   // Note : This Function will set formula such that AND(formula) represents (p == value)
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::setTargetValue() !!" << endl;
}

void
V3SvrBase::assertImplyUnion(const V3SvrDataVec& var_exp) {
   // Note : This function asserts the conjunctions in var_exp, 
   //        i.e. (var_exp[0] || var_exp[1] || ...)
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::assertImplyUnion() !!" << endl;
}

const size_t
V3SvrBase::setTargetValue(const V3NetId& id, const V3BitVecX& value, const uint32_t& depth, const size_t& prev) {
   // Note : This Function returns an expression e to the formula, i.e. e = (p == value)
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::setTargetValue() !!" << endl; return 0;
}

const size_t
V3SvrBase::setImplyUnion(const V3SvrDataVec& var_exp) {
   // Note : This function returns a controlling expression e. An assertion of e implies conjunctions in var_exp, 
   //        i.e. e --> (var_exp[0] || var_exp[1] || ...)
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::setImplyUnion() !!" << endl; return 0;
}

const size_t
V3SvrBase::setImplyIntersection(const V3SvrDataVec& var_exp) {
   // Note : This function returns a controlling expression e. An assertion of e implies disjunctions in var_exp, 
   //        i.e. e --> (var_exp[0] && var_exp[1] && ...)
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::setImplyIntersection() !!" << endl; return 0;
}

const size_t
V3SvrBase::setImplyInit() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::setImplyInit() !!" << endl; return 0;
}

// Retrieval Functions
const V3BitVecX
V3SvrBase::getDataValue(const V3NetId& id, const uint32_t& depth) const {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::getDataValue() !!" << endl; return 0;
}

const bool
V3SvrBase::getDataValue(const size_t& var_exp) const {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::getDataValue() !!" << endl; return 0;
}

void
V3SvrBase::getDataConflict(V3SvrDataVec& var_exp) const {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::getDataConflict() !!" << endl;
}

const size_t
V3SvrBase::getFormula(const V3NetId& id, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::getFormula() !!" << endl;
   return 0;
}

const size_t
V3SvrBase::getFormula(const V3NetId& id, const uint32_t& bit, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::getFormula() !!" << endl;
   return 0;
}

// Variable Interface Functions
const size_t
V3SvrBase::reserveFormula() {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::reserveFormula() !!" << endl; return 0;
}

const bool
V3SvrBase::isNegFormula(const size_t& var_exp) const {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::isNegFormula() !!" << endl; return false;
}

const size_t
V3SvrBase::getNegFormula(const size_t& var_exp) const {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::getNegFormula() !!" << endl; return 0;
}

// Formula Output Functions
const bool
V3SvrBase::setOutputFile(const string fileName) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::setOutputFile() !!" << endl; return false;
}

const bool
V3SvrBase::isOutputSet() const {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::isOutputSet() !!" << endl; return false;
}

// Print Data Functions
void
V3SvrBase::printInfo() const {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::printInfo() !!" << endl;
}


// Gate Formula to Solver Functions
void
V3SvrBase::add_FALSE_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_FALSE_Formula() !!" << endl;
}

void
V3SvrBase::add_PI_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_PI_Formula() !!" << endl;
}

void
V3SvrBase::add_FF_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_FF_Formula() !!" << endl;
}

void
V3SvrBase::add_AND_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_AND_Formula) !!" << endl;
}

void
V3SvrBase::add_XOR_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_XOR_Formula() !!" << endl;
}

void
V3SvrBase::add_MUX_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_MUX_Formula() !!" << endl;
}

void
V3SvrBase::add_RED_AND_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_RED_AND_Formula) !!" << endl;
}

void
V3SvrBase::add_RED_OR_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_RED_OR_Formula() !!" << endl;
}

void
V3SvrBase::add_RED_XOR_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_RED_XOR_Formula() !!" << endl;
}

void
V3SvrBase::add_ADD_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_ADD_Formula() !!" << endl;
}

void
V3SvrBase::add_SUB_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_SUB_Formula() !!" << endl;
}

void
V3SvrBase::add_MULT_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_MULT_Formula() !!" << endl;
}

void
V3SvrBase::add_DIV_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_DIV_Formula() !!" << endl;
}

void
V3SvrBase::add_MODULO_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_MODULO_Formula() !!" << endl;
}

void
V3SvrBase::add_SHL_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_SHL_Formula() !!" << endl;
}

void
V3SvrBase::add_SHR_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_SHR_Formula() !!" << endl;
}

void
V3SvrBase::add_CONST_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_CONST_Formula() !!" << endl;
}

void
V3SvrBase::add_SLICE_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_SLICE_Formula() !!" << endl;
}

void
V3SvrBase::add_MERGE_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_MERGE_Formula) !!" << endl;
}

void
V3SvrBase::add_EQUALITY_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_EQUALITY_Formula() !!" << endl;
}

void
V3SvrBase::add_GEQ_Formula(const V3NetId& outNet, const uint32_t& depth) {
   Msg(MSG_ERR) << "Calling virtual function V3SvrBase::add_GEQ_Formula() !!" << endl;
}

// Network to Solver Functions
void
V3SvrBase::addBoundedVerifyData(const V3NetId& id, uint32_t& depth) {
   addSimpleBoundedVerifyData(id, depth);
}

const bool
V3SvrBase::existVerifyData(const V3NetId& id, const uint32_t& depth) {
   return false;
}

void
V3SvrBase::assertBoundedVerifyData(const V3NetVec& assertIdVec, uint32_t& depth) {
   for (uint32_t i = 0; i < assertIdVec.size(); ++i) {
      addBoundedVerifyData(assertIdVec[i], depth);
      assertProperty(assertIdVec[i], false, depth);
   }
}

// Private Network to Solver Functions
void
V3SvrBase::addVerifyData(const V3NetId& id, const uint32_t& depth) {
   V3GateType type = _ntk->getGateType(id); assert (type < V3_XD);
   if (V3_PIO >= type) return add_PI_Formula(id, depth);
   else if (V3_FF == type) return add_FF_Formula(id, depth);
   else if (type > AIG_FALSE) {  // BV
      switch (type) {
         case BV_RED_AND  : return add_RED_AND_Formula(id, depth);
         case BV_RED_OR   : return add_RED_OR_Formula(id, depth);
         case BV_RED_XOR  : return add_RED_XOR_Formula(id, depth);
         case BV_MUX      : return add_MUX_Formula(id, depth);
         case BV_AND      : return add_AND_Formula(id, depth);
         case BV_XOR      : return add_XOR_Formula(id, depth);
         case BV_ADD      : return add_ADD_Formula(id, depth);
         case BV_SUB      : return add_SUB_Formula(id, depth);
         case BV_MULT     : return add_MULT_Formula(id, depth);
         case BV_DIV      : return add_DIV_Formula(id, depth);
         case BV_MODULO   : return add_MODULO_Formula(id, depth);
         case BV_SHL      : return add_SHL_Formula(id, depth);
         case BV_SHR      : return add_SHR_Formula(id, depth);
         case BV_MERGE    : return add_MERGE_Formula(id, depth);
         case BV_EQUALITY : return add_EQUALITY_Formula(id, depth);
         case BV_GEQ      : return add_GEQ_Formula(id, depth);
         case BV_SLICE    : return add_SLICE_Formula(id, depth);
         default          : return add_CONST_Formula(id, depth);
      }
   }
   else {
      if (AIG_FALSE == type) return add_FALSE_Formula(id, depth);  // AIG_FALSE
      return add_AND_Formula(id, depth);  // AIG_NODE
   }
}

void
V3SvrBase::addSimpleBoundedVerifyData(const V3NetId& id, uint32_t& depth) {
   assert (validNetId(id)); if (existVerifyData(id, depth)) return;
   const V3GateType type = _ntk->getGateType(id); assert (type < V3_XD);
   if (V3_PIO >= type) add_PI_Formula(id, depth);
   else if (V3_FF == type) {
      if (depth) { --depth; addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 0), depth); ++depth; }
      else addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 1), depth); add_FF_Formula(id, depth);
   }
   else if (AIG_FALSE >= type) {
      if (AIG_NODE == type) {
         addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 0), depth);
         addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 1), depth);
         add_AND_Formula(id, depth);
      }
      else { assert (AIG_FALSE == type); add_FALSE_Formula(id, depth); }
   }
   else if (isV3PairType(type)) {
      assert (BV_AND == type || BV_XOR == type || BV_ADD == type || BV_SUB == type || BV_SHL == type || 
              BV_SHR == type || BV_MERGE == type || BV_EQUALITY == type || BV_GEQ == type || BV_MULT == type || 
              BV_DIV == type || BV_MODULO == type);
      addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 0), depth);
      addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 1), depth);
      switch (type) {
         case BV_AND      : add_AND_Formula(id, depth);      break;
         case BV_XOR      : add_XOR_Formula(id, depth);      break;
         case BV_ADD      : add_ADD_Formula(id, depth);      break;
         case BV_SUB      : add_SUB_Formula(id, depth);      break;
         case BV_SHL      : add_SHL_Formula(id, depth);      break;
         case BV_SHR      : add_SHR_Formula(id, depth);      break;
         case BV_MERGE    : add_MERGE_Formula(id, depth);    break;
         case BV_EQUALITY : add_EQUALITY_Formula(id, depth); break;
         case BV_GEQ      : add_GEQ_Formula(id, depth);      break;
         case BV_MULT     : add_MULT_Formula(id, depth);     break;
         case BV_DIV      : add_DIV_Formula(id, depth);      break;
         default          : add_MODULO_Formula(id, depth);   break;
      }
   }
   else if (isV3ReducedType(type)) {
      assert (BV_RED_AND == type || BV_RED_OR == type || BV_RED_XOR == type);
      addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 0), depth);
      switch (type) {
         case BV_RED_AND  : add_RED_AND_Formula(id, depth); break;
         case BV_RED_OR   : add_RED_OR_Formula(id, depth);  break;
         default          : add_RED_XOR_Formula(id, depth); break;
      }
   }
   else if (BV_MUX == type) {
      addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 0), depth);
      addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 1), depth);
      addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 2), depth);
      add_MUX_Formula(id, depth);
   }
   else if (BV_SLICE == type) {
      addSimpleBoundedVerifyData(_ntk->getInputNetId(id, 0), depth);
      add_SLICE_Formula(id, depth);
   }
   else { assert (BV_CONST == type); add_CONST_Formula(id, depth); }
}

#endif

