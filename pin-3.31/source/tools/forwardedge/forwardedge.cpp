/*
 * Copyright (C) 2007-2023 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs 
 *  and could serve as the starting point for developing your first PIN tool
 */

#include "pin.H"
#include <iostream>
#include <fstream>
#include <unordered_map>
using std::cerr;
using std::endl;
using std::string;
using std::unordered_map;
using std::hex;

/* ================================================================== */
// Global variables
/* ================================================================== */

ADDRINT offset;
ADDRINT lowest;
ADDRINT highest;

bool loaded = false;

std::ostream* out = &cerr;

UINT64 indirectCount = 0;

unordered_map<int, bool> addrMap;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */

KNOB< string > KnobInputFile(KNOB_MODE_WRITEONCE, "pintool", "f", "", "specify file name for forwardedge input");
KNOB<ADDRINT> KnobUserLowest(KNOB_MODE_WRITEONCE, "pintool", "low", "0", "specify lowest address");
KNOB<ADDRINT> KnobUserHighest(KNOB_MODE_WRITEONCE, "pintool", "high", "0", "specify highest address");
/* ===================================================================== */
// Analysis routines
/* ===================================================================== */


VOID countIndirect(ADDRINT target) {
    if(loaded) {
        if (target < highest && target > lowest) {
            if (addrMap.count(target-lowest) == 0) {
                printf("address not allowed %lx absolute addr %lx\n", target-lowest, target);
                exit(0);
            }
            // *out << hex << target-lowest << endl;
            indirectCount++;
        }
    } 
}

/* ===================================================================== */
// Instrumentation callbacks
/* ===================================================================== */

VOID Instruction(INS ins, VOID* v) {
    if (INS_IsIndirectControlFlow(ins) && INS_IsCall(ins)) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)countIndirect, IARG_BRANCH_TARGET_ADDR, IARG_END);
    }
}

VOID ImageLoad (IMG img, VOID* V) {
    if (IMG_IsMainExecutable(img)) {
        loaded = true;
        offset = IMG_LoadOffset(img);

        if (KnobUserLowest.Value() <= KnobUserHighest.Value() ) {
            lowest = IMG_LowAddress(img) + KnobUserLowest.Value();
            highest = IMG_LowAddress(img) + KnobUserHighest.Value();
        } else {
            lowest = IMG_LowAddress(img);
            highest = IMG_HighAddress(img);
        }

        printf("highest: %lx lowest: %lx\n", highest, lowest);
    }
    
}

VOID ImageUnload(IMG img, VOID* v) {
    if (IMG_IsMainExecutable(img)) {
        loaded = false;
    }
}

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param[in]   argc            total number of elements in the argv array
 * @param[in]   argv            array of command line arguments, 
 *                              including pin -t <toolname> -- ...
 */
int main(int argc, char* argv[])
{
    // Initialize PIN library. Print help message if -h(elp) is specified
    // in the command line or the command line is invalid
    PIN_Init(argc, argv);

    string inFileName = KnobInputFile.Value();
    if (!inFileName.empty()) {
        std::ifstream inFile;
        inFile.open(inFileName);
        int addr;
        int numaddr = 0;
        while (inFile >> hex >> addr) {
            // printf("read address %x\n", addr);
            addrMap[addr] = true;
            numaddr++;
        }
        printf("numaddr: %d\n", numaddr);
    }

    INS_AddInstrumentFunction(Instruction, 0);

    IMG_AddInstrumentFunction(ImageLoad, 0);

    IMG_AddUnloadFunction(ImageUnload, 0);

    cerr << "===============================================" << endl;
    cerr << "This application is instrumented by forwardedge" << endl;
    cerr << "===============================================" << endl;

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
