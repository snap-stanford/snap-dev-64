#include "Snap.h"
#include "dirent.h"
#include <iostream>
#include <fstream>

static void ParseArgs(int& argc, char *argv[], TStr& Network, TInt64& User) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("\nExample use of metapaths in discovering cross-platform friends on a social network."));
  User = Env.GetIfArgPrefixInt("-u:", 0, "Start user ID");
  Network = Env.GetIfArgPrefixStr("-n:", "facebook", "Start user social network");
}

static int BuildMMNet(const TStr& ModeDir, const TStr& CrossDir, PMMNet& mmnet) {
  // Read modes
  DIR *modedir = opendir(ModeDir.CStr());
  if (modedir == NULL) { return -1; }

  dirent *modedirent;
  while ((modedirent = readdir(modedir)) != NULL) {
    if (modedirent->d_type == DT_REG && ((TStr)modedirent->d_name).EndsWith(".mode.tsv")) {
      TStr modefilename = ModeDir + TStr::Fmt("/%s", modedirent->d_name);
      std::ifstream modefile(modefilename.CStr());

      std::string header;
      std::getline(modefile, header); // #MODE\t<modename>
      TStrV fields; 
      ((TStr)header.c_str()).SplitOnAllCh('\t', fields);
      int64 mid = mmnet->AddModeNet(fields[1]);
      TModeNet& mode = mmnet->GetModeNetById(mid);
      std::getline(modefile, header); // skip next header line

      while (true) {
        std::string line;
        std::getline(modefile, line);
        if (modefile.eof()) { break; }

        mode.AddNode(((TStr)line.c_str()).GetInt64());
      }
      modefile.close();
    }
  }

  // Read crossnets
  DIR *crossdir = opendir(CrossDir.CStr());
  if (crossdir == NULL) { return -2; }

  dirent *crossdirent;
  while ((crossdirent = readdir(crossdir)) != NULL) {
    if (crossdirent->d_type == DT_REG && ((TStr)crossdirent->d_name).EndsWith(".cross.tsv")) {
      TStr crossfilename = CrossDir + TStr::Fmt("/%s", crossdirent->d_name);
      std::ifstream crossfile(crossfilename.CStr());

      std::string header;
      std::getline(crossfile, header); // #<directedness>\t<modename1>\t<modename2
      TStrV fields; 
      ((TStr)header.c_str()).SplitOnAllCh('\t', fields);
      bool directed = (fields[0] == "#DIRECTED");

      int64 cnid = mmnet->AddCrossNet(fields[1], fields[2], fields[1] + "_" + fields[2], directed);
      TCrossNet& crossnet = mmnet->GetCrossNetById(cnid);
      std::getline(crossfile, header); // skip next header line

      while (true) {
        std::string line;
        std::getline(crossfile, line);
        if (crossfile.eof()) { break; }

        ((TStr)line.c_str()).SplitOnAllCh('\t', fields);
        int64 nid1 = fields[0].GetInt64(), nid2 = fields[1].GetInt64();

        crossnet.AddEdge(nid1, nid2);
      }
      crossfile.close();
    }
  }
  return 0;
}

static const char *kModeDir = "modes";
static const char *kCrossDir = "crossnets";
int main(int argc, char *argv[]) {
  TStr Network;
  TInt64 User;
  ParseArgs(argc, argv, Network, User);

  PMMNet mmnet = TMMNet::New();
  int status = BuildMMNet(kModeDir, kCrossDir, mmnet);
  if (status != 0) { return status; }

  if (mmnet->GetModeId(Network.ToLc()) == -1) {
    std::cerr << "No network with name " << Network.CStr() << std::endl;
    return -3;
  }
  TModeNet& startnet = mmnet->GetModeNetByName(Network);

  if (!startnet.IsNode(User)) { 
    std::cerr << "No user with ID " << User.Val << " in network " << Network.CStr() << std::endl;
    return -4;
  }

  // Create metapaths connecting user to friends in original network and other networks
  Network = Network.ToLc();
  TVec<TInt64V, int64> friend_metapaths;
  for (TMMNet::TModeNetI MI = mmnet->BegModeNetI(); MI < mmnet->EndModeNetI(); MI++) {
    TInt64V metapath;
    if (MI.GetModeName() != Network) { 
      TStr crossname = TStr::Fmt("%s_%s", Network.CStr(), MI.GetModeName().CStr());
      if (mmnet->GetCrossId(crossname) == -1) { // this crossnet doesn't exist, but if we reverse the name order, it will
        crossname = TStr::Fmt("%s_%s", MI.GetModeName().CStr(), Network.CStr());
      }
      metapath.Add(mmnet->GetCrossId(crossname));
    }
    metapath.Add(mmnet->GetCrossId(TStr::Fmt("%s_%s", MI.GetModeName().CStr(), MI.GetModeName().CStr())));
    friend_metapaths.Add(metapath);
  }

  // extract subnetwork
  TInt64V StartNodeIds;
  StartNodeIds.Add(User);
  PMMNet subnet = mmnet->GetSubgraphByMetapaths(mmnet->GetModeId(Network), StartNodeIds, friend_metapaths);

  // report friends in other networks
  std::cout << "\nUser " << User.Val << " on site " << Network.CStr() << " has the following friends"
            << " on these platforms:" << std::endl;
    for (TMMNet::TModeNetI MI = subnet->BegModeNetI(); MI < subnet->EndModeNetI(); MI++) {
      std::cout << MI.GetModeName().CStr() << ":";
      for (TNEANet::TNodeI NI = MI.GetModeNet().BegNI(); NI < MI.GetModeNet().EndNI(); NI++) {
        if (NI.GetId() != User || MI.GetModeName() != Network) {
          std::cout << " " << NI.GetId();
        }
      }
      std::cout << std::endl;
    }
  return 0;
}
