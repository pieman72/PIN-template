/**
 * Template for a "PIN tool". Gets handles on a running process and outputs
 * diagnostic information about it.
 */
#include "pin.H"
#include <iostream>
#include <fstream>


// Global variables
UINT64 insCount = 0;	// Number of dynamically executed instructions
UINT64 bblCount = 0;	// Number of dynamically executed basic blocks
UINT64 threadCount = 0;	// Total number of threads, including main thread
std::ostream *out;		// Ostream pointer to be used for diagnostic output


// CLI Args ("knobs")
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool"
	,"o"					// Argument name on CLI
	, "results.txt"			// Default value (can be blank)
	,"Name of output file"	// Description
);


// Usage info help message
INT32 Usage(){
	cerr << "This tool prints out the number of dynamically executed "
		<< "instructions, basic blocks and threads in the application."
		<< endl
		<< endl
		<< KNOB_BASE::StringKnobSummary()
		<< endl;
	return -1;
}


// Track basic blocks
//	@param numInstructionsInBbl: the number of instructions in a given block
VOID incrementBasicCounts(UINT32 numInstructionsInBbl){
	++bblCount;
	insCount += numInstructionsInBbl;
}


// Process a TRACE
//	@param trace: the TRACE itself that will be analyzed
//	@param v: a magical mystery pointer...
VOID processTrace(TRACE trace, VOID *v){
	// Iterate over basic blocks in the trace
	for(BBL bbl=TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl)){
		// Insert a call to incrementBasicCounts() before every basic block
		BBL_InsertCall(
			bbl					// Basic block iterator
			,IPOINT_BEFORE		// Insert the call BEFORE each basic block
			,(AFUNPTR) incrementBasicCounts	// Pointer to inserted function
			,IARG_UINT32		// Type of first arg to incrementBasicCounts
			,BBL_NumIns(bbl)	// Value of first arg to incrementBasicCounts
			,IARG_END			// End of args to incrementBasicCounts
		);
	}
}


// Process each thread that is started
//	@param threadIndex: PIN's unique index number for the thread
//	@param ctxt: the initial register state for the new thread
//	@param flags: OS specific flags for the new thread
//	@param v: a magical mystery pointer...
VOID processThread(THREADID threadIndex, CONTEXT *ctxt, INT32 flags, VOID *v){
	threadCount++;
}


// Print final diagnostic info after tracing completes
VOID Fini(INT32 code, VOID *v){
	*out << endl << "===============================================" << endl;
	*out << "SOME_TOOL analysis results: " << endl;
	*out << "Number of instructions: " << insCount  << endl;
	*out << "Number of basic blocks: " << bblCount  << endl;
	*out << "Number of threads: " << threadCount  << endl;
}


// SOME_TOOL main
int main(int argc, char *argv[]){
	// Pass own args to PIN_Init for handling, and on error print usage message
	if(PIN_Init(argc,argv)) return Usage();

	// Grab the output file name from args, and redirect output to it
	string fileName = KnobOutputFile.Value();
	out = new std::ofstream(fileName.c_str());

	// Register function to be called for instrumenting traces
	TRACE_AddInstrumentFunction(processTrace, 0);

	// Register function to be called for every thread before it starts running
	PIN_AddThreadStartFunction(processThread, 0);

	// Register function to be called when the application exits
	PIN_AddFiniFunction(Fini, 0);

	// Start the program, never returns
	PIN_StartProgram();

	return 0;
}
