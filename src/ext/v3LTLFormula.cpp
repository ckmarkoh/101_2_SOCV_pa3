/****************************************************************************
  FileName     [ v3LTLFormula.cpp ]
  PackageName  [ v3/src/ext ]
  Synopsis     [ V3 LTL Formula. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_LTL_FORMULA_C
#define V3_LTL_FORMULA_C

#include "v3LTLFormula.h"

/* -------------------------------------------------- *\
 * Class V3Formula Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3LTLFormula::V3LTLFormula(V3NtkHandler* const handler, const string& ltlFormulaExp, const string& name) 
   : _handler(handler), _name(name) {
   assert (handler); _formula.clear();
   _rootId = lexLTLFormula(ltlFormulaExp);
}

V3LTLFormula::V3LTLFormula(const V3LTLFormula& ltlFormula, const string& name) 
   : _handler(ltlFormula._handler), _name(name) {
   assert (_handler); _formula = ltlFormula._formula; _rootId = ltlFormula._rootId;
   for (uint32_t i = 0; i < _formula.size(); ++i)
      if (isLeaf(i)) _formula[i].second[0] = (size_t)(new V3Formula(*(getFormula(i))));
}

V3LTLFormula::~V3LTLFormula() {
   for (uint32_t i = 0; i < _formula.size(); ++i) {
      if (isLeaf(i)) delete (V3Formula*)(_formula[i].second[0]);
      _formula[i].second.clear();
   }
   _formula.clear();
}

// Constructor for Simple LTL Formula (AGp and AFp)
V3LTLFormula::V3LTLFormula(V3NtkHandler* const handler, const V3NetId& id, const bool& safe, const string& name) 
   : _handler(handler), _name(name) {
   assert (handler); _formula.clear(); _rootId = V3NtkUD;
   V3Formula* const formula = new V3Formula(handler, id); assert (formula);
   if (formula->isValid()) {
      _formula.push_back(make_pair(V3_LTL_P, V3PtrVec(1, (size_t)(formula))));
      _formula.push_back(make_pair((safe ? V3_LTL_T_G : V3_LTL_T_F), V3PtrVec(1, 0)));
      _rootId = 1; assert (isValid());
   }
   else { delete formula; assert (!isValid()); }
}

// Restricted Copy Functions
V3LTLFormula* const
V3LTLFormula::createSuccessor(V3NtkHandler* const handler) const {
   V3LTLFormula* const ltlFormula = new V3LTLFormula(handler, "", _name); assert (ltlFormula);
   ltlFormula->_formula.clear(); ltlFormula->_formula.reserve(_formula.size());
   ltlFormula->_rootId = _rootId; V3Formula* formula = 0;
   for (uint32_t i = 0; i < _formula.size(); ++i)
      if (isLeaf(i)) {
         formula = getFormula(i)->createSuccessor(handler);
         if (!formula) { delete ltlFormula; return 0; }
         else ltlFormula->_formula.push_back(make_pair(_formula[i].first, V3PtrVec(1, (size_t)formula)));
      }
      else ltlFormula->_formula.push_back(_formula[i]);
   assert (ltlFormula); return ltlFormula;
}

// Elaboration Functions
void
V3LTLFormula::collectLeafFormula(V3UI32Set& netSet) const {
   for (uint32_t i = 0; i < _formula.size(); ++i)
      if (isLeaf(i)) getFormula(i)->collectLeafFormula(netSet);
}

// Print Functions
ostream& operator << (ostream& os, const V3LTLFormula& formula) {
   return os;
}

// Formula Expression Lexing Functions
const uint32_t
V3LTLFormula::lexLTLFormula(const string& exp) {
   if (!exp.size()) return V3NtkUD;
   return V3NtkUD;
}

#endif

