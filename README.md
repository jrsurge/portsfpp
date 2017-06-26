# portsf++

A C++ wrapper around a modified version of portsf by Richard Dobson.

portsf++ was created because portsf doesn't have any C++ wrappers.
portsf also internally manages the files, using a fixed length array - 
a limit we can do without if the library relinquishes memory 
management.

portsf++ uses RAII to ensure resources are properly allocated and 
cleared up, using portsf's original mechanisms

portsf++ also uses pimpl to shield the inner workings of portsf, 
some of which are now exposed with the modifications made to the 
underlying library.
