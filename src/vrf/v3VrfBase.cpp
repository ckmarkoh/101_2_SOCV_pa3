/****************************************************************************
  FileName     [ v3VrfBase.cpp ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Base Class for Verification on V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_BASE_C
#define V3_VRF_BASE_C

#include "v3Msg.h"
#include "v3VrfBase.h"

/* -------------------------------------------------- *\
 * Class V3VrfBase Implementations
\* -------------------------------------------------- */
// Static Member Initialization
unsigned char   V3VrfBase::_extVerbosity     = 0;
unsigned char   V3VrfBase::_intVerbosity     = 0;
V3SolverType    V3VrfBase::_extSolverType    = V3_SVR_TOTAL;
V3SolverType    V3VrfBase::_intSolverType    = V3_SVR_TOTAL;

// Constructor and Destructor
V3VrfBase::V3VrfBase(const V3NtkHandler* const handler) : _handler(handler) {
   assert (_handler); assert (_handler->getNtk()); assert (_handler->getNtk()->getOutputSize());
   _maxDepth = 100; _invConstr.clear();
   _result = V3VrfResultVec(_handler->getNtk()->getOutputSize());
   if (_handler->getNtk()->getModuleSize())
      Msg(MSG_WAR) << "Module Instances will be Treated as Black-Boxes in Model Checking !!" << endl;
}

V3VrfBase::~V3VrfBase() {
   _invConstr.clear(); _result.clear();
}

// Constraints Setting Functions
void
V3VrfBase::addConstraint(const V3NetVec& invList) {
   for (uint32_t i = 0; i < invList.size(); ++i) {
      assert (invList[i].id < _handler->getNtk()->getNetSize());
      _invConstr.push_back(invList[i]);
   }
}

// Verification Main Functions
void
V3VrfBase::verifyInOrder() {
   for (uint32_t i = 0; i < _result.size(); ++i)
      if (!_result[i].isCex() && !_result[i].isInv()) startVerify(i);
}

// Interactive Verbosity Setting Functions
void
V3VrfBase::printReportSettings() {
   if (1ul  & _extVerbosity) {
      if (2ul  & _extVerbosity) Msg(MSG_IFO) << "-NOInteractive ";
      if (4ul  & _extVerbosity) Msg(MSG_IFO) << "-Endline ";
      if (8ul  & _extVerbosity) Msg(MSG_IFO) << "-Solver ";
      if (16ul & _extVerbosity) Msg(MSG_IFO) << "-Usage ";
      if (32ul & _extVerbosity) Msg(MSG_IFO) << "-Profile ";
   }
}

// Solver List Setting Functions
void
V3VrfBase::printSolverSettings() {
   Msg(MSG_IFO) << V3SolverTypeStr[getSolver()];
}

// Private Verification Main Functions
void
V3VrfBase::startVerify(const uint32_t& pIndex) {
   Msg(MSG_ERR) << "Calling virtual function V3VrfBase::startVerify() !!" << endl;
}

#endif

