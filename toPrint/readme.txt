Student:	Jeremy Mah
ID:		1005 3908
COURSE:		CPSC 501
INSTRUCTOR:	Leonard Manzara
TUTORIAL:	tut 3, MW @ 10am
TA:		Lee Ringham

Requires library lsndfile for reading wav files...
(requires super user so cannot install onto lab computers...)
on ubuntu:
	sudo apt-get install libsndfile1-dev

to compile: 
	g++ convolve.cpp -lsndfile

tested on: 
	osx
	ubuntu vm on windows 
http://ubuntuforums.org/showthread.php?t=968690

included important files:
	report
	readme
	gitlog

	convolve.cpp  (same as convolveOptimized.cpp)
	baseConvolve.cpp
	fftConvolve.cpp
	convolveOptimized.cpp

	main.cpp
	utils.cpp
	libsndfile-1.0.26   (for osx, enter the folder and run './configure; make; make install' as defined in the library's readme)
	tabla.wav
	irBIGHALL
	tablaSlowConvolve.wav
	
Decisions:
	reading wav iles uses lsndfile library,
	writing wav files uses code from tutorials and is in utils.cpp
	sound files used were tabla.wav and irBIGALL, both were provided on d2l.

	fourier transformation was one found online which also in C textbook.

	All optimization changes were made in convolve.cpp, as such only multiple versions of that file are included.


