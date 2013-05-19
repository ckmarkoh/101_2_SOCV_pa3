/****************************************************************************
  FileName     [ v3NtkQuteRTL.cpp ]
  PackageName  [ v3/src/io ]
  Synopsis     [ QuteRTL Front-End to V3 Network Construction. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_QUTERTL_C
#define V3_NTK_QUTERTL_C

#include "v3Msg.h"
#include "v3Set.h"
#include "v3StrUtil.h"
#include "v3FileUtil.h"
#include "v3NtkParser.h"
#include "v3NtkWriter.h"

#include "quteRTL.h"

#include <fstream>
#include <string>

// Global Variable
const string quteRTL_MsgFile = "quteRTL.log";  // QuteRTL Log File

V3NetVec    V3QuteRTLInput;

// Set QuteRTL Log File Helper Functions
void setQuteLogFile() {
   ofstream output; output.open(quteRTL_MsgFile.c_str());
   if (output.is_open()) {
      writeV3GeneralHeader("QuteRTL API Output Log", output, "//"); output.close();
      quteSetLogFile(quteRTL_MsgFile.c_str(), true);
   }
   else Msg(MSG_WAR) << "QuteRTL Log \"" << quteRTL_MsgFile << "\" Cannot be Opened !!" << endl;
}

// QuteRTL Module to V3 Ntk Transformation Helper Functions
const uint32_t getOutPinWidthFromQuteRTL(const string& name, CktOutPin* const OutPin) {
   assert (name.size()); assert (OutPin);
   const uint32_t end = quteGetPinEnd(OutPin), begin = quteGetPinBegin(OutPin);
   if (end < begin) Msg(MSG_WAR) << "Inverting Net Found : " << name << "[" << end << ":" << begin << "]" << endl;
   return (end >= begin) ? (1 + end - begin) : (1 + begin - end);
}

const V3NetId dfsBuildNtkFromQuteRTL(V3NtkInput* const quteHandler, CktOutPin* const OutPin) {
   assert (quteHandler); assert (OutPin);
   // Check if OutPin Already Exists
   const string name = quteGetOutPinName(OutPin); assert (name.size());
   V3NetId id = quteHandler->getNetId(name); if (V3NetUD != id) return id;
   // Create V3NetId for OutPin
   id = quteHandler->createNet(name, getOutPinWidthFromQuteRTL(name, OutPin)); if (V3NetUD == id) return id;
   const uint32_t width = quteHandler->getNtk()->getNetWidth(id); assert (width);
   // Get OutPin Info
   CktCell* const cell = quteGetCellFromPin(OutPin); assert (cell);
   const QuteRTL_API_CellType type = quteGetCellType(cell); assert (type < QUTE_TOTAL_CELL);
   assert (type != QUTE_PI_CELL && type != QUTE_PO_CELL && type != QUTE_PIO_CELL && type != QUTE_DFF_CELL);
   // Compute V3 Gate Type According to QuteRTL Cell Type
   V3BvNtk* const ntk = dynamic_cast<V3BvNtk*>(quteHandler->getNtk()); assert (ntk);
   bool invert = false, exactTwo = false;
   V3GateType v3Type;
   switch (type) {
      // Single Input : Reduced
      case QUTE_RED_AND_CELL  : v3Type = BV_RED_AND;                   break;
      case QUTE_RED_OR_CELL   : v3Type = BV_RED_OR;                    break;
      case QUTE_RED_NAND_CELL : v3Type = BV_RED_AND;  invert = true;   break;
      case QUTE_RED_NOR_CELL  : v3Type = BV_RED_OR;   invert = true;   break;
      case QUTE_RED_XOR_CELL  : v3Type = BV_RED_XOR;                   break;
      case QUTE_RED_XNOR_CELL : v3Type = BV_RED_XOR;  invert = true;   break;
      // Single Input : Logic
      case QUTE_BUF_CELL      : v3Type = BV_BUF;                       break;
      case QUTE_INV_CELL      : v3Type = BV_BUF;      invert = true;   break;
      // One+ Input : Logic
      case QUTE_AND_CELL      : v3Type = BV_AND;                       break;
      case QUTE_OR_CELL       : v3Type = BV_OR;                        break;
      case QUTE_NAND_CELL     : v3Type = BV_AND;      invert = true;   break;
      case QUTE_NOR_CELL      : v3Type = BV_OR;       invert = true;   break;
      case QUTE_XOR_CELL      : v3Type = BV_XOR;                       break;
      case QUTE_XNOR_CELL     : v3Type = BV_XOR;      invert = true;   break;
      // One+ Input : Arithmetic
      case QUTE_ADD_CELL      : v3Type = BV_ADD;                       break;
      case QUTE_SUB_CELL      : v3Type = BV_SUB;                       break;
      case QUTE_MULT_CELL     : v3Type = BV_MULT;                      break;
      case QUTE_DIV_CELL      : v3Type = BV_DIV;                       break;
      case QUTE_MODULO_CELL   : v3Type = BV_MODULO;                    break;
      // One+ Input : Model
      case QUTE_MERGE_CELL    : v3Type = BV_MERGE;                     break;
      // Two Inputs : Arithmetic
      case QUTE_SHL_CELL      : v3Type = BV_SHL;                       break;
      case QUTE_SHR_CELL      : v3Type = BV_SHR;                       break;
      // Two Inputs : Comparator
      case QUTE_EQUALITY_CELL : v3Type = BV_EQUALITY; exactTwo = true; break;
      case QUTE_GEQ_CELL      : v3Type = BV_GEQ;      exactTwo = true; break;
      case QUTE_GREATER_CELL  : v3Type = BV_GREATER;  exactTwo = true; break;
      case QUTE_LEQ_CELL      : v3Type = BV_LEQ;      exactTwo = true; break;
      case QUTE_LESS_CELL     : v3Type = BV_LESS;     exactTwo = true; break;
      // Multiplexer
      case QUTE_MUX_CELL      : v3Type = BV_MUX;                       break;
      // Model
      case QUTE_CONST_CELL    : v3Type = BV_CONST;                     break;
      case QUTE_SPLIT_CELL    : v3Type = BV_SLICE;                     break;
      // Unsupported : Model
      case QUTE_MEMORY_CELL   : Msg(MSG_ERR) << "Memory Exists in RTL Design !!" << endl; return V3NetUD;
      // Unsupported : Module Instance
      case QUTE_MODULE_CELL   : Msg(MSG_ERR) << "Sub-Module Exists in RTL Design !!" << endl; return V3NetUD;
      // Unsupported : Latch
      case QUTE_DLAT_CELL     : Msg(MSG_ERR) << "Latch Exists in RTL Design !!" << endl; return V3NetUD;
      default                 : Msg(MSG_ERR) << "Unexpected QuteRTL Cell Type : " << type << endl; return V3NetUD;
   }
   // Build V3 Gate
   V3NetId id1, id2, id3; 
   if (isV3ReducedType(v3Type)) {  // REDUCED
      assert (quteGetCellInputSize(cell) == 1); assert (V3NetUD != id);
      id2 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, 0)); if (V3NetUD == id2) return id2;
      id1 = quteHandler->createNet("", 1); if (V3NetUD == id1) return id1;
      if (!createBvReducedGate(ntk, v3Type, id1, id2)) return V3NetUD;
      if (!createV3BufGate(ntk, id, (invert ? getV3InvertNet(id1) : id1))) return V3NetUD;
   }
   else if (exactTwo) {  // TWO INPUTS
      assert (quteGetCellInputSize(cell) == 2); assert (V3NetUD != id); assert (!invert);
      id1 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, 0)); if (V3NetUD == id1) return id1;
      id2 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, 1)); if (V3NetUD == id2) return id2;
      if (!createBvPairGate(ntk, v3Type, id, id1, id2)) return V3NetUD;
   }
   else if (BV_MUX == v3Type) {  // MULTIPLEXER
      assert (quteGetCellInputSize(cell) == 3); assert (V3NetUD != id); assert (!invert);
      id1 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, 0)); if (V3NetUD == id1) return id1;
      id2 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, 1)); if (V3NetUD == id2) return id2;
      id3 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, 2)); if (V3NetUD == id3) return id3;
      if (!createBvMuxGate(ntk, id, id1, id2, id3)) return V3NetUD;
   }
   else if (BV_SLICE == v3Type) {  // SLICE
      assert (quteGetCellInputSize(cell) == 1); assert (V3NetUD != id); assert (!invert);
      CktOutPin* inOutPin = quteGetCellInputPin(cell, 0); assert (inOutPin);
      id1 = dfsBuildNtkFromQuteRTL(quteHandler, inOutPin); if (V3NetUD == id1) return id1;
      const uint32_t sliceEnd = quteGetSplitOutPinEnd(OutPin), sliceBegin = quteGetSplitOutPinBegin(OutPin);
      const uint32_t end = quteGetPinEnd(inOutPin), begin = quteGetPinBegin(inOutPin);
      if (end >= begin) {
         assert (sliceEnd <= end); assert (sliceBegin >= begin);
         if (!createBvSliceGate(ntk, id, id1, sliceEnd - begin, sliceBegin - begin)) return V3NetUD;
      }
      else {
         assert (sliceEnd >= end); assert (sliceBegin <= begin);
         if (!createBvSliceGate(ntk, id, id1, begin - sliceEnd, begin - sliceBegin)) return V3NetUD;
      }
   }
   else if (BV_CONST == v3Type) {  // CONST
      assert (quteGetCellInputSize(cell) == 0); assert (V3NetUD != id); assert (!invert);
      const string value = quteGetConstCellValue(cell); assert (value.size());
      if (!createBvConstGate(ntk, id, value)) return V3NetUD;
   }
   else if (BV_BUF == v3Type) {  // BUF / INV
      assert (quteGetCellInputSize(cell) == 1); assert (V3NetUD != id);
      id1 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, 0)); if (V3NetUD == id1) return id1;
      if (!createV3BufGate(ntk, id, (invert ? getV3InvertNet(id1) : id1))) return V3NetUD;
   }
   else if (BV_MERGE == v3Type) {  // MERGE
      assert (quteGetCellInputSize(cell) >= 1); assert (V3NetUD != id); id1 = V3NetUD;
      uint32_t merge_width = 0;
      for (uint32_t i = 0, j = quteGetCellInputSize(cell); i < j; ++i) {
         id2 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, i)); if (V3NetUD == id2) return id2;
         merge_width += quteHandler->getNtk()->getNetWidth(id2); assert (merge_width);
         if (V3NetUD == id1) { id1 = id2; continue; }
         id3 = quteHandler->createNet("", merge_width); if (V3NetUD == id3) return id3;
         if (!createBvPairGate(ntk, v3Type, id3, id1, id2)) return V3NetUD; id1 = id3;
      }
      assert (width == merge_width);
      if (!createV3BufGate(ntk, id, (invert ? getV3InvertNet(id1) : id1))) return V3NetUD;
   }
   else {  // ONE+ INPUT
      assert (quteGetCellInputSize(cell) >= 1); assert (V3NetUD != id);
      assert (isV3PairType(v3Type) || (BV_OR == v3Type)); id1 = V3NetUD;
      for (uint32_t i = 0, j = quteGetCellInputSize(cell); i < j; ++i) {
         id2 = dfsBuildNtkFromQuteRTL(quteHandler, quteGetCellInputPin(cell, i)); if (V3NetUD == id2) return id2;
         if (V3NetUD == id1) { id1 = id2; continue; }
         id3 = quteHandler->createNet("", width); if (V3NetUD == id3) return id3;
         if (!createBvPairGate(ntk, v3Type, id3, id1, id2)) return V3NetUD; id1 = id3;
      }
      if (!createV3BufGate(ntk, id, (invert ? getV3InvertNet(id1) : id1))) return V3NetUD;
   }
   // Return V3NetId for OutPin
   return id;
}

const bool V3QuteRTLInputHandler(V3NtkInput* const quteHandler, CktModule* const module) {
   assert (quteHandler); assert (module);
   CktCell* cell; CktOutPin* OutPin;
   V3NetId id; string name;
   // Store Inputs for Clock Signal Pruning
   V3QuteRTLInput.clear(); V3QuteRTLInput.reserve(quteGetDesignIoSize(module, QUTE_PI_CELL));
   // Build Input  (Renders the Same Order As Design Created by QuteRTL)
   V3BvNtk* const ntk = dynamic_cast<V3BvNtk*>(quteHandler->getNtk()); assert (ntk);
   for (uint32_t i = 0, j = quteGetDesignIoSize(module, QUTE_PI_CELL); i < j; ++i) {
      // Get Cell Info from QuteRTL
      cell = quteGetDesignIoCell(module, QUTE_PI_CELL, i); assert (cell);
      assert (quteGetCellOutputSize(cell) == 1);
      OutPin = quteGetCellOutputPin(cell, 0); assert (OutPin);
      name = quteGetOutPinName(OutPin); assert (name.size());
      // Build Input in V3 Ntk
      id = quteHandler->createNet(name, getOutPinWidthFromQuteRTL(name, OutPin)); if (V3NetUD == id) return false;
      V3QuteRTLInput.push_back(id);  //if (!createInput(ntk, id)) return false;
   }
   for (uint32_t i = 0, j = quteGetDesignIoSize(module, QUTE_PIO_CELL); i < j; ++i) {
      // Get Cell Info from QuteRTL
      cell = quteGetDesignIoCell(module, QUTE_PIO_CELL, i); assert (cell);
      assert (quteGetCellOutputSize(cell) == 1);
      OutPin = quteGetCellOutputPin(cell, 0); assert (OutPin);
      name = quteGetOutPinName(OutPin); assert (name.size());
      // Build Input in V3 Ntk
      id = quteHandler->createNet(name, getOutPinWidthFromQuteRTL(name, OutPin)); if (V3NetUD == id) return false;
      if (!createInout(ntk, id)) return false;
   }
   return true;
}

const bool V3QuteRTLFFHandler(V3NtkInput* const quteHandler, CktModule* const module, const bool& async2sync) {
   assert (quteHandler); assert (module);
   CktCell* cell; CktOutPin* OutPin;
   unsigned width; string name;
   V3NetId id, id1, id2, id3;
   // Build FF Nets  (Renders the Same Order As Design Created by QuteRTL)
   V3BvNtk* const ntk = dynamic_cast<V3BvNtk*>(quteHandler->getNtk()); assert (ntk);
   V3NetVec quteFFVec; quteFFVec.clear();
   V3Set<string>::Set quteFFClk; quteFFClk.clear();
   for (uint32_t i = 0, j = quteGetDesignIoSize(module, QUTE_DFF_CELL); i < j; ++i) {
      // Get Cell Info from QuteRTL
      cell = quteGetDesignIoCell(module, QUTE_DFF_CELL, i); assert (cell);
      assert (quteGetCellOutputSize(cell) == 1);
      OutPin = quteGetCellOutputPin(cell, 0); assert (OutPin);
      name = quteGetOutPinName(OutPin); assert (name.size());
      // Build DFF in V3 Ntk
      id = quteHandler->createNet(name, getOutPinWidthFromQuteRTL(name, OutPin));
      if (V3NetUD == id) return false; quteFFVec.push_back(id);
   }
   // DFS Traverse On DFF Fanin Cone
   for (uint32_t i = 0, j = quteGetDesignIoSize(module, QUTE_DFF_CELL); i < j; ++i) {
      // Get Cell Info from QuteRTL
      cell = quteGetDesignIoCell(module, QUTE_DFF_CELL, i); assert (cell);
      const uint32_t inPinSize = quteGetCellInputSize(cell); assert (inPinSize >= 2);
      // Check Clock Signal
      OutPin = quteGetCellInputPin(cell, 1); assert (OutPin);
      name = quteGetOutPinExpr(OutPin); assert (name.size()); quteFFClk.insert(name);
      if (inPinSize > 2) {  // Asynchronous Reset DFF : D, clk, reset, default
         OutPin = quteGetCellInputPin(cell, 0); assert (OutPin);
         id1 = dfsBuildNtkFromQuteRTL(quteHandler, OutPin); if (V3NetUD == id1) return false;
         OutPin = quteGetCellInputPin(cell, 2); assert (OutPin);
         id2 = dfsBuildNtkFromQuteRTL(quteHandler, OutPin); if (V3NetUD == id2) return false;
         OutPin = quteGetCellInputPin(cell, 3); assert (OutPin);
         id3 = dfsBuildNtkFromQuteRTL(quteHandler, OutPin); if (V3NetUD == id3) return false;
         // Build DFF
         name = quteGetOutPinName(OutPin); assert (name.size());
         width = quteHandler->getNtk()->getNetWidth(quteFFVec[i]);
         if (async2sync) {  // Renders all Asynchronous DFF be Regarded as Synchronous DFF
            // Build MUX
            id = quteHandler->createNet(V3AsyncMuxName + name, width); if (V3NetUD == id) return false;
            if (!createBvMuxGate(ntk, id, id1, id3, id2)) return false;
            // Build DFF
            id2 = quteHandler->createNet("", width); if (V3NetUD == id2) return false;
            if (!createBvConstGate(ntk, id2, v3Int2Str(width) + "'d0")) return false;
            if (!createV3FFGate(ntk, quteFFVec[i], id, id2)) return false;
         }
         else {  // Retain Asynchronous Behavior in V3 Ntk
            // Build MUX
            id = quteHandler->createNet(V3AsyncDFFName + name, width); if (V3NetUD == id) return false;
            if (!createBvMuxGate(ntk, quteFFVec[i], id, id3, id2)) return false;
            // Build DFF
            id2 = quteHandler->createNet("", width); if (V3NetUD == id2) return false;
            if (!createBvConstGate(ntk, id2, v3Int2Str(width) + "'d0")) return false;
            if (!createV3FFGate(ntk, id, id1, id2)) return false;
         }
      }
      else {  // Synchronous Reset DFF : D, clk
         // Traverse DFF Input
         OutPin = quteGetCellInputPin(cell, 0); assert (OutPin);
         id1 = dfsBuildNtkFromQuteRTL(quteHandler, OutPin); if (V3NetUD == id1) return false;
         // Build DFF
         width = quteHandler->getNtk()->getNetWidth(quteFFVec[i]);
         id2 = quteHandler->createNet("", width); if (V3NetUD == id2) return false;
         if (!createBvConstGate(ntk, id2, v3Int2Str(width) + "'d0")) return false;
         if (!createV3FFGate(ntk, quteFFVec[i], id1, id2)) return false;
      }
   }
   // Report Multiple Clock Domain if Exists
   if (quteFFClk.size() > 1) {
      Msg(MSG_WAR) << "Multiple Clock Domains Found in RTL Design !!" << endl;
      Msg(MSG_IFO) << "Clock Domains (in terms of RTL signal names) are: ";
      for (V3Set<string>::Set::const_iterator it = quteFFClk.begin(); it != quteFFClk.end(); ++it)
         Msg(MSG_IFO) << (it == quteFFClk.begin() ? "" : ", ") << (*it);
      Msg(MSG_IFO) << endl;
      Msg(MSG_WAR) << "V3 Simply Treat All Clock Domains to be the Same One !!" << endl;
   }
   // Set Clock Signal and Remove Clock from Inputs
   if (quteFFClk.size()) {
      ntk->createClock(quteHandler->getNetId(*(quteFFClk.begin())));
      for (uint32_t i = 0; i < V3QuteRTLInput.size(); ++i) 
         if (ntk->getClock().id == V3QuteRTLInput[i].id) continue;
         else if (!createInput(ntk, V3QuteRTLInput[i])) return false;
   }
   else for (uint32_t i = 0; i < V3QuteRTLInput.size(); ++i) if (!createInput(ntk, V3QuteRTLInput[i])) return false;
   assert (ntk->getInputSize() == (V3QuteRTLInput.size() - (quteFFClk.size() ? 1 : 0)));
   return true;
}

const bool V3QuteRTLOutputHandler(V3NtkInput* const quteHandler, CktModule* const module) {
   assert (quteHandler); assert (module);
   CktCell* cell; CktOutPin* OutPin;
   unsigned width; string name;
   V3NetId id, id1;
   // Build Output  (Renders the Same Order As Design Created by QuteRTL)
   V3BvNtk* const ntk = dynamic_cast<V3BvNtk*>(quteHandler->getNtk()); assert (ntk);
   for (uint32_t i = 0, j = quteGetDesignIoSize(module, QUTE_PO_CELL); i < j; ++i) {
      // Get Cell Info from QuteRTL
      cell = quteGetDesignIoCell(module, QUTE_PO_CELL, i); assert (cell);
      assert (quteGetCellOutputSize(cell) == 1);
      OutPin = quteGetCellOutputPin(cell, 0); assert (OutPin);
      name = quteGetOutPinName(OutPin); assert (name.size());
      width = getOutPinWidthFromQuteRTL(name, OutPin);
      // Traverse PO Input
      assert (quteGetCellInputSize(cell) == 1);
      OutPin = quteGetCellInputPin(cell, 0); assert (OutPin);
      id1 = dfsBuildNtkFromQuteRTL(quteHandler, OutPin); if (V3NetUD == id1) return false;
      // Build Output in V3 Ntk
      id = quteHandler->createNet(name, width); if (V3NetUD == id) return false;
      if ((id != id1) && (!createV3BufGate(ntk, id, id1))) return false;
      if (!createOutput(ntk, id)) return false; quteHandler->recordOutName(ntk->getOutputSize() - 1, name);
   }
   return true;
}

V3NtkInput* const V3QuteRTLHandler(CktModule* const& module, const bool& async2sync) {
   // Create Network Handler
   V3NtkInput* quteHandler = new V3NtkInput(false); assert (module);
   if (!quteHandler) { Msg(MSG_ERR) << "Create RTL Design Failed !!" << endl; return 0; }
   if (!quteHandler->getNtk()) { Msg(MSG_ERR) << "Create BV Network Failed !!" << endl; return 0; }

   // 1. Create Input / Inout Nets
   if (!V3QuteRTLInputHandler(quteHandler, module)) { delete quteHandler; return 0; }
   // 2. Create DFF and Traverse its Fanin Cones
   if (!V3QuteRTLFFHandler(quteHandler, module, async2sync)) { delete quteHandler; return 0; }
   // 3. Create Output
   if (!V3QuteRTLOutputHandler(quteHandler, module)) { delete quteHandler; return 0; }
   
   return quteHandler;
}

// Front-End Framework Integration Main Functions
V3NtkInput* const V3NtkFromQuteRTL(const char* fileName, const bool& isFileList, const bool& async2sync) {
   // Set QuteRTL Log File
   setQuteLogFile();
   // Parse Verilog by QuteRTL from API and Get a Pointer to CktModule in QuteRTL
   CktModule* const module = quteReadRTL(fileName, isFileList);
   if (!module) {
      Msg(MSG_ERR) << "RTL Parse Failed !!  "
                   << "(See QuteRTL Log File \"" << quteRTL_MsgFile << "\" for Detailed Info)" << endl;
      return 0;
   }
   // Remove QuteRTL Log File
   v3DeleteDir(quteRTL_MsgFile.c_str());
   // Traverse CktModule and Construct Ntk in V3
   return V3QuteRTLHandler(module, async2sync);
}

#endif

