/****************************************************************************
  FileName     [ v3NtkElaborate.h ]
  PackageName  [ v3/src/ext ]
  Synopsis     [ Ntk Elaboration. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_ELABORATE_H
#define V3_NTK_ELABORATE_H

#include "v3NtkHash.h"
#include "v3Property.h"
#include "v3NtkHandler.h"

// class V3NtkElaborate : Ntk Elaboration
class V3NtkElaborate : public V3NtkHandler
{
   public : 
      // Constructor and Destructor
      V3NtkElaborate(V3NtkHandler* const);
      ~V3NtkElaborate();
      // I/O Ancestry Functions
      const string getOutputName(const uint32_t&) const;
      // Net Ancestry Functions
      const V3NetId getParentNetId(const V3NetId&) const;
      const V3NetId getCurrentNetId(const V3NetId&) const;
      // Elaboration Functions
      const uint32_t elaborateProperty(V3Property* const, const bool& = true);
      const uint32_t elaborateLTLFormula(V3LTLFormula* const, const bool& = true);
      // Formula Maintenance Functions
      inline const uint32_t getLTLFormulSize() const { return _pOutput.size(); }
      V3LTLFormula* const getLTLFormula(const uint32_t&) const;
   private : 
      typedef V3Vec<V3LTLFormula*>::Vec   V3LTLFormulaVec;
      // Elaboration Helper Functions
      void elaborate(V3LTLFormula* const);
      const V3NetId elaborateLTLFormula(V3LTLFormula* const, const uint32_t&);
      const V3NetId elaborateL2S(const V3NetId&);
      // Private Members
      V3NetVec          _c2pMap;    // V3NetId Mapping From Current to Parent Ntk
      V3NetVec          _p2cMap;    // V3NetId Mapping From Parent to Current Ntk
      // Formula Elaboration Members for Property
      V3PortableType    _netHash;   // Hash Table for Ntk Elaboration
      V3LTLFormulaVec   _pOutput;   // List of V3LTLFormula* for Corresponding Elaborated Output
      // Formula Elaboration Members for L2S
      V3NetVec          _shadow;    // List of V3NetId for Equivalence Against Shadow for L2S
      V3NetId           _saved;     // V3NetId of State Recorded Latch for L2S
      V3NetId           _1stSave;   // V3NetId of First Time Oracle ON for L2S
};

#endif

