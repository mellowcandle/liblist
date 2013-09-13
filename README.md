liblist - by Ramon Fried (2013)
=======
Generic linked list management library in C
The library is not thread safe. if it is used in multithreaded environment, 
the user should take care to sync the access to the list.
Library implementation is straight forward.

Prerequisites
=============
libllist will build on any Unix like machine.
However, in order to run the test suites, check library must be downloaded and installed aswell. http://check.sourceforge.net/
In order to generate the documentation doxygen must be installed aswell.

HOW TO BUILD ?
==============
$ make
$ sudo make install

WHERE DOES IT INSTALLED TO ?
============================
The library is installed in /usr/local/lib and the header to /usr/local/include


