========================================================================
    CONVERSIONS: sample graph conversion usage 
========================================================================

This example demonstrates the usage of TSNAP::ConvertGraphFast. 
The application supports conversions among TUNGraph, TNGraph, and TNEANet. 

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
   -i:Input edgelist file name (default:'../as20graph.txt')
   -b:Input graph binary file name (default:'')
   -s:Specify Graph type to convert from: TTable, TUNGraph, TNGraph, or TNEANet. (default:'TUNGraph')
   -d:Specify Graph type to convert to: TUNGraph, TNGraph, or TNEANet. (default:'TNGraph')

/////////////////////////////////////////////////////////////////////////////
Usage:

Convert TUNGraph->TNGraph.

conversions -s:TNGraph -d:TUNGraph
