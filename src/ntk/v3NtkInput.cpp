/****************************************************************************
  FileName     [ v3NtkInput.cpp ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ Base Input Handler for V3 Ntk. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_INPUT_C
#define V3_NTK_INPUT_C

#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3NtkInput.h"

/* -------------------------------------------------- *\
 * Class V3Parser Implementations
\* -------------------------------------------------- */
// Constructor and Destructor
V3NtkInput::V3NtkInput(const bool& isAig, const string& name) : V3NtkHandler(0, createV3Ntk(!isAig)), _ntkName(name)  {
   assert (_ntk); _nameHash.clear(); _netHash.clear(); _outName.clear();
   _nameHash.insert(make_pair("0", V3NetId::makeNetId(0))); _netHash.insert(make_pair(0, "0"));
}

V3NtkInput::~V3NtkInput() {
   _nameHash.clear(); _netHash.clear();
}

// I/O Ancestry Functions
const string
V3NtkInput::getInputName(const uint32_t& index) const {
   assert (_ntk); if (index >= _ntk->getInputSize()) return "";
   return reinterpret_cast<const V3NtkHandler* const>(this)->getNetName(_ntk->getInput(index));
}

const string
V3NtkInput::getOutputName(const uint32_t& index) const {
   assert (_ntk); if (index >= _ntk->getOutputSize()) return "";
   return (index < _outName.size()) ? _outName[index] : "";
}

const string
V3NtkInput::getInoutName(const uint32_t& index) const {
   assert (_ntk); if (index >= _ntk->getInoutSize()) return "";
   return reinterpret_cast<const V3NtkHandler* const>(this)->getNetName(_ntk->getInout(index));
}

// Net Ancestry Functions
void
V3NtkInput::getNetName(V3NetId& id, string& name) const {
   if (V3NetUD == id || id.cp) { assert (!name.size()); return; }
   V3NetStrHash::const_iterator it = _netHash.find(id.id);
   name = (it == _netHash.end()) ? "" : it->second;
}

// Ntk Input Naming Functions
const bool
V3NtkInput::resetNetName(const uint32_t& index, const string& name) {
   assert (index < _ntk->getNetSize()); assert (name.size());
   if (existNet(name)) {
      Msg(MSG_ERR) << "Net Name \"" << name << "\" Already Exists !!" << endl;
      return false;
   }
   V3NetStrHash::const_iterator it = _netHash.find(index);
   if ((_netHash.end() != it) && it->second.size()) {
      _nameHash.erase(_nameHash.find(it->second)); assert (!existNet(it->second));
   }
   _nameHash.insert(make_pair(name, V3NetId::makeNetId(index))); assert (existNet(name));
   _netHash[index] = name; return true;
}

void
V3NtkInput::recordOutName(const uint32_t& index, const string& name) {
   assert (index < _ntk->getOutputSize()); assert (name.size());
   for (uint32_t i = _outName.size(); i < _ntk->getOutputSize(); ++i) _outName.push_back("");
   assert (index < _outName.size()); _outName[index] = name;
}

const V3NetId
V3NtkInput::createNet(const string& netName, uint32_t width) {
   assert (width);
   V3NetId id = (netName.size()) ? getNetId(netName) : V3NetUD;
   if (id == V3NetUD) {
      id = _ntk->createNet(width); assert (V3NetUD != id);
      if (netName.size()) {
         _nameHash.insert(make_pair(netName, id));
         _netHash.insert(make_pair(id.id, netName));
      }
      return id;
   }
   else if (width == _ntk->getNetWidth(id)) return id;
   Msg(MSG_ERR) << "Existing net \"" << netName << "\" has width = " << _ntk->getNetWidth(id) << " != " << width << endl;
   return V3NetUD;
}

const V3NetId
V3NtkInput::getNetId(const string& netName) const {
   V3StrNetHash::const_iterator it = _nameHash.find(netName);
   return (it != _nameHash.end()) ? it->second : V3NetUD;
}

// Extended Helper Functions
void
V3NtkInput::renderFreeNetAsInput() {
   V3NetId id = V3NetId::makeNetId(1); assert (!_ntk->getInputSize());
   for (uint32_t i = 1; i < _ntk->getNetSize(); ++i, ++id.id) {
      if (V3_PI != _ntk->getGateType(id)) continue;
      assert (i == id.id); _ntk->createInput(id);
   }
}

#endif

