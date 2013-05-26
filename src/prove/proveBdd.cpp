/****************************************************************************
  FileName     [ proveBdd.cpp ]
  PackageName  [ prove ]
  Synopsis     [ For BDD-based verification ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <istream>
#include <sstream>

#include <math.h>
#include "v3NtkUtil.h"
#include "v3Msg.h"
#include "bddMgrV.h"
//#include "util.h"
/*int totallevel=0;
unsigned inbddsize=0;
unsigned dffbddsize=0;
*/
inline string myInt2Str(const int num) {
   // Int to string conversion
   string str = "";
   stringstream ss(str);
   ss << num;
   return ss.str();
}

string decToBin(int a,size_t level)
{
    string binary = "";
    unsigned mask = 1;
    for(unsigned i = 0; i < level; i++)
    {
        if((mask&a) >= 1)
            binary = "1"+binary;
        else
            binary = "0"+binary;
        mask<<=1;
    }
    //cout<<binary<<endl;
    return binary;
}



void drawBddPng(string fname,BddNodeV& b)
{
   string fname1=fname;
   fname1+=".dot";
   ofstream ofile(fname1.c_str());
   b.drawBdd(fname, ofile);
   string cmd="dot -o "+fname+".png -Tpng "+fname1;
   system(cmd.c_str());
   string cmd2="mv  "+fname+".png"+" ~/public_html/bdd_png/ -f ";
   string cmd3="rm  "+fname1+" -f ";

  system(cmd2.c_str());
  system(cmd3.c_str());
}
                                                                                                                                       


void
BddMgrV::buildPInitialState()
{
   // TODO : remember to sa_initState
   // Set Initial State to All Zero
   _initState=BddNodeV::_one;
  V3Ntk* const ntk = v3Handler.getCurHandler()->getNtk();
  for(unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
    const V3NetId& nId = ntk->getLatch(i);
    BddNodeV b=bddMgrV->getBddNodeV(nId.id);
	_initState &=(~b); 
     }
/*    for(unsigned j=0 ; j<module->getIoSize(CKT_IN);j++){
            CktCell* tempio=module->getIoCell(CKT_IN,j);
            const unsigned width = tempio->getOutPin()->getBusWidth();
            for(unsigned k=0; k<width;k++){
                inbddsize++;
            }
        }
        for(unsigned j=0 ; j<module->getDffSize();j++){
            CktCell* tempdff=module->getSeqCell(j);
            const unsigned width = tempdff->getOutPin()->getBusWidth();
            for(unsigned k=0; k<width;k++){
                dffbddsize++;
            }
        }
    totallevel=inbddsize+dffbddsize*2;
*/
	/*
	_initState.exist(18);
	cout<<_initState<<endl;

	_initState.exist(17);
	cout<<_initState<<endl;

	_initState.exist(13);
	cout<<_initState<<endl;

	_initState.exist(1);
	cout<<_initState<<endl;
	*/
}

void
BddMgrV::buildPTransRelation()
{
   // TODO : remember to set _tr, _tri
   // NOTE : _tri is the BDD with PI, Current State, and Next State Variables
   //        _tr  is the BDD with Current and Next State Variables
   //       (PI Variables are Existential Quantified from _tri)
 
	_tri=BddNodeV::_one;


	V3NtkHandler* const ntkhandler=v3Handler.getCurHandler();
	V3Ntk* const ntk = v3Handler.getCurHandler()->getNtk();
	for(unsigned i = 0, n = ntk->getLatchSize(); i < n; i++) {
		const V3NetId& nId = ntk->getLatch(i);
		BddNodeV b=bddMgrV->getBddNodeV(ntkhandler->getNetNameOrFormedWithId(nId)+"_ns");
		const V3NetId& nId2 = ntk->getInputNetId(nId,0);
		BddNodeV b2=bddMgrV->getBddNodeV(nId2.id);
		if(b2()==0){
		//	cout<<"haven't build:"<<nId2.id<<endl;
			ntk->buildBdd(nId2);
			b2=bddMgrV->getBddNodeV(nId2.id);
		}
//		cout<<"nId.cp:"<<nId.cp<<" nid2.cp:"<<nId2.cp<<endl;
		if(nId2.cp){
			b2=~b2;
		}
		_tri &= (~(b ^ b2)); 
	}
	_tr=_tri;
	/*
    CktModule* module = VLDesign.getFltModule(); assert (module);
//  unsigned getIoSize() const;i
    _tr=BddNodeV::_one;
//  BddNodeV _tr2=BddNodeV::_one;
    _tri=BddNodeV::_one;
    for(unsigned i2=0; i2<module->getDffSize();i2++){
        CktCell* tempdff=module->getSeqCell(i2);
        string inPinName = tempdff->getInPin(0)->getOutPin()->getCell()->getOutPinName();
        CktCell* tempdffin=tempdff->getInPin(0)->getOutPin()->getCell();
//      cout<<"inPinName "<<tempdffin->getOutPinName()<<" celltype "<<tempdffin->getCellTypeString() <<endl;
        const unsigned width2 = tempdffin->getOutPin()->getBusWidth();
        for(unsigned j2=0; j2<width2;j2++)
        {
            BddNodeV tempin=getBddNodeV(getBitName(tempdffin->getOutPinName(),j2));
            BddNodeV tempns=getBddNodeV(getNSName(getBitName(tempdff->getOutPinName(),j2)));
            BddNodeV temp2=(~(tempin^tempns));
            _tri &= temp2;//(~(tempin^tempns));

        }
    }*/
  	unsigned inbddsize=ntk->getInoutSize()+ntk->getInputSize(); 
	_tr=_tri;
       for(unsigned j=1 ; j<=inbddsize; j++ ){
                _tr=_tr.exist(j);
        }
    //_tr=tri.exist(1)  

		//drawBddPng("tr_",_tr);

		//drawBddPng("tri_",_tri);
}

void
BddMgrV::buildPImage( int level )
{
   // TODO : remember to add _reachStates and set _isFixed
   // Note:: _reachStates record the set of reachable states

	V3Ntk* const ntk = v3Handler.getCurHandler()->getNtk();
	unsigned inbddsize=ntk->getInoutSize()+ntk->getInputSize();
	unsigned dffbddsize=ntk->getLatchSize(); 

    for(unsigned i=1;i<=unsigned(level);i++){
    //  cout<<"i "<<i<<endl;
        BddNodeV temp_state;
    //  if(i==1) {
    //       temp_state= _tr & _initState;
    //  }
    //  else{
    //      cout<<"_reachStates[i-1]"<<endl<<_reachStates[i-1]<<endl;
		 temp_state= _tr & getPReachState();
    //  }
//              cout<<"_reachStates[i-1]"<<endl<<_getPReachState()<<endl;
        bool ismove=0;

		for(unsigned j=1 ;j<=inbddsize+dffbddsize; j++ ){
	//	cout<<"temp_state_exist"<<endl;
			temp_state=temp_state.exist(j);
                //_tr=_tr.exist(j);
        }
		//cout<<"temp_state:"<<temp_state<<endl;	
//      temp_state=temp_state.nodeMove(6,4,ismove);

		if(temp_state.getLevel()>0){
			temp_state=temp_state.nodeMove(inbddsize+dffbddsize+1,inbddsize+1,ismove);
		}
		cout<<"Computing reachable state (time :"<<_reachStates.size()<<" )"<<endl;
//      cout<<"ismove "<<ismove<<endl;
//      //drawBddPng("temp_State_move"+myInt2Str(i),temp_state);
        if(temp_state==getPReachState()){
            Msg(MSG_IFO) << "Fixed point is reached (time : " << _reachStates.size() << ")" << endl;
            _isFixed=true;

    //  cout<<"Fixed point is reached (time : " <<_reachStates.size()<<")" <<endl;
            return;
        }

		//drawBddPng("img_temp_"+myInt2Str(int(_reachStates.size())),temp_state);
        //temp_state |= getPReachState();
 
		//drawBddPng("img_"+myInt2Str(int(_reachStates.size())),temp_state);
        _reachStates.push_back(temp_state); 

    }




}

void 
BddMgrV::runPCheckProperty( const string &name, BddNodeV monitor )
{
   // TODO : prove the correctness of AG(~monitor)


	V3Ntk* const ntk = v3Handler.getCurHandler()->getNtk();
	unsigned inbddsize=ntk->getInoutSize()+ntk->getInputSize();
	unsigned dffbddsize=ntk->getLatchSize(); 
	unsigned totallevel=inbddsize+dffbddsize*2;

	BddNodeV check = (getPReachState()&(monitor) );

		
    if(check==BddNodeV::_zero){
        Msg(MSG_IFO) << "Monitor \"" << name << "\" is safe";
       if(!_isFixed) Msg(MSG_IFO) << " up to time " << _reachStates.size();
        Msg(MSG_IFO) << "." << endl;

    }
    else{
        Msg(MSG_IFO) << "Monitor \"" << name << "\" is violated." << endl;
		Msg(MSG_IFO) << "Counter Example: " << endl;

		addBddNodeV("monitor", monitor());
   		//drawBdd("monitor", "monitor.dot");
	

		//cout<<"inbdd:"<<inbddsize<<" dff:"<<dffbddsize<<" total:"<<totallevel<<endl;

//		BddNodeV temp_state;
		stack<string> counter_example;
		BddNodeV temp_stat2;
		BddNodeV prev_store;
		BddNodeV checks;
		unsigned z=0;
		while(true){
			if(z==0){
				checks=check;
				prev_store=checks;
			}
/*			else{
				checks=temp_state;
			}*/
				//cout<<checks<<endl;
			
				
				//drawBddPng(name+"checks_11_"+myInt2Str(z),checks);
			
		/*	BddNodeV get_input=checks;
			for(unsigned j=0 ; j<dffbddsize;j++){
		//      cout<<"get_input" <<totallevel-dffbddsize-j<<endl;
				get_input=get_input.exist(totallevel-dffbddsize-j);
			}


			cout<<"timeframe:"<<_reachStates.size()-z<<endl;	
				//drawBddPng(name+"get_input1_"+myInt2Str(z),get_input);
			*/


	/*		for(unsigned j=1 ; j<=inbddsize;j++){
		//      cout<<"get_input" <<totallevel-dffbddsize-j<<endl;
				checks=checks.exist(j);
			}*/

			 //drawBddPng(name+"checks_12_"+myInt2Str(z),checks);
			BddNodeV get_input;

			if(z>0){
				checks=checks^(prev_store&checks);
				prev_store |= checks;
				 //drawBddPng(name+"checks_pre_"+myInt2Str(z),prev_store);
				/**************GET INPUT************************/	
				get_input=( checks & temp_stat2) & _tri;
				/**************GET INPUT************************/	
			}
			else{
				get_input= checks  & _tri;
			}

			for(unsigned j=0 ; j<dffbddsize*2;j++){
		//      cout<<"get_input" <<totallevel-dffbddsize-j<<endl;
				get_input=get_input.exist(totallevel-j);
			}

			
			//cout<<_reachStates.size()-z<<":";	
			for( int j=int( pow(float(2),float(inbddsize)) )-1 ; j >= 0;j-- ){
				string bit_val=decToBin(j,inbddsize); 
				if(evalCube(get_input,bit_val)==1){
				//	cout<<evalCube(get_input,bit_val)<<endl;
					counter_example.push(bit_val);
				//	cout<<bit_val<<endl;;
					break;
				}
				assert(j>0);
			}


			//drawBddPng(name+"get_input_"+myInt2Str(z),get_input);
			
			//drawBddPng(name+"checks_13_"+myInt2Str(z),checks);

			if(checks.getLevel()>0){
				bool ismove;
				checks=checks.nodeMove(inbddsize+dffbddsize,totallevel,ismove);
			}
			
			 //drawBddPng(name+"checks_14_"+myInt2Str(z),checks);
			temp_stat2=checks;
			checks=_tr & checks;

				 //drawBddPng(name+"checks_21_"+myInt2Str(z),checks);
			if(int(_reachStates.size())-1-int(z)<0){
				break;
			}
			//checks=checks^checks;
			checks=checks&_reachStates[_reachStates.size()-1-z];

			//drawBddPng(name+"checks_22_"+myInt2Str(z),checks);
			
			for(unsigned j=0 ; j<dffbddsize;j++){
		//      cout<<"get_input" <<totallevel-dffbddsize-j<<endl;
				checks=checks.exist(totallevel-j);
			}

			//cerr<<"reachState size:"<<_reachStates.size()-1-z<<endl;
				//drawBddPng(name+"checks_23_"+myInt2Str(z),checks);
//			temp_state=checks;

			z++;
		}
		z=0;
		while(counter_example.size()>0){
			cout<<z++<<": "<<counter_example.top()<<endl;
			counter_example.pop();
		}
	}

}
