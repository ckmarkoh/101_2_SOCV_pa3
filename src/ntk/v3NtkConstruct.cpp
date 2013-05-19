/****************************************************************************
  FileName     [ v3NtkConstruct.cpp ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ Generic Functions for V3 Ntk Construction. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_CONSTRUCT_C
#define V3_NTK_CONSTRUCT_C

#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3StrUtil.h"

// Global Member for Ntk Construction
V3InputVec        v3InputVec;

// General Construction Functions for V3 Ntk
V3Ntk* const createV3Ntk(const bool& isBv) {
   if (isBv) { V3BvNtk* const ntk = new V3BvNtk(); assert (ntk); ntk->initialize(); return ntk; }
   else { V3Ntk* const ntk = new V3AigNtk(); assert (ntk); ntk->initialize(); return ntk; }
}

const V3NetId createNet(V3Ntk* const ntk, uint32_t width) {
   return ntk->createNet();
}

const bool createInput(V3Ntk* const ntk, const V3NetId& id) {
   if (ntk->reportInvertingNet(id)) return false;
   if (ntk->reportMultipleDrivenNet(V3_PI, id)) return false;
   ntk->createInput(id); return true;
}

const bool createOutput(V3Ntk* const ntk, const V3NetId& id) {
   ntk->createOutput(id); return true;
}

const bool createInout(V3Ntk* const ntk, const V3NetId& id) {
   if (ntk->reportMultipleDrivenNet(V3_PIO, id)) return false;
   if (ntk->reportUnexpectedFaninSize(V3_PIO, id)) return false;
   ntk->createInout(id); return true;
}

// General Gate Type Construction Functions for V3 Ntk
const bool createV3BufGate(V3Ntk* const ntk, const V3NetId& id, const V3NetId& id1) {
   const V3GateType type = (dynamic_cast<V3BvNtk*>(ntk)) ? BV_BUF : AIG_NODE;
   if (ntk->reportMultipleDrivenNet(type, id)) return false;
   if (ntk->reportNetWidthInconsistency(id, id1, V3GateTypeStr[type] + " I/O")) return false;
   v3InputVec.push_back(V3NetType(id1)); v3InputVec.push_back(V3NetType(id1));
   ntk->setInput(id, v3InputVec); v3InputVec.clear();
   ntk->createGate((dynamic_cast<V3BvNtk*>(ntk)) ? BV_AND : AIG_NODE, id); return true;
}

const bool createV3FFGate(V3Ntk* const ntk, const V3NetId& id, const V3NetId& id1, const V3NetId& init_id) {
   if (ntk->reportMultipleDrivenNet(V3_FF, id)) return false;
   if (ntk->reportNetWidthInconsistency(id, id1, V3GateTypeStr[V3_FF] + " I/O")) return false;
   if (ntk->reportNetWidthInconsistency(id, init_id, V3GateTypeStr[V3_FF] + " INIT/O")) return false;
   v3InputVec.push_back(V3NetType(id1)); v3InputVec.push_back(V3NetType(init_id));
   ntk->setInput(id, v3InputVec); v3InputVec.clear(); ntk->createLatch(id); return true;
}

const bool createV3AndGate(V3Ntk* const ntk, const V3NetId& id, const V3NetId& id1, const V3NetId& id2) {
   V3BvNtk* const bvNtk = dynamic_cast<V3BvNtk*>(ntk);
   if (bvNtk) return createBvPairGate(bvNtk, BV_AND, id, id1, id2);
   else return createAigAndGate(ntk, id, id1, id2);
}

// General Gate Type Construction Functions for AIG Ntk
const bool createAigAndGate(V3AigNtk* const ntk, const V3NetId& id, const V3NetId& id1, const V3NetId& id2) {
   if (ntk->reportMultipleDrivenNet(AIG_NODE, id)) return false;
   v3InputVec.push_back(V3NetType(id1)); v3InputVec.push_back(V3NetType(id2));
   ntk->setInput(id, v3InputVec); v3InputVec.clear(); ntk->createGate(AIG_NODE, id); return true;
}

const bool createAigFalseGate(V3AigNtk* const ntk, const V3NetId& id) {
   if (ntk->reportMultipleDrivenNet(AIG_FALSE, id)) return false;
   ntk->createConst(id); return true;
}

// General Gate Type Construction Functions for BV Ntk
const bool createBvReducedGate(V3BvNtk* const ntk, const V3GateType& type, const V3NetId& id, const V3NetId& id1) {
   assert (isV3ReducedType(type)); if (ntk->reportMultipleDrivenNet(type, id)) return false;
   if (ntk->reportUnexpectedNetWidth(id, 1, V3GateTypeStr[type] + " Output")) return false;
   v3InputVec.push_back(V3NetType(id1)); ntk->setInput(id, v3InputVec);
   v3InputVec.clear(); ntk->createGate(type, id); return true;
}

const bool createBvPairGate(V3BvNtk* const ntk, const V3GateType& type, const V3NetId& id, 
                                                const V3NetId& id1, const V3NetId& id2) {
   assert (isV3PairType(type) || isV3ExtendType(type));
   if (ntk->reportMultipleDrivenNet(type, id)) return false;
   // Extended Gate Type
   if (isV3ExtendType(type)) {
      V3NetId ext1 = id1, ext2 = id2;
      if (isV3ExtendSwapIn(type)) { V3NetId t = ext2; ext2 = ext1; ext1 = t; }
      if (isV3ExtendInvIn(type)) { ext1.cp ^= 1; ext2.cp ^= 1; }
      V3NetId id_new = isV3ExtendInvOut(type) ? ntk->createNet(ntk->getNetWidth(id)) : id;
      assert (V3NetUD != id_new); assert (isV3ExtendInvOut(type) ^ (id == id_new));
      if (!createBvPairGate(ntk, getV3ExtendNormal(type), id_new, ext1, ext2)) return false;
      if (id != id_new) { id_new.cp ^= 1; return createV3BufGate(ntk, id, id_new); } return true;
   }
   else {
      if (type != BV_SHL && type != BV_SHR && type != BV_MERGE)
         if (ntk->reportNetWidthInconsistency(id1, id2, V3GateTypeStr[type] + " I/O")) return false;
      if (type == BV_EQUALITY || type == BV_GEQ) {
         if (ntk->reportUnexpectedNetWidth(id, 1, V3GateTypeStr[type] + " Output")) return false;
      }
      else if (type != BV_MERGE) {
         if (ntk->reportNetWidthInconsistency(id, id1, V3GateTypeStr[type] + " I/O")) return false;
         if (type != BV_SHL && type != BV_SHR) 
            if (ntk->reportNetWidthInconsistency(id, id2, V3GateTypeStr[type] + " I/O")) return false;
      }
      else if (ntk->reportUnexpectedNetWidth(id, ntk->getNetWidth(id1) + ntk->getNetWidth(id2), "BV_MERGE Output")) 
         return false;
      v3InputVec.push_back(V3NetType(id1)); v3InputVec.push_back(V3NetType(id2)); ntk->setInput(id, v3InputVec);
      v3InputVec.clear(); ntk->createGate(type, id); return true;
   }
}

const bool createBvMuxGate(V3BvNtk* const ntk, const V3NetId& id, const V3NetId& fId, const V3NetId& tId, const V3NetId& sId) {
   if (ntk->reportMultipleDrivenNet(BV_MUX, id)) return false;
   if (ntk->reportNetWidthInconsistency(fId, tId, "BV_MUX True / False")) return false;
   if (ntk->reportNetWidthInconsistency(id, fId, "BV_MUX False / Output")) return false;
   if (ntk->reportNetWidthInconsistency(id, tId, "BV_MUX True / Output")) return false;
   if (ntk->reportUnexpectedNetWidth(sId, 1, "BV_MUX Select")) return false;
   v3InputVec.push_back(V3NetType(fId)); v3InputVec.push_back(V3NetType(tId)); v3InputVec.push_back(V3NetType(sId));
   ntk->setInput(id, v3InputVec); v3InputVec.clear(); ntk->createGate(BV_MUX, id); return true;
}

const bool createBvSliceGate(V3BvNtk* const ntk, const V3NetId& id, const V3NetId& id1, uint32_t msb, uint32_t lsb) {
   if (ntk->reportMultipleDrivenNet(BV_SLICE, id)) return false;
   if (ntk->reportUnexpectedNetWidth(id, msb - lsb + 1, "BV_SLICE Output")) return false;
   if (ntk->getNetWidth(id1) <= msb)
      if (ntk->reportUnexpectedNetWidth(id1, msb + 1, "BV_SLICE Input Lower Bound")) return false;
   v3InputVec.push_back(V3NetType(id1)); v3InputVec.push_back(ntk->hashV3BusId(msb, lsb));
   ntk->setInput(id, v3InputVec); v3InputVec.clear(); ntk->createGate(BV_SLICE, id); return true;
}

const bool createBvConstGate(V3BvNtk* const ntk, const V3NetId& id, const string& value_exp) {
   if (ntk->reportMultipleDrivenNet(BV_CONST, id)) return false;
   v3InputVec.push_back(ntk->hashV3ConstBitVec(value_exp));
   ntk->setInput(id, v3InputVec); v3InputVec.clear(); ntk->createConst(id);
   return !ntk->reportUnexpectedNetWidth(id, ntk->getInputConstValue(id)->size(), "BV_CONST Output");
}

// General Hierarchical Ntk Construction Functions for V3 Ntk
const bool createModule(V3Ntk* const ntk, const V3NetVec& inputs, const V3NetVec& outputs, 
                        V3NtkHandler* const moduleHandler, const bool& isBlackBoxed) {
   for (uint32_t i = 0; i < outputs.size(); ++i)
      if (ntk->reportMultipleDrivenNet(V3_MODULE, outputs[i])) return false;
   if (reportIncompatibleModule(ntk, inputs, outputs, moduleHandler)) return false;
   V3NtkModule* const module = new V3NtkModule(inputs, outputs); assert (module);
   module->updateNtkRef(moduleHandler, isBlackBoxed); ntk->createModule(module);
   const uint32_t index = ntk->getModuleSize() - 1; assert (module == ntk->getModule(index));
   v3InputVec.push_back(V3NetType(index));
   for (uint32_t i = 0; i < outputs.size(); ++i) {
      ntk->setInput(outputs[i], v3InputVec); ntk->createGate(V3_MODULE, outputs[i]);
      assert (module == ntk->getModule(outputs[i]));
   }
   v3InputVec.clear(); moduleHandler->incInstRef(); return true;
}

const bool updateModule(V3Ntk* const ntk, V3NtkModule* const module, 
                        V3NtkHandler* const moduleHandler, const bool& isBlackBoxed) {
   assert (ntk); assert (module); assert (moduleHandler);
   if (reportIncompatibleModule(ntk, module->getInputList(), module->getOutputList(), moduleHandler)) return false;
   module->getNtkRef()->decInstRef(); module->updateNtkRef(moduleHandler, isBlackBoxed);
   moduleHandler->incInstRef(); return true;
}

const bool reportInconsistentRefNtk(V3Ntk* const ntk) {
   const bool isBvNtk = dynamic_cast<V3BvNtk*>(ntk);
   bool isBvRef, ok = false;
   for (uint32_t i = 0; i < ntk->getModuleSize(); ++i) {
      isBvRef = dynamic_cast<V3BvNtk*>(ntk->getModule(i)->getNtkRef()->getNtk());
      if (!(isBvNtk ^ isBvRef)) continue;
      Msg(MSG_ERR) << "Module Instance \"" << ntk->getModule(i)->getNtkRef()->getNtkName() << "\" is NOT a " 
                   << (isBvNtk ? "Bit-Vector" : "AIG") << " Network !!" << endl; ok = true;
   }
   return ok;
}

const bool reportIncompatibleModule(V3NtkHandler* const handler1, V3NtkHandler* const handler2) {
   assert (handler1); assert (handler2); V3NetVec inputs, outputs;
   V3Ntk* const ntk1 = handler1->getNtk(); assert (ntk1); inputs.clear(); outputs.clear();
   for (uint32_t i = 0; i < ntk1->getInputSize(); ++i) inputs.push_back(ntk1->getInput(i));
   for (uint32_t i = 0; i < ntk1->getOutputSize(); ++i) outputs.push_back(ntk1->getOutput(i));
   if (reportIncompatibleModule(ntk1, inputs, outputs, handler2)) return true;
   V3Ntk* const ntk2 = handler2->getNtk(); assert (ntk2); inputs.clear(); outputs.clear();
   for (uint32_t i = 0; i < ntk2->getInputSize(); ++i) inputs.push_back(ntk2->getInput(i));
   for (uint32_t i = 0; i < ntk2->getOutputSize(); ++i) outputs.push_back(ntk2->getOutput(i));
   if (reportIncompatibleModule(ntk2, inputs, outputs, handler1)) return true;
   return false;
}

const bool reportIncompatibleModule(V3Ntk* const ntk, const V3NetVec& inputs, const V3NetVec& outputs, 
                                    V3NtkHandler* const moduleHandler) {
   assert (ntk); assert (moduleHandler);
   V3Ntk* const moduleNtk = moduleHandler->getNtk(); assert (moduleNtk); assert (ntk != moduleNtk);
   if (moduleNtk->getInoutSize()) {
      Msg(MSG_ERR) << "Inout Ports are Not Available for Module Instance : \"" << moduleHandler->getNtkName() 
                   << "\"(" << moduleNtk->getInoutSize() << ") !!" << endl; return true;
   }
   const bool isBvNtk = dynamic_cast<V3BvNtk*>(ntk);
   const bool isBvModule = dynamic_cast<V3BvNtk*>(moduleHandler->getNtk());
   if (isBvNtk ^ isBvModule) {
      if (isBvNtk) {
         uint32_t totalBits = 0;
         for (uint32_t i = 0; i < inputs.size(); ++i) {
            assert (inputs[i].id < ntk->getNetSize()); totalBits += ntk->getNetWidth(inputs[i]);
         }
         if (moduleNtk->getInputSize() != totalBits) {
            Msg(MSG_ERR) << "Unmatched Total Input Port Bits for Module Instance : "
                         << "Top Module (" << totalBits << ") != \"" << moduleHandler->getNtkName()
                         << "\"(" << moduleNtk->getInputSize() << ") !!" << endl; return true;
         }
         totalBits = 0;
         for (uint32_t i = 0; i < outputs.size(); ++i) {
            assert (outputs[i].id < ntk->getNetSize()); totalBits += ntk->getNetWidth(outputs[i]);
         }
         if (moduleNtk->getOutputSize() != totalBits) {
            Msg(MSG_ERR) << "Unmatched Total Output Port Bits for Module Instance : "
                         << "Top Module (" << totalBits << ") != \"" << moduleHandler->getNtkName()
                         << "\"(" << moduleNtk->getOutputSize() << ") !!" << endl; return true;
         }
      }
      else {
         uint32_t totalBits = 0;
         for (uint32_t i = 0; i < moduleNtk->getInputSize(); ++i) 
            totalBits += moduleNtk->getNetWidth(moduleNtk->getInput(i));
         if (inputs.size() != totalBits) {
            Msg(MSG_ERR) << "Unmatched Total Input Port Bits for Module Instance : "
                         << "Top Module (" << inputs.size() << ") != \"" << moduleHandler->getNtkName()
                         << "\"(" << totalBits << ") !!" << endl; return true;
         }
         totalBits = 0;
         for (uint32_t i = 0; i < moduleNtk->getOutputSize(); ++i) 
            totalBits += moduleNtk->getNetWidth(moduleNtk->getOutput(i));
         if (outputs.size() != totalBits) {
            Msg(MSG_ERR) << "Unmatched Total Output Port Bits for Module Instance : "
                         << "Top Module (" << outputs.size() << ") != \"" << moduleHandler->getNtkName()
                         << "\"(" << totalBits << ") !!" << endl; return true;
         }
      }
   }
   else {
      if (inputs.size() != moduleNtk->getInputSize()) {
         Msg(MSG_ERR) << "Unmatched Input Port Number for Module Instance : "
                      << "Top Module (" << inputs.size() << ") != \"" << moduleHandler->getNtkName()
                      << "\"(" << moduleNtk->getInputSize() << ") !!" << endl; return true;
      }
      if (outputs.size() != moduleNtk->getOutputSize()) {
         Msg(MSG_ERR) << "Unmatched Output Port Number for Module Instance : "
                      << "Top Module (" << outputs.size() << ") != \"" << moduleHandler->getNtkName()
                      << "\"(" << moduleNtk->getOutputSize() << ") !!" << endl; return true;
      }
      if (!isBvNtk) return false;
      for (uint32_t i = 0; i < moduleNtk->getInputSize(); ++i) {
         assert (inputs[i].id < ntk->getNetSize());
         if (ntk->getNetWidth(inputs[i]) == moduleNtk->getNetWidth(moduleNtk->getInput(i))) continue;
         Msg(MSG_ERR) << "Unmatched Input Port (" << i << ") Width for Module Instance : "
                   << "Top Module (" << ntk->getNetWidth(inputs[i]) << ") != \"" << moduleHandler->getNtkName()
                   << "\"(" << moduleNtk->getNetWidth(moduleNtk->getInput(i)) << ") !!" << endl; return true;
      }
      for (uint32_t i = 0; i < moduleNtk->getOutputSize(); ++i) {
         assert (outputs[i].id < ntk->getNetSize());
         if (ntk->getNetWidth(outputs[i]) == moduleNtk->getNetWidth(moduleNtk->getOutput(i))) continue;
         Msg(MSG_ERR) << "Unmatched Output Port (" << i << ") Width for Module Instance : "
                   << "Top Module (" << ntk->getNetWidth(outputs[i]) << ") != \"" << moduleHandler->getNtkName()
                   << "\"(" << moduleNtk->getNetWidth(moduleNtk->getOutput(i)) << ") !!" << endl; return true;
      }
   }
   return false;
}

#endif

