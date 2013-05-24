/****************************************************************************
  FileName     [ v3Ntk.cpp ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ V3 Network. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_C
#define V3_NTK_C

#include "v3Ntk.h"
#include "v3Msg.h"
#include "v3StrUtil.h"
#include "v3NtkHandler.h" // MODIFICATION FOR SoCV BDD

/* -------------------------------------------------- *\
 * Class V3Ntk Implementations
\* -------------------------------------------------- */
// Constuctor and Destructor
V3Ntk::V3Ntk() {
   for (uint32_t i = 0; i < 3; ++i) _IOList[i].clear();
   _FFList.clear(); _ConstList.clear();
   _typeMisc.clear(); _levelData.clear(); _outputNet.clear();
   _inputData.clear(); _cutSignals.clear(); _ntkModule.clear();
   _globalMisc = 0; _globalClk = V3NetUD; _levelSize = 0;

   _isBddBuilt = false; // MODIFICATION FOR SoCV BDD
}

V3Ntk::~V3Ntk() {
}

// Ntk Construction Functions
void
V3Ntk::initialize() {
   assert (!_inputData.size());
   // Create Constant AIG_FALSE
   const V3NetId id = createNet(1); assert (!id.id); createConst(id);
}

const V3NetId
V3Ntk::createNet(const uint32_t& width) {
   // Validation Check
   if (width != 1) {
      Msg(MSG_ERR) << "Unexpected Net width = " << width << " in Base / AIG Network !!" << endl;
      return V3NetUD;
   }
   assert (_inputData.size() == _typeMisc.size());
   // Create New V3NetId
   V3NetId id = V3NetId::makeNetId(_inputData.size()); assert (!isV3NetInverted(id));
   _typeMisc.push_back(V3MiscType()); _inputData.push_back(V3InputVec());
   assert (V3_PI == _typeMisc.back().type); assert (!_typeMisc.back().misc);
   assert (!_inputData.back().size()); return id;
}

void
V3Ntk::createModule(V3NtkModule* const module) {
   assert (module); assert (module->getNtkRef());
   _ntkModule.push_back(module);
}

void
V3Ntk::createInput(const V3NetId& id) {
   assert (validNetId(id)); assert (!isV3NetInverted(id));
   assert (!reportMultipleDrivenNet(V3_PI, id));
   _IOList[0].push_back(id);
}

void
V3Ntk::createOutput(const V3NetId& id) {
   assert (validNetId(id)); _IOList[1].push_back(id);
}

void
V3Ntk::createInout(const V3NetId& id) {
   assert (validNetId(id)); assert (!isV3NetInverted(id));
   createGate(V3_PIO, id); _IOList[2].push_back(id);
}

void
V3Ntk::createLatch(const V3NetId& id) {
   assert (validNetId(id)); assert (!isV3NetInverted(id));
   createGate(V3_FF, id); _FFList.push_back(id);
}

void
V3Ntk::createConst(const V3NetId& id) {
   assert (validNetId(id)); assert (!isV3NetInverted(id));
   createGate(dynamic_cast<const V3BvNtk*>(this) ? BV_CONST : AIG_FALSE, id); _ConstList.push_back(id);
}

void
V3Ntk::createClock(const V3NetId& id) {
   assert (validNetId(id)); assert (V3_PI == getGateType(id));
   assert (V3NetUD == _globalClk); _globalClk = id;
}

void
V3Ntk::setInput(const V3NetId& id, const V3InputVec& inputs) {
   assert (validNetId(id)); assert (V3_PI == getGateType(id));
   V3InputVec& fanInVec = _inputData[id.id]; assert (!fanInVec.size());
   for (uint32_t i = 0; i < inputs.size(); ++i) fanInVec.push_back(inputs[i]);
}

void
V3Ntk::createGate(const V3GateType& type, const V3NetId& id) {
   // Check Validation
   assert (validNetId(id)); assert (type < V3_XD); assert (type > V3_PI);
   assert (dynamic_cast<const V3BvNtk*>(this) || type <= AIG_FALSE);
   assert (!(dynamic_cast<const V3BvNtk*>(this)) || (type <= V3_MODULE || type > AIG_FALSE));
   assert (!reportMultipleDrivenNet(type, id)); assert (!reportUnexpectedFaninSize(type, id));
   // Set Gate Type
   _typeMisc[id.id].type = type;
}

// Ntk Reconstruction Functions
void
V3Ntk::replaceOutput(const uint32_t& index, const V3NetId& id) {
   assert (index < getOutputSize()); assert (validNetId(id)); _IOList[1][index] = id;
}

// Ntk Structure Functions
const uint32_t
V3Ntk::getNetWidth(const V3NetId& id) const {
   assert (validNetId(id)); return 1;
}

// Ntk Traversal Functions
void
V3Ntk::computeFanout() {
   // Initialize Fanout List
   for (uint32_t i = 0; i < _outputNet.size(); ++i) _outputNet[i].clear();
   _outputNet.clear(); _outputNet.resize(getNetSize()); newMiscData();
   // Set Latest Misc Data on (Pseudo) PI / PIO / Const
   for (uint32_t i = 0; i < getInputSize(); ++i) setLatestMiscData(getInput(i));
   for (uint32_t i = 0; i < getLatchSize(); ++i) setLatestMiscData(getLatch(i));
   for (uint32_t i = 0; i < getConstSize(); ++i) setLatestMiscData(getConst(i));
   // DFS Compute Fanout List From (Pseudo) PO / PIO
   for (uint32_t i = 0; i < getLatchSize(); ++i) {
      dfsComputeFanoutList(getInputNetId(getLatch(i), 0));
      dfsComputeFanoutList(getInputNetId(getLatch(i), 1));
   }
   for (uint32_t i = 0; i < getInoutSize(); ++i)
      dfsComputeFanoutList(getInputNetId(getInout(i), 0));
   for (uint32_t i = 0; i < getOutputSize(); ++i) dfsComputeFanoutList(getOutput(i));
}

void
V3Ntk::computeLevel() {
   // Initialize Level Data
   _levelData.clear(); _levelData = V3UI32Vec(getNetSize(), V3NtkUD); _levelSize = 0;
   // Set Level 0 on (Pseudo) PI / PIO / Const
   for (uint32_t i = 0; i < getInputSize(); ++i) _levelData[getInput(i).id] = 0;
   for (uint32_t i = 0; i < getLatchSize(); ++i) _levelData[getLatch(i).id] = 0;
   for (uint32_t i = 0; i < getConstSize(); ++i) _levelData[getConst(i).id] = 0;
   // DFS Levelize From (Pseudo) PO / PIO
   V3NetId id;
   for (uint32_t i = 0; i < getLatchSize(); ++i) {
      id = getInputNetId(getLatch(i), 0); dfsComputeLevel(id);
      if (_levelData[id.id] > _levelSize) _levelSize = _levelData[id.id];
      id = getInputNetId(getLatch(i), 1); dfsComputeLevel(id);
      if (_levelData[id.id] > _levelSize) _levelSize = _levelData[id.id];
   }
   for (uint32_t i = 0; i < getInoutSize(); ++i) {
      id = getInputNetId(getInout(i), 0); dfsComputeLevel(id);
      if (_levelData[id.id] > _levelSize) _levelSize = _levelData[id.id];
   }
   for (uint32_t i = 0; i < getOutputSize(); ++i) {
      id = getOutput(i); dfsComputeLevel(id);
      if (_levelData[id.id] > _levelSize) _levelSize = _levelData[id.id];
   }
   ++_levelSize;
}

// Helper Functions for Ntk Construction Validation Check
const bool
V3Ntk::reportInvertingNet(const V3NetId& id) const {
   assert (validNetId(id));
   if (isV3NetInverted(id))
      Msg(MSG_ERR) << "Unexpected Inverting Net = " << id.id << endl;
   else return false;
   return true;
}

const bool
V3Ntk::reportMultipleDrivenNet(const V3GateType& type, const V3NetId& id) const {
   assert (type < V3_GATE_TOTAL); assert (validNetId(id));
   if (V3_PI != getGateType(id))
      Msg(MSG_ERR) << "Multiple-Driven @ " << id.id << "(" << V3GateTypeStr[type] << ")"
                   << ", Exist " << V3GateTypeStr[getGateType(id)] << endl;
   else if (_globalClk.id == id.id)
      Msg(MSG_ERR) << "Clock Signal \"" << _globalClk.id << "\" Cannot be Driven, found @ " 
                   << id.id << "(" << V3GateTypeStr[type] << ")" << endl;
   else return false;
   return true;
}

const bool
V3Ntk::reportUnexpectedFaninSize(const V3GateType& type, const V3NetId& id) const {
   assert (type < V3_XD); assert (validNetId(id));
   if (((isV3ReducedType(type) || BV_CONST == type || V3_PIO == type) && (getInputNetSize(id) != 1)) || 
       ((isV3PairType(type) || AIG_NODE == type || V3_FF == type || BV_SLICE == type) && (getInputNetSize(id) != 2)) || 
       ((BV_MUX == type) && (getInputNetSize(id) != 3)) || ((V3_MODULE == type) && (getInputNetSize(id) != 1)))
      Msg(MSG_ERR) << "Expecting Fanin Size is One while " << getInputNetSize(id) 
                   << " is Found @ " << id.id << " for Gate Type = " << V3GateTypeStr[type] << endl;
   else return false;
   return true;
}

const bool
V3Ntk::reportNetWidthInconsistency(const V3NetId& id1, const V3NetId& id2, const string& s) const {
   if (getNetWidth(id1) != getNetWidth(id2))
      Msg(MSG_ERR) << "Width of " << s << " are inconsistent : " << id1.id << "(" << getNetWidth(id1) << ") != " 
                   << id2.id << "(" << getNetWidth(id2) << ")" << endl;
   else return false;
   return true;
}

const bool
V3Ntk::reportUnexpectedNetWidth(const V3NetId& id, const uint32_t& width, const string& s) const {
   if (width != getNetWidth(id))
      Msg(MSG_ERR) << "Width of " << s << " is unexpected : " << id.id << "(" << getNetWidth(id) << ") != " 
                   << width << endl;
   else return false;
   return true;
}

// Ntk Traversal Private Functions
void
V3Ntk::dfsComputeFanoutList(const V3NetId& id) {
   assert (validNetId(id)); if (isLatestMiscData(id)) return;
   // Set Latest Misc Data
   setLatestMiscData(id);
   // Traverse Fanin Logics
   const V3GateType type = getGateType(id); assert (V3_XD > type);
   assert (V3_FF < type && AIG_FALSE != type && BV_CONST != type);
   if (V3_MODULE == type) {
      assert (getInputNetSize(id) == 1); assert (getModule(id));
      const V3NetVec& inputs = getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) {
         dfsComputeFanoutList(inputs[i]); _outputNet[inputs[i].id].push_back(id);
      }
   }
   else {
      const uint32_t inSize = (type == AIG_NODE || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 1;
      for (uint32_t i = 0; i < inSize; ++i) {
         dfsComputeFanoutList(getInputNetId(id, i)); _outputNet[getInputNetId(id, i).id].push_back(id);
      }
   }
}

void
V3Ntk::dfsComputeLevel(const V3NetId& id) {
   assert (validNetId(id)); assert (id.id < _levelData.size());
   if (V3NtkUD != _levelData[id.id]) return; _levelData[id.id] = 0;
   // Traverse Fanin Logics
   const V3GateType type = getGateType(id); assert (V3_XD > type);
   assert (V3_FF < type && AIG_FALSE != type && BV_CONST != type);
   if (V3_MODULE == type) {
      assert (getInputNetSize(id) == 1); assert (getModule(id));
      const V3NetVec& inputs = getModule(id)->getInputList();
      for (uint32_t i = 0; i < inputs.size(); ++i) {
         dfsComputeLevel(inputs[i]);
         if (_levelData[id.id] < _levelData[inputs[i].id]) 
            _levelData[id.id] = _levelData[inputs[i].id];
      }
   }
   else {   
      const uint32_t inSize = (type == AIG_NODE || isV3PairType(type)) ? 2 : (BV_MUX == type) ? 3 : 1;
      for (uint32_t i = 0; i < inSize; ++i) {
         dfsComputeLevel(getInputNetId(id, i));
         if (_levelData[id.id] < _levelData[getInputNetId(id, i).id]) 
            _levelData[id.id] = _levelData[getInputNetId(id, i).id];
      }
   }
   ++_levelData[id.id];
}

/* -------------------------------------------------- *\
 * Class V3BvNtk Implementations
\* -------------------------------------------------- */
// Initialization to Static Members
V3ConstHash  V3BvNtk::_V3ConstHash = V3ConstHash();
V3BusIdHash  V3BvNtk::_V3BusIdHash = V3BusIdHash();
V3BitVecXVec V3BvNtk::_V3ValueVec  = V3BitVecXVec();
V3BusPairVec V3BvNtk::_V3BusIdVec  = V3BusPairVec();

// Constructors for BV Network Gates
V3BvNtk::V3BvNtk() : V3Ntk() {
   _netWidth.clear();
}

V3BvNtk::~V3BvNtk() {
};

// Ntk Construction Functions
void
V3BvNtk::initialize() {
   assert (!_inputData.size());
   // Create Constant BV_CONST = 1'b0 for Sync with AIG_FALSE
   const V3NetId id = createNet(1); assert (!id.id);
   _inputData.back().push_back(0); createConst(id);
}

const V3NetId
V3BvNtk::createNet(const uint32_t& width) {
   // Validation Check
   if (!width) {
      Msg(MSG_ERR) << "Unexpected Net width = " << width << " in BV Network !!" << endl;
      return V3NetUD;
   }
   assert (_inputData.size() == _netWidth.size()); _netWidth.push_back(width);
   assert (_inputData.size() == _typeMisc.size());
   // Create New V3NetId
   V3NetId id = V3NetId::makeNetId(_inputData.size()); assert (!isV3NetInverted(id));
   _typeMisc.push_back(V3MiscType()); _inputData.push_back(V3InputVec());
   assert (V3_PI == _typeMisc.back().type); assert (!_typeMisc.back().misc);
   assert (!_inputData.back().size()); return id;
}

// Ntk Structure Functions
const uint32_t
V3BvNtk::getNetWidth(const V3NetId& id) const {
   assert (validNetId(id)); return _netWidth[id.id];
}

// Ntk for BV Gate Functions
const V3BVXId
V3BvNtk::hashV3ConstBitVec(const string& input_exp) {
   V3BitVecX* const value = new V3BitVecX(input_exp.c_str()); assert (value);
   const string exp = v3Int2Str(value->size()) + "'" + value->toExp();
   V3ConstHash::const_iterator it = _V3ConstHash.find(exp);
   if (it != _V3ConstHash.end()) { delete value; return it->second; }
   assert (_V3ConstHash.size() == _V3ValueVec.size());
   _V3ConstHash.insert(make_pair(exp, _V3ValueVec.size()));
   _V3ValueVec.push_back(value); return _V3ValueVec.size() - 1;
}

const V3BusId
V3BvNtk::hashV3BusId(const uint32_t& msb, const uint32_t& lsb) {
   const V3PairType msb_lsb(msb, lsb);
   assert (msb == msb_lsb.bus[0] && lsb == msb_lsb.bus[1]);
   V3BusIdHash::const_iterator it = _V3BusIdHash.find(msb_lsb.pair);
   if (it != _V3BusIdHash.end()) return it->second;
   assert (_V3BusIdHash.size() == _V3BusIdVec.size());
   _V3BusIdHash.insert(make_pair(msb_lsb.pair, _V3BusIdVec.size()));
   _V3BusIdVec.push_back(msb_lsb); return _V3BusIdVec.size() - 1;
}

const uint32_t
V3BvNtk::getConstWidth(const V3BVXId& valueId) const {
   assert (valueId < _V3ValueVec.size()); return _V3ValueVec[valueId]->size();
}

const uint32_t
V3BvNtk::getSliceWidth(const V3BusId& busId) const {
   assert (busId < _V3BusIdVec.size());
   return 1 + ((_V3BusIdVec[busId].bus[0] > _V3BusIdVec[busId].bus[1]) ? 
              (_V3BusIdVec[busId].bus[0] - _V3BusIdVec[busId].bus[1]) : 
              (_V3BusIdVec[busId].bus[1] - _V3BusIdVec[busId].bus[0]));
}

const V3BitVecX* const
V3BvNtk::getInputConstValue(const V3NetId& id) const {
   assert (validNetId(id)); assert (BV_CONST == getGateType(id));
   const V3BVXId valueId = _inputData[id.id][0].value;
   assert (valueId < _V3ValueVec.size()); return _V3ValueVec[valueId];
}

const uint32_t
V3BvNtk::getInputSliceBit(const V3NetId& id, const bool& msb) const {
   assert (validNetId(id)); assert (BV_SLICE == getGateType(id));
   const V3BusId busId = _inputData[id.id][1].value; assert (busId < _V3BusIdVec.size());
   return msb ? _V3BusIdVec[busId].bus[0] : _V3BusIdVec[busId].bus[1];
}

// MODIFICATION FOR SoCV
void V3Ntk::printFaninRec(V3NtkHandler* const handler, const V3NetId& netId, const int& depth, const int& level) {
  bool isVisit = isLatestMiscData(netId);
  setLatestMiscData(netId);

  string spacing = "";
  for(int i = 0; i < depth; ++i) spacing += "  ";

  if(netId.cp) {
    Msg(MSG_IFO) << spacing << '~' << handler->getNetNameOrFormedWithId(~netId) << " / ";
    Msg(MSG_IFO) << '!';
  } else {
    Msg(MSG_IFO) << spacing << handler->getNetNameOrFormedWithId(netId) << " / ";
  }
  Msg(MSG_IFO) << netId.id;
  if (isVisit) Msg(MSG_IFO) << " (*)";
  Msg(MSG_IFO) << endl;

  if(depth != 0 && getGateType(netId) == V3_FF) return;
  if(depth < level && !isVisit) {
    for(unsigned i = 0; i < getInputNetSize(netId); ++i) {
      printFaninRec(handler, getInputNetId(netId, i), depth+1, level);
    }
  }
}
// END OF MODIFICATION

#endif
