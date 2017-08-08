/////////////////////////////////////////////////
// Graph Base
namespace TSnap {

TStr GetFlagStr(const TGraphFlag& GraphFlag) {
  switch (GraphFlag) {
    case gfUndef : return "Undef";
    case gfDirected : return "Directed";
    case gfMultiGraph : return "Multigraph";
    case gfNodeDat : return "NodeDat";
    case gfEdgeDat : return "EdgeDat";
    case gfSources : return "Sources";
    case gfBipart : return "Bipartite";
    default: FailR("Unknown graph type");
  };
  return TStr();
}

};  // namespace TSnap

/////////////////////////////////////////////////
// Union Find
int64 TUnionFind::Find(const int64& Key) {
  int64 SetId = Key, parent = Parent(Key);
  // find set id
  while (parent != -1) {
    SetId = parent;
    parent = Parent(parent);
  }
  // flatten
  parent = Key;
  while (parent != -1) {
    const int64 tmp = Parent(parent);
    if (tmp != -1) { Parent(parent) = SetId; }
    parent = tmp;
  }
  return SetId;
}

void TUnionFind::Union(const int64& Key1, const int64& Key2) {
  const int64 root1 = Find(Key1);
  const int64 root2 = Find(Key2);
  TInt64& rank1 = Rank(root1);
  TInt64& rank2 = Rank(root2);
  if (rank1 > rank2) { Parent(root2) = root1; }
  else if (rank1 < rank2) { Parent(root1) = root2; }
  else if (root1 != root2) {
    Parent(root2) = root1;
    Rank(root1)++;
  }
}

void TUnionFind::Dump() {
  printf("  key\tset\n");
  for (int64 i = 0; i < KIdSetH.Len(); i++) {
    printf("  %s\t%s\n",
      TInt64::GetStr(int64(KIdSetH.GetKey(i))).CStr(),
      TInt64::GetStr(int64(Find(KIdSetH.GetKey(i)))).CStr());
  }
  printf("\n");
}
