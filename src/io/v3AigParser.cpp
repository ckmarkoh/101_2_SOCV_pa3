/****************************************************************************
  FileName     [ v3AigParser.cpp ]
  PackageName  [ v3/src/io ]
  Synopsis     [ AIG to V3 Network Parser. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_AIG_PARSER_C
#define V3_AIG_PARSER_C

#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3StrUtil.h"
#include "v3NtkParser.h"

#include <cstdio>

// Function aiger_getnoneofch : Referenced from AIGER Document
unsigned char aiger_getnoneofch(FILE* file) {
   int ch = getc(file);
   if (ch != EOF) return ch;
   Msg(MSG_ERR) << "Unexpected EOF of AIGER Input Found !!!" << endl;
   return EOF;
}

// Function decode : Referenced from AIGER Document
unsigned aiger_decode(FILE* file) {
   unsigned x = 0, i = 0;
   unsigned char ch;
   while ((ch = aiger_getnoneofch(file)) & 0x80)
      x |= (ch & 0x7f) << (7 * i++);
   return x | (ch << (7 * i));
}

bool aiger_symbol(FILE* file, unsigned char& c, uint32_t& index, string& name) {
   int ch = getc(file); if (EOF == ch) return false;
   c = (unsigned char)ch; if ('c' == c) return true; name = "";
   while (true) {
      ch = getc(file); if (EOF == ch) return false;
      if (' ' == ch && name.size()) break;
      else name += (unsigned char)ch;
   }
   int temp; if (!v3Str2Int(name, temp)) return false;
   if (temp < 0) return false; index = (uint32_t)temp; name = "";
   while (true) {
      ch = getc(file); if (EOF == ch) return false;
      if ('\n' == ch) break;
      else name += (unsigned char)ch;
   }
   return true;
}

// Main AIG Parsing Function
V3NtkInput* const V3AigParser(const char* fileName, const bool& symbol) {
   // Open Input AIGER File
   assert (fileName); FILE* input; input = fopen(fileName, "r");
   if (!input) { Msg(MSG_ERR) << "AIG Input File \"" << fileName << "\" Not Found !!" << endl; return 0; }
   // Start Parsing AIG Header
   unsigned m = 0, in = 0, l = 0, o = 0, a = 0;
   char header[8];
   // Parse Header
   if (fscanf(input, "%s %u %u %u %u %u", header, &m, &in, &l, &o, &a) != 6) {
      Msg(MSG_ERR) << "AIGER Header has Incorrect Format !!!" << endl; return 0;
   }
   else if ((strcmp(header, "aig")) || (m != (in + l + a))) {
      Msg(MSG_ERR) << "AIGER Header has Incorrect Format !!!" << endl; return 0;
   }
   // Create Network Handler
   V3NtkInput* aigHandler = new V3NtkInput(true);
   if (!aigHandler) { Msg(MSG_ERR) << "Create AIG Parser Failed !!" << endl; return 0; }
   if (!aigHandler->getNtk()) { Msg(MSG_ERR) << "Create AIG Network Failed !!" << endl; return 0; }
   V3AigNtk* const ntk = aigHandler->getNtk(); assert (ntk);
   // Start Parsing AIG Content
   V3NetVec vars(m + 1);  // For even number nodes (including AIG_FALSE)
   unsigned i, j;
   // Create Constant (AIG_FALSE)
   V3NetId aigConst = V3NetId::makeNetId(0); vars[0] = aigConst;
   // Create m Nets
   for (i = 1; i <= m; ++i) {
      vars[i] = aigHandler->createNet(symbol ? "" : v3Int2Str(i));
      if (V3NetUD == vars[i]) { delete aigHandler; return 0; }
      assert (i == vars[i].id);  // Obey with V3NetId Encoding
   }
   // Create Input Gates
   for (i = 1; i <= in; ++i)
      if (!createInput(ntk, vars[i])) { delete aigHandler; return 0; }
   // Create FF Gates
   V3NetType in1, in2;
   for (i = in + 1, j = in + l; i <= j; ++i) {
      if (fscanf(input, "%u", &in1.value) != 1) {
         Msg(MSG_ERR) << "Unexpected Latch Input id !!!" << endl; delete aigHandler; return 0;
      }
      // FF are initialized to AIG_FALSE by default
      if (!createV3FFGate(ntk, vars[i], in1.id, aigConst)) { delete aigHandler; return 0; }
   }
   // Create Outputs
   for (i = 0; i < o; ++i) {
      if (fscanf(input, "%u", &in1.value) != 1) {
         Msg(MSG_ERR) << "Unexpected Output Input id !!!" << endl; delete aigHandler; return 0;
      }
      if (!createOutput(ntk, in1.id)) { delete aigHandler; return 0; }
   }
   // Create AND Gates
   getc(input);  // End of Line Character
   for (i = in + l + 1, j = m; i <= j; ++i) {
      in1.value = aiger_decode(input); in1.value = (i << 1) - in1.value; //assert (in1 < (i << 1));
      in2.value = aiger_decode(input); in2.value = in1.value - in2.value; //assert (in2 <= in1);
      if (!createAigAndGate(ntk, vars[i], in2.id, in1.id)) { delete aigHandler; return 0; }
   }
   V3StrVec outputName(ntk->getOutputSize(), "");
   // Parse Symbol Table for Signal Names
   if (symbol) {
      unsigned char c; uint32_t ioIndex; string name;
      while (!feof(input)) {
         if (aiger_symbol(input, c, ioIndex, name)) {
            if ('i' == c) {  // Set PI Names
               if (ioIndex >= ntk->getInputSize()) {
                  Msg(MSG_ERR) << "Unexpected Symbol Item for Primary Input with Exceeded Index = \"" 
                               << ioIndex << "\" !!" << endl; delete aigHandler; return 0;
               }
               else if (!aigHandler->resetNetName(ntk->getInput(ioIndex).id, name)) {
                  delete aigHandler; return 0;
               }
            }
            else if ('l' == c) {  // Set Latch Names
               if (ioIndex >= ntk->getLatchSize()) {
                  Msg(MSG_ERR) << "Unexpected Symbol Item for Latch with Exceeded Index = \"" 
                               << ioIndex << "\" !!" << endl; delete aigHandler; return 0;
               }
               else if (!aigHandler->resetNetName(ntk->getLatch(ioIndex).id, name)) {
                  delete aigHandler; return 0;
               }
            }
            else if ('o' == c) {  // Set Output Names
               if (ioIndex >= ntk->getOutputSize()) {
                  Msg(MSG_ERR) << "Unexpected Symbol Item for Primary Output with Exceeded Index = \"" 
                               << ioIndex << "\" !!" << endl; delete aigHandler; return 0;
               }
               else if (outputName[ioIndex].size()) {
                  Msg(MSG_ERR) << "Name of Output " << ioIndex << " has Already Set !!" << endl;
                  delete aigHandler; return 0;
               }
               else outputName[ioIndex] = name;
            }
            else if ('c' == c) break;
            else {
               Msg(MSG_ERR) << "Unexpected Symbolic Prefix = \'" << c << "\' !!" << endl;
               delete aigHandler; return 0;
            }
         }
         else {
            Msg(MSG_ERR) << "Unexpected Format in the Symbolic Table !!" << endl;
            delete aigHandler; return 0;
         }
      }
   }
   // Set Output Names if Not Exists
   for (uint32_t i = 0; i < ntk->getOutputSize(); ++i) 
      if (outputName[i].size()) aigHandler->recordOutName(i, outputName[i]);
      else aigHandler->recordOutName(i, "aiger_output_" + v3Int2Str(i));
   fclose(input); assert (aigHandler); return aigHandler;
}

#endif

