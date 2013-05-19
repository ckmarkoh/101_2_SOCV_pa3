/****************************************************************************
  FileName     [ v3Property.cpp ]
  PackageName  [ v3/src/ext ]
  Synopsis     [ Property Handler for V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_PROPERTY_C
#define V3_PROPERTY_C

#include "v3Property.h"

/* -------------------------------------------------- *\
 * Class V3Property Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3Property::V3Property(V3LTLFormula* const formula) : _formula(formula) {
   assert (formula); 
}

V3Property::~V3Property() {
   // Remove LTL Formula
   if (_formula) delete _formula;
   // Remove Result
   _result.clear();
}

// Record Verification Result Functions
void
V3Property::setResult(const V3VrfResult& r) {
   assert (r.isCex() || r.isInv());
   if (r.isInv()) { assert (!isFired()); _result.setIndInv(r.getIndInv()); assert (isProven()); }
   else { assert (!isProven()); _result.setCexTrace(r.getCexTrace()); assert (isFired()); }
}

// Print Functions
ostream& operator << (ostream& os, const V3Property& p) {
   os << (p.isProven() ? "[T] " : p.isFired() ? "[F] " : "    ") << *(p.getLTLFormula()); return os;
}

#endif

