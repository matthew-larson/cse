// Copyright (c) 1997-2017 The CSE Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

// cgwthr.cpp -- Weather file functions for CSE simulator

//------------------------------- INCLUDES ----------------------------------
#include <cnglob.h>	// CSE global definitions

#include <ancrec.h>		// record: base class for rccn.h classes
#include <rccn.h>		// TOPRAT .. class declarations
#include <msghans.h>	// MH_R0168
#include <mmpak.h>		// mminit mmean. restored 9-95.

#include <psychro.h>	// psyHumRat1
#include <slpak.h>		// slday, slaniso
#include <wfpak.h>
#include <cnguts.h>
#include <cul.h>		// FsSET 6-95

#include <cgwthr.h>		// decls for this file


//------------------------- PUBLIC VARIABLES USED --------------------------

//--- Preset factors applied in cgwthr.cpp:cgWfRead() to Wthr members
// Top.radBeamF	= 1.;	// Beam radiation fctr. appl sees ANISO( <fileVal>) * BeamRadFactor.
// Top.radDiffF	= 1.;	// Diffuse variant of BeamRadFactor, ditto.
// Top.windF	= 1.; 	// Wind speed factor: appl sees <fileValue> * WindFactor.
//						   set by user input/default to 1.; used in cgWfRead().

//--- Weather file info is in following "basAnc records" (in rundata.cpp) (so can be probed):
// WFILE Wfile: info about open weather file.  Members set/used here, also cncult2.cpp 1-95.
// WFDATA Wthr: this hour's weather data. ditto.  Class decls in rccn.h, story in wfpak.h.
// WFDATA WthrNxHr: NEXT hour's weather info, conditionally read ahead here,
//		    because next-hour values are needed re subhourly interpolation of irradiance data,
//		    but Wthr must contain current data for probes & stdf setpoint use. 1-95.

//---------------------------- LOCAL VARIABLES ------------------------------

//---------------------------- LOCAL FUNCTIONS ------------------------------

//---------------------------------------------------------------------------
void FC cgWthrClean( 		// cgwthr overall init/cleanup routine
	CLEANCASE cs )	// [STARTUP,] ENTRY, DONE, or CRASH (type in cnglob.h)
{
// added while converting from main program to repeatedly-callable, DLLable subroutine.
	/* function should: initialize any variables for which data-initialization was previously assumed,
	                    free all heap memory and NULL the pointers to it,
	                    close any files and free other resources. */

// close weather file possible if end of run
	if (cs != ENTRY)		// not at startup: not if Wfile.init() has not been called
		cgWfDone();		// close any open weather file

// init weather file info record
	Wfile.wf_Init();		// sets record to say no file open
	Wthr.wd_Init();			// clears record
	WthrNxHr.wd_Init();		// clears record

	// add any other init/cleanup found necessary or desirable

}		// cgWthrClean
//---------------------------------------------------------------------------
RC TOPRAT::tp_WthrInit()  	// Initialize weather data/ display any error messages.
// also uses: tp_wfName, tp_TDVfName, [,.isDT]
// returns non-RCOK if error, message already issued.
{
	RC rc = Wfile.wf_Open( tp_wfName, tp_TDVfName);	// open wthr file and option TDV file
													// init WFILE object, msg if error.
	if (!rc)
		rc = Wfile.wf_FillWDYEAR( WRN);		// read/cache entire weather file
											//   TODO: handle partial years

	// design conditions
	//  set any derived values
	//  WHY here: location dependence, call after locinit
	DESCOND* pDC;
	RLUP( DcR, pDC)
		rc |= pDC->dc_RunInit();

	return rc;
}		// TOPRAT::tp_WthrInit
//---------------------------------------------------------------------------
RC TOPRAT::tp_WthrBegHour()		// start-hour weather stuff: read file, set up public Wthr and Top members.
// tp_WthrBegSubhr must also be called, after this function.

// uses: .isBegRun, .isDT, .jDayST, .iHrST, .radBeamF, .readDiffF, .windF,
//       .tp_dsDay, .auszMon, .heatDsTDbO, .heatDsTWbO;
// sets: all Top hourly weather members
{
	RC rc = RCOK;

	if (isBegRun)			// if first hour of warmup, or of run if no warmup, or of 1st rep of autoSize des day
	{
		// get hour's weather data to WthrNxHr if previous call did not read ahead
		//   -- at first hour of simulation.
		rc = WthrNxHr.d.wd_WfReader( FALSE,	// fcn below. FALSE to read current hour.
							 &Wfile);  		// public open weather file structure
		if (rc)
			return rc;			// if failed (msg issued), return to caller
	}
// set previous hour members to current hour values before overwriting
	else					// if (!isBegRun) -- else same done below in this fcn
		tp_SetPvHrWthr();

// set current hour WFDATA record from next hour's, which has already been read
	Wthr = WthrNxHr;

// get NEXT HOUR's weather data, needed to interpolate irradiance variables
	if ( iHrST < 23 || !isLastDay	// no read-ahead at main sim last day, last hour: might be no wthr data in file
			||  tp_autoSizing)		// do read-ahead when autosizing: wraps day, and day may be repeated.
	{
		rc = WthrNxHr.d.wd_WfReader( TRUE, 	// read next hour weather data, RCOK if OK. fcn below.
						 &Wfile); 			// public open weather file structure
		if (rc)
			return rc;			// if failed (msg issued), return to caller
	}
	// else: last hour of run: re-use current hour values (already in WthrNxHr) as next hour values.

// set top members from weather data
	// integrated-over-hour values
	radBeamNxHrAv = WthrNxHr.d.wd_bmrad;	// next hour beam irradiance
	radDiffNxHrAv = WthrNxHr.d.wd_dfrad;	// next hour diffuse irradiance
	radBeamHrAv = Wthr.d.wd_bmrad;		// beam irradiance
	radDiffHrAv = Wthr.d.wd_dfrad;		// diffuse irradiance
	// instantaneous values at end of hour
	tDbOHr = Wthr.d.wd_db;				// dry bulb temp, deg F
	tWbOHr = Wthr.d.wd_wb;				// wet bulb temp, deg F
	// sky temp: select value per user model choice
	//   default = from weather file or as estimated (generally Berdahl-Martin)
	tSkyHr = skyModelLW == C_SKYMODLWCH_DEFAULT
		   ? Wthr.d.wd_tSky
		   : Wthr.d.wd_CalcSkyTemp( skyModelLW, iHrST);
	windDirDegHr = Wthr.d.wd_wndDir;	// wind direction, deg, 0=N, 90=E
	windSpeedHr = Wthr.d.wd_wndSpd;		// wind speed, mph
						// Note: windSpeedMin and windF adjustments
						//       already applied in WFDATA::wd_WfReader

// hourly derived weather data
	wOHr =	psyHumRat1( tDbOHr, tWbOHr);	// compute end-current-hour humidity ratio, psychro1.cpp.
	// -1 return for over boiling not expected.
	// wOHr used only for $variables, 1-95.

// initialize "previous hour" members to current hour values on first call of run
	if (isBegRun)					// else same assignments done above in this function
		tp_SetPvHrWthr();

// hourly averages for instantaneous wthr data (bmrad, dfrad come from wthr file as averages)
	tDbOHrAv = .5f*(tDbOPvHr + tDbOHr);			// avg drybulb temp, used re hrly masses & $variable
	tWbOHrAv = .5f*(tWbOPvHr + tWbOHr);			// avg wetbulb temp, used re $variable
	windSpeedHrAv = .5f*(windSpeedPvHr + windSpeedHr);	// avg wind speed, used re $variable
	wOHrAv = .5f*(wOPvHr + wOHr);				// avg humidity ratio, used re $variable

	return rc;
}			// TOPRAT::tp_WthrBegHour
//-----------------------------------------------------------------------------
void TOPRAT::tp_SetPvHrWthr()		// copy current hour wthr mbrs to previous
{
	radBeamPvHrAv = radBeamHrAv;
	radDiffPvHrAv = radDiffHrAv;
	tDbOPvHr = tDbOHr;
	tWbOPvHr = tWbOHr;
	tSkyPvHr = tSkyHr;
	windSpeedPvHr = windSpeedHr;
	wOPvHr = wOHr;
}		// TOPRAT::tp_SetPvHrWthr
//---------------------------------------------------------------------------
RC TOPRAT::tp_WthrBegSubhr()		// start-subhour weather stuff: set subhourly public Top members (mostly interpolated)

// uses: iSubhr, .subhrDur, [.subhrCum,] .isBegRun; Top members set by cgWthrBegHour.
// sets: Top subhourly weather members
{
// save previous-subhr value(s)
	tDbOPvSh = tDbOSh;

// common setup
	float dur = 1.f/nSubSteps;			// duration of subhour
	float f = float(iSubhr)/nSubSteps;    	// fraction of hour passed at start this subhour
	float g = float(iSubhr+1)/nSubSteps;	// fraction of hour passed at end this subhour
	float h = 1.f - g;					// fraction of hour remaining after this subhour

// interpolate values for data with instantenous end-hour values in weather file
	tDbOSh = h * tDbOPvHr + g * tDbOHr;			// end-subhour drybulb temp
	tWbOSh = h * tWbOPvHr + g * tWbOHr;			// end-subhour wetbulb temp
	tSkySh = h * tSkyPvHr + g * tSkyHr;			// end-subhour sky temp
	windSpeedSh = h * windSpeedPvHr + g * windSpeedHr;	// end-subhour wind speed
	// windPresVSh calculated below

	/* setup for interpolating subhour average value for data with average (integrated) values in file:
	   Determine a = fraction previous hour, b = fraction current hour, c = fraction next hour  even if subhr overlaps middle hour.
	   The simple cases are:
	      subhr in 1st half hour: use part prev, part curr hour:   a = .5 - m;  b = .5 + m;   c = 0;
	      subhr in 2nd half hour: use part curr, part next hour:   a = 0;       b = 1.5 - m;  c = m - .5;
	   where  m = (f + g)/2. Use weighted sum when subhr overlaps middle of hour. */

	float f1 = min( f, .5f), g1 = min( g, .5f);		// start and end of portion of subhr in 1st half of hour
	float f2 = max( f, .5f), g2 = max( g, .5f);		// .. in 2nd half of hour
	float fr1 = (g1 - f1)/dur, fr2 = (g2 - f2)/dur; 	// fractions of subhr in 1st and 2nd halves of hour
	float m1 = .5f*(f1 + g1), m2 = .5f*(f2 + g2);	// middle of portions of subhr in 1st and 2nd halves of hour

	float a = (.5f - m1)*fr1;				// fraction of previous hour avg value to use in forming subhr avg value
	float b = (.5f + m1)*fr1 + (1.5f - m2)*fr2;		// .. current ..
	float c = (m2 - .5f)*fr2;				// .. next ..

	ASSERT( fabs(a+b+c-1.) < 1.e-5);			// a + b + c should be 1.0 within limits of float arithmetic

#ifdef SOLAVNEND // undef in cndefns.h (via cnglob.h) 1-18-94: only if computing & using end-ivl as well as ivl avg solar values
o// setup for end-subhour instantaneous values
o
o   float aes, bes, ces;		// fractions of prev, curr, next hr avg to use in end-subhour value
o   if (g < .5) { aes = .5f - g; bes = .5f + g; ces = 0.f; }
o   else { aes = 0.f; bes = 1.5f - g; ces = g - .5f; }
#endif

// Compute adjustment factors (constant over hour) to make subhour values add up to given hour value. See Rob's NILES106.TXT, 1-95.

	double beamAdj = radBeamHrAv==0.f  ?  0.f  : 							// prevents /0
					 8*radBeamHrAv / (radBeamPvHrAv + 6*radBeamHrAv + radBeamNxHrAv);

	double diffAdj = radDiffHrAv==0.f  ?  0.f  :
					 8*radDiffHrAv / (radDiffPvHrAv + 6*radDiffHrAv + radDiffNxHrAv);

// Interpolate subhour average power values for data with average values in file. Multiply by subhrDur for energy.

	radBeamShAv = (a * radBeamPvHrAv + b * radBeamHrAv + c * radBeamNxHrAv) * beamAdj;
	radDiffShAv = (a * radDiffPvHrAv + b * radDiffHrAv + c * radDiffNxHrAv) * diffAdj;

// Interpolate end-subhour instantaneous values for same data

#ifdef SOLAVNEND // undef in cndefns.h
o   radBeamSh = (aes * radBeamPvHrAv + bes * radBeamHrAv + ces * radBeamNxHrAv) * beamAdj;
o   radDiffSh = (aes * radDiffPvHrAv + bes * radDiffHrAv + ces * radDiffNxHrAv) * diffAdj;
o
o#ifdef needed	// unneeded 1-95
o*//Interpolate end-hour instantaneous power values for same data (same as last subhour value)
o*   if (isBegHour)					// compute only at first subhour of hour: no change during hour
o*   {   							// (computed here cuz use beam/diffAdj)
o*      /* AEH, BEH, CEH: fractions of previous, current, and next hour average to use for end-hour instantaneous value:
o*         same as last-subhour aes, bes, ces values. Comment out AEH since is zero. */
o*      //#define AEH 0.f
o*      #define BEH .5f
o*      #define CEH .5f
o*      radBeamHr = (/*AEH * radBeamPvHrAv + */ BEH * Top.radBeamHrAv + CEH * radBeamNxHrAv) * beamAdj;
o*      radDiffHr = (/*AEH * radDiffPvHrAv + */ BEH * radDiffHrAv + CEH * radDiffNxHrAv) * diffAdj;
o*      #undef AEH
o*      #undef BEH
o*      #undef CEH
o*}
o#endif
#endif

// subhourly derived weather data
	wOSh = psyHumRat1( tDbOSh, tWbOSh);		// humidity ratio at end subhour. psychro1.cpp.
	hOSh = psyEnthalpy( tDbOSh, wOSh);		// outdoor enthalpy at end subhour. psychro2.cpp
	windSpeedSquaredSh = windSpeedSh * windSpeedSh;	// windspeed squared (mph^2) at end subhour, re infiltration.
	windSpeedSqrtSh = sqrt( windSpeedSh);			// sqrt( windspeed), mph^.5, at end of subhour, re outside surf convection
	windSpeedPt8Sh = pow( windSpeedSh, 0.8f);		// mph^.8 at end of subhour, re outside surf convection

	tp_airxOSh = airX( tDbOSh);					// heat capacity of air flow @ temp (Btuh/cfm-F): vhc * 60 min/hr.
#if 1	// density rework, 10-18-2012
	tp_rhoDryOSh = psyDenDryAir( tDbOSh, wOSh);				// outdoor dry air density at end of subhour, lbm/ft3
	tp_rhoMoistOSh = psyDenMoistAir2( tp_rhoDryOSh, wOSh);	// outdoor moist air density at end of subhour, lbm/ft3
#if 0
x	float rhoMoistX = psyDensity( tDbOSh, wOSh);	// density at end of subhour, lb/ft3
x	float dif = fabs( tp_rhoMoistOSh-rhoMoistX);
x	if (dif > 0.)
x	{	if (dif > .000001)
x			printf( "rhoMoist mismatch\n");
x	}
x	float rhoDryX = psyDenDryAir2( rhoMoistX, wOSh);
x	dif = fabs( tp_rhoDryOSh-rhoDryX);
x	if (dif > 0.)
x	{	if (dif > .000001)
x			printf( "rhoDry mismatch\n");
x	}
#endif
#else
x	tp_rhoMoistOSh = psyDensity( tDbOSh, wOSh);	// density at end of subhour, lb/ft3
x	tp_rhoDryOSh = psyDenDryAir2( tp_rhoMoistOSh, wOSh);
#endif

// first time, init previous-subhr value(s) to current-subhour value
	if (isBegRun)					// if 1st subhour of run. else, same sets were done at entry.
	{	tDbOPvSh = tDbOSh;
	}

// form subhour average(s) for data with instantenous values in weather file
	tDbOShAv = .5f*(tDbOPvSh + tDbOSh);	// subhour average temp -- only one 1-95 -- used re subhourly masses

	return RCOK;
}			// TOPRAT::tp_WthrBegSubhr
//---------------------------------------------------------------------------
RC WDHR::wd_WfReader( 	// read an hour's weather data and make adjustments
						//   (or synthesize data, for heat design day case)
	BOO nextHour,		// TRUE for next hour, FALSE for current hour (Top.jDayST, Top.iHrST)
	WFILE* pWF) 		// open weather file info object
// also uses Top.jDayST, Top.iHrST;
//               Top.tp_dsDay, .auszMon, heatDsTDbO, .heatDsTWbO, .isBegRun;
//               Top.radBeamF, .readDiffF, .windF;
// returns RCOK if successful
{
	DOY jDayST = Top.jDayST;  		// day of simulation 1-365, std time, appropriate value for autosize design days
	int iHrST = Top.iHrST;  		// hour 0-23, standard time.
	int wfOp = WF_SAVESLRGEOM;		// wf_Read(): do not overwrite solar geometry values
	if (Top.isDT)
		wfOp |= WF_DSTFIX;			// wf_Read(): handle day-transition values during DST
	RC rc = RCOK;

	// modify iHrST and jDayST as needed
	switch (Top.tp_dsDay)
	{
	case 1:			// autoSizing. heating design day. Synthesize no-solar weather, probably const temp.
		// same hour done each call, ignore minor waste
		//  hourly variable conditions allowed but not currently used, 12-2012
		iHrST = 0;
		break;

	case 2:			// autoSizing. cooling. use wthr file design day data for month.
		if (nextHour)  				// if NEXT hour's data requested
			if (++iHrST > 23)    		// if end of day
				iHrST = 0;				// wrap to start SAME day cuz design day is simulated repeatedly
		break;

	case 0:			// main simulation day - not autoSizing
		if (nextHour)					// if caller wants next hour's weather data
		{	if (++iHrST > 23)				// next hour / if was already last hour of day
			{	iHrST = 0;					// next hour is first hour
				if (++jDayST > 365)			// of next day / if was already last day of year
					jDayST = 1;				// wrap back to jan 1 from Dec 31
				// at end warmup, read ahead to 1st run day even if not next date (usually is next date, see cnguts:cgRunI)
				// to have right data for run hour and to not require another day's data in short packed files
				if (Top.isLastWarmupDay)		// if now last day of warmup (and hour 23 if here)
					//if (!Top.tp_autoSizing)		// autoSizing doesn't get here
					jDayST = Top.tp_begDay;   	// read hour 0 of first hour of run
			}
		}
		break;
	}

	// set up solar conditions
#if defined( SOLARFIX)
	slday( jDayST, SLTMLST, 1);	// set up curr SLLOCDAT struct for today if needed
								// sets info re declination of earth's axis, hourly sunupf[], dircos[], slazm[], etc.
								// used by slaniso & most other slpak calls.
#else
	if (iHrST == 0)
		slday( jDayST, SLTMLST);
#endif

	static float dchoriz[3] = { 0.f, 0.f, 1.f };	// dir cos of a horiz surface
	float verSun = 0.f;				// hour's vertical fract of beam (cosi to horiz)
									// init 0 for when sun down.
	float cosz;
	wd_sunup = slsurfhr(dchoriz, iHrST, &verSun, &wd_slAzm, &cosz);
	if (!wd_sunup)
		wd_slAzm = wd_slAlt = 0.f;
	else
		wd_slAlt = kPiOver2 - acos( cosz);
	// NOTE: wfOp includes WF_SAVESLRGEOM -- wd_sunup, wd_slAzm, wd_slAlt survive wf_Read()
	
	// read or generate data
	switch (Top.tp_dsDay)
	{
	case 1:			// autoSizing. heating design day. Synthesize no-solar weather, probably const temp.
		rc = pWF->wf_Read( this, jDayST, iHrST, WRN|wfOp);	// Read data for midnight from weather file
															// WHY: sets ground temp to plausible winter values
		wd_wndSpd = 15;
		wd_wndDir = 0;  						// 15 mph north wind
		wd_bmrad = wd_dfrad = wd_glrad = 0;		// no insolation -- dark all day
		wd_cldCvr = 0;							// no cloud cover (impacts tSky)
		wd_db = Top.heatDsTDbO;					// outdoor temperature as input or as dfl'd from wthr file hdr
		if (Top.IsSet( TOPRAT_HEATDSTWBO))		// if user input the heat design day wetbulb
			wd_wb = Top.heatDsTWbO;				// use input value. hourly variable allowed but not expected.
		else
		{	// calculate default wetbulb
			float w = psyHumRat2( wd_db, .7);	// humidity ratio for drybulb & 70% rel hum
			wd_wb = psyTWetBulb( wd_db, w);		// wetbulb for drybulb & humidity ratio
		}
		rc |= wd_EstimateMissingET1( 0);		// rederive wd_tSky, wd_taDp,
		break;

	case 2:			// autoSizing. cooling. use wthr file design day data for month.
		if (Top.tp_AuszWthrSource() == TOPRAT_COOLDSMO)
			// read month's design day
			rc = pWF->wf_Read( this, Top.auszMon, iHrST, 	// Read hour's data from wthr file design day
					 WRN | WF_DSNDAY );			// option says 2nd arg is design day mon 1-12 not Julian date.
		else
		{	// read actual day
			rc = pWF->wf_Read( this, jDayST, iHrST, WRN|wfOp);	// Read hour's data from weather file
			if (Top.tp_AuszWthrSource() == TOPRAT_COOLDSCOND)
			{	// DSCOND design day: overwrite/adjust weather file values with generated
				int iDC = Top.tp_coolDsCond[ Top.tp_dsDayI-1];
				const DESCOND& DC = DcR[ iDC];
				wd_FillFromDESCOND( DC, iHrST);
			}
		}
		break;

	case 0:			// main simulation day - not autoSizing
		rc = pWF->wf_Read( this, jDayST, iHrST, WRN|wfOp);	// Read hour's data from weather file
		break;
	}
	if (rc)			// if wfRead failed, eg date not in file or type without design days
		return rc;

// Optional anisotropic sky adjustment
	if (Top.skyModel==C_SKYMODCH_ANISO)
		slaniso( &wd_bmrad, &wd_dfrad, iHrST);	// adjust data in place to approx model sky brighter
			       								//   near sun, etc, while still computing with sun's
												//	 beam and isotropic diffuse radiation. slpak.cpp. */

// other adjustments
	wd_bmrad *= Top.radBeamF; 	// Adjust radiation values for possible user input factors, defaults 1.0.
	wd_dfrad *= Top.radDiffF; 	// ..
	wd_wndSpd = Top.windF * max( wd_wndSpd, Top.windSpeedMin);
					// apply wind speed min (user input, default = .5)
					//   and adjust by factor (per user input, default .25)

	// Ensure solar data values are not negative (as found in some CSW files)
	wd_bmrad = max(wd_bmrad, 0.f);
	wd_dfrad = max(wd_dfrad, 0.f);
	wd_glrad = max(wd_glrad, 0.f);


	return rc;
}		// WDHR::wd_WfReader
//---------------------------------------------------------------------------
void FC cgWfDone()   		// Close, clean up hourly simulator weather file
{
	Wfile.wf_Close();		// close wthr file (nop if unopened)
}			// cgWfDone
//=============================================================================
void WDHR::wd_FillFromDESCOND(		// overwrite/adjust hourly data for design conditions
	const DESCOND& dc,	// design conditions
	int iHr)			// current hour, 0-23
{
	// generate db/wb/dbAvg for hour
	float astCor = slASTCor();
	float dbAvg;
	dc.dc_GenerateTemps( iHr, astCor, wd_db, wd_wb, dbAvg);

	// determine humrat, fix wb if too low
	float w = psyHumRatX( wd_db, wd_wb);
	wd_taDp = psyTDewPoint( w);
	wd_cldCvr = 0.f;		// assume clear sky
	wd_tSky = wd_CalcSkyTemp( C_SKYMODLWCH_BERDAHLMARTIN, iHr);

	// peak temps: yesterday = today
	wd_taDbPk = wd_taDbPvPk = dc.dc_DB;

	// average temps: yesterday = today; before that, use weather file
	//   DSTFix??
	float dbAvgDiff = dbAvg - wd_taDbAvg01;
	wd_taDbAvg = wd_taDbAvg01 = dbAvg;
	wd_taDbAvg07 += dbAvgDiff / 7.f;
	wd_taDbAvg14 += dbAvgDiff / 14.f;
	wd_taDbAvg31 += dbAvgDiff / 31.f;

	wd_wndSpd = dc.dc_wndSpd;
	wd_wndDir = 0.f;

	// derive irradiance
	//   note: tauB/tauD both 0 -> no solar
	if (wd_sunup && dc.dc_tauB > 0.f && dc.dc_tauD > 0.f)
		wd_glrad = slASHRAETauModel(
			kPiOver2 - wd_slAlt,
			dc.dc_tauB, dc.dc_tauD,
			wd_bmrad, wd_dfrad);
	else
		wd_glrad = wd_bmrad = wd_dfrad = 0.f;

	// Remaining items: use weather file values
    //    wd_tGrnd, wd_tMains, wd_tdvElec, wd_tdvFuel, wd_tdvElecPk, wd_tdvElecAvg,
	//    wd_tdvElecPvPk, wd_tdvElecAvg01

}		// WDHR::wd_FillFromDESCOND
//===========================================================================

/////////////////////////////////////////////////////////////////////////////
// class DESCOND: cooling design conditions
//                uses ASHRAE methods to generate design day hourly data
/////////////////////////////////////////////////////////////////////////////
RC DESCOND::dc_CkF()		// check after input
{	RC rc = RCOK;
	return rc;
}		// DESCOND::dc_CkF
//-----------------------------------------------------------------------------
#if defined( _DEBUG)
static void DCTauTest( int doy)
{
float ebnlist[] = { 150.f, 200.f, 250.f, 300.f, -1.f };
float edflist[] = { 20.f, 30.f, 40.f };

	DESCOND dc( &DcR, 1);
	dc.dc_doy = doy;
	strcpy( dc.name, "Test");

	for (int ib=0; ebnlist[ ib]>=0.f; ib++)
	{	dc.dc_ebnSlrNoon = ebnlist[ ib];
		for (int id=0; edflist[ id]>=0.f; id++)
		{	dc.dc_edhSlrNoon = edflist[ id];
			dc.dc_CheckFixSolar( 0);
		}
	}
}		// DCTauTest
#endif
//------------------------------------------------------------------------------
RC DESCOND::dc_RunInit()		// init for run
// call after all input-time expressions have been evaluated
// call after locInit (re latitude dependency)
{
#if defined( _DEBUG)
static int tested = 0;
	if (!tested)
	{	DCTauTest( dc_doy);
		tested++;
	}
#endif

	RC rc = RCOK;

	if (dc_MCWB > dc_DB)
		rc |= oer( "dcMCWB (%g) must be <= dcDB (%g)", dc_MCWB, dc_DB);

	// if any solar values given, check and cross-derive
	//   else leave all 0 = no solar
	if (dc_tauB + dc_tauD + dc_ebnSlrNoon + dc_edhSlrNoon > 0.f)
	{	int nTau = IsSetCount( DESCOND_TAUB, DESCOND_TAUD, 0);
		int nE   = IsSetCount( DESCOND_EBNSLRNOON, DESCOND_EDHSLRNOON, 0);
		if (nTau == 0)
			rc |= dc_CheckFixSolar( 0);		// derive tauB / tauD
		else if (nE == 0)
			rc |= dc_CheckFixSolar( 1);		// derive Ebn / Edh
		else
			rc |= oer( "must give either dcTauB/dcTauD and"
				 " not dcEbnSlrNoon/dcEdhSlrNoon or vice-versa");
	}
	else
		oInfo( "no irradiance due to dcTauB/dcTauD/dcEbnSlrNoon/dcEdhSlrNoon all 0");

	return rc;
}	// DESCOND::dc_RunInit
//-----------------------------------------------------------------------------
RC DESCOND::dc_CheckFixSolar(
	int options)	// option bits
					//  0: derive tau from solar noon
					//  1: derive solar noon from tau
{
	RC rc = RCOK;

	// solar noon angles
	slday( dc_doy, SLTMSOLAR, 1);
	float sunAlt, sunAzm;
	slaltazm( 12.f, &sunAlt, &sunAzm);
	float sunZen = kPiOver2 - sunAlt;

	if (options & 1)
	{	rc |=   limitCheck( DESCOND_TAUB, .08, 1.)
		      | limitCheck( DESCOND_TAUD, 1.2, 3.2);
		if (!rc)
			slASHRAETauModel( sunZen, dc_tauB, dc_tauD,
						dc_ebnSlrNoon, dc_edhSlrNoon);
	}
	else	
	{	rc |=   limitCheck( DESCOND_EBNSLRNOON, 0., 370.)
		      | limitCheck( DESCOND_EDHSLRNOON, 0., 110.);
		if (!rc)
		{	bool ret = slASHRAETauModelInv( sunZen, dc_ebnSlrNoon, dc_edhSlrNoon,
					dc_tauB, dc_tauD);
			if (!ret)
				rc = oer( "failed to derive dcTauB and dcTauD."
					"  Check dcEbnSlrNoon and dcEdhSlrNoon input values.");
#if defined( _DEBUG)
			else
			{	float ebn, edh;
				float egl = slASHRAETauModel( sunZen, dc_tauB, dc_tauD, ebn, edh);
				if (frDiff( ebn, dc_ebnSlrNoon) > .01f
				  || frDiff( edh, dc_edhSlrNoon) > .01f)
					oWarn( "ebn/edh mismatch");
			}
#endif
		}
	}
	return rc;
}	// DESCOND::dc_CheckFixSolar
//-----------------------------------------------------------------------------
void DESCOND::dc_GenerateTemps(
	int iHr,				// hour of day (0 - 23)
							//   0 = 1st hour of day (00h00 - 01h00)
							//   local time consistent with slloccur setup (usually LST)
	float astCor,			// apparent solar time correction, hr
							//    includes longitude, tzn, eqtime, DST
							//    AST = LxT + astCor
	float& taDb,			// returned: dry-bulb temp, F
	float& taWb,			// returned: wet-bulb temp, F
	float& taDbAvg) const	// returned: day average dry-bulb temp, F
{
// HOF 2017 profiles
//   DB/WB DR fraction by hour
//   Profile from 1363-RP, shifted one hour per 1453-RP
static const double DRF[ 24] =
{ .88, .92, .95, .98,  1., .98, .91, .74,
  .55, .38, .23, .13, .05,  0.,  0., .06,
  .14, .24, .39, .50, .59, .68, .75, .82
};
static const double DRFMean = 0.53625;	// mean of DRF[]
										// must recompute if profile changes
#if defined( _DEBUG)
	static double DRFMeanX = -1.;
	if (DRFMeanX < 0.)
	{	DRFMeanX = VSum( DRF, 24) / 24.;
		if (frDiff( DRFMeanX, DRFMean) > .000001)
			printf( "\nDESCOND DRF profile trouble!");
	}
#endif

#if 0
	if (dc_twbDes > dc_tdbDes)
	{	hbo_Err( ERR, _T("DESCONDCOOLING '%s': design WB > design DB"), dcID);
		dc_twbDes = dc_tdbDes;
	}

	double rhoDX, rhoMX;	// unused
	/* ?=*/ Psychro.PropertiesFromTdbTwb( dc_tdbDes, dc_twbDes, dc_wDes, dc_rhDes, dc_hDes,
			dc_tdpDes, rhoDX, rhoMX);

	if (dc_DRwbDes < 0.)
	{	// default WB range assuming constant dewpoint
		double TdbMin = dc_tdbDes - dc_DRdbDes;
		double TwbMin = Psychro.WetBulb( TdbMin, dc_wDes);
		dc_DRwbDes = max( 0., dc_twbDes - TwbMin);
	}
#endif

	// ASHRAE HOF method
	//   DB and WB generated from profile
	//   caller derives DP
	int iHShift = iRound( astCor);
	// if (DST) iHShift = iHShift - 1;  NO -- astCor includes DST if needed
	int iHX = (iHr + iHShift + 24)%24;
	taDb = dc_DB - dc_MCDBR * DRF[ iHX];
	taWb = min( taDb, dc_MCWB - dc_MCWBR * DRF[ iHX]);
	taDbAvg = dc_DB - DRFMean * dc_MCDBR;
}		// DESCOND::dc_GenerateTemps
//=============================================================================


// cgwthr.cpp end
