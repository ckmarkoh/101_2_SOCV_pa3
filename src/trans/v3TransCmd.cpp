/****************************************************************************
  FileName     [ v3TransCmd.cpp ]
  PackageName  [ v3/src/trans ]
  Synopsis     [ Ntk Transformation Commands ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_TRANS_CMD_C
#define V3_TRANS_CMD_C

#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3StrUtil.h"
#include "v3TransCmd.h"

#include "v3BvBlastAig.h"
#include "v3NtkHandler.h"

#include <iomanip>

bool initTransCmd() {
   return (
         v3CmdMgr->regCmd("BLAst NTk",    3, 2, new V3BlastNtkCmd)
   );
}

//----------------------------------------------------------------------
// BLAst NTk
//----------------------------------------------------------------------
V3CmdExecStatus
V3BlastNtkCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   size_t n = options.size();
   if (n) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   V3NtkHandler* const handler = v3Handler.getCurHandler();
   if (handler) {
      if (dynamic_cast<V3BvNtk*>(handler->getNtk())) {
         V3BvBlastAig* const blastHandler = new V3BvBlastAig(handler); assert (blastHandler);
         v3Handler.pushAndSetCurHandler(blastHandler);
      }
      else Msg(MSG_ERR) << "Current Network is Already an AIG Network !!" << endl;
   }
   else Msg(MSG_ERR) << "Empty Ntk !!" << endl;
   return CMD_EXEC_DONE;
}

void
V3BlastNtkCmd::usage() const {
   Msg(MSG_IFO) << "Usage: BLAst NTk" << endl;
}

void
V3BlastNtkCmd::help() const {
   Msg(MSG_IFO) << setw(20) << left << "BLAst NTk: " << "Bit-blast BV Network to AIG Network." << endl;
}

#endif

