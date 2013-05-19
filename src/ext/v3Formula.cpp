/****************************************************************************
  FileName     [ v3Formula.cpp ]
  PackageName  [ v3/src/ext ]
  Synopsis     [ V3 External Formula. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_FORMULA_C
#define V3_FORMULA_C

#include "v3Msg.h"
#include "v3Formula.h"
#include "v3NtkUtil.h"

/* -------------------------------------------------- *\
 * Class V3Formula Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3Formula::V3Formula(V3NtkHandler* const handler, const string& formulaExp) : _handler(handler) {
   assert (handler); _formula.clear();
   _rootId = lexFormula(formulaExp);  // Lex formulaExp into _formula
}

V3Formula::V3Formula(const V3Formula& formula) : _handler(formula._handler) {
   assert (_handler); _formula = formula._formula; _rootId = formula._rootId;
}

V3Formula::~V3Formula() {
   _formula.clear();
}

// Constructor for Simple Formula (Specify an Existing V3NetId)
V3Formula::V3Formula(V3NtkHandler* const handler, const V3NetId& id) : _handler(handler) {
   assert (handler); _formula.clear(); _rootId = V3NtkUD;
   if (id.id < handler->getNtk()->getNetSize()) {
      _formula.push_back(make_pair(V3_PI, V3InputVec()));
      _formula.back().second.push_back(V3NetType(id));
      _rootId = 0; assert (isValid());
   }
   else assert (!isValid());
}

// Restricted Copy Functions
V3Formula* const
V3Formula::createSuccessor(V3NtkHandler* const handler) const {
   assert (handler);
   if (_handler != handler->getHandler()) {
      Msg(MSG_ERR) << "Require handler to be the Immediate Sucessor of formula's Handler !!" << endl;
      return 0;
   }
   V3Formula* const formula = new V3Formula(handler); assert (formula);
   formula->_formula.clear(); formula->_formula.reserve(_formula.size());
   formula->_rootId = _rootId; V3NetId id; V3InputVec inputs; inputs.clear();
   for (uint32_t i = 0; i < _formula.size(); ++i)
      if (isLeaf(i)) {
         id = getNetId(i); assert (V3NetUD != id);
         id = handler->getCurrentNetId(id); inputs.push_back(id);
         if (V3NetUD == id) {
            Msg(MSG_ERR) << "Unresolvable Formula Found !!" << endl;
            delete formula; return 0;
         }
         else formula->_formula.push_back(make_pair(_formula[i].first, inputs));
         inputs.clear();
      }
      else formula->_formula.push_back(_formula[i]);
   assert (formula); return formula;
}

// Elaboration Functions
void
V3Formula::collectLeafFormula(V3UI32Set& netSet) const {
   for (uint32_t i = 0; i < _formula.size(); ++i)
      if (isLeaf(i)) netSet.insert(getNetId(i).id);
}

// Print Functions
ostream& operator << (ostream& os, const V3Formula& formula) {
   return os;
}

// Formula Expression Lexing Functions
const uint32_t
V3Formula::lexFormula(const string& exp) {
   if (!exp.size()) return V3NtkUD;
   return V3NtkUD;
}

#endif

