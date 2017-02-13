namespace TSnap {

// Mandatory name of edge capacity attribute for flow networks.
const TStr CapAttrName = "capacity";

/// Returns the maximum integer valued flow in the network \c Net from source \c SrcNId to sink \c SnkNId. ##TSnap::GetMaxFlowIntEK
int64 GetMaxFlowIntEK (PNEANet &Net, const int64 &SrcNId, const int64 &SnkNId);
/// Returns the maximum integer valued flow in the network \c Net from source \c SrcNId to sink \c SnkNId. ##TSnap::GetMaxFlowIntEK
int64 GetMaxFlowIntPR (PNEANet &Net, const int64 &SrcNId, const int64 &SnkNId);

};
