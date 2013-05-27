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
//#include "v3NtkElaborate.h"
//#include "v3VrfUMC.h"
#include "reader.h"
#include "satMgr.h"
using namespace std;

#define SPIND_UBMC 0
#define IND_UBMC  0
#define CBA_UBMC  0
#define PBA_UBMC  0
#define INTP_UBMC 0
#define PDR_UBMC  0
#define MIX_SP_UBMC 1

void SATMgr::verifyProperty(const string& name, const V3NetId& monitor) {
  // TODO: finish your own SAT-based property checking
  // BMC+INDUCTION(no simple path) are originally provided
  // simplified from vrf/v3VrfUMC.cpp

  // Initialize
  // duplicate the network, so you can modified
  // the ntk for the proving property without
  // destroying the original network
  // [NOTE!!] if you have constructed the new cells in the ntk
  // please call V3SvrMiniSat::resizeNtkData(# cell difference)
  // to ensure the solver updating the ntk data,
  // or you can delete the solver and new one
	V3SvrMiniSat* satSolver=0;
	SatProofRes pRes;
  #if INTP_UBMC
  //cout<<endl<<"***start interpolation UBMC***"<<endl;
  _ntk = new V3Ntk(); *_ntk = *(v3Handler.getCurHandler()->getNtk());
  satSolver = new V3SvrMiniSat(_ntk, false, true);
//  bind(satSolver);
  // Prove the monitor here!!
  bind(satSolver);
  pRes.setMaxDepth(100);
  pRes.setSatSolver(satSolver);
  itpUbmc(monitor, pRes);
  pRes.reportResult(name);
  if (pRes.isFired())
    pRes.reportCex(monitor, _ntk);
  delete satSolver; delete _ntk;
  reset();
  //cout<<"***end interpolation UBMC***"<<endl<<endl;
  #endif

  #if SPIND_UBMC
  //cout<<endl<<"***start simple path induction UBMC***"<<endl;
  _ntk = new V3Ntk(); *_ntk = *(v3Handler.getCurHandler()->getNtk());
  satSolver = new V3SvrMiniSat(_ntk, false, true);
//  bind(satSolver);
  // Prove the monitor here!!
  pRes.setMaxDepth(100);
  pRes.setSatSolver(satSolver);
  sPindUbmc(monitor, pRes);
  pRes.reportResult(name);
  if (pRes.isFired())
    pRes.reportCex(monitor, _ntk);
  delete satSolver; delete _ntk;
  reset();
  //cout<<"***end simple path induction UBMC***"<<endl<<endl;
  #endif

  #if MIX_SP_UBMC
  //cout<<endl<<"***start simple path induction UBMC***"<<endl;
  _ntk = new V3Ntk(); *_ntk = *(v3Handler.getCurHandler()->getNtk());
  satSolver = new V3SvrMiniSat(_ntk, false, true);
//  bind(satSolver);
  // Prove the monitor here!!
  pRes.setMaxDepth(100);
  pRes.setSatSolver(satSolver);
  mixSPindUbmc(monitor, pRes);
  pRes.reportResult(name);
  if (pRes.isFired())
    pRes.reportCex(monitor, _ntk);
  delete satSolver; delete _ntk;
  reset();
  //cout<<"***end simple path induction UBMC***"<<endl<<endl;
  #endif

  #if IND_UBMC
  _ntk = new V3Ntk(); *_ntk = *(v3Handler.getCurHandler()->getNtk());
  satSolver = new V3SvrMiniSat(_ntk, false, true);
//  bind(satSolver);
  // Prove the monitor here!!
  pRes.setMaxDepth(100);
  pRes.setSatSolver(satSolver);
  indUbmc(monitor, pRes);
  pRes.reportResult(name);
  if (pRes.isFired())
    pRes.reportCex(monitor, _ntk);
  delete satSolver; delete _ntk;
  reset();
  #endif
}


void SATMgr::itpUbmc(const V3NetId& monitor, SatProofRes& pRes) {

  V3SvrMiniSat* satSolver = pRes.getSatSolver();

	ClauseId ft_0,ft_1;
//	V3NetId s_0,s_i0,s_i1,s_itp;
  for (uint32_t i = 0, j = pRes.getMaxDepth(); i < j; ++i) {
    satSolver->addBoundedVerifyData(monitor, i);
    satSolver->assumeRelease();
	satSolver->assumeProperty(monitor, false, i);
    satSolver->simplify();
    satSolver->assumeInit(); // Conjunction with initial condition
    if (satSolver->assump_solve()) {
	  cout<<"disproved"<<endl;
		  pRes.setFired(i); break;
    }
    for (uint32_t k = i - 1; k < i; ++k){
      satSolver->assertProperty(monitor, true, k);
	}
		
    satSolver->assumeRelease();

	if(i==0){
//		s_0=createNet(_ntk->getInoutSize());
		for(size_t z=0; z<_ntk->getInputSize();z++){

		}

		ft_0=getNumClauses();
		for(int z=0; z<ft_0; z++){
			markOnsetClause(z);	
		}
		//cout<<"c0:"<<getNumClauses()<<endl;
	}
	if(i>0){
		ft_1=getNumClauses();
		for(int z=ft_0; z<getNumClauses(); z++){
			markOffsetClause(z);
		}
		cout<<"c1:"<<getNumClauses()<<endl;
//		V3SvrData s0=satSolver->setImplyInit();		
//		V3SvrData c0=satSolver->getFormula(monitor, 0);
//		cout<<"c2:"<<getNumClauses()<<endl;
		for(unsigned k=0;k<=i;k++){ 
		//	for(int z=ft_1; z<getNumClauses();z++){
		//		markOnsetClause(z);
		//	}
		//	V3NetId s1=getItp();
	
		}
	}
  }
}
void SATMgr::mixSPindUbmc(const V3NetId& monitor, SatProofRes& pRes) {
  V3SvrMiniSat* satSolver = pRes.getSatSolver();
  for (uint32_t i = 0, j = pRes.getMaxDepth(); i < j; ++i) {
    satSolver->addBoundedVerifyData(monitor, i);
    satSolver->assumeRelease();
	satSolver->assumeProperty(monitor, false, i);
	/***SP***/
	if(i%10==0){
		for(size_t i2=0;i2<=i;i2++){
			for(size_t z=2;z<=i2;z++){
				V3PtrVec ffp_eq_vec;
				ffp_eq_vec.clear();
				for (unsigned k=0;k<_ntk->getLatchSize();k++) {
				  V3NetId  ffp=_ntk->getLatch(k);
				  if((satSolver->existVerifyData(ffp, i2-1))&&
					(satSolver->existVerifyData(ffp,i2-z)) 
					){
						V3SvrData ffp_pre1=satSolver->getFormula(ffp, i2-1);
						V3SvrData ffp_pre2=satSolver->getFormula(ffp, i2-z);
						V3PtrVec ffp_f1,ffp_f2,ffp_u1;
						ffp_f1.clear();
						ffp_f2.clear();
						ffp_u1.clear();
						ffp_f1.push_back(ffp_pre1);
						ffp_f1.push_back(satSolver->getNegFormula(ffp_pre2));
						ffp_f2.push_back(satSolver->getNegFormula(ffp_pre1));
						ffp_f2.push_back(ffp_pre2);
						V3SvrData ffp_i1=satSolver->setImplyIntersection(ffp_f1);
						V3SvrData ffp_i2=satSolver->setImplyIntersection(ffp_f2);
						ffp_u1.push_back(ffp_i1);
						ffp_u1.push_back(ffp_i2);
						V3SvrData ffp_xor=satSolver->setImplyUnion(ffp_u1);
						ffp_eq_vec.push_back(ffp_xor);
				//	}
				  } //else  cout <<"id:"<< ffp.id<<" val:" <<"x"<<endl;
				}
				V3SvrData ffp_eq= satSolver->setImplyUnion(ffp_eq_vec);
				satSolver->assumeProperty(ffp_eq,false);
			}
		}
	}
	/***END SP***/

    satSolver->simplify();
    // Assumption Solve : If UNSAT, proved!
    if (!satSolver->assump_solve()) {
		cout<<"proved"<<endl;
      pRes.setProved(i); break;
    }
    satSolver->assumeInit(); // Conjunction with initial condition
    if (satSolver->assump_solve()) {
	  cout<<"disproved"<<endl;
		  pRes.setFired(i); break;
       assert (pRes.isFired()); break;
    }
    for (uint32_t k = i - 1; k < i; ++k){
      satSolver->assertProperty(monitor, true, k);
	}
  }
}
void SATMgr::sPindUbmc(const V3NetId& monitor, SatProofRes& pRes) {
  // Solver Data
//  V3SvrData pFormulaData; 
//  V3PtrVec pFormula;

  V3SvrMiniSat* satSolver = pRes.getSatSolver();

  // Start UMC Based Verification
  for (uint32_t i = 0, j = pRes.getMaxDepth(); i < j; ++i) {
//	cout<<"timeframe i:"<<i<<endl;
//	cout<<"monitor.id:"<<monitor.id<<endl;
    // Add time frame expanded circuit to SAT Solver
    satSolver->addBoundedVerifyData(monitor, i);
//    pFormula.push_back(satSolver->getFormula(monitor, i));
//	cout<<"pformula"<<pFormula[0]<<endl;
//	cout<<satSolver->getVerifyData(monitor, i)<<endl;
   // Add assume for assumption solve only
    satSolver->assumeRelease();
  //  if (1 == pFormula.size()){
		satSolver->assumeProperty(monitor, false, i);
//	}
  //  else {
	//	assert(0);//TODO ADD
    // }
	/***SP***/
	for(size_t z=2;z<=i;z++){
		V3PtrVec ffp_eq_vec;
		ffp_eq_vec.clear();
		for (unsigned k=0;k<_ntk->getLatchSize();k++) {
		  V3NetId  ffp=_ntk->getLatch(k);
		  if((satSolver->existVerifyData(ffp, i-1))&&
		 	(satSolver->existVerifyData(ffp,i-z)) 
			){
		//	cout<<"latch size:"<<_ntk->getLatchSize()<<endl;
		//	if(i>1){
			//cout<<"build"<<endl;
				V3SvrData ffp_pre1=satSolver->getFormula(ffp, i-1);
				V3SvrData ffp_pre2=satSolver->getFormula(ffp, i-z);
				V3PtrVec ffp_f1,ffp_f2,ffp_u1;
				ffp_f1.clear();
				ffp_f2.clear();
				ffp_u1.clear();
				ffp_f1.push_back(ffp_pre1);
				ffp_f1.push_back(satSolver->getNegFormula(ffp_pre2));
				ffp_f2.push_back(satSolver->getNegFormula(ffp_pre1));
				ffp_f2.push_back(ffp_pre2);
				V3SvrData ffp_i1=satSolver->setImplyIntersection(ffp_f1);
				V3SvrData ffp_i2=satSolver->setImplyIntersection(ffp_f2);
				ffp_u1.push_back(ffp_i1);
				ffp_u1.push_back(ffp_i2);
				V3SvrData ffp_xor=satSolver->setImplyUnion(ffp_u1);
				ffp_eq_vec.push_back(ffp_xor);
		//	}
		  } //else  cout <<"id:"<< ffp.id<<" val:" <<"x"<<endl;
		}
		V3SvrData ffp_eq= satSolver->setImplyUnion(ffp_eq_vec);
		satSolver->assertProperty(ffp_eq,false);
	}
	
	/***END SP***/

    satSolver->simplify();
    // Assumption Solve : If UNSAT, proved!
    if (!satSolver->assump_solve()) {
		cout<<"proved"<<endl;
      pRes.setProved(i); break;
    }

/*	V3BitVecX dataValue;
	V3BitVecX dataValue2;
  	for (uint32_t z = 2; z <= i; ++z) {
	//	cout << i << ": " <<" z:"<<z<<endl;
		for (unsigned k=0;k<_ntk->getLatchSize();k++) {
			V3NetId  ffp=_ntk->getLatch(k);
		//	cout<<"latch size:"<<_ntk->getLatchSize()<<endl;
		  if((satSolver->existVerifyData(ffp, i-1))&&
		 	(satSolver->existVerifyData(ffp,i-z)) 
			){
			dataValue = satSolver->getDataValue(ffp, i-1);
			dataValue2 = satSolver->getDataValue(ffp, i-z);
		//	assert (dataValue.size() == ntk->getNetWidth(ntk->getLatch(j)));

		//	cout <<"i-1:"<<endl<<" "<< dataValue<<endl;
		//	cout <<"i-z:"<<endl<<" "<< dataValue2<<endl;
			 } 
		}
	}*/
	
//    satSolver->assumeRelease();
//	satSolver->assumeProperty(monitor, false, i);
    // Assumption Solve : If SAT, disproved!
    satSolver->assumeInit(); // Conjunction with initial condition
    if (satSolver->assump_solve()) {
	  cout<<"disproved"<<endl;
		//if ('0' != satSolver->getDataValue(pFormula[0])) {
		  pRes.setFired(i); break;
		//}
       assert (pRes.isFired()); break;
    }



    // Add assert back to the property
//	assert(pFormula.size()==1);//TODO ADD
    for (uint32_t k = i - 1; k < i; ++k){
      satSolver->assertProperty(monitor, true, k);
	}
//    pFormula.clear(); //pFormulaData = 0;
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

void SATMgr::bind(V3SvrMiniSat* ptrMinisat) {
  _ptrMinisat = ptrMinisat;
  if(_ptrMinisat->_Solver->proof == NULL) {
    Msg(MSG_ERR) << "The Solver has no Proof!! Try Declaring the Solver with proofLog be set!!" << endl;
    exit(0);
  }
}

void SATMgr::reset() {
  _ptrMinisat = NULL;
  _ntk = NULL;
  _varGroup.clear();
  _var2Net.clear();
  _isClauseOn.clear();
  _isClaOnDup.clear();
}

void SATMgr::markOnsetClause(const ClauseId& cid) {
  unsigned cSize = getNumClauses();
  assert(cid < (int)cSize);
  if(_isClauseOn.size() < cSize) {
    _isClauseOn.resize(cSize, false);
  }
  _isClauseOn[cid] = true;
}

void SATMgr::markOffsetClause(const ClauseId& cid) {
  unsigned cSize = getNumClauses();
  assert(cid < (int)cSize);
  if(_isClauseOn.size() < cSize) {
    _isClauseOn.resize(cSize, false);
  }
  _isClauseOn[cid] = false;
}

void SATMgr::mapVar2Net(const Var& var, const V3NetId& net) {
  assert(_var2Net.find(var) == _var2Net.end());
  _var2Net[var] = net;
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

  return netId;
}

vector<Clause> SATMgr::getUNSATCore() const {
  assert(_ptrMinisat);
  assert(_ptrMinisat->_Solver->proof);

  vector<Clause> unsatCore;
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

  return unsatCore;
}

void SATMgr::retrieveProof( Reader& rdr, vector<Clause>& unsatCore) const {
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
      vec<Lit> lits;
      idx = tmp >> 1;
      lits.push(toLit(idx));
      while( _varGroup[idx >> 1] != COMMON ){
        tmp = rdr.get64();
        if( tmp == 0 ) break;
        idx += tmp;
        lits.push(toLit(idx));
      }
      unsatCore.push_back(Clause(false, lits));
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
  unsigned int tmp, cid, idx, tmp_cid, root_cid;

  ///// Clear all /////
  clausePos.clear();
  usedClause.clear();
  _varGroup.clear();
  _varGroup.resize(_ptrMinisat->_Solver->nVars(), NONE);
  _isClaOnDup.clear();
  assert((int)_isClauseOn.size() == getNumClauses());

  ///// Generate clausePos && varGroup /////
  assert( !rdr.null() );
  rdr.seek(0);
  root_cid = 0;
  for( unsigned int pos = 0; (tmp = rdr.get64()) != RDR_EOF ; pos = rdr.Current_Pos() ){
    cid = clausePos.size();
    clausePos.push_back( pos );
    if((tmp & 1) == 0) {
      //Root Clause
      _isClaOnDup.push_back(_isClauseOn[root_cid]);
      idx = tmp >> 1;
      if( _isClauseOn[ root_cid ] ) {
        if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_ON;
        else if(_varGroup[idx >> 1] == LOCAL_OFF) _varGroup[idx >> 1] = COMMON;
      } else {
        if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_OFF;
        else if(_varGroup[idx >> 1] == LOCAL_ON) _varGroup[idx >> 1] = COMMON;
      }
      while(1) {
        tmp = rdr.get64();
        if(tmp == 0) break;
        idx += tmp;
        if( _isClauseOn[ root_cid ] ) {
          if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_ON;
          else if(_varGroup[idx >> 1] == LOCAL_OFF) _varGroup[idx >> 1] = COMMON;
        } else {
          if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_OFF;
          else if(_varGroup[idx >> 1] == LOCAL_ON) _varGroup[idx >> 1] = COMMON;
        }
      }
      ++root_cid;
    } else {
      _isClaOnDup.push_back(false);
      idx = 0;
      while(1) {
        tmp = rdr.get64();
        if(tmp == 0) break;
        idx = 1;
        tmp = rdr.get64();
      }
      if( idx == 0 ) {
        clausePos.pop_back(); // Clause Deleted
        _isClaOnDup.pop_back(); // Clause Deleted
      }
    }
  }

  ///// Generate usedClause /////
  priority_queue<unsigned int> clause_queue;
  vector<bool> in_queue;
  in_queue.resize( clausePos.size() );
  for( unsigned int i = 0; i < in_queue.size() ; i++ ) in_queue[i] = false;
  in_queue[ in_queue.size() - 1 ] = true;
  clause_queue.push( clausePos.size() - 1 ); //Push root empty clause
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
  // ntk size
  uint32_t netSize = _ntk->getNetSize();
  // temperate variables
  V3NetId nId, nId1, nId2;
  int i, cid, tmp, idx, tmp_cid;
  // const 1 & const 0
  V3NetId CONST0, CONST1;
  CONST0 = _ntk->getConst(0);
  CONST1 = ~CONST0;

  rdr.open( proofName.c_str() );
  retrieveProof( rdr, clausePos, usedClause );

  for(i = 0; i < (int)usedClause.size() ; i++) {
    cid = usedClause[i];
    rdr.seek( clausePos[ cid ] );
    tmp = rdr.get64();
    if((tmp & 1) == 0) {
      //Root Clause
      if( _isClaOnDup[ cid ] ) {
        idx = tmp >> 1;
        while( _varGroup[idx >> 1] != COMMON ){
          tmp = rdr.get64();
          if( tmp == 0 ) break;
          idx += tmp;
        }

        if ( _varGroup[idx >> 1] == COMMON ) {
          assert(_var2Net.find(idx >> 1) != _var2Net.end());
          nId = (_var2Net.find(idx >> 1))->second;
          nId1 = (_var2Net.find(idx >> 1))->second;
          if((idx & 1) == 1) nId1 = ~nId1;
          if((idx & 1) == 1) nId = ~nId;
          while(1) {
            tmp = rdr.get64();
            if( tmp == 0 ) break;
            idx += tmp;
            if( _varGroup[idx >> 1] == COMMON ){
              assert(_var2Net.find(idx >> 1) != _var2Net.end());
              nId2 = (_var2Net.find(idx >> 1))->second;
              if((idx & 1) == 1) nId2 = ~nId2;
              // or
              nId = ~_ntk->createNet();
              createV3AndGate(_ntk, nId, ~nId1, ~nId2);
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
      nId = (claItpLookup.find( tmp_cid ))->second;
      nId1 = (claItpLookup.find( tmp_cid ))->second;
      while(1) {
        idx = rdr.get64();
        if( idx == 0 ) break;
        idx--;
        //Var is idx
        tmp_cid = cid - rdr.get64();
        assert( claItpLookup.find( tmp_cid ) != claItpLookup.end() );
        nId2 = (claItpLookup.find( tmp_cid ))->second;
        if( nId1 != nId2 ) {
          if( _varGroup[idx] == LOCAL_ON ) { // Local to A. Build OR Gate.
            if( nId1 == CONST1 || nId2 == CONST1 ) {
              nId = CONST1;
              nId1 = nId;
            } else if( nId1 == CONST0 ) {
              nId = nId2;
              nId1 = nId;
            } else if( nId2 == CONST0 ) {
              nId = nId1;
              nId1 = nId;
            } else {
              // or
              nId = ~_ntk->createNet();
              createV3AndGate(_ntk, nId, ~nId1, ~nId2);
              nId1 = nId;
            }
          } else { // Build AND Gate.
            if( nId1 == CONST0 || nId2 == CONST0 ){
              nId = CONST0;
              nId1 = nId;
            } else if( nId1 == CONST1 ) {
              nId = nId2;
              nId1 = nId;
            } else if( nId2 == CONST1 ) {
              nId = nId1;
              nId1 = nId;
            } else {
              // and
              nId = _ntk->createNet();
              createV3AndGate(_ntk, nId, nId1, nId2);
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

  _ptrMinisat->resizeNtkData(_ntk->getNetSize() - netSize); // resize Solver data to ntk size

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

void SatProofRes::reportCex(const V3NetId& monitor, const V3Ntk* const ntk) const {
  assert (_satSolver != 0);

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
