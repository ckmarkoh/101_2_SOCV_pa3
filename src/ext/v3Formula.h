/****************************************************************************
  FileName     [ v3Formula.h ]
  PackageName  [ v3/src/ext ]
  Synopsis     [ V3 External Formula. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_FORMULA_H
#define V3_FORMULA_H

#include "v3NtkHandler.h"

// V3Formula : External Formula Storage
class V3Formula
{
   public : 
      // Constructor and Destructor
      V3Formula(V3NtkHandler* const, const string& = "");
      V3Formula(const V3Formula&);
      ~V3Formula();
      // Constructor for Simple Formula (Specify an Existing V3NetId)
      V3Formula(V3NtkHandler* const, const V3NetId&);
      // Inline Formula Interface Functions
      inline V3NtkHandler* const getHandler() const { return _handler; }
      inline const bool isValid() const { return _rootId < _formula.size(); }
      inline const bool isValid(const uint32_t& i) const { return i < _formula.size(); }
      inline const bool isLeaf(const uint32_t& i) const { assert (isValid(i)); return V3_PI == _formula[i].first; }
      inline const V3GateType& getGateType(const uint32_t& i) const { assert (!isLeaf(i)); return _formula[i].first; }
      inline const V3NetId& getNetId(const uint32_t& i) const {
         assert (isLeaf(i)); return _formula[i].second[0].id; }
      inline const uint32_t getBranchSize(const uint32_t& i) const {
         assert (!isLeaf(i)); return _formula[i].second.size(); }
      inline const uint32_t getBranchIndex(const uint32_t& i, const uint32_t& j) const {
         assert (!isLeaf(i)); assert (j < getBranchSize(i)); return _formula[i].second[j].value; }
      inline const uint32_t getRoot() const { assert (isValid()); return _rootId; }
      // Restricted Copy Functions
      V3Formula* const createSuccessor(V3NtkHandler* const) const;
      // Elaboration Functions
      void collectLeafFormula(V3UI32Set&) const;
      // Print Functions
      friend ostream& operator << (ostream&, const V3Formula&);
   private : 
      typedef pair<V3GateType, V3InputVec>   V3FormulaNode;
      typedef V3Vec<V3FormulaNode>::Vec      V3FormulaList;
      // Formula Expression Lexing Functions
      const uint32_t lexFormula(const string&);
      // Private Data Members
      V3NtkHandler* const  _handler;
      V3FormulaList        _formula;
      uint32_t             _rootId;
};

#endif

