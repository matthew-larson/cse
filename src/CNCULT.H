// Copyright (c) 1997-2016 The CSE Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

// cncult.h: public declarations for cncult.cpp, cncult2,3,4,5,6.cpp.

	// see also: irats.h: declarations of input record arrays (rats)
	//           cnculti.h: cncult internal functions shared only amoung cncult,2,3,4,5,6.cpp


/*-------------------------------- DEFINES --------------------------------*/

/*--- "Use Class" bits for object member expressions ---

	Mechanism intended (1991) for use eg to not evaluate hourly-variable exprs for solar inputs
	on on-solar-calc days... But never (1995) used thus, only UENDIVL bit now significant.

	For CULT.uc and thence useCl args to exPile, exEvf.  Data type: USI; unsigned:4 in CULT.uc. */

//former use class values for CULT.uc, which is now (1995) always 0 (changed to 1 at access in cul.cpp) and could be deleted:
// #define UGEN  1  	general: none of the below (may match a cul.cpp dfl)
// #define USOL  2  	used in solar calcs only.  no uses found (except in exEvfUp calls 12-91, deleted 9-95).

//end-interval-evaluation bit in use class: only bit now explicitly used, 9-95:
#define UENDIVL 4	/* expression is to be evaluated at end of interval, not start (eg contains results probes).
			   supplied by exman:exPile upon seeing EVENDIVL bit in the 'evf' word, 12-9-91.
			   Should be only bit on.  Make an independent characteristic of expr if found necessary. */


/*------------------------- FUNCTION DECLARATIONS -------------------------*/

// cncult.cpp (many functions in cncult.cpp are called only via ptrs in CULT tables pointed to by cnTopCult)
extern CULT cnTopCult[];		// top level CULT for CSE (CULT = Cne User Language Table, cul.c)
void cnPreInit();

// cncult2.cpp
void cncultClean(CLEANCASE cs);		// cncultn.cpp cleanup fcn 12-3-93
//topCkf: see cnculti.h.
RC topReCkf();				// re-check/setup for main run after autosize

// cncult4.cpp -- externally called fcns re report/export formatting
char* getErrTitleText();			// access ERR report title text
char* getLogTitleText();			// access LOG report title text
char* getInpTitleText();			// access INP report title text
char* getHeaderText( int pageN);    // access header text
char* getFooterText( int pageN);	// .. footer ..  inserts given page number
// int getCpl( TOPRAT** pTp=NULL);	// get chars/line, default if Top not yet init & input value unavailable
int getBodyLpp();					// get lines of report body per page, similarly
void freeHdrFtr();					// free header, footer, report title texts
RC   FC freeRepTexts();
// impFcn(): import files: see impf.h 2-94.

// end of cncult.h