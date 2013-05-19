/****************************************************************************
  FileName     [ satMgr.cpp ]
  PackageName  [ sat ]
  Synopsis     [ Define sat prove package interface ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3SvrMiniSat.h"
#include "v3NtkElaborate.h"
#include "v3VrfUMC.h"
#include "reader.h"
#include "satMgr.h"
using namespace std;

#define IND_UBMC  1
#define CBA_UBMC  0
#define PBA_UBMC  0
#define INTP_UBMC 0
#define PDR_UBMC  0

void SATMgr::verifyProperty(const string& name, const V3NetId& monitor) {
  // TODO: finish your own SAT-based property checking
  // BMC+INDUCTION(no simple path) are originally provided
  // simplified from vrf/v3VrfUMC.cpp

  // Initialize
  V3SvrMiniSat* satSolver
   = new V3SvrMiniSat(v3Handler.getCurHandler()->getNtk(), false);

  SatProofRes pRes;

  // Prove the monitor here!!
  #ifdef IND_UBMC
  pRes.setMaxDepth(100);
  pRes.setSatSolver(satSolver);
  SPindUbmc(monitor, pRes);
  #endif

  pRes.reportResult(name);
  if (pRes.isFired())
    pRes.reportCex(monitor);

  delete satSolver;
}

void SATMgr::SPindUbmc(const V3NetId& monitor, SatProofRes& pRes) {
  // Solver Data
  V3SvrData pFormulaData; V3PtrVec pFormula;
  uint32_t boundDepth = 1;
  uint32_t incDepth = 1;

  V3SvrMiniSat* satSolver = pRes.getSatSolver();

  // Start UMC Based Verification
  for (uint32_t i = 0, j = pRes.getMaxDepth(); i < j; ++i) {
    // Add time frame expanded circuit to SAT Solver
    satSolver->addBoundedVerifyData(monitor, i);
    pFormula.push_back(satSolver->getFormula(monitor, i));
    // Check if the bound is achieved
    if ((1 + i) < boundDepth){	
		continue; 
	}
	
//	for(uint32_t z=0; z=	

	cout<<"i:"<<i<<endl;
	assert ((1 + i) == boundDepth);
    assert ((1 + i) >= pFormula.size()); boundDepth += incDepth;
    // Add assume for assumption solve only
    satSolver->assumeRelease();
	cout<<"pFormula.size():"<<pFormula.size()<<endl;
    if (1 == pFormula.size()){
		satSolver->assumeProperty(monitor, false, i);
	}
    else {
      pFormulaData = satSolver->setImplyUnion(pFormula);
      assert (pFormulaData); satSolver->assumeProperty(pFormulaData);
    }
    satSolver->simplify();
    // Assumption Solve : If UNSAT, proved!
    if (!satSolver->assump_solve()) {
		cout<<"proved"<<endl;
      pRes.setProved(i); break;
    }
    // Assumption Solve : If SAT, disproved!
    satSolver->assumeInit(); // Conjunction with initial condition
    if (satSolver->assump_solve()) {
	  cout<<"disproved"<<endl;
      for (uint32_t k = 0; k < pFormula.size(); ++k){
        if ('0' != satSolver->getDataValue(pFormula[k])) {
          pRes.setFired(1 + i + k - pFormula.size()); break;
        }
	  }
      assert (pRes.isFired()); break;
    }
    // Add assert back to the property
    if (1 < pFormula.size()) {
      assert (pFormulaData); satSolver->assertProperty(pFormulaData, true); }
    for (uint32_t k = i - pFormula.size(); k < i; ++k)
      satSolver->assertProperty(monitor, true, k);
    pFormula.clear(); pFormulaData = 0;
  }
}




void SATMgr::indUbmc(const V3NetId& monitor, SatProofRes& pRes) {
  // Solver Data
  V3SvrData pFormulaData; V3PtrVec pFormula;
  uint32_t boundDepth = 1;
  uint32_t incDepth = 1;

  V3SvrMiniSat* satSolver = pRes.getSatSolver();

  // Start UMC Based Verification
  for (uint32_t i = 0, j = pRes.getMaxDepth(); i < j; ++i) {
    // Add time frame expanded circuit to SAT Solver
    satSolver->addBoundedVerifyData(monitor, i);
    pFormula.push_back(satSolver->getFormula(monitor, i));
    // Check if the bound is achieved
    if ((1 + i) < boundDepth) continue; assert ((1 + i) == boundDepth);
    assert ((1 + i) >= pFormula.size()); boundDepth += incDepth;
    // Add assume for assumption solve only
    satSolver->assumeRelease();
    if (1 == pFormula.size()) satSolver->assumeProperty(monitor, false, i);
    else {
      pFormulaData = satSolver->setImplyUnion(pFormula);
      assert (pFormulaData); satSolver->assumeProperty(pFormulaData);
    }
    satSolver->simplify();
    // Assumption Solve : If UNSAT, proved!
    if (!satSolver->assump_solve()) {
      pRes.setProved(i); break;
    }
    // Assumption Solve : If SAT, disproved!
    satSolver->assumeInit(); // Conjunction with initial condition
    if (satSolver->assump_solve()) {
      for (uint32_t k = 0; k < pFormula.size(); ++k)
        if ('0' != satSolver->getDataValue(pFormula[k])) {
          pRes.setFired(1 + i + k - pFormula.size()); break;
        }
      assert (pRes.isFired()); break;
    }
    // Add assert back to the property
    if (1 < pFormula.size()) {
      assert (pFormulaData); satSolver->assertProperty(pFormulaData, true); }
    for (uint32_t k = i - pFormula.size(); k < i; ++k)
      satSolver->assertProperty(monitor, true, k);
    pFormula.clear(); pFormulaData = 0;
  }
}

void SATMgr::reset() {
  _ptrMinisat = NULL;
  _varGroup.clear();
  _var2Net.clear();
  _isClauseOn.clear();
}

void SATMgr::markOnsetClause(const ClauseId& cid) {
  unsigned cSize = _ptrMinisat->_Solver->nClauses();
  assert(cid < (int)cSize);
  if(_isClauseOn.size() < cSize) {
    unsigned origSize = _isClauseOn.size();
    _isClauseOn.resize(cSize);
    for(unsigned i = origSize; i < cSize; ++i)
      _isClauseOn[i] = false;
  }
  _isClauseOn[cid] = true;
}

void SATMgr::markVarGroup(const Var& var, const VAR_GROUP& vg) {
  unsigned vSize = _ptrMinisat->_Solver->nVars();
  assert(var < (int)vSize);
  if(_varGroup.size() < vSize) {
    unsigned origSize = _varGroup.size();
    _varGroup.resize(vSize);
    for(unsigned i = origSize; i < vSize; ++i)
      _varGroup[i] = NONE;
  }
  _varGroup[var] = vg;
}

void SATMgr::mapVar2Net(const Var& var, const V3NetId& net) {
  assert(_var2Net.find(var) == _var2Net.end());
  _var2Net[var] = net;
}

bool SATMgr::solveWithProof(const bool& doAssump) {
  // clear previous proof histiry
  if(_ptrMinisat->_Solver->proof != NULL)
    delete _ptrMinisat->_Solver->proof;

  _ptrMinisat->_Solver->proof = new Proof();

  if(doAssump) return _ptrMinisat->assump_solve();
  return _ptrMinisat->solve();
}

V3NetId SATMgr::getItp() const {
  assert(_ptrMinisat);
  assert(_ptrMinisat->_Solver->proof);
  // save proof log
  string proofName = "socv_proof.itp";
  _ptrMinisat->_Solver->proof->save(proofName.c_str());

  // bulding ITP
  V3NetId netId = buildItp(proofName);

  // delete proof log
  unlink(proofName.c_str());
  delete _ptrMinisat->_Solver->proof;
  _ptrMinisat->_Solver->proof = NULL;

  return netId;
}

vector<ClauseId> SATMgr::getUNSATCore() const {
  assert(_ptrMinisat);
  assert(_ptrMinisat->_Solver->proof);

  vector<ClauseId> unsatCore;
  unsatCore.clear();

  // save proof log
  string proofName = "socv_proof.itp";
  _ptrMinisat->_Solver->proof->save(proofName.c_str());

  // generate unsat core
  Reader rdr;
  rdr.open( proofName.c_str() );
  retrieveProof( rdr, unsatCore );

  // delete proof log
  unlink(proofName.c_str());
  delete _ptrMinisat->_Solver->proof;
  _ptrMinisat->_Solver->proof = NULL;

  return unsatCore;
}

void SATMgr::retrieveProof( Reader& rdr, vector<ClauseId>& unsatCore) const {
  unsigned int tmp, cid, idx, tmp_cid;

  ///// Clear all /////
  vector<unsigned int> clausePos; clausePos.clear();
  unsatCore.clear();

  ///// Generate clausePos /////
  assert( !rdr.null() );
  rdr.seek(0);
  for( unsigned int pos = 0; (tmp = rdr.get64()) != RDR_EOF ; pos = rdr.Current_Pos() ){
    cid = clausePos.size();
    clausePos.push_back( pos );
    if((tmp & 1) == 0) {
      while((tmp = rdr.get64()) != 0) {}
    } else {
      idx = 0;
      while((tmp = rdr.get64()) != 0) { idx = 1; }
      if( idx == 0 ) clausePos.pop_back(); // Clause Deleted
    }
  }

  ///// Generate unsatCore /////
  priority_queue<unsigned int> clause_queue;
  vector<bool> in_queue;
  in_queue.resize( clausePos.size() );
  for( unsigned int i = 0; i < in_queue.size() ; i++ ) in_queue[i] = false;
  in_queue[ in_queue.size() - 1 ] = true;
  clause_queue.push( clausePos.size() - 1 ); //Push leaf (empty) clause
  while( clause_queue.size() != 0 ) {
    cid = clause_queue.top();
    clause_queue.pop();

    rdr.seek( clausePos[cid] );

    tmp = rdr.get64();
    if((tmp & 1) == 0) {
      //root clause
      unsatCore.push_back(cid);
    } else {
      //derived clause
      tmp_cid = cid - (tmp >> 1);
      if( !in_queue[ tmp_cid ] ){
        in_queue[ tmp_cid ] = true;
        clause_queue.push( tmp_cid );
      }
      while(1) {
        tmp = rdr.get64();
        if( tmp == 0 ) break;
        tmp_cid = cid - rdr.get64();
        if( !in_queue[ tmp_cid ] ) {
          in_queue[ tmp_cid ] = true;
          clause_queue.push( tmp_cid );
        }
      }
    }
  }
}

void SATMgr::retrieveProof( Reader& rdr, vector<unsigned int>& clausePos, vector<ClauseId>& usedClause) const {
  unsigned int tmp, cid, idx, tmp_cid;

  ///// Clear all /////
  clausePos.clear();
  usedClause.clear();

  ///// Generate clausePos /////
  assert( !rdr.null() );
  rdr.seek(0);
  for( unsigned int pos = 0; (tmp = rdr.get64()) != RDR_EOF ; pos = rdr.Current_Pos() ){
    cid = clausePos.size();
    clausePos.push_back( pos );
    if((tmp & 1) == 0) {
      // root clause
      while((tmp = rdr.get64()) != 0) {}
    } else {
      idx = 0;
      while((tmp = rdr.get64()) != 0) { idx = 1; }
      if( idx == 0 ) clausePos.pop_back(); // Clause Deleted
    }
  }

  ///// Generate usedClause /////
  priority_queue<unsigned int> clause_queue;
  vector<bool> in_queue;
  in_queue.resize( clausePos.size() );
  for( unsigned int i = 0; i < in_queue.size() ; i++ ) in_queue[i] = false;
  in_queue[ in_queue.size() - 1 ] = true;
  clause_queue.push( clausePos.size() - 1 ); //Push root clause
  while( clause_queue.size() != 0 ){
    cid = clause_queue.top();
    clause_queue.pop();

    rdr.seek( clausePos[cid] );

    tmp = rdr.get64();
    if((tmp & 1) == 0) continue; //root clause 

    // else, derived clause
    tmp_cid = cid - (tmp >> 1);
    if( !in_queue[ tmp_cid ] ){
      in_queue[ tmp_cid ] = true;
      clause_queue.push( tmp_cid );
    }
    while(1) {
      tmp = rdr.get64();
      if( tmp == 0 ) break;
      tmp_cid = cid - rdr.get64();
      if( !in_queue[ tmp_cid ] ) {
        in_queue[ tmp_cid ] = true;
        clause_queue.push( tmp_cid );
      }
    }
  }
  for( unsigned int i = 0; i < in_queue.size() ; i++ ) {
    if( in_queue[i] ) {
      usedClause.push_back(i);
    }
  }
}

// build the McMillan Interpolant
V3NetId SATMgr::buildItp(const string& proofName) const {
  Reader rdr;
  // records
  map<ClauseId, V3NetId> claItpLookup;
  vector<unsigned int> clausePos;
  vector<ClauseId> usedClause;
  // ntk
  V3Ntk* const ntk = v3Handler.getCurHandler()->getNtk();
  // temperate variables
  V3NetId nId, nId1, nId2;
  int i, cid, tmp, idx, tmp_cid;
  // const 1 & const 0
  V3NetId CONST0, CONST1;
  CONST0 = ntk->getConst(0);
  CONST1 = ~CONST0;

  rdr.open( proofName.c_str() );
  retrieveProof( rdr, clausePos, usedClause );

  for(i = 0; i < (int)usedClause.size() ; i++){
    cid = usedClause[i];
    rdr.seek( clausePos[ cid ] );
    tmp = rdr.get64();
    if((tmp & 1) == 0){
      //Root Clause
      if( _isClauseOn[ cid ] ){
        idx = tmp >> 1;
        while( _varGroup[idx >> 1] != COMMON ){
          tmp = rdr.get64();
          if( tmp == 0 ) break;
          idx += tmp;
        }

        if ( _varGroup[idx >> 1] == COMMON ) {
          assert(_var2Net.find(idx >> 1) != _var2Net.end());
          nId1 = (_var2Net.find(idx >> 1))->second;
          if((idx & 1) == 1) nId1 = ~nId1;
          while(1) {
            tmp = rdr.get64();
            if( tmp == 0 ) break;
            idx += tmp;
            if( _varGroup[idx >> 1] == COMMON ){
              assert(_var2Net.find(idx >> 1) != _var2Net.end());
              nId2 = (_var2Net.find(idx >> 1))->second;
              if((idx & 1) == 1) nId2 = ~nId2;
              // or
              nId = ntk->createNet();
              createV3AndGate(ntk, nId, ~nId1, ~nId2);
              nId = ~nId;
              nId1 = nId;
            }
          }
        } else {
          nId = CONST0;
        }
        claItpLookup[cid] = nId;
      } else {
        claItpLookup[cid] = CONST1;
      }
    } else {
      //Derived Clause
      tmp_cid = cid - (tmp >> 1);
      assert( claItpLookup.find( tmp_cid ) != claItpLookup.end() );
      nId1 = (claItpLookup.find( tmp_cid ))->second;
      while(1) {
        idx = rdr.get64();
        if( idx == 0 ) break;
        idx--;
        //Var is idx
        tmp_cid = cid - rdr.get64();
        assert( claItpLookup.find( tmp_cid ) != claItpLookup.end() );
        nId2 = claItpLookup[ tmp_cid ];
        if( nId1 != nId2 ) {
          if( _varGroup[idx] == LOCAL_ON ) { // Local to A. Build OR Gate.
            if( nId1 == CONST1 || nId2 == CONST1 ) {
              nId = CONST1;
            } else if( nId1 == CONST0 ) {
              nId = nId2;
            } else if( nId2 == CONST0 ) {
              nId = nId1;
            } else {
              // or
              nId = ntk->createNet();
              createV3AndGate(ntk, nId, ~nId1, ~nId2);
              nId = ~nId;
              nId1 = nId;
            }
          } else { // Build AND Gate.
            if( nId1 == CONST0 || nId2 == CONST0 ){
              nId = CONST0;
            } else if( nId1 == CONST1 ) {
              nId = nId2;
            } else if( nId2 == CONST1 ) {
              nId = nId1;
            } else {
              // or
              nId = ntk->createNet();
              createV3AndGate(ntk, nId, nId1, nId2);
              nId1 = nId;
            }
          }
        }
      }
      claItpLookup[cid] = nId;
    }
  }

  cid = usedClause[ usedClause.size() - 1 ];
  nId = claItpLookup[cid];

  return nId;
}

void SatProofRes::reportResult(const string& name) const {
  // Report Verification Result
  Msg(MSG_IFO) << endl;
  if (isProved()) {
    Msg(MSG_IFO) << "Monitor \"" << name << "\" is safe.\n";
  } else if (isFired()) {
    Msg(MSG_IFO) << "Monitor \"" << name << "\" is violated.\n";
  } else Msg(MSG_IFO) << "UNDECIDED at depth = " << _maxDepth << endl;
}

void SatProofRes::reportCex(const V3NetId& monitor) const {
  assert (_satSolver != 0);
  V3Ntk* const ntk = v3Handler.getCurHandler()->getNtk();

  // Output Pattern Value (PI + PIO)
  V3BitVecX dataValue;
  for (uint32_t i = 0; i <= _fired; ++i) {
    Msg(MSG_IFO) << i << ": ";
    for (int j = ntk->getInoutSize()-1; j >= 0; --j) {
      if (_satSolver->existVerifyData(ntk->getInout(j), i)) {
        dataValue = _satSolver->getDataValue(ntk->getInout(j), i);
        assert (dataValue.size() == ntk->getNetWidth(ntk->getInout(j)));
        Msg(MSG_IFO) << dataValue[0];
      } else Msg(MSG_IFO) << 'x';
    }
    for (int j = ntk->getInputSize()-1; j >= 0; --j) {
      if (_satSolver->existVerifyData(ntk->getInput(j), i)) {
        dataValue = _satSolver->getDataValue(ntk->getInput(j), i);
        assert (dataValue.size() == ntk->getNetWidth(ntk->getInput(j)));
        Msg(MSG_IFO) << dataValue[0];
      } else Msg(MSG_IFO) << 'x';
    }
    Msg(MSG_IFO) << endl;
    assert (_satSolver->existVerifyData(monitor, i));
  }
}
