/****************************************************************************
  FileName     [ partBddNodeV.cpp ]
  PackageName  [ ]
  Synopsis     [ Define BDD Node member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-2009 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <sstream>
#include <fstream>
#include <cassert>
#include "bddNodeV.h"
#include "bddMgrV.h"

BddNodeV
BddNodeV::exist(unsigned l) const
{
   // TODO: return existentially quantified BddNodeV on this BddNodeV
	if(l>this->getLevel()){
		return *this;
	}
	else if(l==this->getLevel()){ //OK
		//cout<<"this_level:"<<this->getLevel()<<" l:"<<l<<endl;
		//cout<<this->getLeft()<<endl;
		//cout<<this->getLeftCofactor(this->getLevel())<<endl;
		BddNodeV ex=this->getLeftCofactor(l)|this->getRightCofactor(l);
		//cout<<"ex:"<<endl<<ex<<endl;
		return ex; 
	}
	else{
		//cout<<"this_level2:"<<this->getLevel()<<" l:"<<l<<endl;
		BddNodeV left_b=this->getLeftCofactor(this->getLevel()).exist(l);
		BddNodeV right_b=this->getRightCofactor(this->getLevel()).exist(l);
		BddNodeV ite=bddMgrV->ite(bddMgrV->getSupport(this->getLevel()),left_b,right_b);
		//cout<<"left_b:"<<endl<<left_b<<endl;
		//cout<<"right_b:"<<endl<<right_b<<endl;
		return ite;
	}
}
/*
BddNodeV
BddNodeV::existRec(unsigned l) const
{

   return NULL;
}
*/
const void* BddNodeV::socv_func_1(vector<void*> arg_vec) const
{
  return NULL;
}


const void* BddNodeV::socv_func_2(vector<void*> arg_vec) const
{
  return NULL;
}

const void* BddNodeV::socv_func_3(vector<void*> arg_vec) const
{
  return NULL;
}

const void* BddNodeV::socv_func_4(vector<void*> arg_vec) const
{
  return NULL;
}
