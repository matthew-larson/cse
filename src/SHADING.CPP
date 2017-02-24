///////////////////////////////////////////////////////////////////////////////
// shading.cpp -- routines re determination of shaded/sunlit fractions of
//                surfaces for given sun position
///////////////////////////////////////////////////////////////////////////////

// Copyright (c) 1997-2016 The CSE Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

// member names changed 1-92 to match user input language names

/* window is rectangular, in vertical surface.
	sun assumed at/above horizon.
	overhang is horizontal rectangle covering full window width or more
	overhang may have "flap" hanging down at outer edge
	there may be one fin on each side of window.
	  top of fins is at/above top of window;
	  bottom of fins is at/above bottom of window (but below top). */

/* generally caller checks data --
   this function does not verify most of its geometric assumptions.
   Negative values are not expected in any of the arguments;
     use 0 depth to omit a fin or overhang. */


/*------------------------------- INCLUDES --------------------------------*/
#include <cnglob.h>

#include <ancrec.h>	// record: base class for rccn.h classes
#include <rccn.h>	// WSHADRAT, SHADEX, 

#include <cnguts.h>	// decl for shadwin()

#include <ashrae/geometry.h>


///////////////////////////////////////////////////////////////////////////////
// overhang/fin shading
///////////////////////////////////////////////////////////////////////////////
// window is rectangular, in vertical surface.
//	sun assumed at/above horizon.
//	overhang is horizontal rectangle covering full window width or more
//	overhang may have "flap" hanging down at outer edge
//	there may be one fin on each side of window.
//	  top of fins is at/above top of window;
//	  bottom of fins is at/above bottom of window (but below top).

// generally caller checks data --
//   this function does not verify most of its geometric assumptions.
//   Negative values are not expected in any of the arguments;
//   use 0 depth to omit a fin or overhang.

#if 0
0 no longer needed with newer compilers, 2-9-17
0 #pragma optimize("",off)	// else get bad code at ver = cosz etc. expression below
0							// More comments at that line. MSC 6.0.
#endif

//==========================================================================
float FC shadwin(			// sunlit fraction of window shaded with fins and overhang
	const WSHADRAT* p,	// struct of dimensions of window/ovhng/fin
	float gamma,		// azimuth of sun less azimuth of window:
						// angle 0 if sun normal to window, positive cw looking down
	float cosz)			// cosine of solar zenith angle   

// returns: fraction of area sunlit, in range 0 to 1.0.
{
	/* comment comments:
	    "near" = toward sun (horizontally), "far" or beyond = away from sun.
	    "ray" = extended slanted shadow line from top of fin or near (sunward)
	            end of overhang.
	    _1 variables are for hor overhang,
	    _2  are for vertical flap at end of overhang,
	    _3  are for fin. */
// flow control
	SI K;			// 0 normal, 1 when doing fin-flap shadow overlap
	SI L;			// 1 if overhang horiz shadow edge is in window, else 0
// angles
	float cosg, sing;	// sin and cos of gamma (angle from window normal to sun, + for sun to left when looking at window)
	float ver, hor; 	// components of shadow on wall of unit projection
// window
	float H, W;		// hite, wid of window (later of flap shadow)
	float WAREA;		// whole window area
// overhang
	float E, U;		// near (to sun) flap horiz extension; dist flap above win
	float HU, WE;		// H+U;  W+E: Width + Extension
	float X1, Y1;		// components hor oh shad slantEdge -- how far over & down
	float AREAO;
	float H2, W2;		// fin flap shadow hite and width
	float AREAV;
// fin
	float FD;  		// fin depth: how far out fin projects
	float FU; 		// "fin up": fin top extension above top win
	float FO; 		// "fin over": how far sunward from window to fin
	float FBU;		// "fin bottom up": how far above bot win fin ends
	float X3;		// x compon fin shadow slantEdge: hor far over shadow goes
	float ARSIF, AREA1;	// shadow area & working copy
// shadow ray intercepts for oh then fin.  NB these relate to "ray"; may be beyond actual shadow corner:
	float NY, FY;		// how far down slant ray is at near, far win edge intercepts
// result
	float fsunlit;

	/* triangle area for specific altitude and global hor and ver components.
	   Divide by 0 protection: most alt's are 0 if ver is 0. */
#define TRIA(alt) ( (alt)==(float)0.f ? (float)0.f : (alt)*(alt)*0.5f*hor/ver )
	// the (float)s really are necessary to prevent data conversion warnings in MSC 5.1. rob 1-24-90.

// init
	K = 0;				// 1 later during fin-flap overlap
	cosg = (float)cos(gamma);		// sin and cos of ...
	sing = (float)sin(gamma);		// ... solar azimuth rel to window
	if (cosg <= 0.f) 			// if sun behind wall
		return 0.f;			// window fully in shadow
	if (cosz >= 1.f)			// sun at zenith: no sun enters win
		return 0.f;			// (rob 1-90 prevents div by 0)

	// OLD: following line compiles incorrectly with default optimization -Oswr;
	//      expression produces reciprocal of correct result.  #pragma opt off added above.
	//      8-5-90 chip.
	ver = cosz/sqrt(1.f-cosz*cosz)/cosg;	// tan(zenith angle) / cos(gamma)

	hor = (float)fabs(sing)/cosg;	// abs(tan(gamma)): unit fin shade
	AREAO = 0.f;  			// hor overhang shadow area
	AREAV = 0.f;  			// ver overhang flap shadow area
	ARSIF = 0.f;  			// fin shadow area
	AREA1 = 0.f;  			// working copies of above
	H = p->wHeight;
	W = p->wWidth;
	WAREA = H * W;			// whole window area
	W2 = H2 = 0.f;			// init width & hite of oh flap shadow
// horiz overhang setup
	E= gamma < 0.f ? p->ohExR : p->ohExL;	// pertinent (near sun) oh extension
	U = p->ohDistUp; 				// distance from top win to overhang
	HU = H + U;					// from overhang to bottom window
	WE = W + E;  				// from near end oh to far side wind
	Y1 = p->ohDepth * ver;			// how far down oh shadow comes
	X1 = p->ohDepth * hor;	    		// hor component oh shad slantedge = hor displ corners oh shadow
	/* NOTE: X1, Y1 are components of actual length (to corner) of shadow slanted edge, based on oh depth.
	         NY, FY relate to slanted ray intercepts with window edges, irrespective of whether beyond actual corner. */
	L = Y1 > U  &&  Y1 < HU  &&  X1 < WE;	// true if near shad bot corn (hence hor edge) is in win
	if (p->ohDepth <= 0.f)			// if no overhang
		goto fin37;				// go do fins.  flap is ignored.

	/*----- horizontal overhang shadow area "AREAO" -----*/

	if (Y1 > U				// if shadow comes below top window
	&& (hor==0.f ||			// /0 protection
	(FY=WE*ver/hor) > U))		// and edge passes below top far win corn
		// (set FY to how far down slant ray hits far side)
	{
		if (Y1 > HU)			// chop shadow at bot win to simplify
			Y1 = HU;			// (adjusting X1 to match doesn't matter in any below-win cases)
		// area: start with window rectangle above shadow bottom
		AREAO = W * (Y1 - U);		// as tho shadow corner sunward of window

		// subtract unshaded portions of rectangle if shad corner is in win
		if (hor != 0.f)			// else done and don't divide by 0
		{
			NY = E * ver/hor;   		// "near Y": distance down from oh to where slant ray hits near (to sun) win edge
			// if corner not sunward of window, subtract unshaded triangle area
			if (NY < Y1)  			// if slant hits nr side abov sh bot
			{
				AREAO -= TRIA(Y1 - NY); 		// triangle sunward of edge
				// add back possible corners thereof hanging out above & beyond win
				if (NY < U)
					// slantedge passes thru top, not near side, of window
					AREAO += TRIA(U - NY);		// add back trian ABOVE win
				FY = WE * ver/hor;  		// "far Y": distance down to where slant ray hits far win edge
				if (FY < Y1)			// if hits abov shad bot
					AREAO += TRIA(Y1 - FY); 	// add back trian BEYOND win
			}
		}
	}
	if (AREAO >= WAREA)			// if fully shaded
		goto exit68;			// go exit

	/*----- flap (vertical projection at end of horiz overhang) area AREAV -----*/

// flapShad width: from farther of oh extension, slant x to end window
	W2 = WE - max( E, X1);
	/* flapShad hite: from: lower of hor oh shad bot, window top.
		          to: hier of hor oh shad bot + flap ht, bottom window. */
	H2 = min( Y1 + p->ohFlap, HU) - max( Y1, U);
// if hite or wid <= 0, no shadow, AREAV stays 0, H2 and W2 don't matter.
	if (H2 > 0.f && W2 > 0.f)
	{
		AREAV = H2 * W2;			// area shaded by flap
		if (AREAO+AREAV >= WAREA)	// if now fully shaded
			goto exit68;			// go exit
	}

	/*----- FINS: select sunward fin and set up -----*/
fin37:			// come here if no overhang
	if (gamma==0.f)  		// if sun straight on, no fin shadows, exit
		goto exit68;		// (and prevent division by hor: is 0)
	if (gamma > 0.f)		    // if sun coming from left
	{
		FD = p->lfDepth;			// fin depth: how far out fin projects
		FU = p->lfTopUp;			// "fin up": fin top extension above top win
		FO = p->lfDistL;			// "fin over": how far sunward from window
		FBU = p->lfBotUp;  		// "fin bottom up": how far above bot win
	}
	else  			    // else right fin shades toward window
	{
		FD = p->rfDepth; 		// fin depth
		FU = p->rfTopUp;			// top extension
		FO = p->rfDistR;			// hor distance from window
		FBU = p->rfBotUp;		// bottom distance above windowsill
	}
	if (FD <= 0.f)			// if fin depth 0
		goto exit68;			// no fin, go exit

	X3 = FD*hor;			// how far over (antisunward) shadow comes

	/*---- FIN... Adjustments to not double-count overlap
			     of fin's shadow with horizontal overhang's shadow ----*/
	if (AREAO > 0.f)			// if have overhang
	{
		float FUO;
		FUO = U + (FO - E)*ver/hor;	/* how far up fin would extend for fin and oh shad slantEdges
       					   to just meet for this sun posn.  Can be < 0. */
		if (FU > FUO)			// if fin shadow overlaps oh shadow
		{
			if (L==0			// if oh shad has no hor edge in win
					|| X3 - FO < X1 - E)  	// or fin shad corn (thus ver edge) nearer than oh shad corner
			{
				/* fin shadow ver edge intersects oh shadow slanted edge
				   (or intersects beyond window): overlap is parallelogram */
				FU = FUO;			// chop off top of fin that shades oh shadow. FU < 0 (large E) appears ok 1-90.
				if (H + FU <= FBU) 	// if top of fin now below bottom
					goto exit68;		// no fin shadow
			}
			else			// vert edge fin shadow intersects hor edge oh shad
			{
				/* count fin shadow area in chopped-off part of window now:
					        A triangle or trapezoid nearer than oh shadow; compute as all of that area not oh shadow. */
				AREA1 = W*(Y1 - U) - AREAO;
				// shorten win to calc fin shad belo hor edge of oh shad only
				H -= Y1 - U;		// reduce window ht to below area oh shades
				FU += Y1 - U;		// keep top of fin same
				//  rob 1-90: might be needed (unproven), else harmless:
				U = Y1;			// keep overhang position same: U may be used below re fin - flap shadow interaction
			}
		}
	}

	/*----- FIN... shadow area on window (K = 0) or overhang flap (K = 1) ----*/

fin73:	// come back here with window params changed to match flap shadow, to compute fin shadow on flap shadow, to deduct.
	/* COMPUTE: AREA1, ARSIF: fin shadow area assuming fin extends to bot win;
	        ARSH1, ARSHF: lit area to deduct due to short fin
	        note these variables already init; add to them here. */
	NY = FO*ver/hor;		// "near Y":  y component slant ray distance from top fin to near side win
	if (X3 > FO			// else shadow all sunward of window
			&& NY < H + FU)		// else top shadow slants below window
	{
		/* chop non-overlapping areas off of shadow and window: Simplifies later code;
		   ok even tho rerun vs flap shadow because latter is within former. */

		// far (from sun) side: chop larger of shadow, window to smaller
		if (X3 > W + FO)
			X3 = W + FO;
		else			// window goes beyond shadow
			W = X3 - FO;
		FY = (W + FO)*ver/hor;		// "far Y":  y component slant ray distance from top fin to far side win
		// HFU = H + FU;	* top fin to bot window distance *  not used
		// subtract any lit rectangle and triang above shad.  nb FU can be < 0.
		if (FY > FU)			// if some of top of win lit
		{
			float FYbelowWIN;
			if (NY <= FU)				// if less than full width lit
				AREA1 -= TRIA(FY-FU);		    // subtract top far triangle
			else					// full wid (both top corners) lit
				AREA1 -=  W * (NY-FU)		    // subtract lit top rectangle
						  +  TRIA(FY-NY);		    // and lit far triangle below it
			FYbelowWIN = FY - (H + FU);
			if (FYbelowWIN > 0.f)			// if far lit triang goes below win
				AREA1 += TRIA(FYbelowWIN);		    // add back area belo win: wanna subtract a trapezoid only
		}
		// subtract any lit rectangle and triangle below fin shadow
		if (NY < FBU)			     // if some of bottom of win lit
			if (FY >= FBU)			// if less than full width lit
				AREA1 -= TRIA(FBU-NY);		    // subtract bot near triangle
			else					// full wid (both bot corners) lit
				AREA1 -=  W * (FBU-FY)		    // subtact lit bottom rectangle
						  +  TRIA(FY-NY);		    // and lit near triang above it
		// add in entire area as tho shaded (LAST for best precision: largest #)
		AREA1 += H * W;
	}

	/*---- FIN... done this shadow; sum and proceed from window to flap ----*/

	if (K)				// if just computed shadow on flap shadow
		AREA1 = -AREA1;			// negate fin shadow on flap shadow
	ARSIF += AREA1;			// combine shadow areas
	if (K==0 && AREAV > 0.f)		// if just did window and have flap shadow
	{
		float HbelowFlapShad;
		/* change window geometry to that of oh flap shadow, and go back thru fin shadow code:
		   Computes fin shadow on flap shadow(!!), which is then deducted (just above) to undo double count. */
		K++;				// K = 1: say doing fin on flap

		AREA1 = 0.f;			// re-init working areas
		// adjust fin-over to be relative to near side flap shadow
		if (X1 > E)			// if near edge flap shadow is in window
			FO += X1 - E; 		// adjust FO to keep relative
		// else near edge of area H2*W2 IS near edge window, no FO change
		HbelowFlapShad = HU - Y1 - p->ohFlap;
		if (HbelowFlapShad > 0.f)	// if bot flap shadow above bot win
		{
			FBU -= HbelowFlapShad;	// adjust fin-bottom-up to keep fin effectively in same place
			if (FBU < 0.f)
				FBU = 0.f;
		}
		if (Y1 > U)			// if flap shadow top below top win
			FU += Y1 - U;			// make fin-up relative to flap shadow
		H = H2;				// replace window ht and wid with
		W = W2;				// ... those of flap shadow
		goto fin73;
	}

exit68:  		// come here to exit
	fsunlit = 1.f - (AREAO + AREAV + ARSIF ) / WAREA;
// return 0 if result so small that is probably just roundoff errors; in particular, for small negative results.  rob 1-90.
	return fsunlit > 0.0000001f ? fsunlit : 0.f;

}		// ::shadwin
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// class SURFGEOM: nestable surface geometry
///////////////////////////////////////////////////////////////////////////////
SURFGEOM::SURFGEOM()
{
	gx_polygon = new CPolygon3D();
}	// SURFGEOM::SURFGEOM
//-----------------------------------------------------------------------------
SURFGEOM::~SURFGEOM()
{
	delete gx_polygon;
}	// SURFGEOM::~SURFGEOM
//-----------------------------------------------------------------------------
void SURFGEOM::gx_Init(			// parent linkage etc
	record* pParent,		// parent record: SHADEX, PVARRAY, etc
	int options /*=0*/)		// options TBD
{
	gx_pParent = pParent;
}		// SURFGEOM::gx_Init
//-----------------------------------------------------------------------------
void SURFGEOM::gx_CopySubObjects()		// copy heap subobjects
// call AFTER bitwise parent Copy()
//   subojects are duplicated
{	if (gx_polygon)
		gx_polygon = new CPolygon3D( *gx_polygon);
}	// SURFGEOM::gx_CopySubObjects
//-----------------------------------------------------------------------------
RC SURFGEOM::gx_CheckAndMakePolygon(
	int phase,
	int fnG)		// SURFGEOM field number within parent
// returns RCOK iff success

{
	// gx_polygon->DeleteAll();  clean out on first call?

	int fnVI = fnG + SURFGEOM_VRTINP;
	int nSet, nVal;
	RC rc = ArrayStatus( &gx_pParent->fStat( fnVI), DIM_POLYGONXYZ, nSet, nVal);

	if (rc)
		gx_pParent->ooer( fnVI, "Internal error: invalid ARRAY");
	else if (nSet == 0)
		return RCOK;

	else if (nSet < 9 || nSet%3 != 0)
		rc = gx_pParent->ooer( fnVI, "Invalid number of XYZ coordinates (%d)."
			   "  Must be a multiple of 3 and >= 9.", nSet);

	if (!rc)
	{	// fill polygon from input verticies
		gx_polygon->DeleteAll();
		gx_polygon->Add( gx_vrtInp, nSet/3);

		// validate polyon
		// compare to tilt / azm?
	}
	
	return rc;

}	// SURFGEOM::gx_CheckAndMakePolygon
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// class SHADEX: geometric shade object
///////////////////////////////////////////////////////////////////////////////
SHADEX::SHADEX( basAnc *b, TI i, SI noZ /*=0*/)
	: record( b, i, noZ)
{
	FixUp();
}	// SHADEX::SHADEX
//----------------------------------------------------------------------------
/*virtual*/ void SHADEX::FixUp()
{
	sx_g.gx_Init( this);
}		// SHADEX::FixUp
//----------------------------------------------------------------------------
void SHADEX::Copy( const record* pSrc, int options/*=0*/)
{	// bitwise copy of record
	record::Copy( pSrc, options);	// calls FixUp()
	// copy SURFGEOM heap subobjects
	sx_g.gx_CopySubObjects();
}	// SHADEX::Copy
//----------------------------------------------------------------------------
RC SHADEX::sx_CkF()
{
	RC rc = RCOK;

	rc = sx_g.gx_CheckAndMakePolygon( 0, SHADEX_G);

	return rc;
}		// SHADEX::xs_CkF
//=============================================================================


// shading.cpp end
