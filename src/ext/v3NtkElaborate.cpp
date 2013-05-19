/****************************************************************************
  FileName     [ v3NtkElaborate.cpp ]
  PackageName  [ v3/src/ext ]
  Synopsis     [ Ntk Elaboration. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_ELABORATE_C
#define V3_NTK_ELABORATE_C

#include "v3Msg.h"
#include "v3ExtUtil.h"
#include "v3NtkUtil.h"
#include "v3StrUtil.h"
#include "v3NtkElaborate.h"

/* -------------------------------------------------- *\
 * Class V3NtkElaborate Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3NtkElaborate::V3NtkElaborate(V3NtkHandler* const handler) 
   : V3NtkHandler(handler) {
   assert (_handler); assert (!_ntk); _c2pMap.clear(); _p2cMap.clear(); _pOutput.clear();
   const uint32_t shadowSize = _handler->getNtk()->getLatchSize();
   _saved = V3NetUD; _1stSave = V3NetUD; _shadow.clear(); _shadow.reserve(shadowSize);
   for (uint32_t i = 0; i < shadowSize; ++i) _shadow.push_back(V3NetUD);
}

V3NtkElaborate::~V3NtkElaborate() {
   for (uint32_t i = 0; i < _pOutput.size(); ++i) delete _pOutput[i];
   _c2pMap.clear(); _p2cMap.clear(); _pOutput.clear(); _shadow.clear();
}

// I/O Ancestry Functions
const string
V3NtkElaborate::getOutputName(const uint32_t& index) const {
   assert (_ntk); assert (index < _ntk->getOutputSize());
   if (index >= _pOutput.size()) return "c_" + v3Int2Str((size_t)(this)) + "_" + v3Int2Str(index);
   else return "p_" + v3Int2Str((size_t)(_pOutput[index])) + "_" + _pOutput[index]->getName();
}

// Net Ancestry Functions
const V3NetId
V3NtkElaborate::getParentNetId(const V3NetId& id) const {
   if (V3NetUD == id || _c2pMap.size() <= id.id) return V3NetUD;
   return (isV3NetInverted(id) ? getV3InvertNet(_c2pMap[id.id]) : _c2pMap[id.id]);
}

const V3NetId
V3NtkElaborate::getCurrentNetId(const V3NetId& id) const {
   if (V3NetUD == id || _p2cMap.size() <= id.id) return V3NetUD;
   return (isV3NetInverted(id) ? getV3InvertNet(_p2cMap[id.id]) : _p2cMap[id.id]);
}

// Elaboration Functions
const uint32_t
V3NtkElaborate::elaborateProperty(V3Property* const p, const bool& l2s) {
   assert (p);
   V3LTLFormula* const ltlFormula = p->getLTLFormula(); assert (ltlFormula);
   const uint32_t pIndex = elaborateLTLFormula(ltlFormula, l2s);
   assert ((1 + pIndex) == _ntk->getOutputSize()); return pIndex;
}

const uint32_t
V3NtkElaborate::elaborateLTLFormula(V3LTLFormula* const ltlFormula, const bool& l2s) {
   // Make Sure Mapping Tables are Maintained, or NO properties can be proliferated
   if (!isMutable() || (_ntk && (!_p2cMap.size()))) return V3NtkUD;
   // Elaborate LTL Formula into Ntk
   assert (ltlFormula); assert (_handler == ltlFormula->getHandler()); elaborate(ltlFormula);
   // Create Formula for this Ntk, and Perform LTL Formula Rewriting if Enabled
   V3LTLFormula* const formula = ltlFormula->createSuccessor(this);
   // Currently Support ONLY AG(p) and AF(p)
   const uint32_t rootIndex = formula->getRoot(); assert (!formula->isLeaf(rootIndex));
   V3NetId id = elaborateLTLFormula(formula, rootIndex); if (V3NetUD == id) return V3NtkUD;
   id = (l2s && V3_LTL_T_F == formula->getOpType(rootIndex)) ? elaborateL2S(id) : ~id;
   // Record LTL Formula and Set to Output
   _pOutput.push_back(formula); _ntk->createOutput(id); assert (_pOutput.size() == _ntk->getOutputSize());
   return _ntk->getOutputSize() - 1;
}

// Private Member Functions
void
V3NtkElaborate::elaborate(V3LTLFormula* const ltlFormula) {
   // Collect Leaf Formula, and Retain Only New Ones
   V3UI32Set targetNetIdSet; targetNetIdSet.clear(); assert (isMutable());
   assert (ltlFormula); ltlFormula->collectLeafFormula(targetNetIdSet);
   V3NetVec targetNets; targetNets.clear(); targetNets.reserve(targetNetIdSet.size());
   if (_p2cMap.size())
      for (V3UI32Set::const_iterator it = targetNetIdSet.begin(); it != targetNetIdSet.end(); ++it) {
         assert ((*it) < _p2cMap.size()); if (V3NetUD != _p2cMap[*it]) continue;
         targetNets.push_back(V3NetId::makeNetId(*it));
      }
   else 
      for (V3UI32Set::const_iterator it = targetNetIdSet.begin(); it != targetNetIdSet.end(); ++it)
         targetNets.push_back(V3NetId::makeNetId(*it));
   // Create Ntk if NOT Exist, or Augment New Formula Logic to Existing Ntk
   if (!_ntk) {
      assert (!_p2cMap.size()); assert (!_c2pMap.size()); assert (!_pOutput.size());
      _ntk = elaborateNtk(_handler, targetNets, _p2cMap, _c2pMap, _netHash); assert (_ntk);
   }
   else if (targetNets.size()) {
      attachToNtk(_handler, _ntk, targetNets, _p2cMap, _c2pMap, _netHash); assert (_ntk);
   }
   // Reserve Mapping Tables if Needed
   if (!V3NtkHandler::P2CMapON()) _p2cMap.clear();
   if (!V3NtkHandler::C2PMapON()) _c2pMap.clear();
}

const V3NetId
V3NtkElaborate::elaborateLTLFormula(V3LTLFormula* const ltlFormula, const uint32_t& rootIndex) {
   assert (ltlFormula); assert (ltlFormula->isValid(rootIndex)); assert (isMutable());
   assert (_ntk); assert (this == ltlFormula->getHandler());
   if (ltlFormula->isLeaf(rootIndex)) {
      V3Formula* const formula = ltlFormula->getFormula(rootIndex); assert (formula);
      assert (ltlFormula->getHandler() == formula->getHandler());
      return dynamic_cast<V3BvNtk*>(ltlFormula->getHandler()->getNtk()) ? 
             elaborateBvFormula(formula, formula->getRoot(), _netHash) : 
             elaborateAigFormula(formula, formula->getRoot(), _netHash);
   }
   switch (ltlFormula->getOpType(rootIndex)) {
      case V3_LTL_T_G :  // Currently Support Single Layered LTL
         assert (1 == ltlFormula->getBranchSize(rootIndex));
         assert (ltlFormula->isLeaf(ltlFormula->getBranchIndex(rootIndex, 0)));
         return elaborateLTLFormula(ltlFormula, ltlFormula->getBranchIndex(rootIndex, 0));
      case V3_LTL_T_F :  // Currently Support Single Layered LTL
         assert (1 == ltlFormula->getBranchSize(rootIndex));
         assert (ltlFormula->isLeaf(ltlFormula->getBranchIndex(rootIndex, 0)));
         return elaborateLTLFormula(ltlFormula, ltlFormula->getBranchIndex(rootIndex, 0));
      default      : 
         Msg(MSG_WAR) << "Verification of General LTL Formula Has NOT Been Supported Yet !!" << endl;
   }
   return V3NetUD;
}

const V3NetId
V3NtkElaborate::elaborateL2S(const V3NetId& id) {
   assert (_ntk); assert (_p2cMap.size()); assert (id.id < _ntk->getNetSize());
   assert (isMutable()); assert (_shadow.size() == _handler->getNtk()->getLatchSize());
   // Create L2S Data Members if NOT Exists
   V3BvNtk* const bvNtk = dynamic_cast<V3BvNtk*>(_ntk);
   V3InputVec inputs; inputs.clear(); inputs.reserve(4);
   V3GateType type;
   if ((V3NetUD == _saved) || (V3NetUD == _1stSave)) {
      assert ((V3NetUD == _saved) && (V3NetUD == _1stSave));
      _saved = _ntk->createNet(1); _1stSave = _ntk->createNet(1);
      const V3NetId oracle = _ntk->createNet(1), inSaved = _ntk->createNet(1);
      inputs.push_back(~_saved); inputs.push_back(~oracle); _ntk->setInput(inSaved, inputs);
      _ntk->createGate((bvNtk ? BV_AND : AIG_NODE), inSaved); inputs.clear();
      inputs.push_back(~inSaved); inputs.push_back(0); _ntk->setInput(_saved, inputs);
      _ntk->createLatch(_saved); inputs.clear(); _ntk->createInput(oracle);
      // Set Inputs of _1stSave
      inputs.push_back(oracle); inputs.push_back(~_saved); _ntk->setInput(_1stSave, inputs);
      _ntk->createGate((bvNtk ? BV_AND : AIG_NODE), _1stSave); inputs.clear();
   }
   // Create Equivalence Logic and Shadow FF if NOT Exists
   for (uint32_t i = 0; i < _shadow.size(); ++i) {
      if (V3NetUD == _shadow[i] && V3NetUD != _p2cMap[_handler->getNtk()->getLatch(i).id]) {
         assert (V3_FF == _ntk->getGateType(_p2cMap[_handler->getNtk()->getLatch(i).id]));
         _shadow[i] = _ntk->createNet(_ntk->getNetWidth(_p2cMap[_handler->getNtk()->getLatch(i).id]));
         // Create Input MUX of Shadow FF
         V3NetId shadowMux = V3NetUD;
         if (bvNtk) {
            inputs.push_back(_p2cMap[_handler->getNtk()->getLatch(i).id]);
            inputs.push_back(_shadow[i]); inputs.push_back(~_1stSave);
            type = BV_MUX; shadowMux = elaborateBvGate(bvNtk, type, inputs, _netHash);
         }
         else {
            inputs.push_back(_1stSave); inputs.push_back(_p2cMap[_handler->getNtk()->getLatch(i).id]);
            inputs.push_back(~_1stSave); inputs.push_back(_shadow[i]);
            type = AIG_NODE; shadowMux = elaborateAigAndOrAndGate(_ntk, inputs, _netHash);
         }
         assert (V3NetUD != shadowMux); inputs.clear();
         // Create Shadow FF
         inputs.clear(); inputs.push_back(shadowMux);
         const uint32_t width = (bvNtk) ? _ntk->getNetWidth(_p2cMap[_handler->getNtk()->getLatch(i).id]) : 1;
         if (width > 1) {
            V3InputVec constInputs(1, bvNtk->hashV3ConstBitVec(v3Int2Str(width) + "'d0"));
            type = BV_CONST; inputs.push_back(elaborateBvGate(bvNtk, type, constInputs, _netHash));
         }
         else inputs.push_back(0);
         _ntk->setInput(_shadow[i], inputs); _ntk->createLatch(_shadow[i]); inputs.clear();
         // Create Equivalence Gate and Update _shadow
         if (bvNtk) {
            inputs.push_back(_p2cMap[_handler->getNtk()->getLatch(i).id]);
            inputs.push_back(_shadow[i]); type = BV_EQUALITY;
            _shadow[i] = elaborateBvGate(bvNtk, type, inputs, _netHash);
         }
         else {
            inputs.push_back(_p2cMap[_handler->getNtk()->getLatch(i).id]);
            inputs.push_back(_shadow[i]); inputs.push_back(~_shadow[i]);
            inputs.push_back(~_p2cMap[_handler->getNtk()->getLatch(i).id]);
            _shadow[i] = elaborateAigAndOrAndGate(_ntk, inputs, _netHash);
         }
         inputs.clear(); assert (V3NetUD != _shadow[i]);
      }
   }
   // Build LTL Formula Output Logic
   const V3NetId pLatch = _ntk->createNet(1), in_pLatch = _ntk->createNet(1);
   inputs.push_back(~pLatch); inputs.push_back(~id); _ntk->setInput(in_pLatch, inputs);
   _ntk->createGate((bvNtk ? BV_AND : AIG_NODE), in_pLatch); inputs.clear();
   inputs.push_back(~in_pLatch); inputs.push_back(0); _ntk->setInput(pLatch, inputs);
   _ntk->createLatch(pLatch); inputs.clear();
   // Build LTL Formula Output
   V3NetId pId = V3NetUD; inputs.push_back(_saved); inputs.push_back(~pLatch);
   if (bvNtk) { type = BV_AND; pId = elaborateBvGate(bvNtk, type, inputs, _netHash); }
   else { type = AIG_NODE; pId = elaborateAigGate(_ntk, type, inputs, _netHash); }
   assert (V3NetUD != pId); inputs.clear();
   // Build LTL Formula Output with Related Latches Only
   _ntk->newMiscData(); dfsRecurMarkFaninCone(_ntk, id);
   for (uint32_t i = 0; i < _shadow.size(); ++i) {
      if (V3NetUD == _shadow[i]) continue;
      if (!_ntk->isLatestMiscData(_p2cMap[_handler->getNtk()->getLatch(i).id])) continue;
      inputs.push_back(pId); inputs.push_back(_shadow[i]);
      if (bvNtk) { type = BV_AND; pId = elaborateBvGate(bvNtk, type, inputs, _netHash); }
      else { type = AIG_NODE; pId = elaborateAigGate(_ntk, type, inputs, _netHash); }
      assert (V3NetUD != pId); inputs.clear();
   }
   return pId;
}

#endif

