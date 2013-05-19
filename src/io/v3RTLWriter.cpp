/****************************************************************************
  FileName     [ v3RTLWriter.cpp ]
  PackageName  [ v3/src/io ]
  Synopsis     [ RTL (Verilog) Writer for V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_RTL_WRITER_H
#define V3_RTL_WRITER_H

#include "v3Msg.h"
#include "v3StrUtil.h"
#include "v3NtkWriter.h"

// RTL Operators
const string V3TypeRTL[] = {
   "", "", "", "", " & ", "",                            // PI, PIO, FF, MODULE, AIG GATES
   "&", "|", "^", "", " & ", " ^ ",                      // BV_(RED, LOGIC)
   " + ", " - ", " * ", " / ", " % ", " << ",  ">> ",    // BV_(ARITH)
   "", " == ", " >= ", "", ""                            // BV_(MODEL), BV_(COMP)
};

const string getVerilogName(const V3NtkHandler* const handler, const string& name) {
   assert (handler); assert (name.size());
   string rtlName = (isdigit(name[0])) ? "id" : "";//handler->applyAuxNetNamePrefix("id") : "";
   for (uint32_t i = 0; i < name.size(); ++i) 
      if ('[' == name[i]) rtlName += "_v3_slice_";
      else if (':' == name[i]) rtlName += "_";
      else if (']' == name[i]) rtlName += "";
      else rtlName += name[i];
   return rtlName;
}

// RTL (Verilog) Writer Main Function
void V3RTLWriter(const V3NtkHandler* const handler, const char* fileName, const bool& symbol, const bool& initial) {
   assert (handler); assert (handler->getNtk());
   V3Ntk* const ntk = handler->getNtk(); assert (ntk);
   // Check if Module Instance Exists
   if (ntk->getModuleSize()) {
      Msg(MSG_ERR) << "Hierarchical Verilog Output has Not been Implemented Yet !!" << endl; return;
   }
   // Open RTL (Verilog) Output File
   assert (fileName); ofstream output; output.open(fileName);
   if (!output.is_open()) { Msg(MSG_ERR) << "RTL Output File \"" << fileName << "\" Not Found !!" << endl; return; }
   // V3 Header for RTL Output
   const_cast<V3NtkHandler*>(handler)->setAuxRenaming(); writeV3GeneralHeader("RTL (Verilog)", output, "//");
   output << "// Internal nets are renamed with prefix \"" << handler->applyAuxNetNamePrefix("") << "\".\n" << endl;
   // Check if Clock is Necessary but Missing
   const string clockName = ((V3NetUD == ntk->getClock()) ? "v3_clock" : V3RTLNameOrId(handler, ntk->getClock()));
   // Compute Name Mapping Table
   V3Vec<string>::Vec rtlName(ntk->getNetSize(), "");
   V3Vec<string>::Vec ioName; ioName.reserve(ntk->getInputSize() + ntk->getOutputSize() + ntk->getInoutSize());
   if (ntk->getLatchSize()) ioName.push_back(clockName);
   for (uint32_t i = 0; i < ntk->getInputSize(); ++i) {
      assert (!rtlName[ntk->getInput(i).id].size());
      ioName.push_back(V3RTLNameOrId(handler, ntk->getInput(i))); rtlName[ntk->getInput(i).id] = ioName.back();
   }
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) {
      Msg(MSG_WAR) << "Inout Port has not been Completely Supported !!" << endl;
      assert (!rtlName[ntk->getInout(i).id].size());
      ioName.push_back(V3RTLNameOrId(handler, ntk->getInout(i))); rtlName[ntk->getInout(i).id] = ioName.back();
   }
   for (uint32_t i = 0; i < ntk->getOutputSize(); ++i) 
      ioName.push_back(V3RTLNameBase(handler, handler->getOutputName(i)));
   V3NetId id = V3NetId::makeNetId(0);
   if (symbol) {
      for (; id.id < ntk->getNetSize(); ++id.id) if (!rtlName[id.id].size()) rtlName[id.id] = V3RTLNameOrId(handler, id);
   }
   else {
      for (; id.id < ntk->getNetSize(); ++id.id) if (!rtlName[id.id].size()) rtlName[id.id] = V3RTLNameById(handler, id);
   }
   // Output Verilog Module
   output << "module " << "v3_rtl" << "\n(" << endl;
   for (uint32_t i = 0; i < ioName.size(); ++i) output << (i ? ",\n" : "") << V3_INDENT << ioName[i];
   output << "\n);" << endl;
   // Output Verilog I/O Declarations
   uint32_t ioIndex = 0;
   if (ntk->getLatchSize()) {
      output << "\n" << V3_INDENT << "// Clock Signal for Synchronous DFF" << endl;
      output << V3_INDENT << "input " << ioName[ioIndex++] << ";" << endl;
   }
   output << "\n" << V3_INDENT << "// I/O Declarations" << endl;
   for (uint32_t i = 0; i < ntk->getInputSize(); ++i) {
      output << V3_INDENT << "input ";
      if (ntk->getNetWidth(ntk->getInput(i)) > 1) output << "[" << ntk->getNetWidth(ntk->getInput(i)) - 1 << ":0] ";
      assert (ioIndex < ioName.size()); output << ioName[ioIndex++] << ";" << endl;
   }
   for (uint32_t i = 0; i < ntk->getInoutSize(); ++i) {
      output << V3_INDENT << "inout ";
      if (ntk->getNetWidth(ntk->getInout(i)) > 1) output << "[" << ntk->getNetWidth(ntk->getInout(i)) - 1 << ":0] ";
      assert (ioIndex < ioName.size()); output << ioName[ioIndex++] << ";" << endl;
   }
   for (uint32_t i = 0; i < ntk->getOutputSize(); ++i) {
      output << V3_INDENT << "output ";
      if (ntk->getNetWidth(ntk->getOutput(i)) > 1) output << "[" << ntk->getNetWidth(ntk->getOutput(i)) - 1 << ":0] ";
      assert (ioIndex < ioName.size()); output << ioName[ioIndex++] << ";" << endl;
   }
   assert (ioIndex == ioName.size());
   // Output Verilog Wire and Reg Declarations
   output << "\n" << V3_INDENT << "// Wire and Reg Declarations" << endl; id.id = 0;
   for (id.id = 0; id.id < ntk->getNetSize(); ++id.id) {
      output << V3_INDENT << ((V3_FF == ntk->getGateType(id)) ? "reg " : "wire ");
      if (ntk->getNetWidth(id) > 1) output << "[" << ntk->getNetWidth(id) - 1 << ":0] ";
      output << rtlName[id.id] << ";" << endl;
   }
   // Output Verilog Output Names if Eliminated
   V3Vec<string>::Vec outAssign; outAssign.clear();
   for (uint32_t i = 0, j = ioName.size() - ntk->getOutputSize(); i < ntk->getOutputSize(); ++i) {
      id = ntk->getOutput(i); string name = rtlName[id.id];
      assert (name.size()); if (ioName[j + i] == name) continue; 
      if (!outAssign.size()) output << "\n" << V3_INDENT << "// Output Net Declarations" << endl;
      output << V3_INDENT << "wire "; if (ntk->getNetWidth(id) > 1) output << "[" << ntk->getNetWidth(id) - 1 << ":0] ";
      output << ioName[j + i] << ";" << endl; outAssign.push_back("assign " + ioName[j + i] + " = " + V3RTLName(id));
   }
   // Output Verilog Inout Assignments
   if (ntk->getInoutSize()) {
      output << "\n" << V3_INDENT << "// Inout Assignments" << endl;
      Msg(MSG_WAR) << "Inout Assignment has Not been Supported Yet !!" << endl;
   }
   // Output Verilog Combinational Assignments
   output << "\n" << V3_INDENT << "// Combinational Assignments" << endl;
   V3BvNtk* const bvNtk = dynamic_cast<V3BvNtk*>(ntk);
   V3GateType type; V3NetId id1, id2, id3;
   for (id.id = 0; id.id < ntk->getNetSize(); ++id.id) {
      type = ntk->getGateType(id); assert (V3_XD > type); if (V3_FF >= type) continue;
      output << V3_INDENT << "assign " << rtlName[id.id] << " = ";
      if (AIG_NODE == type || isV3PairType(type)) {
         id1 = ntk->getInputNetId(id, 0); id2 = ntk->getInputNetId(id, 1);
         if (type == BV_MERGE) output << "{" << V3RTLName(id1) << ", " << V3RTLName(id2) << "};" << endl;
         else output << V3RTLName(id1) << V3TypeRTL[type] << V3RTLName(id2) << ";" << endl;
      }
      else if (isV3ReducedType(type)) {
         id1 = ntk->getInputNetId(id, 0); if (id1.cp) { output << "~"; id1.cp = 0; }
         output << V3TypeRTL[type] << V3RTLName(id1) << ";" << endl;
      }
      else if (BV_MUX == type) {
         id1 = ntk->getInputNetId(id, 0); id2 = ntk->getInputNetId(id, 1); id3 = ntk->getInputNetId(id, 2);
         output << V3RTLName(id3) << " ? " << V3RTLName(id2) << " : " << V3RTLName(id1) << ";" << endl;
      }
      else if (BV_SLICE == type) {
         id1 = ntk->getInputNetId(id, 0); assert (bvNtk);
         if (bvNtk->getInputSliceBit(id, true) != bvNtk->getInputSliceBit(id, false)) 
            output << V3RTLName(id1) << "[" << bvNtk->getInputSliceBit(id, true) << ":" 
                   << bvNtk->getInputSliceBit(id, false) << "];" << endl;
         else output << V3RTLName(id1) << "[" << bvNtk->getInputSliceBit(id, true) << "];" << endl;
      }
      else if (BV_CONST == type) { assert (bvNtk); output << *(bvNtk->getInputConstValue(id)) << "; " << endl; }
      else { assert (AIG_FALSE == type); output << "1'b0;" << endl; }
   }
   // Output Verilog Eliminated Output Assignments
   if (outAssign.size()) output << "\n" << V3_INDENT << "// Output Net Assignments" << endl;
   for (uint32_t i = 0; i < outAssign.size(); ++i) output << V3_INDENT << outAssign[i] << ";" << endl;
   // Output Verilog Initial State
   if (initial && ntk->getLatchSize()) {
      output << "\n" << V3_INDENT << "// Initial State" << endl;
      output << V3_INDENT << "initial begin" << endl;
      for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) {
         id2 = ntk->getInputNetId(ntk->getLatch(i), 1);
         output << V3_INDENT << V3_INDENT << rtlName[ntk->getLatch(i).id] << " = " << V3RTLName(id2) << ";" << endl;
      }
      output << V3_INDENT << "end" << endl;
   }
   // Output Verilog Non-blocking Assignments
   if (ntk->getLatchSize()) {
      output << "\n" << V3_INDENT << "// Non-blocking Assignments" << endl;
      output << V3_INDENT << "always @ (posedge " << clockName << ") begin" << endl;
      for (uint32_t i = 0; i < ntk->getLatchSize(); ++i) {
         id1 = ntk->getInputNetId(ntk->getLatch(i), 0);
         output << V3_INDENT << V3_INDENT << rtlName[ntk->getLatch(i).id] << " <= " << V3RTLName(id1) << ";" << endl;
      }
      output << V3_INDENT << "end" << endl;
   }
   output << "endmodule" << endl; output.close(); const_cast<V3NtkHandler*>(handler)->resetAuxRenaming();
}

#endif

