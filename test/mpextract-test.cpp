#include "Snap.h"
#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include <algorithm>
#include <cassert>

void printattrdata(const TModeNet& mode) {
  TStr64V IntAttrNamesN, FltAttrNamesN, StrAttrNamesN, IntVAttrNamesN;
  TStr64V IntAttrNamesE, FltAttrNamesE, StrAttrNamesE, IntVAttrNamesE;
  mode.GetAttrNNames(IntAttrNamesN, FltAttrNamesN, StrAttrNamesN);
  mode.GetIntVAttrNNames(IntVAttrNamesN);
  mode.GetAttrENames(IntAttrNamesE, FltAttrNamesE, StrAttrNamesE);
  mode.GetIntVAttrENames(IntVAttrNamesE);

  // NOTE: deletes attr data AND removes from table
  // for(int i = 0; i < IntVAttrNamesN.Len(); i++) mode.DelAttrN(IntVAttrNamesN[i]); 
  // IntVAttrNamesN.Clr();
  // mode.GetIntVAttrNNames(IntVAttrNamesN);

  std::cout << "Node int attributes: "; 
  for(int i = 0; i < IntAttrNamesN.Len(); i++) std::cout << IntAttrNamesN[i].CStr() << ", "; std::cout << std::endl;
  std::cout << "Node float attributes: "; 
  for(int i = 0; i < FltAttrNamesN.Len(); i++) std::cout << FltAttrNamesN[i].CStr() << ", "; std::cout << std::endl;
  std::cout << "Node str attributes: ";
  for(int i = 0; i < StrAttrNamesN.Len(); i++) std::cout << StrAttrNamesN[i].CStr() << ", "; std::cout << std::endl;
  std::cout << "Node int-vector attributes: ";
  for(int i = 0; i < IntVAttrNamesN.Len(); i++) std::cout << IntVAttrNamesN[i].CStr() << ", "; std::cout << std::endl;
  std::cout << "Edge int attributes: "; 
  for(int i = 0; i < IntAttrNamesE.Len(); i++) std::cout << IntAttrNamesE[i].CStr() << ", "; std::cout << std::endl;
  std::cout << "Edge float attributes: "; 
  for(int i = 0; i < FltAttrNamesE.Len(); i++) std::cout << FltAttrNamesE[i].CStr() << ", "; std::cout << std::endl;
  std::cout << "Edge str attributes: "; 
  for(int i = 0; i < StrAttrNamesE.Len(); i++) std::cout << StrAttrNamesE[i].CStr() << ", "; std::cout << std::endl;
  std::cout << "Edge int-vector attributes: "; 
  for(int i = 0; i < IntVAttrNamesE.Len(); i++) std::cout << IntVAttrNamesE[i].CStr() << ", "; std::cout << std::endl;
}

void print_intvattr_elements(const TModeNet& mode, TInt64 node, const TStr& attrname) {
  TInt64V intv = mode.GetIntVAttrDatN(node, attrname);
  for(int64 i = 0; i < intv.Len(); i++) std::cout << (int64)intv[i] << ", ";
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  PMMNet mmnet = TMMNet::New();
  std::cout << "Adding modes to network..." << std::endl;
  for(int i = 0; i < 100; i++) {
    std::ostringstream oss; oss << "Mode" << i;
    mmnet->AddModeNet(TStr(oss.str().c_str()));
  }
  
  std::cout << "Adding crossnets to network..." << std::endl;
  for(int i = 0; i < 100; i++) {
    for(int j = 0; j <= i; j++) {
      std::ostringstream oss1, oss2, oss;
      oss1 << "Mode" << i;
      oss2 << "Mode" << j;
      oss << i << "to" << j;
      if(float(rand())/RAND_MAX < 0.2) mmnet->AddCrossNet(TStr(oss1.str().c_str()), TStr(oss2.str().c_str()),
                                                          TStr(oss.str().c_str()));
    }
  }


  std::cout << "Adding nodes to modes..." << std::endl;
  int nnodes = 0;
  for(TMMNet::TModeNetI it = mmnet->BegModeNetI(); it < mmnet->EndModeNetI(); it++) {
    int dnnodes = 10 + rand() % 990; 
    for(int i = 0; i < dnnodes; i++) {
      it.GetModeNet().AddNode();
      nnodes++;
    }
  }
  
  int i = 1;
  int nedges = 0;
  for(TMMNet::TCrossNetI it = mmnet->BegCrossNetI(); it < mmnet->EndCrossNetI(); i++, it++) {
    TCrossNet& cn = it.GetCrossNet();
    TModeNet& md1 = mmnet->GetModeNetById(cn.GetMode1());
    TModeNet& md2 = mmnet->GetModeNetById(cn.GetMode2());
    if(i % 100 == 1) {
      std::cout << "Adding edges to crossnets " << i << " to " << MIN(i+99, mmnet->GetCrossNets()) 
                << " of " << mmnet->GetCrossNets() << std::endl;
    }

    for(TNEANet::TNodeI jt = md1.BegNI(); jt < md1.EndNI(); jt++) {      
      if(float(rand())/RAND_MAX < 0.05) {
        for(TNEANet::TNodeI kt = md2.BegNI(); kt < md2.EndNI(); kt++) {
          if(float(rand())/RAND_MAX < 0.25) {
            cn.AddEdge(jt.GetId(), kt.GetId(), TInt64::Mx - nedges);
            nedges++;
          }
        }
      }
    }
  }
  
  for(TMMNet::TModeNetI it = mmnet->BegModeNetI(); it < mmnet->EndModeNetI(); it++) {
    std::cout << "\n------MODE " << it.GetModeName().CStr() << " ATTRIBUTE NAMES------" << std::endl;
    printattrdata(it.GetModeNet());
  }

  // Add attributes to a mode of the new network, and use CopyModeWithoutNodes to copy mode to a new TMMNet.
  // Check attribute names remain but no nodes or node attribute data does
  // to use, change CopyModeWithoutNodes from private to public
  std::cout << "Adding dummy attribute to one mode of TMMNet..." << std::endl;
  TInt64 mid = 0;
  TModeNet& mode = mmnet->GetModeNetById(mid);
  mode.AddIntAttrN("TEST_INTN");
  mode.AddFltAttrN("TEST_FLTN");
  mode.AddStrAttrN("TEST_STRN");
  mode.AddIntVAttrN("TEST_INTVN");
  mode.AddIntAttrE("TEST_INTE");
  mode.AddFltAttrE("TEST_FLTE");
  mode.AddStrAttrE("TEST_STRE");
  mode.AddIntVAttrE("TEST_INTVE");
  
  i = 0;
  for(TNEANet::TNodeI it = mode.BegNI(); it < mode.EndNI(); it++) {
    mode.AddIntAttrDatN(it, i, "TEST_INTN");
    mode.AddFltAttrDatN(it, i + 0.5, "TEST_FLTN");
    std::ostringstream oss; oss << i << " + qkwrhqiowhrqiohrqiowrkql";
    mode.AddStrAttrDatN(it, TStr(oss.str().c_str()), "TEST_STRN");
    for(int j = 0; j <= i; j++) mode.AppendIntVAttrDatN(it, j, "TEST_INTVN");
    i++;
  }


  // Copy a mode from the original, run basic sanity check
  std::cout << "\nMaking new MMNet and copying a mode w/o nodes to it from original..." << std::endl;
  PMMNet mmnet2 = TMMNet::New();
  mmnet->CopyModeWithoutNodes(mmnet, mmnet2, mid);
  std::cout << "Checking that new mode has no nodes or edges, but still has test attribute name..." << std::endl;
  TModeNet& copymode = mmnet2->GetModeNetById(mid);
  TStr modename = mmnet2->GetModeName(mid);
  assert(copymode.GetMxNid() == 0);
  assert(copymode.GetMxEid() == 0);
  assert(copymode.IsIntAttrN("TEST_INTN"));

  // Inspect attribute names of copy and dump its nodes (should be none)
  std::cout << "\n------COPY MODE " << modename.CStr() << " ATTRIBUTE NAMES------" << std::endl;
  printattrdata(copymode);
  std::cout << "\nMode dump:" << std::endl;
  copymode.Dump();

  // Copy crossnetless node data to the copy and reinspect attribute names and mode dump
  std::cout << "\nCopying nodes from original mode to copy without neighbors..." << std::endl;
  TInt64V ToCopy;
  for(TNEANet::TNodeI it = mode.BegNI(); it < mode.EndNI(); it++) ToCopy.Add(it.GetId());
  TModeNet::CopyNodesWithoutNeighbors(mode, copymode, ToCopy);

  std::cout << "Mode status:" << std::endl;
  std::cout << "\n------COPY MODE " << modename.CStr() << " ATTRIBUTE NAMES------" << std::endl;
  printattrdata(copymode);
  std::cout << "\nMode dump:" << std::endl;
  copymode.Dump();

  // Check that values of non-crossnet attribute are copied properly
  TStr64V cnnames; mode.GetCrossNetNames(cnnames);
  TStr firstCN = cnnames[0];
  TInt64 lastid = copymode.GetMxNId() - 1;

  std::cout << "\nElements of TEST_INTVN attribute for last node of " << modename.CStr() << ": ";
  print_intvattr_elements(mode, lastid, "TEST_INTVN");  
  std::cout << "\nElements of TEST_INTVN attribute for last node of " << modename.CStr() << " copy: ";
  print_intvattr_elements(copymode, lastid, "TEST_INTVN"); 

  TCrossNet cn = mmnet->GetCrossNetByName(firstCN);
  return 0;
}
