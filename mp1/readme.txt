Here comes the source code for MP1. The following files are included:

-- my_allocator.h / my_allocator.c : Header/Implementation files for the allocator.
The implementation file contains placeholders for the implementations
of the functions defined in the header. This code is operational
as-is, but you need to replace the implementation with your code!

-- memtest.c : This file tests your allocator by calling the highly
   recursive function 'Ackerman', which in turn allocates and then
   allocates memory many times over.  You will need to add the
   following to the main function in this file (at the marked locations):
   .. Processing of command-line arguments.
   .. Initialization of the allocator before starting the allocations.
   .. Clean-up of the allocator on exit.

-- makefile : This file tells 'make' how to compile everything together.

