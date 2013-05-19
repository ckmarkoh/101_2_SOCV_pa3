/****************************************************************************
  FileName     [ v3NtkHandler.h ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ V3 Ntk Handler. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_HANDLER_H
#define V3_NTK_HANDLER_H

#include "v3Ntk.h"

// Forward Declarations
class V3NtkHandler;
class V3Property;
class V3Handler;

// Global V3 Handler
extern V3Handler v3Handler;

// Defines for Property
typedef V3Vec<V3Property*>::Vec           V3PropertyVec;
typedef V3Map<string, V3Property*>::Map   V3PropertyMap;

// Defines for Handler List
typedef V3Vec<V3NtkHandler*>::Vec         V3HandlerVec;

// Class V3NtkHierInfo : Hierarchical Ntk
class V3NtkHierInfo {
   public : 
      // Constructor and Destructor
      V3NtkHierInfo() { clear(); }
      ~V3NtkHierInfo() { clear(); }
      // Set Hier Info Functions
      inline void pushNtk(V3NtkHandler* const n, const uint32_t& p) { _hierInfo.push_back(V3NtkHierData(n, p)); }
      inline void pushRefId(const uint32_t& id, const uint32_t& i) { _refIdMap.push_back(V3PairType(id, i)); }
      inline void clearMap() { for (uint32_t i = 0; i < getNtkSize(); ++i) _hierInfo[i]._p2cMap.clear(); }
      inline void clearRefId() { _refIdMap.clear(); }
      inline void clear() { _hierInfo.clear(); _refIdMap.clear(); }
      // Get Hier Info Functions
      inline const uint32_t getNtkSize() const { return _hierInfo.size(); }
      inline const uint32_t getRefSize() const { return _refIdMap.size(); }
      inline V3NtkHandler* const getHandler(const uint32_t& i) const {
         assert (isNtkValid(i)); return _hierInfo[i]._handler; }
      inline const uint32_t getParentId(const uint32_t& i) const {
         assert (isNtkValid(i)); return _hierInfo[i]._parentId; }
      inline const V3NetVec& getMap(const uint32_t& i) const {
         assert (isNtkValid(i)); return _hierInfo[i]._p2cMap; }
      inline V3NetVec& getMap(const uint32_t& i) {
         assert (isNtkValid(i)); return _hierInfo[i]._p2cMap; }
      inline const V3PairType& getRefId(const uint32_t& i) const {
         assert (isRefValid(i)); return _refIdMap[i]; }
   private : 
      // Class V3NtkHierData : Internal Helper Structure for Hierarchical Ntk
      class V3NtkHierData {
         public : 
            // Constructor and Destructor
            V3NtkHierData(V3NtkHandler* const n, const uint32_t& p) : _handler(n), _parentId(p) { _p2cMap.clear(); }
            V3NtkHierData& operator = (const V3NtkHierData& d) {
               *(const_cast<V3NtkHandler**>(&_handler)) = d._handler;
               *(const_cast<uint32_t*>(&_parentId)) = d._parentId;
               _p2cMap.clear(); return *this;
            }
            V3NtkHierData(const V3NtkHierData& d) : _handler(d._handler), _parentId(d._parentId) { _p2cMap.clear(); }
            ~V3NtkHierData() { _p2cMap.clear(); }
            // Private Data Members
            V3NtkHandler* const  _handler;
            const uint32_t       _parentId;
            V3NetVec             _p2cMap;
      };
      // Private Member Functions
      inline const bool isNtkValid(const uint32_t& i) const { return i < _hierInfo.size(); }
      inline const bool isRefValid(const uint32_t& i) const { return i < _refIdMap.size(); }
      // Private Members
      V3Vec<V3NtkHierData>::Vec  _hierInfo;
      V3Vec<V3PairType>::Vec     _refIdMap;
};

// V3NtkHandler : Base V3 Ntk Handler
class V3NtkHandler
{
   public : 
      // Constructor and Destructor
      V3NtkHandler(V3NtkHandler* const = 0, V3Ntk* const = 0);
      virtual ~V3NtkHandler();
      // Ntk Ancestry Functions
      virtual const string getNtkName() const;
      inline V3Ntk* const getNtk() const { return _ntk; }
      inline V3NtkHandler* const getHandler() const { return _handler; }
      V3NtkHandler* const getPrimitive() const;
      // I/O Ancestry Functions
      virtual const string getInputName(const uint32_t&) const;
      virtual const string getOutputName(const uint32_t&) const;
      virtual const string getInoutName(const uint32_t&) const;
      // Net Ancestry Functions
      const string getNetNameOrFormedWithId(const V3NetId&) const;
      const string getNetExpression(const V3NetId&) const;
      const string getNetName(const V3NetId&) const;
      virtual void getNetName(V3NetId&, string&) const;
      virtual const V3NetId getParentNetId(const V3NetId&) const;
      virtual const V3NetId getCurrentNetId(const V3NetId&) const;
      // Ntk Instance Ntk Reference Functions
      inline void incInstRef() { ++_instRef; assert (_instRef); }
      inline void decInstRef() { assert (_instRef); --_instRef; }
      // Ntk Child Ntk Reference Functions
      inline void incRefCount() { ++_refCount; assert (_refCount); }
      inline void decRefCount() { assert (_refCount); --_refCount; }
      // Property Maintanence Functions
      void setProperty(V3Property* const);
      const string getAuxPropertyName() const;
      inline const uint32_t getPropertySize() const { return _property.size(); }
      inline const bool existProperty(const string& s) const { return _property.end() != _property.find(s); }
      V3Property* const getProperty(const string& s) const {
         assert (existProperty(s)); return _property.find(s)->second; }
      // Ntk Verbosity Functions
      static void printVerbositySettings();
      static inline void setExternalVerbosity() { _intVerbosity = _extVerbosity; }
      static inline void setInternalVerbosity() { _extVerbosity = _intVerbosity; }
      static inline void resetVerbositySettings() { _extVerbosity = 0; setReduce(1); setP2CMap(1); setC2PMap(1); }
      static inline void setReduce (const bool& t) { if (t) _extVerbosity |=  1ul; else _extVerbosity &= ~1ul;  }
      static inline void setStrash (const bool& t) { if (t) _extVerbosity |=  2ul; else _extVerbosity &= ~2ul;  }
      static inline void setRewrite(const bool& t) { if (t) _extVerbosity |=  4ul; else _extVerbosity &= ~4ul;  }
      static inline void setP2CMap (const bool& t) { if (t) _extVerbosity |=  8ul; else _extVerbosity &= ~8ul;  }
      static inline void setC2PMap (const bool& t) { if (t) _extVerbosity |= 16ul; else _extVerbosity &= ~16ul; }
      // Verbosity Helper Functions
      static inline const bool reduceON()  { return _extVerbosity & 1ul;  }
      static inline const bool strashON()  { return _extVerbosity & 2ul;  }
      static inline const bool rewriteON() { return _extVerbosity & 4ul;  }
      static inline const bool P2CMapON()  { return _extVerbosity & 8ul;  }
      static inline const bool C2PMapON()  { return _extVerbosity & 16ul; }
      inline const bool isMutable() const { return !(_refCount || _instRef); }
      // Ntk Printing Functions
      void printSummary() const;
      void printPrimary() const;
      void printVerbose() const;
      void printNetlist() const;
      // Renaming Functions
      static void setAuxRenaming();
      static void resetAuxRenaming();
      static const string applyAuxNetNamePrefix(const string&);
   protected : 
      // Private Helper Functions
      const V3NetId getPrimitiveNetId(const V3NetId&) const;
      // Private Ntk Members
      V3NtkHandler* const  _handler;   // Parent Ntk Handler
      V3Ntk*               _ntk;       // Ntk Derived From Parent
      V3PropertyMap        _property;  // Property List
      // Alterable Renaming Prefix
      static string        V3AuxNameInvPrefix;
      static string        V3AuxNetNamePrefix;
      static string        V3AuxExpansionName;
   private : 
      // Static Members for Global Control
      static unsigned char _extVerbosity;
      static unsigned char _intVerbosity;
      // Ntk Handler Data Members
      uint32_t             _instRef;
      uint32_t             _refCount;
};

// V3Handler : Handler for Overall V3 Framework
class V3Handler
{
   public : 
      // Constructor and Destructor
      V3Handler();
      ~V3Handler();
      // Ntk Handler Maintanence Functions
      inline const uint32_t getCurHandlerId() const { return _curHandlerId; }
      inline const uint32_t getHandlerCount() const { return _ntkHandlerList.size(); }
      inline V3NtkHandler* const getCurHandler() const { 
         return getHandlerCount() ? _ntkHandlerList[_curHandlerId] : 0; }
      // Ntk Ancestry and Hierarchy Maintanence Functions
      V3NtkHandler* const getHandler(const uint32_t&) const;
      void pushAndSetCurHandler(V3NtkHandler* const);
      void setCurHandlerFromId(const uint32_t&);
      void setCurHandlerFromPath(const string&);
      void setLastHandler();
      void setRootHandler();
      void setPrevHandler();
      void setBaseHandler();
      // Print Functions
      void printNtkInAncestry() const;
      void printNtkInHierarchy() const;
      void printRecurHierarchy(V3NtkHandler* const, const uint32_t&, const uint32_t&) const;
      void printNtkRelation(const string&) const;
   private : 
      // Handler Helper Functions
      const uint32_t getCurPrimitiveIndex() const;
      // Private Data Members
      uint32_t       _curHandlerId;    // Id of Current Ntk Handler 
      uint32_t       _lastHandlerId;   // Id of Last Ntk Handler 
      V3HandlerVec   _ntkHandlerList;  // Ntk Handler List
      V3UI32Vec      _curRefIdVec;     // Current Ref Indices from Base Ntk
      V3UI32Vec      _lastRefIdVec;    // Last Ref Indices from Base Ntk
};

#endif

