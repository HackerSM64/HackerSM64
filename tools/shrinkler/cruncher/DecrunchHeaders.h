// Copyright 1999-2022 Aske Simon Christensen. See LICENSE.txt for usage terms.

/*

Binary Amiga code for the decrunch headers.

The .dat files are generated from the .bin files by the Makefile.

*/

#pragma once

unsigned char Header1[] = {
#include "Header1.dat"
};

unsigned char Header1C[] = {
#include "Header1C.dat"
};

unsigned char Header1T[] = {
#include "Header1T.dat"
};

unsigned char Header1CT[] = {
#include "Header1CT.dat"
};

unsigned char Header2[] = {
#include "Header2.dat"
};

unsigned char Header2C[] = {
#include "Header2C.dat"
};

unsigned char OverlapHeader[] = {
#include "OverlapHeader.dat"
};

unsigned char OverlapHeaderC[] = {
#include "OverlapHeaderC.dat"
};

unsigned char OverlapHeaderT[] = {
#include "OverlapHeaderT.dat"
};

unsigned char OverlapHeaderCT[] = {
#include "OverlapHeaderCT.dat"
};

unsigned char MiniHeader[] = {
#include "MiniHeader.dat"
};

unsigned char MiniHeaderC[] = {
#include "MiniHeaderC.dat"
};
