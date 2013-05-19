/****************************************************************************
  FileName     [ v3VrfCmd.cpp ]
  PackageName  [ v3/src/vrf ]
  Synopsis     [ Verification Commands ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_VRF_CMD_C
#define V3_VRF_CMD_C

#include "v3Msg.h"
#include "v3VrfCmd.h"
#include "v3VrfSIM.h"
#include "v3VrfUMC.h"
#include "v3ExtUtil.h"
#include "v3NtkUtil.h"
#include "v3StrUtil.h"
#include "v3VrfResult.h"
#include "v3NtkElaborate.h"

#include <iomanip>

bool initVrfCmd() {
   // Reset Verification Settings
   V3VrfBase::resetReportSettings(); V3VrfBase::resetSolverSettings();
   return (
         // Verification Verbosity Manipulation Commands
         v3CmdMgr->regCmd("SET REport",         3, 2, new V3SetReportCmd       ) &&
         v3CmdMgr->regCmd("PRInt REport",       3, 2, new V3PrintReportCmd     ) &&
         v3CmdMgr->regCmd("SET SOlver",         3, 2, new V3SetSolverCmd       ) &&
         v3CmdMgr->regCmd("PRInt SOlver",       3, 2, new V3PrintSolverCmd     ) &&
         // Verification Property Setting Commands
         v3CmdMgr->regCmd("SET PRoperty",       3, 2, new V3SetPropertyCmd     ) &&
         // Verification Main Commands
         v3CmdMgr->regCmd("VERify SIM",         3, 3, new V3SIMVrfCmd          ) &&
         v3CmdMgr->regCmd("VERify UMC",         3, 3, new V3UMCVrfCmd          ) &&
         // Verification Result Reporting Commands
         v3CmdMgr->regCmd("CHEck REsult",       3, 2, new V3CheckResultCmd     ) &&
         v3CmdMgr->regCmd("WRIte REsult",       3, 2, new V3WriteResultCmd     ) &&
         v3CmdMgr->regCmd("PLOt REsult",        3, 2, new V3PlotResultCmd      )
   );
}

//----------------------------------------------------------------------
// SET REport [-All] [-RESUlt] [-Endline] [-Solver] [-Usage]
//            [-Profile] [-ON |-OFF |-RESET] 
//----------------------------------------------------------------------
V3CmdExecStatus
V3SetReportCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   bool all = false, result = false, endline = false, solver = false, usage = false, profile = false;
   bool reset = false, on = false, off = false;
   
   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (v3StrNCmp("-All", token, 2) == 0) {
         if (all) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else all = true;
      }
      else if (v3StrNCmp("-RESUlt", token, 5) == 0) {
         if (result) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else result = true;
      }
      else if (v3StrNCmp("-Endline", token, 2) == 0) {
         if (endline) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else endline = true;
      }
      else if (v3StrNCmp("-Solver", token, 2) == 0) {
         if (solver) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else solver = true;
      }
      else if (v3StrNCmp("-Usage", token, 2) == 0) {
         if (usage) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else usage = true;
      }
      else if (v3StrNCmp("-Profile", token, 2) == 0) {
         if (profile) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else profile = true;
      }
      else if (v3StrNCmp("-ON", token, 3) == 0) {
         if (reset || on || off) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else on = true;
      }
      else if (v3StrNCmp("-OFF", token, 4) == 0) {
         if (reset || on || off) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else off = true;
      }
      else if (v3StrNCmp("-RESET", token, 6) == 0) {
         if (reset || on || off) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else reset = true;
      }
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }

   if (!(on || off || reset)) on = true;
   if (reset) all = true;
   if (all) result = endline = solver = usage = profile = true;

   // Set Verification Verbosities
   if (reset) V3VrfBase::resetReportSettings();
   else {
      if (result)  V3VrfBase::setRstOnly(off);
      if (all)     V3VrfBase::setReport (on);
      if (endline) V3VrfBase::setEndline(on);
      if (solver)  V3VrfBase::setSolver (on);
      if (usage)   V3VrfBase::setUsage  (on);
      if (profile) V3VrfBase::setProfile(on);
   }
   // Print Verification Report Verbosities
   V3PrintReportCmd printReport; return printReport.exec("");
}

void
V3SetReportCmd::usage() const {
   Msg(MSG_IFO) << "Usage: SET REport [-All] [-RESUlt] [-Endline] [-Solver] [-Usage] [-Profile]" << endl;
   Msg(MSG_IFO) << "                  [-ON |-OFF |-RESET]" << endl;
   Msg(MSG_IFO) << "Note : -RESET will reset all settings to default" << endl;
   Msg(MSG_IFO) << "Verbosities are : " << endl;
   Msg(MSG_IFO) << "                  -RESUlt : Toggles interactive verification status report." << endl;
   Msg(MSG_IFO) << "                  -Endline: Toggles endline or carriage return in report."   << endl;
   Msg(MSG_IFO) << "                  -Solver : Toggles solver information report."              << endl;
   Msg(MSG_IFO) << "                  -Usage  : Toggles verification time usag report."          << endl;
   Msg(MSG_IFO) << "                  -Profile: Toggles checker specific profiling report."      << endl;
   Msg(MSG_IFO) << "                  -All    : All of the above, optional for -RESET."          << endl;
}

void
V3SetReportCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "SET REport: " << "Set Verification Report Verbosities." << endl;
}

//----------------------------------------------------------------------
// PRInt REport
//----------------------------------------------------------------------
V3CmdExecStatus
V3PrintReportCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   if (options.size()) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   Msg(MSG_IFO) << "Verification Report : \"";
   V3VrfBase::printReportSettings();
   Msg(MSG_IFO) << "\" ON" << endl;
   return CMD_EXEC_DONE;
}

void
V3PrintReportCmd::usage() const {
   Msg(MSG_IFO) << "Usage: PRInt REport" << endl;
}

void
V3PrintReportCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "PRInt REport: " << "Print Verification Report Verbosities." << endl;
}

//----------------------------------------------------------------------
// SET SOlver [|-Minisat |-Boolector |-MAthsat]
//----------------------------------------------------------------------
V3CmdExecStatus
V3SetSolverCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   bool msat = false, boolector = false, mathsat = false;
   
   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (v3StrNCmp("-Minisat", token, 2) == 0) {
         if (msat || boolector || mathsat) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else msat = true;
      }
      else if (v3StrNCmp("-Boolector", token, 2) == 0) {
         if (msat || boolector || mathsat) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else boolector = true;
      }
      else if (v3StrNCmp("-MAthsat", token, 3) == 0) {
         if (msat || boolector || mathsat) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else mathsat = true;
      }
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }

   if (!(msat || boolector || mathsat)) msat = true;

   // Set Verification Solvers
   if (msat) V3VrfBase::setSolver(V3_SVR_MINISAT);
   else if (boolector) V3VrfBase::setSolver(V3_SVR_BOOLECTOR);
   else if (mathsat) Msg(MSG_WAR) << "MathSAT has not been incorporated into V3 !!" << endl;
   else V3VrfBase::resetSolverSettings();
   // Print Verification Solvers
   V3PrintSolverCmd printSolver; return printSolver.exec("");

   return CMD_EXEC_DONE;
}

void
V3SetSolverCmd::usage() const {
   Msg(MSG_IFO) << "Usage: SET SOlver [|-Minisat |-Boolector |-MAthsat]" << endl;
}

void
V3SetSolverCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "SET SOlver: " << "Set Active Solvers for Verification." << endl;
}

//----------------------------------------------------------------------
// PRInt SOlver
//----------------------------------------------------------------------
V3CmdExecStatus
V3PrintSolverCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   if (options.size()) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   Msg(MSG_IFO) << "Active Solvers: \"";
   V3VrfBase::printSolverSettings();
   Msg(MSG_IFO) << "\"" << endl;
   return CMD_EXEC_DONE;
}

void
V3PrintSolverCmd::usage() const {
   Msg(MSG_IFO) << "Usage: PRInt SOlver" << endl;
}

void
V3PrintSolverCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "PRInt SOlver: " << "Print Active Solvers for Verification." << endl;
}

//----------------------------------------------------------------------
// SET PRoperty [-Name <(string propertyName)>] [|-Safety |-Liveness] 
//              <-Output (string outputIndex)>
//----------------------------------------------------------------------
V3CmdExecStatus
V3SetPropertyCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   bool safe = false, live = false;
   bool name = false, nameON = false;
   string propertyName = "", pFormula = "";
   uint32_t outputIndex = V3NtkUD;

   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (v3StrNCmp("-Safety", token, 2) == 0) {
         if (safe || live) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else if (nameON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
         else safe = true;
      }
      else if (v3StrNCmp("-Liveness", token, 2) == 0) {
         if (safe || live) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else if (nameON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
         else live = true;
      }
      else if (v3StrNCmp("-Name", token, 2) == 0) {
         if (name) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else name = nameON = true;
      }
      else if (nameON) { propertyName = token; nameON = false; }
      else if (V3NtkUD == outputIndex) {
         int temp; if (!v3Str2Int(token, temp)) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
         if (temp < 0) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token); outputIndex = (uint32_t)temp;
      }
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }

   if (!(safe || live)) safe = true;
   if (nameON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
   if (V3NtkUD == outputIndex) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string outputIndex)");

   V3NtkHandler* const handler = v3Handler.getCurHandler();
   if (handler) {
      // Create LTL Formula
      V3LTLFormula* f = 0;
      if (!propertyName.size()) propertyName = handler->getAuxPropertyName();
      if (handler->existProperty(propertyName))
         Msg(MSG_ERR) << "Property has Already Exists : " << *(handler->getProperty(propertyName)) << " !!" << endl;
      else {
         assert (propertyName.size()); assert (!handler->existProperty(propertyName));
         if (outputIndex < handler->getNtk()->getOutputSize())
            f = new V3LTLFormula(handler, ~(handler->getNtk()->getOutput(outputIndex)), safe, propertyName);
         else Msg(MSG_ERR) << "Output with Index " << outputIndex << " does NOT Exist in Current Ntk !!" << endl;
      }
      // Add to property data base
      if (f && f->isValid()) {
         V3Property* const p = new V3Property(f); assert (p);
         // Set Property
         handler->setProperty(p);
         Msg(MSG_IFO) << "Property " << propertyName << " has been set sucessfully !!" << endl;
      }
      else {
         Msg(MSG_ERR) << "Failed to Create Property " << propertyName << " !!" << endl;
         if (f) delete f;
      }
   }
   else Msg(MSG_ERR) << "Empty Ntk !!" << endl;
   return CMD_EXEC_DONE;
}

void
V3SetPropertyCmd::usage() const {
   Msg(MSG_IFO) << "Usage: SET PRoperty [-Name <(string propertyName)>] [|-Safety |-Liveness]" << endl;
   Msg(MSG_IFO) << "                    <(string outputIndex)>" << endl;
}

void
V3SetPropertyCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "SET PRoperty: " << "Set Property to Current Network." << endl;
}

//----------------------------------------------------------------------
// VERify SIM <(string propertyName)>
//            [<-Time (unsigned MaxTime)>] [<-Cycle (unsigned MaxCycle)>]
//----------------------------------------------------------------------
V3CmdExecStatus
V3SIMVrfCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   bool time = false, cycle = false;
   bool timeON = false, cycleON = false;
   string propertyName = "";
   uint32_t maxTime = 0, maxCycle = 0;
   
   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (v3StrNCmp("-Time", token, 2) == 0) {
         if (time) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else time = timeON = true;
      }
      else if (v3StrNCmp("-Cycle", token, 2) == 0) {
         if (cycle) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else cycle = cycleON = true;
      }
      else if (timeON) {
         int temp; if (!v3Str2Int(token, temp)) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
         if (temp <= 0) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
         maxTime = (uint32_t)temp; assert (maxTime); timeON = false;
      }
      else if (cycleON) {
         int temp; if (!v3Str2Int(token, temp)) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
         if (temp <= 0) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
         maxCycle = (uint32_t)temp; assert (maxCycle); cycleON = false;
      }
      else if (propertyName == "") propertyName = token;
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }
   
   if (timeON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned MaxTime)");
   if (cycleON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned MaxCycle)");

   // Get Properties to be Verified
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   if (handler) {
      if (propertyName.size()) {
         if (handler->existProperty(propertyName)) {
            // Elaborate Property
            V3Property* const property = handler->getProperty(propertyName); assert (property);
            V3NtkElaborate* const pNtk = new V3NtkElaborate(handler); assert (pNtk);
            pNtk->elaborateProperty(property);
            // Initialize Checker
            V3VrfSIM* const checker = new V3VrfSIM(pNtk); assert (checker);
            // SIM Specific Settings
            if (maxTime) checker->setTimeBound(maxTime);
            if (maxCycle) checker->setMaxDepth(maxCycle);
            checker->printSettings(); checker->verifyInOrder();
            // Set Verification Result
            if (checker->getResult(0).isCex())
               property->setResult(checker->getResult(0));
            // Free Checker and Elaborated Ntk
            delete checker; delete pNtk;
         }
         else Msg(MSG_ERR) << "Property Not Found !!" << endl;
      }
      else {
         return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
         Msg(MSG_IFO) << "Totally " << handler->getPropertySize() << " Properties to be Verified !!" << endl;
         Msg(MSG_WAR) << "Currently do not support multiple properties under verification !!" << endl;
      }
   }
   else Msg(MSG_ERR) << "Empty Ntk !!" << endl;
   return CMD_EXEC_DONE;
}

void
V3SIMVrfCmd::usage() const {
   Msg(MSG_IFO) << "Usage: VERify SIM <(string propertyName)>" << endl;
   Msg(MSG_IFO) << "                  [<-Time (unsigned MaxTime)>] [<-Cycle (unsigned MaxCycle)>]" << endl;
}

void
V3SIMVrfCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "VERify SIM: " << "Perform (Constrained) Random Simulation." << endl;
}

//----------------------------------------------------------------------
// VERify UMC <(string propertyName)>
//            [-Max-depth (unsigned MaxDepth)]
//            [-Pre-depth (unsigned PreDepth)]
//            [-Inc-depth (unsigned IncDepth)]
//            [-NOProve]
//----------------------------------------------------------------------
V3CmdExecStatus
V3UMCVrfCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   string propertyName = "";
   bool maxD = false, preD = false, incD = false;
   bool maxDON = false, preDON = false, incDON = false;
   uint32_t maxDepth = 0, preDepth = 0, incDepth = 0;
   bool noProve = false;
   
   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (v3StrNCmp("-NOProve", token, 4) == 0) {
         if (noProve) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else if (maxDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned MaxDepth)");
         else if (preDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned PreDepth)");
         else if (incDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned IncDepth)");
         else noProve = true;
      }
      else if (v3StrNCmp("-Max-depth", token, 2) == 0) {
         if (maxD) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else if (preDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned PreDepth)");
         else if (incDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned IncDepth)");
         else maxD = maxDON = true;
      }
      else if (v3StrNCmp("-Pre-depth", token, 2) == 0) {
         if (preD) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else if (maxDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned MaxDepth)");
         else if (incDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned IncDepth)");
         else preD = preDON = true;
      }
      else if (v3StrNCmp("-Inc-depth", token, 2) == 0) {
         if (incD) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else if (maxDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned MaxDepth)");
         else if (preDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned PreDepth)");
         else incD = incDON = true;
      }
      else if (maxDON || preDON || incDON) {
         int temp; if (!v3Str2Int(token, temp)) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
         if (temp < 0) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
         if (maxDON) { maxDepth = (uint32_t)temp; maxDON = false; }
         else if (preDON) { preDepth = (uint32_t)temp; preDON = false; }
         else { incDepth = (uint32_t)temp; incDON = false; }
      }
      else if (propertyName == "") propertyName = token;
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }
   
   if (maxDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned MaxDepth)");
   if (preDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned PreDepth)");
   if (incDON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(unsigned IncDepth)");

   // Get Properties to be Verified
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   if (handler) {
      if (propertyName.size()) {
         if (handler->existProperty(propertyName)) {
            // Elaborate Property
            V3Property* const property = handler->getProperty(propertyName); assert (property);
            V3NtkElaborate* const pNtk = new V3NtkElaborate(handler); assert (pNtk);
            pNtk->elaborateProperty(property);
            // Initialize Checker
            V3VrfUMC* const checker = new V3VrfUMC(pNtk); assert (checker);
            // UMC Specific Settings
            if (noProve) checker->setFireOnly();
            if (maxD) checker->setMaxDepth(maxDepth);
            if (preD) checker->setPreDepth(preDepth);
            if (incD) checker->setIncDepth(incDepth);
            checker->verifyInOrder();
            // Set Verification Result
            if (checker->getResult(0).isCex() || checker->getResult(0).isInv())
               property->setResult(checker->getResult(0));
            // Free Checker and Elaborated Ntk
            delete checker; delete pNtk;
         }
         else Msg(MSG_ERR) << "Property Not Found !!" << endl;
      }
      else {
         return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
         Msg(MSG_IFO) << "Totally " << handler->getPropertySize() << " Properties to be Verified !!" << endl;
         Msg(MSG_WAR) << "Currently do not support multiple properties under verification !!" << endl;
      }
   }
   else Msg(MSG_ERR) << "Empty Ntk !!" << endl;
   return CMD_EXEC_DONE;
}

void
V3UMCVrfCmd::usage() const {
   Msg(MSG_IFO) << "Usage: VERify UMC <(string propertyName)> [-Max-depth (unsigned MaxDepth)]" << endl;
   Msg(MSG_IFO) << "                  [-Inc-depth (unsigned IncDepth)] [-NOProve]" << endl;
}

void
V3UMCVrfCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "VERify UMC: " << "Perform Unbounded Model Checking (BMC, SAT-based)" << endl;
}

//----------------------------------------------------------------------
// CHEck REsult <(string propertyName)>
//              <[-Trace | -Invariant] <(string resultFileName)> >
//----------------------------------------------------------------------
V3CmdExecStatus
V3CheckResultCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   bool trace = false, invariant = false, inputON = false;
   string propertyName = "", resultFileName = "";

   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (v3StrNCmp("-Trace", token, 2) == 0) {
         if (trace || invariant) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else trace = inputON = true;
      }
      else if (v3StrNCmp("-Invariant", token, 2) == 0) {
         if (trace || invariant) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         else invariant = inputON = true;
      }
      else if (inputON) {
         resultFileName = token;
         inputON = false;
      }
      else if (propertyName == "") propertyName = token;
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }

   if (propertyName == "") return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
   if (inputON) return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string resultFileName)");

   // Get Properties to be Checked
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   if (handler) {
      if (handler->existProperty(propertyName)) {
         // Elaborate Property
         V3Property* const property = handler->getProperty(propertyName); assert (property);
         V3NtkElaborate* const pNtk = new V3NtkElaborate(handler); assert (pNtk);
         pNtk->elaborateProperty(property);
         // Set Verification Result from Input File if Specified
         if (trace) {  // Set Counterexample Trace as Input
            V3CexTrace* const cex = V3CexTraceParser(pNtk, resultFileName);
            if (!cex) Msg(MSG_ERR) << "Failed to Parse Counterexample Trace Data !!" << endl;
            else { V3VrfResult result; result.setCexTrace(cex); property->setResult(result); }
         }
         else if (invariant)  // Set Inductive Invariant as Input
            Msg(MSG_WAR) << "Currently Inductive Invariant cannot be recoreded !!" << endl;
         // Check Verification Result
         int result = 0;
         if (property->isFired()) {  // Check if a real counter-example is found
            result = simulationCheckFiredResult(property->getCexTrace(), pNtk);
            // Report Check Result
            if (result > 0) Msg(MSG_IFO) << "A real counter-example is found ";
            else if (result < 0) Msg(MSG_IFO) << "A spurious counter-example is found ";
            else Msg(MSG_IFO) << "Fail to check verification validation ";
            Msg(MSG_IFO) << "for property \"" << property->getLTLFormula()->getName() << "\"." << endl;
         }
         else if (property->isProven())  // Check if a real inductie invariant is found
            Msg(MSG_WAR) << "Inductive Invariant Cannot be Checked Currently !!" << endl;
         else Msg(MSG_ERR) << "Property \"" << property->getLTLFormula()->getName()
                           << "\" has not been verified !!" << endl;
         delete pNtk;  // Destruct Elaborated Ntk
      }
      else Msg(MSG_ERR) << "Property Not Found !!" << endl;
   }
   else Msg(MSG_ERR) << "Empty Ntk !!" << endl;
   return CMD_EXEC_DONE;
}

void
V3CheckResultCmd::usage() const {
   Msg(MSG_IFO) << "Usage: CHEck REsult <(string propertyName)> <[-Trace | -Invariant] <(string resultFileName)> >" 
                << endl;
}

void
V3CheckResultCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "CHEck REsult: " << "Verify Verification Result." << endl;
}

//----------------------------------------------------------------------
// WRIte REsult <(string propertyName)> <(string resultFileName)>
//----------------------------------------------------------------------
V3CmdExecStatus
V3WriteResultCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   string propertyName = "", resultFileName = "";

   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (propertyName == "") propertyName = token;
      else if (resultFileName == "") resultFileName = token;
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }

   if (propertyName == "") return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
   if (resultFileName == "") return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string resultFileName)");

   // Get Properties for Output
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   if (handler) {
      if (handler->existProperty(propertyName)) {
         // Elaborate Property
         V3Property* const property = handler->getProperty(propertyName); assert (property);
         V3NtkElaborate* const pNtk = new V3NtkElaborate(handler); assert (pNtk);
         pNtk->elaborateProperty(property);
         // Write Verification Results
         if (property->isFired())  // Write Counterexample Trace
            V3CexTraceWriter(pNtk, property->getCexTrace(), resultFileName);
         else if (property->isProven())  // Write Inductive Invariant
            Msg(MSG_WAR) << "Currently Inductive Invariant cannot be reported !!" << endl;
         else Msg(MSG_ERR) << "Property \"" << property->getLTLFormula()->getName()
                           << "\" has not been verified !!" << endl;
         delete pNtk;  // Destruct Elaborated Ntk
      }
      else Msg(MSG_ERR) << "Property Not Found !!" << endl;
   }
   else Msg(MSG_ERR) << "Empty Ntk !!" << endl;
   return CMD_EXEC_DONE;
}

void
V3WriteResultCmd::usage() const {
   Msg(MSG_IFO) << "Usage: WRIte REsult <(string propertyName)> <(string resultFileName)>" << endl;
}

void
V3WriteResultCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "WRIte REsult: " << "Write Counterexample Trace or Inductive Invariant." << endl;
}

//----------------------------------------------------------------------
// PLOt REsult <(string propertyName)> <(string resultFileName)> 
//----------------------------------------------------------------------
V3CmdExecStatus
V3PlotResultCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   string propertyName = "", resultFileName = "";

   size_t n = options.size();
   for (size_t i = 0; i < n; ++i) {
      const string& token = options[i];
      if (propertyName == "") propertyName = token;
      else if (resultFileName == "") resultFileName = token;
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }

   if (propertyName == "") return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string propertyName)");
   if (resultFileName == "") return V3CmdExec::errorOption(CMD_OPT_MISSING, "(string resultFileName)");

   // Get Properties for Output
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   if (handler) {
      if (handler->existProperty(propertyName)) {
         // Elaborate Property
         V3Property* const property = handler->getProperty(propertyName); assert (property);
         V3NtkElaborate* const pNtk = new V3NtkElaborate(handler); assert (pNtk);
         pNtk->elaborateProperty(property);
         // Plot Verification Results
         if (property->isFired())  // Plot Counterexample Trace
            V3CexTraceVisualizer(pNtk, property->getCexTrace(), resultFileName);
         else if (property->isProven())  // Write Inductive Invariant
            Msg(MSG_WAR) << "Currently Inductive Invariant cannot be reported !!" << endl;
         else Msg(MSG_ERR) << "Property \"" << property->getLTLFormula()->getName()
                           << "\" has not been verified !!" << endl;
         delete pNtk;  // Destruct Elaborated Ntk
      }
      else Msg(MSG_ERR) << "Property Not Found !!" << endl;
   }
   else Msg(MSG_ERR) << "Empty Ntk !!" << endl;
   return CMD_EXEC_DONE;
}

void
V3PlotResultCmd::usage() const {
   Msg(MSG_IFO) << "Usage: PLOt REsult <(string propertyName)> <(string resultFileName)>" << endl;
}

void
V3PlotResultCmd::help() const {
   Msg(MSG_IFO) << setw(20) << /*left <<*/ "PLOt REsult: " << "Plot Counterexample Trace or Inductive Invariant." << endl;
}

#endif

