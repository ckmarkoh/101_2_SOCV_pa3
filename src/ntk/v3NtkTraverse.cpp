/****************************************************************************
  FileName     [ v3NtkTraverse.cpp ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ Generic Functions for V3 Ntk Traversal. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_TRAVERSE_C
#define V3_NTK_TRAVERSE_C

#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3StrUtil.h"

// General DFS Traversal Recursive Functions for V3 Ntk
void dfsGeneralOrder(V3Ntk* const ntk, const V3NetId& id, V3NetVec& orderMap) {
   assert (ntk); if (ntk->isLatestMiscData(id)) return;
   // Set Latest Misc Data
   ntk->setLatestMiscData(id);
   // Traverse Fanin Logics
   const V3GateType type = ntk->getGateType(id); assert (V3_XD > type); assert (V3_FF < type && AIG_FALSE != type);
   if (V3_MODULE == type) {
      assert (ntk->getInputNetSize(id) == 1); assert (ntk->getModule(id));
      const V3NetVec& inputs = ntk->getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) dfsGeneralOrder(ntk, inputs[i], orderMap);
   }
   else {
      const uint32_t inSize = (type == AIG_NODE || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 
                              (isV3ReducedType(type) || BV_SLICE == type) ? 1 : 0;
      for (uint32_t i = 0; i < inSize; ++i) dfsGeneralOrder(ntk, ntk->getInputNetId(id, i), orderMap);
   }
   orderMap.push_back(id);  // Record Order
}

void dfsSimulationOrder(V3Ntk* const ntk, const V3NetId& id, V3NetVec& orderMap) {
   assert (ntk); if (ntk->isLatestMiscData(id)) return;
   // Set Latest Misc Data
   ntk->setLatestMiscData(id);
   // Traverse Fanin Logics
   const V3GateType type = ntk->getGateType(id); assert (V3_XD > type);
   assert (V3_FF < type && AIG_FALSE != type && BV_CONST != type);
   if (V3_MODULE == type) {
      assert (ntk->getInputNetSize(id) == 1); assert (ntk->getModule(id));
      const V3NetVec& inputs = ntk->getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) dfsSimulationOrder(ntk, inputs[i], orderMap);
   }
   else {
      const uint32_t inSize = (type == AIG_NODE || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 1;
      for (uint32_t i = 0; i < inSize; ++i) dfsSimulationOrder(ntk, ntk->getInputNetId(id, i), orderMap);
   }
   orderMap.push_back(id);  // Record Order
}

// General DFS Fanin Cone Marking Functions for V3 Ntk
void dfsMarkFaninCone(V3Ntk* const ntk, const V3NetId& id) {
   assert (ntk); if (ntk->isLatestMiscData(id)) return;
   // Set Latest Misc Data
   ntk->setLatestMiscData(id);
   // Traverse Fanin Logics
   const V3GateType type = ntk->getGateType(id); assert (V3_XD > type);
   if (V3_FF >= type || AIG_FALSE == type || BV_CONST == type) return;
   if (V3_MODULE == type) {
      assert (ntk->getInputNetSize(id) == 1); assert (ntk->getModule(id));
      const V3NetVec& inputs = ntk->getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) dfsMarkFaninCone(ntk, inputs[i]);
   }
   else {
      const uint32_t inSize = (type == AIG_NODE || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 1;
      for (uint32_t i = 0; i < inSize; ++i) dfsMarkFaninCone(ntk, ntk->getInputNetId(id, i));
   }
}

void dfsMarkFaninCone(V3Ntk* const ntk, const V3NetId& id, const V3BoolVec& insensitiveList) {
   assert (ntk); assert (id.id < insensitiveList.size());
   if (ntk->isLatestMiscData(id) || insensitiveList[id.id]) return;
   // Set Latest Misc Data
   ntk->setLatestMiscData(id);
   // Traverse Fanin Logics
   const V3GateType type = ntk->getGateType(id); assert (V3_XD > type);
   if (V3_FF >= type || AIG_FALSE == type || BV_CONST == type) return;
   else if (V3_MODULE == type) {
      assert (ntk->getInputNetSize(id) == 1); assert (ntk->getModule(id));
      const V3NetVec& inputs = ntk->getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) dfsMarkFaninCone(ntk, inputs[i], insensitiveList);
   }
   else {
      const uint32_t inSize = (type == AIG_NODE || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 1;
      for (uint32_t i = 0; i < inSize; ++i) dfsMarkFaninCone(ntk, ntk->getInputNetId(id, i), insensitiveList);
   }
}

void dfsRecurMarkFaninCone(V3Ntk* const ntk, const V3NetId& id) {
   assert (ntk); if (ntk->isLatestMiscData(id)) return;
   // Set Latest Misc Data
   ntk->setLatestMiscData(id);
   // Traverse Fanin Logics
   const V3GateType type = ntk->getGateType(id); assert (V3_XD > type);
   if (V3_FF > type || AIG_FALSE == type || BV_CONST == type) return;
   else if (V3_MODULE == type) {
      assert (ntk->getInputNetSize(id) == 1); assert (ntk->getModule(id));
      const V3NetVec& inputs = ntk->getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) dfsRecurMarkFaninCone(ntk, inputs[i]);
   }
   else {
      const uint32_t inSize = (type == AIG_NODE || type == V3_FF || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 1;
      for (uint32_t i = 0; i < inSize; ++i) dfsRecurMarkFaninCone(ntk, ntk->getInputNetId(id, i));
   }
}

// General DFS Traversal Functions for V3 Ntk
const uint32_t dfsNtkForGeneralOrder(V3Ntk* const ntk, V3NetVec& orderMap, const bool& allNets) {
   assert (ntk); ntk->newMiscData();
   orderMap.clear(); orderMap.reserve(ntk->getNetSize());
   orderMap.push_back(V3NetId::makeNetId(0));  // AIG_FALSE or BV_CONST (1'b0)
   // (Pseudo) Primary Inputs
   for (uint32_t i = 0; i < ntk->getInputSize(); ++i) orderMap.push_back(ntk->getInput(i));
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) orderMap.push_back(ntk->getInout(i));
   for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) orderMap.push_back(ntk->getLatch(i));
   // Set Latest Misc Data
   for (uint32_t i = 0; i < orderMap.size(); ++i) ntk->setLatestMiscData(orderMap[i]);
   // Pseudo Primary Input Initial State Logics
   for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) 
      dfsGeneralOrder(ntk, ntk->getInputNetId(ntk->getLatch(i), 1), orderMap);
   // Record End of Initial Logic if Needed  (e.g. simulator)
   const uint32_t initEndIndex = orderMap.size();
   // (Pseudo) Primary Output Fanin Logics
   for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) 
      dfsGeneralOrder(ntk, ntk->getInputNetId(ntk->getLatch(i), 0), orderMap);
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) 
      dfsGeneralOrder(ntk, ntk->getInputNetId(ntk->getInout(i), 0), orderMap);
   for (uint32_t i = 0; i < ntk->getOutputSize(); ++i)
      dfsGeneralOrder(ntk, ntk->getOutput(i), orderMap);
   assert (orderMap.size() <= ntk->getNetSize()); return initEndIndex;
}

const uint32_t dfsNtkForSimulationOrder(V3Ntk* const ntk, V3NetVec& orderMap, const bool& allNets) {
   assert (ntk); ntk->newMiscData();
   orderMap.clear(); orderMap.reserve(ntk->getNetSize());
   // Constants
   for (uint32_t i = 0; i < ntk->getConstSize(); ++i) orderMap.push_back(ntk->getConst(i));
   // (Pseudo) Primary Inputs
   for (uint32_t i = 0; i < ntk->getInputSize(); ++i) orderMap.push_back(ntk->getInput(i));
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) orderMap.push_back(ntk->getInout(i));
   for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) orderMap.push_back(ntk->getLatch(i));
   // Set Latest Misc Data
   for (uint32_t i = 0; i < orderMap.size(); ++i) ntk->setLatestMiscData(orderMap[i]);
   // Pseudo Primary Input Initial State Logics
   for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) 
      dfsSimulationOrder(ntk, ntk->getInputNetId(ntk->getLatch(i), 1), orderMap);
   // Record End of Initial Logic if Needed  (e.g. simulator)
   const uint32_t initEndIndex = orderMap.size();
   // (Pseudo) Primary Output Fanin Logics
   for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) 
      dfsSimulationOrder(ntk, ntk->getInputNetId(ntk->getLatch(i), 0), orderMap);
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) 
      dfsSimulationOrder(ntk, ntk->getInputNetId(ntk->getInout(i), 0), orderMap);
   for (uint32_t i = 0; i < ntk->getOutputSize(); ++i)
      dfsSimulationOrder(ntk, ntk->getOutput(i), orderMap);
   assert (orderMap.size() <= ntk->getNetSize()); return initEndIndex;
}

const uint32_t dfsNtkForReductionOrder(V3Ntk* const ntk, V3NetVec& orderMap, const V3NetVec& targetNets, const bool& reduceLatch) {
   assert (ntk); assert (targetNets.size()); ntk->newMiscData();
   orderMap.clear(); orderMap.reserve(ntk->getNetSize());
   orderMap.push_back(V3NetId::makeNetId(0));  // AIG_FALSE or BV_CONST (1'b0)
   // Mark Fanin Cone of targetNets Recursively  (Through FF Boundaries)
   if (targetNets.size()) for (uint32_t i = 0; i < targetNets.size(); ++i) dfsRecurMarkFaninCone(ntk, targetNets[i]);
   else for (uint32_t i = 0; i < ntk->getOutputSize(); ++i) dfsRecurMarkFaninCone(ntk, ntk->getOutput(i));
   // (Pseudo) Primary Inputs
   V3NetVec latchList; latchList.clear(); latchList.reserve(ntk->getLatchSize());
   for (uint32_t i = 0; i < ntk->getInputSize(); ++i) orderMap.push_back(ntk->getInput(i));
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) orderMap.push_back(ntk->getInout(i));
   for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) 
      if (!reduceLatch || ntk->isLatestMiscData(ntk->getLatch(i))) {
         orderMap.push_back(ntk->getLatch(i)); latchList.push_back(ntk->getLatch(i)); }
   // DFS Traverse Marked Nets
   ntk->newMiscData();
   // Set Latest Misc Data
   for (uint32_t i = 0; i < orderMap.size(); ++i) ntk->setLatestMiscData(orderMap[i]);
   // Pseudo Primary Input Initial State Logics
   for (uint32_t i = 0; i < latchList.size(); ++i) 
      dfsGeneralOrder(ntk, ntk->getInputNetId(latchList[i], 1), orderMap);
   // Record End of Initial Logic if Needed  (e.g. simulator)
   const uint32_t initEndIndex = orderMap.size();
   // (Pseudo) Primary Output Fanin Logics
   for (uint32_t i = 0; i < latchList.size(); ++i) 
      dfsGeneralOrder(ntk, ntk->getInputNetId(latchList[i], 0), orderMap);
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) 
      dfsGeneralOrder(ntk, ntk->getInputNetId(ntk->getInout(i), 0), orderMap);
   if (targetNets.size()) for (uint32_t i = 0; i < targetNets.size(); ++i) dfsGeneralOrder(ntk, targetNets[i], orderMap);
   else for (uint32_t i = 0; i < ntk->getOutputSize(); ++i) dfsGeneralOrder(ntk, ntk->getOutput(i), orderMap);
   assert (orderMap.size() <= ntk->getNetSize()); return initEndIndex;
}

// General BFS Fanin Cone Indexing Recursive Functions for V3 Ntk
void bfsIndexFaninConeFF(V3Ntk* const ntk, V3NetVec& ffList, const V3NetId& id) {
   assert (ntk); if (ntk->isLatestMiscData(id)) return;
   // Set Latest Misc Data
   ntk->setLatestMiscData(id);
   // Traverse Fanin Logics
   const V3GateType type = ntk->getGateType(id); assert (V3_XD > type);
   if (V3_MODULE == type) {
      assert (ntk->getInputNetSize(id) == 1); assert (ntk->getModule(id));
      const V3NetVec& inputs = ntk->getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) bfsIndexFaninConeFF(ntk, ffList, inputs[i]);
   }
   else if (V3_FF == type) ffList.push_back(id);
   else {
      const uint32_t inSize = (type == AIG_NODE || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 
                              (isV3ReducedType(type) || BV_SLICE == type) ? 1 : 0;
      for (uint32_t i = 0; i < inSize; ++i) bfsIndexFaninConeFF(ntk, ffList, ntk->getInputNetId(id, i));
   }
}

// General BFS Fanin Cone Indexing Functions for V3 Ntk
void bfsIndexFaninConeFF(V3Ntk* const ntk, V3NetVec& ffList, const V3NetVec& sourceNets) {
   assert (ntk); ntk->newMiscData(); ffList.clear();
   V3NetVec source = sourceNets; uint32_t end = 0;
   while (true) {
      for (uint32_t i = 0; i < source.size(); ++i) bfsIndexFaninConeFF(ntk, ffList, source[i]);
      if (end == ffList.size()) break; assert (end < ffList.size());
      source.clear(); source.reserve(ffList.size() - end);
      while (end != ffList.size()) {
         source.push_back(ntk->getInputNetId(ffList[end], 0));
         source.push_back(ntk->getInputNetId(ffList[end], 1));
         ++end;
      }
   }
}

#endif

