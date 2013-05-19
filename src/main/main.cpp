/****************************************************************************
  FileName     [ main.cpp ]
  PackageName  [ main ]
  Synopsis     [ Main Function ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_MAIN_C
#define V3_MAIN_C

#include "v3Usage.h"
#include "v3CmdMgr.h"
#include "v3StrUtil.h"
#include "v3NtkHandler.h"

// Global Variables for V3MsgMgr
string V3Msg::_allName = "";
ofstream V3Msg::_allout;
V3MsgMgr Msg;

// Global Variable for V3Usage
V3Usage v3Usage;

// Global Variable for V3CmdMgr
V3CmdMgr* v3CmdMgr = new V3CmdMgr("socv"); // MODIFICATION FOR SoCV

// Global Variable for V3Handler
V3Handler v3Handler;

// Various Commands Registration
extern bool initAlgCmd();
extern bool initNtkCmd();
extern bool initVrfCmd();
extern bool initTransCmd();
extern bool initCommonCmd();
extern bool initBddCmd(); // MODIFICATION FOR SoCV BDD
extern bool initProveCmd(); // MODIFICATION FOR SoCV BDD
extern bool initSatCmd(); // MODIFICATION FOR SoCV BDD

// Program Usage
static void usage() {
   Msg(MSG_IFO) << "Usage: <v3> [ -File < dofile > ]" << endl; exit(-1);
}

int main(int argc, char** argv) {
   bool dof = false, dofON = false;
   string dofile = "";
   
   // Set Random Number Seed
   srand(time(NULL));

   for (int i = 1; i < argc; ++i) {
      if (v3StrNCmp("-File", argv[i], 2) == 0) {
         if (dof) { Msg(MSG_ERR) << "Extra Argument \"" << argv[i] << "\" !!" << endl; usage(); }
         else dof = dofON = true;
      }
      else if (dofON) { dofile = argv[i]; dofON = false; }
      else { Msg(MSG_ERR) << "Unknown Argument \"" << argv[i] << "\" !!" << endl; usage(); }
   }

   // Register Commands
   if (!(initAlgCmd() && initNtkCmd() && initVrfCmd() && initTransCmd() && initCommonCmd()
         && initBddCmd() && initProveCmd() && initSatCmd())) { // MODIFICATION FOR SoCV BDD
      Msg(MSG_ERR) << "Command Register Failed !!!" << endl; exit(0);
   }

   // Dofile
   if (dof) {
      if (dofile.size() == 0) { Msg(MSG_ERR) << "Missing Argument \"< dofile >\" !!" << endl; usage(); }
      else if (!v3CmdMgr->openDofile(dofile.c_str())) {
         Msg(MSG_ERR) << "Cannot Open File \"" << dofile << "\" !!" << endl; usage();
      }
   }
   // Start Program
   V3CmdExecStatus status = CMD_EXEC_DONE;
   while (status != CMD_EXEC_QUIT) {
      v3CmdMgr->setPrompt();
      status = v3CmdMgr->execOneCmd();
      cout << endl;
   }
   return 0;
}

#endif

