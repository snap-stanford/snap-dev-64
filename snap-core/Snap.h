#ifndef SNAP_H
#define SNAP_H

/// ##mainpage
#define _USE_MATH_DEFINES    // to use cmath's constants for VS

/////////////////////////////////////////////////
// SNAP library
#include "base.h"
#include "gnuplot.h"
#include "linalg.h"

#include "gbase.h"
#include "util.h"
#include "attr.h"            // sparse attributes

// graph data structures
#include "graph.h"           // graphs
#include "graphmp.h"         // graphs
#include "network.h"         // networks
#include "networkmp.h"       // networks OMP
#include "bignet.h"          // large networks
//#include "timenet.h"         // time evolving networks             TODO 64
#include "mmnet.h"           // multimodal networks

// algorithms
#include "subgraph.h"        // subgraph manipulations
#include "anf.h"             // approximate diameter calculation
//#include "bfsdfs.h"          // breadth and depth first search      TODO 64
#include "cncom.h"           // connected components
#include "kcore.h"           // k-core decomposition
#include "alg.h"             // misc graph algorithms
//#include "triad.h"           // clustering coefficient and triads   TODO 64
#include "gsvd.h"            // SVD and eigenvector computations
//#include "gstat.h"           // graph statistics                      TODO 64
//#include "centr.h"           // centrality measures                   TODO 64
//#include "cmty.h"            // community detection algorithms        TODO 64
#include "flow.h"            // network flow algorithms
//#include "coreper.h"         // core-periphery algorithms             TODO 64
#include "randwalk.h"        // Personalized PageRank

// graph generators
//#include "ggen.h"            // graph generators (preferential attachment, small-world, ...)       TODO 64
//#include "ff.h"              // forest fire graph generator            TODO 64

#include "gio.h"
#include "gviz.h"
#include "ghash.h"
//#include "statplot.h"                                                 TODO 64

// table data structures and algorithms
//#include "table.h"         // table                    
//#include "conv.h" 	   // conversion functions - table to graph
#include "numpy.h" 	   // numpy conversion
//#include "casc.h"          // cascade detection^M

#endif // SNAP_H
