/****************************************************************************
  FileName     [ v3CmdComm.cpp ]
  PackageName  [ v3/src/cmd ]
  Synopsis     [ Global Commands Implementation ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyright(c) 2011 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_CMD_COMM_C
#define V3_CMD_COMM_C

#include "v3StrUtil.h"
#include "v3Usage.h"
#include "v3Msg.h"
#include "v3CmdComm.h"

bool initCommonCmd() {
   return (
         v3CmdMgr->regCmd("DOfile",       2,    new V3DofileCmd   ) &&
         v3CmdMgr->regCmd("HELp",         3,    new V3HelpCmd     ) &&
         v3CmdMgr->regCmd("HIStory",      3,    new V3HistoryCmd  ) &&
         v3CmdMgr->regCmd("USAGE",        5,    new V3UsageCmd    ) &&
         v3CmdMgr->regCmd("Quit",         1,    new V3QuitCmd     ) &&
         v3CmdMgr->regCmd("SET LOgfile",  3, 2, new V3LogFileCmd  )
   );
}

// Global Variables
extern V3Usage    v3Usage;

//----------------------------------------------------------------------
// HELp [(string cmd)]
//----------------------------------------------------------------------
V3CmdExecStatus
V3HelpCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);
   
   if (options.size() == 0) v3CmdMgr->printHelps();  // without cmd
   else {  // with cmd
      string cmd = "";
      for (unsigned i = 0, n = options.size(); i < n; ++i) { if (i) cmd.append(" "); cmd.append(options[i]); }
      V3CmdExec* e = v3CmdMgr->getCmd(cmd);
      if (e) { e->usage(); return CMD_EXEC_DONE; }  // if exact match
      V3CmdExecSubSet list = v3CmdMgr->getCmdListFromPart(cmd);
      if (list.size()) {  // if partial match
         for (V3CmdExecSubSet::iterator it = list.begin(); it != list.end(); ++it) (*it)->help();
         return CMD_EXEC_DONE;
      }
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, cmd); 
   }
   
   return CMD_EXEC_DONE;
}

void
V3HelpCmd::usage() const {
   Msg(MSG_IFO) << "Usage: HELp [(string cmd)]" << endl;
}

void
V3HelpCmd::help() const {
   Msg(MSG_IFO) << setw(20) << left << "HELp: " << "Print this help message." << endl;
}

//----------------------------------------------------------------------
// Quit [-Force]
//----------------------------------------------------------------------
V3CmdExecStatus
V3QuitCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   if (options.size() > 1) return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
   else if (options.size()) {
      if (v3StrNCmp("-Forced", options[0], 2) == 0) return CMD_EXEC_QUIT;
      else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   }

   Msg(MSG_IFO) << "Are you sure to quit (Yes/No)? [No] ";
   char str[1024]; cin.getline(str, 1024);
   string ss = string(str);
   if (ss.size()) {
      size_t s = ss.find_first_not_of(' ', 0);
      if (s != string::npos)
         ss = ss.substr(s);
   }
   if (v3StrNCmp("Yes", ss, 1) == 0) return CMD_EXEC_QUIT;
   else return CMD_EXEC_DONE;
}

void
V3QuitCmd::usage() const {
   Msg(MSG_IFO) << "Usage: Quit [-Force]" << endl;
}

void
V3QuitCmd::help() const
{
   Msg(MSG_IFO) << setw(20) << left << "Quit: " << "Quit the execution." << endl;
}

//----------------------------------------------------------------------
// HIStory [(int nPrint)]
//----------------------------------------------------------------------
V3CmdExecStatus
V3HistoryCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   if (options.size() > 1) return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
   int nPrint = -1;
   if (options.size() && !v3Str2Int(options[0], nPrint)) return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   v3CmdMgr->printHistory(nPrint);
   return CMD_EXEC_DONE;
}

void
V3HistoryCmd::usage() const {
   Msg(MSG_IFO) << "Usage: HIStory [(int nPrint)]" << endl;
}

void
V3HistoryCmd::help() const {
   Msg(MSG_IFO) << setw(20) << left << "HIStory: " << "Print command history." << endl;
}

//----------------------------------------------------------------------
// DOfile <(string filename)>
//----------------------------------------------------------------------
V3CmdExecStatus
V3DofileCmd::exec(const string& option) {     
   vector<string> options;
   V3CmdExec::lexOptions(option, options);
   
   if (options.size() == 0) return V3CmdExec::errorOption(CMD_OPT_MISSING, "<(string filename)>");
   else if (options.size() > 1) return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
   else if (!v3CmdMgr->openDofile(options[0])) {
      v3CmdMgr->closeDofile(); 
      return V3CmdExec::errorOption(CMD_OPT_FOPEN_FAIL, options[0]);
   }
   return CMD_EXEC_DONE;
}

void
V3DofileCmd::usage() const {  
  Msg(MSG_IFO) << "Usage: DOfile <(string filename)>" << endl;
}  
      
void
V3DofileCmd::help() const {
   Msg(MSG_IFO) << setw(20) << left << "DOfile: " << "Execute the commands in the dofile." << endl;
}

//----------------------------------------------------------------------
// USAGE [| -All | -Time | -Memory]
//----------------------------------------------------------------------
V3CmdExecStatus
V3UsageCmd::exec(const string& option) {     
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   bool repTime = false, repMem = false, repAll = false;
   size_t n = options.size();
   if (n == 0) repAll = true;
   else {
      for (size_t i = 0; i < n; ++i) {
         const string& token = options[i];
         if (v3StrNCmp("-All", token, 2) == 0) {
            if (repTime | repMem | repAll)
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else repAll = true; 
         }
         else if (v3StrNCmp("-Time", token, 2) == 0) {
            if (repTime | repMem | repAll)
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else repTime = true; 
         }
         else if (v3StrNCmp("-Memory", token, 2) == 0) {
            if (repTime | repMem | repAll)
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else repMem = true;
         }
         else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
   }
   if (repAll) repTime = repMem = true;

   v3Usage.report(repTime, repMem);
   return CMD_EXEC_DONE;
}

void
V3UsageCmd::usage() const {  
   Msg(MSG_IFO) << "Usage: USAGE [| -All | -Time | -Memory]" << endl;
}  
      
void
V3UsageCmd::help() const {
   Msg(MSG_IFO) << setw(20) << left << "USAGE: " << "Report the runtime and / or memory usage." << endl;
}

//----------------------------------------------------------------------
// SET LOgfile [| -All | -Cmd | -Error | -Warning | -Info | -Debug]
//             <(string filename)> [| -Fileonly | -Both] [-APPend]
//----------------------------------------------------------------------
V3CmdExecStatus
V3LogFileCmd::exec(const string& option) {
   vector<string> options;
   V3CmdExec::lexOptions(option, options);

   bool all = false, cmd = false, error = false, warning = false, info = false, debug = false;
   bool fileonly = false, both = false;
   bool append = false;
   string filename = "";
   size_t n = options.size();

   if (n == 0) all = true;
   else {
      for (size_t i = 0; i < n; ++i) {
         const string& token = options[i];
         if (v3StrNCmp("-APPend", token, 4) == 0) {
            if (append) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            append = true;
         }
         else if (v3StrNCmp("-All", token, 2) == 0) {
            if (all || cmd || error || warning || info || debug)
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else all = true;
         }
         else if (v3StrNCmp("-Cmd", token, 2) == 0) {
            if (all || cmd || error || warning || info || debug) 
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else cmd = true;
         }
         else if (v3StrNCmp("-Error", token, 2) == 0) {
            if (all || cmd || error || warning || info || debug) 
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else error = true;
         }
         else if (v3StrNCmp("-Warning", token, 2) == 0) {
            if (all || cmd || error || warning || info || debug) 
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else warning = true;
         }
         else if (v3StrNCmp("-Info", token, 2) == 0) {
            if (all || cmd || error || warning || info || debug) 
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else info = true;
         }
         else if (v3StrNCmp("-Debug", token, 2) == 0) {
            if (all || cmd || error || warning || info || debug) 
               return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            else debug = true;
         }
         else if (v3StrNCmp("-Fileonly", token, 2) == 0) {
            if (fileonly || both) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            fileonly = true;
         }
         else if (v3StrNCmp("-Both", token, 2) == 0) {
            if (fileonly || both) return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
            both = true;
         }
         else {
            if (filename == "") filename = token;
            else return V3CmdExec::errorOption(CMD_OPT_EXTRA, token);
         }
      }
   }

   if (filename == "") return V3CmdExec::errorOption(CMD_OPT_MISSING, "<(string filename)>");
   if (!(all || cmd || error || warning || info || debug)) all = true;
   if (!(fileonly || both)) fileonly = true;

   if (all) { 
      Msg.setAllOutFile(filename);
      Msg.startAllOutFile(append);
      if (fileonly) Msg.stopAllDefault();
      else Msg.startAllDefault();
   }
   else {
      V3MsgType type = (cmd) ? MSG_LOG : (error) ? MSG_ERR : (warning) ? MSG_WAR : (info) ? MSG_IFO : MSG_DBG;
      Msg.setOutFile(filename, type);
      Msg.startOutFile(type, append);
      if (fileonly) Msg.stopDefault(type);
      else if (type != MSG_LOG) Msg.startDefault(type);
   }
   return CMD_EXEC_DONE;
}

void
V3LogFileCmd::usage() const {
   Msg(MSG_IFO) << "Usage: SET LOgfile [| -All | -Cmd | -Error | -Warning | -Info | -Debug]" << endl;
   Msg(MSG_IFO) << "                   <(string filename)> [| -Fileonly | -Both] [-APPend]" << endl;
}

void
V3LogFileCmd::help() const {
   Msg(MSG_IFO) << setw(20) << left << "SET LOgfile: " << "Copy and/or redirect messages to assigned file." << endl;
}

#endif

