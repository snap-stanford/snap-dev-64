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

  // note deletes attr data AND removes from table
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



int main(int argc, char *argv[]) {
  PMMNet mmnet = TMMNet::New();
  std::cout << "Adding modes to network..." << std::endl;
  for(int i = 0; i < 1000; i++) {
    std::ostringstream oss; oss << "Mode" << i;
    mmnet->AddModeNet(TStr(oss.str().c_str()));
  }
  
  std::cout << "Adding crossnets to network..." << std::endl;
  for(int i = 0; i < 1000; i++) {
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
    if(i % 1000 == 1) {
      std::cout << "Adding edges to crossnets " << i << " to " << MIN(i+999, mmnet->GetCrossNets()) 
                << " of " << mmnet->GetCrossNets() << std::endl;
    }

    for(TNEANet::TNodeI it = md1.BegNI(); it < md1.EndNI(); it++) {      
      if(float(rand())/RAND_MAX < 0.001) {
        for(TNEANet::TNodeI jt = md2.BegNI(); jt < md2.EndNI(); jt++) {
          if(float(rand())/RAND_MAX < 0.25) {
            cn.AddEdge(it.GetId(), jt.GetId());
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
  std::cout << "Adding dummy attribute to one mode of TMMNet..." << std::endl;
  TInt64 mid = 0;
  TModeNet& mode = mmnet->GetModeNetById(mid);
  mode.AddIntAttrN("TEST");
  
  i = 0;
  for(TNEANet::TNodeI it = mode.BegNI(); it < mode.EndNI(); it++) {
    mode.AddIntAttrDatN(it, i, "TEST");
    i++;
  }

  // to use, change CopyModeWithoutNodes from private to public
  // std::cout << "\nMaking new MMNet and copying a mode w/o nodes to it from original..." << std::endl;
  // PMMNet mmnet2 = TMMNet::New();
  // mmnet->CopyModeWithoutNodes(mmnet, mmnet2, mid);
  // std::cout << "Checking that new mode has no nodes or edges, but still has test attribute name..." << std::endl;
  // TModeNet& copymode = mmnet2->GetModeNetById(mid);
  // assert(copymode.GetMxNid() == 0);
  // assert(copymode.GetMxEid() == 0);
  // assert(copymode.IsIntAttrN("TEST"));
  // std::cout << "Mode dump:" << std::endl;
  // copymode.Dump();

  return 0;
}
