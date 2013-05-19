/****************************************************************************
  FileName     [ v3NtkDuplicate.cpp ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ Generic Functions for V3 Ntk Duplication. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_DUPLICATE_C
#define V3_NTK_DUPLICATE_C

#include "v3Msg.h"
#include "v3NtkHash.h"
#include "v3NtkUtil.h"

// General Duplication Helper Functions for V3 Ntk
void duplicateGeneralInit(V3Ntk* const ntk, V3NetVec& p2cMap, V3NetVec& orderMap, const bool& reduceON) {
   assert (ntk);
   // Compute General DFS Order for Duplication
   dfsNtkForGeneralOrder(ntk, orderMap, !reduceON);
   // Initialize Mapper From Parent (Index) to Current (V3NetId) Ntk
   p2cMap.clear(); p2cMap.reserve(ntk->getNetSize()); p2cMap.push_back(V3NetId::makeNetId(0));
   for (uint32_t i = 1, j = ntk->getNetSize(); i < j; ++i) p2cMap.push_back(V3NetUD);
}

void duplicateReductionInit(V3Ntk* const ntk, const V3NetVec& targetNets, V3NetVec& p2cMap, V3NetVec& orderMap) {
   assert (ntk); assert (targetNets.size());
   // Compute Reduced DFS Order for Duplication
   dfsNtkForReductionOrder(ntk, orderMap, targetNets);
   // Initialize Mapper From Parent (Index) to Current (V3NetId) Ntk
   p2cMap.clear(); p2cMap.reserve(ntk->getNetSize()); p2cMap.push_back(V3NetId::makeNetId(0));
   for (uint32_t i = 1, j = ntk->getNetSize(); i < j; ++i) p2cMap.push_back(V3NetUD);
}

const V3NetId duplicateNet(V3Ntk* const ntk, V3Ntk* const pNtk, const V3NetId& pId, V3NetVec& p2cMap, V3NetVec& c2pMap) {
   assert (ntk); assert (pNtk); assert (pId.id < pNtk->getNetSize());
   assert (pId.id < p2cMap.size()); assert (V3NetUD == p2cMap[pId.id]);
   assert (c2pMap.size() == ntk->getNetSize()); c2pMap.push_back(pId);
   p2cMap[pId.id] = ntk->createNet(pNtk->getNetWidth(pId));
   assert (V3NetUD != p2cMap[pId.id]); return p2cMap[pId.id];
}

void duplicateInputNets(V3NtkHandler* const handler, V3Ntk* const ntk, V3NetVec& p2cMap, V3NetVec& c2pMap, V3NetVec& orderMap) {
   assert (handler); assert (ntk); V3Ntk* const pNtk = handler->getNtk(); assert (pNtk);
   assert ((bool)(dynamic_cast<V3BvNtk*>(pNtk)) == (bool)(dynamic_cast<V3BvNtk*>(ntk)));
   assert (p2cMap.size() == pNtk->getNetSize()); assert (c2pMap.size());
   // Create IO Nets
   uint32_t i = 1;
   for (uint32_t j = i + pNtk->getInputSize(); i < j; ++i) {
      assert (i < orderMap.size()); assert (V3_PI == pNtk->getGateType(orderMap[i]));
      assert (V3NetUD == p2cMap[orderMap[i].id]); duplicateNet(ntk, pNtk, orderMap[i], p2cMap, c2pMap);
   }
   for (uint32_t j = i + pNtk->getInoutSize(); i < j; ++i) {
      assert (i < orderMap.size()); assert (V3_PIO == pNtk->getGateType(orderMap[i]));
      assert (V3NetUD == p2cMap[orderMap[i].id]); duplicateNet(ntk, pNtk, orderMap[i], p2cMap, c2pMap);
   }
}

void duplicateInputGates(V3NtkHandler* const handler, V3Ntk* const ntk, V3NetVec& p2cMap, V3NetVec& c2pMap) {
   assert (handler); assert (ntk); V3Ntk* const pNtk = handler->getNtk(); assert (pNtk);
   assert ((bool)(dynamic_cast<V3BvNtk*>(pNtk)) == (bool)(dynamic_cast<V3BvNtk*>(ntk)));
   assert (p2cMap.size() == pNtk->getNetSize()); assert (c2pMap.size());
   // Construct Primary Input / Inout
   for (uint32_t i = 0; i < pNtk->getInputSize(); ++i) {
      assert (V3_PI == pNtk->getGateType(pNtk->getInput(i)));
      assert (V3NetUD != p2cMap[pNtk->getInput(i).id]);
      ntk->createInput(p2cMap[pNtk->getInput(i).id]);
   }
   V3InputVec inputs; inputs.clear(); inputs.reserve(2); V3NetId id;
   for (uint32_t i = 0; i < pNtk->getInoutSize(); ++i) {
      assert (V3_PIO == pNtk->getGateType(pNtk->getInout(i)));
      assert (V3NetUD != p2cMap[pNtk->getInout(i).id]);
      id = pNtk->getInputNetId(pNtk->getInout(i), 0); inputs.push_back(V3NetId::makeNetId(p2cMap[id.id].id, id.cp));
      id = pNtk->getInputNetId(pNtk->getInout(i), 1); inputs.push_back(V3NetId::makeNetId(p2cMap[id.id].id, id.cp));
      id = p2cMap[pNtk->getInout(i).id]; ntk->setInput(id, inputs); inputs.clear(); ntk->createInout(id);
   }
   // Construct Clock
   if (V3NetUD != pNtk->getClock()) {
      if (V3NetUD == p2cMap[pNtk->getClock().id]) {
         p2cMap[pNtk->getClock().id] = ntk->createNet();
         c2pMap.push_back(pNtk->getClock());
      }
      id = p2cMap[pNtk->getClock().id]; assert (id.id < ntk->getNetSize());
      if (pNtk->getClock().cp) id = ~id; ntk->createClock(id);
   }
}

void duplicateNtk(V3NtkHandler* const handler, V3Ntk* const ntk, V3NetVec& p2cMap, V3NetVec& c2pMap, V3NetVec& orderMap, V3PortableType& netHash, V3NtkHierInfo& hierInfo, const uint32_t& flattenLevel) {
   assert (handler); assert (ntk); V3Ntk* const pNtk = handler->getNtk(); assert (pNtk);
   assert ((bool)(dynamic_cast<V3BvNtk*>(pNtk)) == (bool)(dynamic_cast<V3BvNtk*>(ntk)));
   const uint32_t ntkIndex = hierInfo.getNtkSize() - 1;
   // Create Latch Nets
   uint32_t i = 1 + pNtk->getInputSize() + pNtk->getInoutSize();
   for (; i < orderMap.size(); ++i) {
      assert (i < orderMap.size());
      if (V3_FF != pNtk->getGateType(orderMap[i])) break;
      if (V3NetUD != p2cMap[orderMap[i].id]) continue;
      duplicateNet(ntk, pNtk, orderMap[i], p2cMap, c2pMap);
   }
   // Module Instance Data Storage
   V3BoolVec moduleList(pNtk->getModuleSize(), false);
   V3NetVec orderMap2, cInputs, cOutputs;
   // Net and Gate Type Data Storage
   V3InputVec inputs; inputs.clear(); inputs.reserve(3);
   V3GateType type; uint32_t inSize; V3NetId id;
   // Construct Gates in General Order (All outputs for a module instance will be created simply)
   V3BvNtk* const bvNtk = dynamic_cast<V3BvNtk*>(ntk);
   for (; i < orderMap.size(); ++i) {
      if (V3NetUD != p2cMap[orderMap[i].id]) {  // Created Module Instance Outputs
         assert (V3_MODULE == pNtk->getGateType(orderMap[i]));
         assert (moduleList[V3NetType(pNtk->getInputNetId(orderMap[i], 0)).value]);
         continue;
      }
      type = pNtk->getGateType(orderMap[i]); assert (V3_XD > type);
      if (V3_MODULE == type) {
         // Create All Outputs for the Module
         const uint32_t pModuleIndex = V3NetType(pNtk->getInputNetId(orderMap[i], 0)).value;
         V3NtkHandler* const pModuleHandler = pNtk->getModule(pModuleIndex)->getNtkRef();
         V3Ntk* const pModuleNtk = pModuleHandler->getNtk(); assert (pModuleNtk);
         if (!moduleList[pModuleIndex]) {
            if (flattenLevel) {
               if (pNtk->getModule(pModuleIndex)->isNtkRefBlackBoxed()) {
                  Msg(MSG_WAR) << "Omit Blackboxed Module \"" << pModuleHandler->getNtkName() << "\""
                               << " in Ntk Flatten !!" << endl; continue;
               }
               else {
                  // Record Bound of Reference Id for Current Ntk for Bookkeeping
                  hierInfo.pushRefId(c2pMap.size(), ntkIndex);
                  hierInfo.pushNtk(pModuleHandler, ntkIndex);
                  V3NetVec& p2cMap2 = hierInfo.getMap(hierInfo.getNtkSize() - 1);
                  // Construct Mapping Tables
                  duplicateGeneralInit(pModuleHandler->getNtk(), p2cMap2, orderMap2, V3NtkHandler::reduceON());
                  // Set Inputs for the Module Instance
                  const V3NetVec& pInputs = pNtk->getModule(pModuleIndex)->getInputList();
                  assert (pModuleNtk->getInputSize() == pInputs.size());
                  for (uint32_t j = 0; j < pInputs.size(); ++j) {
                     assert (V3NetUD != p2cMap[pInputs[j].id]); assert (!pModuleNtk->getInput(j).cp);
                     assert (V3NetUD == p2cMap2[pModuleNtk->getInput(j).id]);
                     p2cMap2[pModuleNtk->getInput(j).id] = p2cMap[pInputs[j].id];
                  }
                  // Recursively Duplicate Module Instance
                  duplicateNtk(pModuleHandler, ntk, p2cMap2, c2pMap, orderMap2, netHash, hierInfo, flattenLevel - 1);
                  // Set Outputs of Module Instance to p2cMap
                  const V3NetVec& pOutputs = pNtk->getModule(pModuleIndex)->getOutputList();
                  assert (pModuleNtk->getOutputSize() == pOutputs.size());
                  for (uint32_t j = 0; j < pOutputs.size(); ++j) {
                     assert (V3NetUD == p2cMap[pOutputs[j].id]);
                     id = p2cMap2[pModuleNtk->getOutput(j).id]; assert (id.id < c2pMap.size());
                     p2cMap[pOutputs[j].id] = V3NetId::makeNetId(id.id, 
                                                                 pOutputs[j].cp ^ pModuleNtk->getOutput(j).cp ^ id.cp);
                  }
               }
            }
            else {
               const V3NetVec& pOutputs = pNtk->getModule(pModuleIndex)->getOutputList();
               for (uint32_t j = 0; j < pOutputs.size(); ++j) {
                  assert (V3NetUD == p2cMap[pOutputs[j].id]);
                  id = duplicateNet(ntk, pNtk, pOutputs[j], p2cMap, c2pMap);
                  assert (id == p2cMap[pOutputs[j].id]); cOutputs.push_back(id);
               }
               const V3NetVec& pInputs = pNtk->getModule(pModuleIndex)->getInputList();
               for (uint32_t j = 0; j < pInputs.size(); ++j) {
                  assert (V3NetUD != p2cMap[pInputs[j].id]);
                  cInputs.push_back(p2cMap[pInputs[j].id]);
               }
               createModule(ntk, cInputs, cOutputs, pNtk->getModule(pModuleIndex)->getNtkRef(), 
                            pNtk->getModule(pModuleIndex)->isNtkRefBlackBoxed());
               cInputs.clear(); cOutputs.clear();
            }
            moduleList[pModuleIndex] = true;
         }
      }
      else if (V3_PI == type) duplicateNet(ntk, pNtk, orderMap[i], p2cMap, c2pMap);
      else if (bvNtk) {
         if (BV_SLICE == type) {
            id = pNtk->getInputNetId(orderMap[i], 0); assert (V3NetUD != id);
            id = V3NetId::makeNetId(p2cMap[id.id].id, id.cp); inputs.push_back(id);
            inputs.push_back(pNtk->getInputNetId(orderMap[i], 1));
         }
         else if (BV_CONST == type) inputs.push_back(pNtk->getInputNetId(orderMap[i], 0));
         else {
            inSize = (isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 1;
            for (uint32_t j = 0; j < inSize; ++j) {
               id = pNtk->getInputNetId(orderMap[i], j); assert (V3NetUD != id);
               id = V3NetId::makeNetId(p2cMap[id.id].id, id.cp); inputs.push_back(id);
            }
         }
         id = p2cMap[orderMap[i].id] = elaborateBvGate(bvNtk, type, inputs, netHash);
         assert (pNtk->getNetWidth(orderMap[i]) == bvNtk->getNetWidth(id)); inputs.clear();
         if (c2pMap.size() <= id.id) {
            while (c2pMap.size() < id.id) c2pMap.push_back(V3NetUD);
            c2pMap.push_back(V3NetId::makeNetId(orderMap[i].id, id.cp));
            assert (c2pMap.size() == ntk->getNetSize());
         }
      }
      else {
         if (AIG_NODE == type) {
            id = pNtk->getInputNetId(orderMap[i], 0); assert (V3NetUD != id);
            id = V3NetId::makeNetId(p2cMap[id.id].id, id.cp); inputs.push_back(id);
            id = pNtk->getInputNetId(orderMap[i], 1); assert (V3NetUD != id);
            id = V3NetId::makeNetId(p2cMap[id.id].id, id.cp); inputs.push_back(id);
         }
         id = p2cMap[orderMap[i].id] = elaborateAigGate(ntk, type, inputs, netHash); inputs.clear();
         if (c2pMap.size() <= id.id) {
            while (c2pMap.size() < id.id) c2pMap.push_back(V3NetUD);
            c2pMap.push_back(V3NetId::makeNetId(orderMap[i].id, id.cp));
            assert (c2pMap.size() == ntk->getNetSize());
         }
      }
      // Assert All Parent Nets are Duplicated
      assert (V3NetUD != p2cMap[orderMap[i].id]);
   }
   // Construct DFF
   for (i = 0; i < pNtk->getLatchSize(); ++i) {
      assert (V3_FF == pNtk->getGateType(pNtk->getLatch(i)));
      if (V3NetUD == p2cMap[pNtk->getLatch(i).id]) continue;
      if (V3_FF == ntk->getGateType(p2cMap[pNtk->getLatch(i).id])) continue;
      assert (V3_PI == ntk->getGateType(p2cMap[pNtk->getLatch(i).id]));
      id = pNtk->getInputNetId(pNtk->getLatch(i), 0); inputs.push_back(V3NetId::makeNetId(p2cMap[id.id].id, id.cp));
      id = pNtk->getInputNetId(pNtk->getLatch(i), 1); inputs.push_back(V3NetId::makeNetId(p2cMap[id.id].id, id.cp));
      id = p2cMap[pNtk->getLatch(i).id]; ntk->setInput(id, inputs); inputs.clear(); ntk->createLatch(id);
   }
   // Set Hierarchical Info
   hierInfo.pushRefId(c2pMap.size(), ntkIndex);
   assert (ntkIndex < hierInfo.getNtkSize());
   assert (handler == hierInfo.getHandler(ntkIndex));
}

// General Elaboration Helper Functions for V3 Ntk
const bool rewriteAigGate(V3AigNtk* const ntk, const V3GateType& type, V3InputVec& inputs) {
   assert (ntk);
   // Will be released in the future
   return false;
}

const bool rewriteBvGate(V3BvNtk* const ntk, const V3GateType& type, V3InputVec& inputs) {
   assert (ntk); assert (V3_XD > type); assert (AIG_FALSE < type);
   // Will be released in the future
   return false;
}

const V3NetId strashAigGate(V3AigNtk* const ntk, const V3GateType& type, const V3InputVec& inputs, V3PortableType& netHash) {
   assert (ntk); if (AIG_FALSE == type) return V3NetUD;
   assert (AIG_NODE == type); assert (2 == inputs.size());
   // Check Hash Table
   V3UI64Hash* hashTable = 0;
   const uint64_t hashKey = V3UnorderedNetType2HashKey(inputs[0], inputs[1]);
   if (netHash.existList(AIG_NODE)) {
      hashTable = (V3UI64Hash*)(netHash.getList(AIG_NODE)); assert (hashTable);
      V3UI64Hash::const_iterator it = hashTable->find(hashKey);
      if (hashTable->end() != it) return it->second;
   }
   else {
      hashTable = new V3UI64Hash(); assert (hashTable);
      netHash.setList(AIG_NODE, (size_t)hashTable);
   }
   // Create Net and Push into Hash Table
   const V3NetId id = ntk->createNet(1);
   hashTable->insert(make_pair(hashKey, id)); return id;
}

const V3NetId strashBvGate(V3BvNtk* const ntk, const V3GateType& type, const V3InputVec& inputs, V3PortableType& netHash) {
   assert (ntk); assert (V3_XD > type); assert (AIG_FALSE < type);
   // Check Hash Table According to Gate Types
   if (isV3PairType(type)) {
      V3UI64Hash* hashTable = 0; assert (2 == inputs.size());
      const uint64_t hashKey = isV3ExchangableType(type) ? V3UnorderedNetType2HashKey(inputs[0], inputs[1]) 
                                                         : V3OrderedNetType2HashKey(inputs[0], inputs[1]);
      if (netHash.existList(type)) {
         hashTable = (V3UI64Hash*)(netHash.getList(type)); assert (hashTable);
         V3UI64Hash::const_iterator it = hashTable->find(hashKey);
         if (hashTable->end() != it) return it->second;
      }
      else {
         hashTable = new V3UI64Hash(); assert (hashTable);
         netHash.setList(type, (size_t)hashTable);
      }
      // Create Net and Push into Hash Table
      const uint32_t width = (BV_MERGE == type) ? (ntk->getNetWidth(inputs[0].id) + ntk->getNetWidth(inputs[1].id)) : 
                             (type < BV_EQUALITY) ? ntk->getNetWidth(inputs[0].id) : 1;
      const V3NetId id = ntk->createNet(width); hashTable->insert(make_pair(hashKey, id)); return id;
   }
   else if (isV3ReducedType(type)) {
      V3UI32Hash* hashTable = 0; assert (1 == inputs.size());
      const uint32_t hashKey = V3NetType2HashKey(inputs[0]);
      if (netHash.existList(type)) {
         hashTable = (V3UI32Hash*)(netHash.getList(type)); assert (hashTable);
         V3UI32Hash::const_iterator it = hashTable->find(hashKey);
         if (hashTable->end() != it) return it->second;
      }
      else {
         hashTable = new V3UI32Hash(); assert (hashTable);
         netHash.setList(type, (size_t)hashTable);
      }
      // Create Net and Push into Hash Table
      const V3NetId id = ntk->createNet(1);
      hashTable->insert(make_pair(hashKey, id)); return id;
   }
   else if (BV_MUX == type) {
      V3MuxHash* hashTable = 0; assert (3 == inputs.size());
      const V3MuxKey hashKey = V3MuxNetType2HashKey(inputs[0], inputs[1], inputs[2]);
      if (netHash.existList(BV_MUX)) {
         hashTable = (V3MuxHash*)(netHash.getList(BV_MUX)); assert (hashTable);
         V3MuxHash::const_iterator it = hashTable->find(hashKey);
         if (hashTable->end() != it) return it->second;
      }
      else {
         hashTable = new V3MuxHash(); assert (hashTable);
         netHash.setList(BV_MUX, (size_t)hashTable);
      }
      // Create Net and Push into Hash Table
      const V3NetId id = ntk->createNet(ntk->getNetWidth(inputs[0].id));
      hashTable->insert(make_pair(hashKey, id)); return id;
   }
   else if (BV_SLICE == type) {
      V3UI64Hash* hashTable = 0; assert (2 == inputs.size());
      const uint64_t hashKey = V3OrderedNetType2HashKey(inputs[0], inputs[1]);
      if (netHash.existList(BV_SLICE)) {
         hashTable = (V3UI64Hash*)(netHash.getList(BV_SLICE)); assert (hashTable);
         V3UI64Hash::const_iterator it = hashTable->find(hashKey);
         if (hashTable->end() != it) return it->second;
      }
      else {
         hashTable = new V3UI64Hash(); assert (hashTable);
         netHash.setList(BV_SLICE, (size_t)hashTable);
      }
      // Create Net and Push into Hash Table
      const V3NetId id = ntk->createNet(ntk->getSliceWidth(inputs[1].value));
      hashTable->insert(make_pair(hashKey, id)); return id;
   }
   else {
      V3UI32Hash* hashTable = 0; assert (BV_CONST == type); assert (1 == inputs.size());
      const uint32_t hashKey = V3NetType2HashKey(inputs[0]);
      if (!netHash.existList(BV_CONST)) {
         hashTable = new V3UI32Hash(); assert (hashTable);
         netHash.setList(BV_CONST, (size_t)hashTable);
         hashTable->insert(make_pair(V3NetType2HashKey(0), V3NetId::makeNetId(0)));
      }
      else {
         hashTable = (V3UI32Hash*)(netHash.getList(BV_CONST));
         assert (hashTable);
      }
      V3UI32Hash::const_iterator it = hashTable->find(hashKey);
      if (hashTable->end() != it) return it->second;
      // Create Net and Push into Hash Table
      const V3NetId id = ntk->createNet(ntk->getConstWidth(inputs[0].value));
      hashTable->insert(make_pair(hashKey, id)); return id;
   }
   return V3NetUD;
}

// General Gate Type Elaboration Functions for V3 Ntk
const V3NetId elaborateAigGate(V3AigNtk* const ntk, const V3GateType& type, V3InputVec& inputs, V3PortableType& netHash) {
   assert (ntk); assert (AIG_NODE == type || AIG_FALSE == type);
   const uint32_t netSize = ntk->getNetSize(); V3NetId id = V3NetUD;
   const bool cpId = (V3NtkHandler::rewriteON()) ? rewriteAigGate(ntk, type, inputs) : false;
   if (V3NtkHandler::strashON()) id = strashAigGate(ntk, type, inputs, netHash);
   if (netSize <= id.id) {
      if (AIG_NODE == type) {
         assert (2 == inputs.size());
         if (V3NetUD == id) id = ntk->createNet(1);
         ntk->setInput(id, inputs); ntk->createGate(type, id);
      }
      else {
         assert (AIG_NODE == type); assert (!inputs.size());
         if (V3NetUD == id) id = ntk->createNet(1);
         ntk->createConst(id);
      }
   }
   assert (type == ntk->getGateType(id)); assert (!id.cp); return (cpId ? ~id : id);
}

const V3NetId elaborateBvGate(V3BvNtk* const ntk, const V3GateType& type, V3InputVec& inputs, V3PortableType& netHash) {
   assert (ntk); assert (V3_XD > type); assert (AIG_FALSE < type);
   const uint32_t netSize = ntk->getNetSize(); V3NetId id = V3NetUD;
   const bool cpId = (V3NtkHandler::rewriteON()) ? rewriteBvGate(ntk, type, inputs) : false;
   if (V3NtkHandler::strashON()) id = strashBvGate(ntk, type, inputs, netHash);
   if (netSize <= id.id) {
      if (isV3PairType(type)) {
         assert (2 == inputs.size());
         if (V3NetUD == id) 
            id = ntk->createNet((BV_MERGE == type) ? (ntk->getNetWidth(inputs[0].id) + ntk->getNetWidth(inputs[1].id)) : 
                                (type < BV_EQUALITY) ? ntk->getNetWidth(inputs[0].id) : 1);
         ntk->setInput(id, inputs); ntk->createGate(type, id);
      }
      else if (isV3ReducedType(type)) {
         assert (1 == inputs.size());
         if (V3NetUD == id) id = ntk->createNet(1);
         ntk->setInput(id, inputs); ntk->createGate(type, id);
      }
      else if (BV_MUX == type) {
         assert (3 == inputs.size());
         if (V3NetUD == id) id = ntk->createNet(ntk->getNetWidth(inputs[0].id));
         ntk->setInput(id, inputs); ntk->createGate(type, id);
      }
      else if (BV_SLICE == type) {
         assert (2 == inputs.size());
         if (V3NetUD == id) id = ntk->createNet(ntk->getSliceWidth(inputs[1].value));
         ntk->setInput(id, inputs); ntk->createGate(type, id);
      }
      else {
         assert (BV_CONST == type); assert (1 == inputs.size());
         if (V3NetUD == id) id = ntk->createNet(ntk->getConstWidth(inputs[0].value));
         ntk->setInput(id, inputs); ntk->createConst(id);
      }
   }
   assert (type == ntk->getGateType(id)); assert (!id.cp); return (cpId ? ~id : id);
}

const V3NetId elaborateAigAndOrAndGate(V3AigNtk* const ntk, V3InputVec& inputs, V3PortableType& netHash) {
   assert (ntk); assert (4 == inputs.size());
   V3InputVec andInputs(2); V3GateType type; V3NetId id1, id2;
   andInputs[0] = inputs[0]; andInputs[1] = inputs[1];
   type = AIG_NODE; id1 = elaborateAigGate(ntk, type, andInputs, netHash);
   andInputs[0] = inputs[2]; andInputs[1] = inputs[3];
   type = AIG_NODE; id2 = elaborateAigGate(ntk, type, andInputs, netHash);
   andInputs[0] = ~id1; andInputs[1] = ~id2; type = AIG_NODE;
   return ~elaborateAigGate(ntk, type, andInputs, netHash);
}

// General Duplication Functions for V3 Ntk  (Note that PO will NOT be created !!)
V3Ntk* const duplicateNtk(V3NtkHandler* const handler, V3NetVec& p2cMap, V3NetVec& c2pMap) {
   assert (handler); assert (handler->getNtk());
   // Initialize Mapping Tables
   V3NetVec orderMap; c2pMap.clear();
   duplicateGeneralInit(handler->getNtk(), p2cMap, orderMap, V3NtkHandler::reduceON());
   assert (orderMap.size()); assert (!orderMap[0].id); c2pMap.push_back(orderMap[0]);
   // Initialize Net Hash Tables
   const bool isBvNtk = dynamic_cast<V3BvNtk*>(handler->getNtk());
   V3PortableType netHash = V3PortableType((isBvNtk ? (V3_XD - BV_RED_AND) : 1), (isBvNtk ? BV_RED_AND : AIG_NODE));
   // Create Duplicate Ntk
   V3Ntk* const ntk = createV3Ntk(dynamic_cast<V3BvNtk*>(handler->getNtk())); assert (ntk);
   duplicateInputNets(handler, ntk, p2cMap, c2pMap, orderMap);
   V3NtkHierInfo hierInfo; hierInfo.clear(); hierInfo.pushNtk(handler, 0);
   duplicateNtk(handler, ntk, p2cMap, c2pMap, orderMap, netHash, hierInfo, 0);
   duplicateInputGates(handler, ntk, p2cMap, c2pMap); return ntk;
}

V3Ntk* const elaborateNtk(V3NtkHandler* const handler, const V3NetVec& targetNets, V3NetVec& p2cMap, V3NetVec& c2pMap, V3PortableType& netHash) {
   assert (handler); assert (handler->getNtk());
   // Initialize Mapping Tables
   V3NetVec orderMap; c2pMap.clear();
   duplicateReductionInit(handler->getNtk(), targetNets, p2cMap, orderMap);
   assert (orderMap.size()); assert (!orderMap[0].id); c2pMap.push_back(orderMap[0]);
   // Initialize Net Hash Tables
   const bool isBvNtk = dynamic_cast<V3BvNtk*>(handler->getNtk());
   netHash = V3PortableType((isBvNtk ? (V3_XD - BV_RED_AND) : 1), (isBvNtk ? BV_RED_AND : 0));
   // Create Duplicate Ntk
   V3Ntk* const ntk = createV3Ntk(dynamic_cast<V3BvNtk*>(handler->getNtk())); assert (ntk);
   duplicateInputNets(handler, ntk, p2cMap, c2pMap, orderMap);
   V3NtkHierInfo hierInfo; hierInfo.clear(); hierInfo.pushNtk(handler, 0);
   duplicateNtk(handler, ntk, p2cMap, c2pMap, orderMap, netHash, hierInfo, 0);
   duplicateInputGates(handler, ntk, p2cMap, c2pMap); return ntk;
}

V3Ntk* const flattenNtk(V3NtkHandler* const handler, V3NetVec& c2pMap, V3NtkHierInfo& hierInfo, const uint32_t& flattenLevel) {
   assert (handler); assert (handler->getNtk()); assert (flattenLevel); hierInfo.clear();
   // Push Top Ntk into the Hierarchy
   hierInfo.clear(); hierInfo.pushNtk(handler, 0);
   // Initialize Mapping Tables
   V3NetVec orderMap; c2pMap.clear();
   duplicateGeneralInit(handler->getNtk(), hierInfo.getMap(0), orderMap, V3NtkHandler::reduceON());
   assert (orderMap.size()); assert (!orderMap[0].id); c2pMap.push_back(orderMap[0]);
   // Initialize Net Hash Tables
   const bool isBvNtk = dynamic_cast<V3BvNtk*>(handler->getNtk());
   V3PortableType netHash = V3PortableType((isBvNtk ? (V3_XD - BV_RED_AND) : 1), (isBvNtk ? BV_RED_AND : 0));
   // Create Duplicate Ntk
   V3Ntk* const ntk = createV3Ntk(dynamic_cast<V3BvNtk*>(handler->getNtk())); assert (ntk);
   duplicateInputNets(handler, ntk, hierInfo.getMap(0), c2pMap, orderMap);
   duplicateNtk(handler, ntk, hierInfo.getMap(0), c2pMap, orderMap, netHash, hierInfo, flattenLevel);
   duplicateInputGates(handler, ntk, hierInfo.getMap(0), c2pMap); return ntk;
}

V3Ntk* const attachToNtk(V3NtkHandler* const handler, V3Ntk* const ntk, const V3NetVec& targetNets, V3NetVec& p2cMap, V3NetVec& c2pMap, V3PortableType& netHash) {
   assert (handler); assert (handler->getNtk()); assert (ntk);
   assert (p2cMap.size() == handler->getNtk()->getNetSize()); assert (c2pMap.size());
   assert ((bool)(dynamic_cast<V3BvNtk*>(handler->getNtk())) == (bool)(dynamic_cast<V3BvNtk*>(ntk)));
   // Compute Reduced DFS Order for Duplication
   V3NetVec orderMap; dfsNtkForReductionOrder(ntk, orderMap, targetNets);
   // Attach to Existing Ntk
   V3NtkHierInfo hierInfo; hierInfo.clear(); hierInfo.pushNtk(handler, 0);
   duplicateNtk(handler, ntk, p2cMap, c2pMap, orderMap, netHash, hierInfo, 0); return ntk;
}

#endif

