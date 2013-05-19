/****************************************************************************
  FileName     [ v3NtkInput.h ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ Base Input Handler for V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_INPUT_H
#define V3_NTK_INPUT_H

#include "v3NtkHandler.h"

// Defines for Arrays
typedef V3Vec<string>::Vec  V3StringVec;

// V3NtkInput : Base V3 Ntk Parser
class V3NtkInput : public V3NtkHandler
{
   public : 
      // Constructor and Destructor
      V3NtkInput(const bool&, const string& = "");
      ~V3NtkInput();
      // Ntk Ancestry Functions
      inline const string getNtkName() const { return _ntkName; }
      // I/O Ancestry Functions
      const string getInputName(const uint32_t&) const;
      const string getOutputName(const uint32_t&) const;
      const string getInoutName(const uint32_t&) const;
      // Net Ancestry Functions
      void getNetName(V3NetId&, string&) const;
      // Ntk Input Naming Functions
      inline const bool existNet(const string& s) const { return V3NetUD != getNetId(s); }
      const bool resetNetName(const uint32_t&, const string&);
      void recordOutName(const uint32_t&, const string&);
      const V3NetId createNet(const string& = "", uint32_t = 1);
      const V3NetId getNetId(const string&) const;
      // Ntk Extended Helper Functions
      void renderFreeNetAsInput();
   protected : 
      typedef V3HashMap<string, V3NetId>::Hash  V3StrNetHash;
      typedef V3HashMap<uint32_t, string>::Hash V3NetStrHash;
      // Private Data Members
      const string   _ntkName;   // Name of Input Ntk  (Need not to be unique)
      V3StringVec    _outName;   // Special Handlings for Output Port Name
      V3StrNetHash   _nameHash;  // Hash Table for V3NetId from External Signal Name
      V3NetStrHash   _netHash;   // Hash Table for External Signal Name from V3NetId
};

#endif

