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
//#include "timenet.h"         // time evolving networks             Need to uncomment this
#include "mmnet.h"           // multimodal networks

// algorithms
#include "subgraph.h"        // subgraph manipulations
#include "anf.h"             // approximate diameter calculation
//#include "bfsdfs.h"          // breadth and depth first search      Need to uncomment this
#include "cncom.h"           // connected components
#include "kcore.h"           // k-core decomposition
#include "alg.h"             // misc graph algorithms
//#include "triad.h"           // clustering coefficient and triads   Need to uncomment this
#include "gsvd.h"            // SVD and eigenvector computations
//#include "gstat.h"           // graph statistics                      Need to uncomment this
//#include "centr.h"           // centrality measures                   Need to uncomment this
//#include "cmty.h"            // community detection algorithms        Need to uncomment this
#include "flow.h"            // network flow algorithms
//#include "coreper.h"         // core-periphery algorithms             Need to uncomment this
#include "randwalk.h"        // Personalized PageRank

// graph generators
//#include "ggen.h"            // graph generators (preferential attachment, small-world, ...)       Need to uncomment this
//#include "ff.h"              // forest fire graph generator            Need to uncomment this

#include "gio.h"
#include "gviz.h"
#include "ghash.h"
//#include "statplot.h"                                                 Need to uncomment this

// table data structures and algorithms
//#include "table.h"         // table                    Need to uncomment this
//#include "conv.h" 	   // conversion functions - table to graph
#include "numpy.h" 	   // numpy conversion
//#include "casc.h"          // cascade detection^M

#endif // SNAP_H
