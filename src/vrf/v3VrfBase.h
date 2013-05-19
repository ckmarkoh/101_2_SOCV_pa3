/****************************************************************************
  FileName     [ v3VrfBase.h ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Base Class for Verification on V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_BASE_H
#define V3_VRF_BASE_H

#include "v3Property.h"
#include "v3NtkHandler.h"
#include "v3SvrHandler.h"

// class V3VrfBase : Base Verification Handler
class V3VrfBase
{
   public : 
      // Constructor and Destructor
      V3VrfBase(const V3NtkHandler* const);
      virtual ~V3VrfBase();
      // Constraints Setting Functions
      void addConstraint(const V3NetVec&);
      // Verification Main Functions
      void verifyInOrder();
      // Inline Interface Functions
      inline void setMaxDepth(const uint32_t& d) { assert (d); _maxDepth = d; }
      inline const V3VrfResult& getResult(const uint32_t& i) const { assert (i < _result.size()); return _result[i]; }
      // Interactive Verbosity Setting Functions
      static void printReportSettings();
      static inline void resetReportSettings() { _extVerbosity &= 64ul; setReport(1); setUsage(1); }
      static inline void setReport (const bool& t) { if (t) _extVerbosity |= 1ul;  else _extVerbosity &= ~1ul;  }
      static inline void setRstOnly(const bool& t) { if (t) _extVerbosity |= 2ul;  else _extVerbosity &= ~2ul;  }
      static inline void setEndline(const bool& t) { if (t) _extVerbosity |= 4ul;  else _extVerbosity &= ~4ul;  }
      static inline void setSolver (const bool& t) { if (t) _extVerbosity |= 8ul;  else _extVerbosity &= ~8ul;  }
      static inline void setUsage  (const bool& t) { if (t) _extVerbosity |= 16ul; else _extVerbosity &= ~16ul; }
      static inline void setProfile(const bool& t) { if (t) _extVerbosity |= 32ul; else _extVerbosity &= ~32ul; }
      // Verbosity Helper Functions
      static inline const bool reportON()  { return (1ul & _extVerbosity); }
      static inline const bool intactON()  { return reportON() && !(2ul  & _extVerbosity); }
      static inline const bool endLineON() { return intactON() &&  (4ul  & _extVerbosity); }
      static inline const bool svrInfoON() { return reportON() &&  (8ul  & _extVerbosity); }
      static inline const bool usageON()   { return reportON() &&  (16ul & _extVerbosity); }
      static inline const bool profileON() { return reportON() &&  (32ul & _extVerbosity); }
      // Solver List Setting Functions
      static void printSolverSettings();
      static inline void resetSolverSettings() { _extSolverType = V3_SVR_MINISAT; }
      static inline void setSolver(const V3SolverType& type) { _extSolverType = type; }
      // Solver List Helper Functions
      static inline const V3SolverType getSolver() { return _extSolverType; }
   protected : 
      typedef V3Vec<V3VrfResult>::Vec  V3VrfResultVec;
      // Private Verification Main Functions
      virtual void startVerify(const uint32_t&);
      // Functions for Internal Verbosity Handlings
      inline void setExternalVerbosity() { _intVerbosity = _extVerbosity; }
      inline void setInternalVerbosity() { _extVerbosity = _intVerbosity; }
      inline void setExternalSolverType() { _intSolverType = _extSolverType; }
      inline void setInternalSolverType() { _extSolverType = _intSolverType; }
      // Private Data Members
      const V3NtkHandler* const  _handler;
      V3NetVec                   _invConstr;
      uint32_t                   _maxDepth;
      // Private Members for Verification Results
      V3VrfResultVec             _result;
   private : 
      // Static Members for Global Control
      static unsigned char       _extVerbosity;
      static unsigned char       _intVerbosity;
      static V3SolverType        _extSolverType;
      static V3SolverType        _intSolverType;
};

#endif

