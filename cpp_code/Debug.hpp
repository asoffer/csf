#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

// Change to 1 to turn them on
// Change to 0 to turn them off
#define ASSERTIONS_ON 0 
#define DEBUG_MESSAGES_ON 0

/*******************************************************************************
* Assertion Macros
*
* Note that the NULL pointer dereferences in fatalAssert() and alwaysAssert()
* are so you can view the callstack which led to the assertion.
*******************************************************************************/
#if ASSERTIONS_ON
	// Asserts the condition, and terminates the program if false.
	#define fatalAssert( X, MSG ) if( !( X ) ) { std::cout << "Fatal assertion failed: " << std::endl << "\tContidion: " << #X << std::endl << "\tFile: " << __FILE__ << std::endl << "\tLine: " << __LINE__ << std::endl << "\tMessage: " << MSG << std::endl; int *p = NULL; cout << *p; }
	// Asserts the condition, but does not terminate the program if false.
	#define softAssert( X, MSG ) if( !( X ) ) { std::cout << "Soft assertion failed: " << std::endl << "\tContidion: " << #X << std::endl << "\tFile: " << __FILE__ << std::endl << "\tLine: " << __LINE__ << std::endl << "\tMessage: " << MSG << std::endl; }
	// A fatal assert that always occurs if executed.
	#define alwaysAssert( MSG ) { std::cout << "Always assert here: " << std::endl << "\tFile: " << __FILE__ << std::endl << "\tLine: " << __LINE__ << std::endl << "\tMessage: " << MSG << std::endl; int *p = NULL; cout << *p; }
#else
	#define fatalAssert( X, MSG ) {}
	#define softAssert( X, MSG ) {}
	#define alwaysAssert( MSG ) {}
#endif

/***************************************
* Debug Print Macros
***************************************/
#if DEBUG_MESSAGES_ON
	#define debugMessage( X ) printf X 
#else
	#define debugMessage( X ) {}
#endif

#endif

