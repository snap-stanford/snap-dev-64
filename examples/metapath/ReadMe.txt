======================================================================== 
    Metapath
========================================================================

A metapath is a sequence of crossnets (edge types or relation types) in 
a multimodal network. In the same way that categorizing edges with
crossnets clarifies their meaning in a graph with many node types, 
enumerating paths in a graph by the sequence of edge types they contain
allows us to name a particular type of composite relation in the graph
and know which objects share this relation.

The method TMMNet::GetSubgraphByMetapath accepts a set of starting modes
and a sequence of contiguous crossnets constituting a metapath. It visits
all nodes and edges that are reachable from the starting set by following 
any path whose sequence of edge types match the given crossnets. It 
returns the entire multimodal subnetwork of nodes and edges visited 
during the traversal, allowing for small-scale analytics of the subgraph
we are interested in.

This example involves a multimodal network where each mode represents a
social network (Facebook, LinkedIn, Instagram, Twitter, or GitHub).
Intra-mode crossnets contain edges representing connections within
the networks (directed edges for sites where connections are follows,
undirected edges where connections are reciprocal friendships). 
Inter-mode crossnets connect a user node in one network to nodes
representing accounts owned by the same person on other sites. 

////////////////////////////////////////////////////////////////////////
Parameters:
Start user id (-u:)
Start user social network (-n:) (choose from facebook, linkedin, instagram,
				 twitter, github)


////////////////////////////////////////////////////////////////////////
Usage:

Print a list of all accounts, across all social networks, that are 
connected to an account owned by the person who owns the Facebook profile
with ID 534.

./metapath -u:534 -n:facebook
