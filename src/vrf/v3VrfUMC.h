/****************************************************************************
  FileName     [ v3VrfUMC.h ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Unbounded Model Checking on V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_UMC_H
#define V3_VRF_UMC_H

#include "v3VrfBase.h"

// class V3VrfUMC : Verification Handler for Unbounded Model Checking
class V3VrfUMC : public V3VrfBase
{
   public : 
      // Constructor and Destructor
      V3VrfUMC(const V3NtkHandler* const);
      ~V3VrfUMC();
      // UMC Specific Functions
      inline void setPreDepth(const uint32_t& d) { _preDepth = d; }
      inline void setIncDepth(const uint32_t& d) { _incDepth = d; }
      inline void setProveOnly() { _umcAttr |= 1ul; }
      inline void setFireOnly()  { _umcAttr |= 2ul; }
   private : 
      // Private Verification Main Functions
      void startVerify(const uint32_t&);
      // UMC Specific Helper Functions
      inline const bool isProveOnly() { return _umcAttr & 1ul; }
      inline const bool isFireOnly()  { return _umcAttr & 2ul; }
      // Private Data Members
      uint32_t       _preDepth;     // Pre-Run Depth (default = 0)
      uint32_t       _incDepth;     // Increment Depth (default = 1)
      unsigned char  _umcAttr;      // Special Settings for UMC
      V3SvrBase*     _solver;       // Single Instance SAT Solver
};

#endif

