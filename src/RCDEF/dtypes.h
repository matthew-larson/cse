// Copyright (c) 1997-2016 The CSE Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

/* DTYPES.H: data types include file automatically generated by rcdef.exe. */

   /* this file is #included in cnglob.h. */

   /* do not edit this file -- it is written over any time rcdef is run --
      instead change the input, dtypes.def (also records.def),
      and re-run rcdef. */

   /* see also CHOICB.H, for choice data type stuff */

   /* text of these automatically generated comments written 10-88
      -- likely to be obsolete! */



/* Typedefs for data types

   For record fields, and general use.
   Type names are as specified in CNDTYPES.DEF.

   For *choicb data types, the declaration is for a short int to hold a
   "choicb index" to represent the current selection
   (see C_xxx defines in CHOICB.H).

   For *choicn data types, the declaration is for a float to hold a
   numerical value or a NAN representing "unset" or the current selection.
   (see C_xxx defines in CHOICB.H, and cnblog.h NCHOICE and CHN macros).

   (Structures for records (as specified in records.def) are not here,
   but in several RCxxxx.H files whose names are given in records.def). */

typedef int INT;
typedef int* INTP;
typedef short SI;
typedef short BOO;
typedef unsigned short USI;
typedef long LI;
typedef unsigned long ULI;
typedef float FLOAT;
typedef float * FLOATP;
typedef void * VOIDP;
typedef short DOW;
typedef short MONTH;
typedef char CH;
typedef char * CHP;
typedef unsigned char UCH;
typedef SI TI;
typedef char ANAME[64];
typedef struct {SI year;SI month;SI mday;SI wday;} IDATE;
typedef struct {SI hour;SI min;SI sec;} ITIME;
typedef short DOY;
typedef unsigned SHOY;
typedef struct {SI year;SI month;SI mday;SI wday;SI hour;SI min;SI sec;} IDATETIME;
typedef time_t LDATETIME;
typedef double DBL;
typedef double* DBLP;
typedef struct SGTARG * SGTARGP;
typedef struct MASSLAYER * MASSLAYERP;
typedef class MASSMODEL * MASSMODELP;
typedef class IZXRAT * IZXRATP;
typedef struct { void *val; USI ty; } VALNDT;
typedef enum { cmNONE=0,cmSo=1,cmStH=2,cmStC=4,cmStBOTH=6 } TCCM;
typedef enum { So=1,StH=2,StC=4,St=6,Lh=8,LhSo=9,LhStH=10,Ar=16,ArSo=17,ArStH=18,ArStC=20,ArStBOTH=22,nv=32 } ZHXTY;
typedef enum { uNONE=0, uSo=1, uStH=2, uStC=4, /*uSt=6,*/ uMn=8, uMxH=16, uMxC=32/*,uPegged=56*/} TCUSE;
typedef enum { cuNONE, cuHEAT, cuCOOL } COILUSED;
typedef enum { ahOFF=0,ahFAN=1,ahHEATBIT=2,ahHEATING=3,ahCOOLBIT=4,ahCOOLING=5,ahON=7 } AHMODE;
typedef enum { tldMIN, tldSP, tldMAX } TOWLOADCASE;
typedef XFILE * XFILEP;
typedef char WFLOC[32];
typedef char WFLID[18];
typedef struct { CHP fieldName; SI fnmi; CHP fp; BOO nDecoded; FLOAT fnv; } FNRT;
typedef FNRT * FNRTP;
typedef struct { CHP fieldName; SI fnr; } FNMT;
typedef FNMT * FNMTP;
typedef enum WFileFormat { UNK=-1,BSGS=1,BSGSdemo,ET1,ET2,CSW,T24DLL,EPW } WFILEFORMAT;
typedef char WFLOC2[22];
typedef class YACAM * YACAMP;
typedef class WDYEAR * WDYEARP;
typedef enum { nul,znTot,znTotO,znTotIz,znCAir,massI,massO } RIGTARGTY;
typedef float RIGTARG;
typedef struct { RIGTARGTY targTy; TI targTi; float targFr; RIGTARG * targP; } RIGDIST;
typedef RIGDIST * RIGDISTP;
typedef class ZNR * ZNRP;
typedef struct XSURF * XSURFP;
typedef class DUCTSEG* DUCTSEGP;
typedef SI NOYESCH;
typedef SI SKYMODCH;
typedef SI SKYMODLWCH;
typedef SI DOWCH;
typedef SI MONCH;
typedef SI OSTYCH;
typedef SI ZNMODELCH;
typedef SI IZNVTYCH;
typedef SI EXCNDCH;
typedef SI SFMODELCH;
typedef SI GROUNDMODELCH;
typedef SI CONVMODELCH;
typedef SI RSYSMODECTRLCH;
typedef SI RSYSTYCH;
typedef SI RSYSOAVTYCH;
typedef SI WSCALCMODECH;
typedef SI WHHEATSRCCH;
typedef SI WHTYPECH;
typedef SI WHASHPTYCH;
typedef SI DHWEUCH;
typedef SI DHWMODELCH;
typedef SI DHWLSEGTYCH;
typedef SI PVMODCH;
typedef SI PVARRCH;
typedef SI FENMODELCH;
typedef SI SIDECH;
typedef SI FILESTATCH;
typedef SI IVLCH;
typedef SI RPTYCH;
typedef SI RPTHDCH;
typedef SI JUSTCH;
typedef SI ENDUSECH;
typedef SI HDAYCASECH;
typedef SI FANAPPCH;
typedef SI FANTYCH;
typedef SI MOTTYCH;
typedef SI MOTPOSCH;
typedef SI FANPLCH;
typedef SI COILAPPCH;
typedef SI COILTYCH;
typedef SI ECOTYCH;
typedef SI DUCTTYCH;
typedef SI OAMNCH;
typedef SI EXSHDCH;
typedef SI INSHDCH;
typedef SI HUMTHCH;
typedef SI OFFONCH;
typedef SI TPSTGCH;
typedef SI CTTYCH;
typedef SI CCHCM;
typedef SI DIFFSAMECH;
typedef float AHSCHVC;
typedef float OFFAVAILVC;
typedef float TFANSCHVC;
typedef float OFFONVC;
typedef float NOYESVC;
typedef float OFFAVAILONVC;
typedef float VENTAVAILVC;
typedef float TSCMNC;
typedef float RANC;



/* Data type extern declarations     names are  X<typeName> or as in dtypes.def

   These define a single word for an external declaration for the type,
   for non-choice types. */

#define XINT extern INT
#define XINTP extern INTP
#define XSI extern SI
#define XUSI extern USI
#define XLI extern LI
#define XULI extern ULI
#define XF extern FLOAT
#define XCH extern CH



/* Data type defines    names are  DT<typeName>

   Values stored in the field information in a record descriptor (also for
   general use) consisting of Dttab offset plus bits (rcpak.h) as follows:

   DTBCHOICB (0x2000)  Multiple-choice, "improved and simplified" CN style
                       (see srd.h): takes one of several values given
                       in dtypes.def; values are stored as ints 1, 2, 3 ... ;
                       Symbols for values are defined in CHOICB.H.  1-91.

   DTBCHOICN (0x4000)  Number OR Multiple-choice value stored in a float.
                       Choice values are stored in the float as NANs:
                       hi word is 0x7f81, 0x7f82, 0x7f83 ....
                       Symbols for values are defined in CHOICB.H.  2-92.

   LOWORD(Dttab[ dtype & DTBMASK]) contains, for all data types,
   the SIZE in bytes of the data type.

   For a choice types, the number of choices and the
   text pointers of each choice follow per rcpak.h struct CHOICB.

   The position in Dttab comes from the "type handle" specified in dtypes.h;
   changing these invalidates compiled code. */

#define DTNONE 0x0000         // supplied by rcdef (rob 12-1-91)
#define DTUNDEF 0xc000        // supplied by rcdef
#define DTNA 0xc001           // supplied by rcdef
#define DTINT 0x01
#define DTINTP 0x02
#define DTSI 0x03
#define DTBOO 0x04
#define DTUSI 0x05
#define DTLI 0x06
#define DTULI 0x07
#define DTFLOAT 0x08
#define DTFLOATP 0x09
#define DTVOIDP 0x0a
#define DTDOW 0x0b
#define DTMONTH 0x0c
#define DTCH 0x0d
#define DTCHP 0x0e
#define DTUCH 0x0f
#define DTTI 0x10
#define DTANAME 0x11
#define DTIDATE 0x12
#define DTITIME 0x13
#define DTDOY 0x14
#define DTSHOY 0x15
#define DTIDATETIME 0x16
#define DTLDATETIME 0x17
#define DTDBL 0x18
#define DTDBLP 0x19
#define DTSGTARGP 0x1a
#define DTMASSLAYERP 0x1b
#define DTMASSMODELP 0x1c
#define DTIZXRATP 0x1d
#define DTVALNDT 0x1e
#define DTTCCM 0x1f
#define DTZHXTY 0x20
#define DTTCUSE 0x21
#define DTCOILUSED 0x22
#define DTAHMODE 0x23
#define DTTOWLOADCASE 0x24
#define DTXFILEP 0x25
#define DTWFLOC 0x26
#define DTWFLID 0x27
#define DTFNRT 0x28
#define DTFNRTP 0x29
#define DTFNMT 0x2a
#define DTFNMTP 0x2b
#define DTWFILEFORMAT 0x2c
#define DTWFLOC2 0x2d
#define DTYACAMP 0x2e
#define DTWDYEARP 0x2f
#define DTRIGTARGTY 0x30
#define DTRIGTARG 0x31
#define DTRIGDIST 0x32
#define DTRIGDISTP 0x33
#define DTZNRP 0x34
#define DTXSURFP 0x35
#define DTDUCTSEGP 0x36
#define DTNOYESCH 0x2037
#define DTSKYMODCH 0x203a
#define DTSKYMODLWCH 0x203d
#define DTDOWCH 0x2042
#define DTMONCH 0x204a
#define DTOSTYCH 0x2057
#define DTZNMODELCH 0x205e
#define DTIZNVTYCH 0x2062
#define DTEXCNDCH 0x2071
#define DTSFMODELCH 0x2077
#define DTGROUNDMODELCH 0x207e
#define DTCONVMODELCH 0x2081
#define DTRSYSMODECTRLCH 0x2089
#define DTRSYSTYCH 0x208e
#define DTRSYSOAVTYCH 0x2096
#define DTWSCALCMODECH 0x209a
#define DTWHHEATSRCCH 0x209d
#define DTWHTYPECH 0x20a2
#define DTWHASHPTYCH 0x20a7
#define DTDHWEUCH 0x20ba
#define DTDHWMODELCH 0x20c1
#define DTDHWLSEGTYCH 0x20c5
#define DTPVMODCH 0x20c8
#define DTPVARRCH 0x20cd
#define DTFENMODELCH 0x20d3
#define DTSIDECH 0x20d6
#define DTFILESTATCH 0x20d9
#define DTIVLCH 0x20dd
#define DTRPTYCH 0x20e4
#define DTRPTHDCH 0x20f3
#define DTJUSTCH 0x20f7
#define DTENDUSECH 0x20fa
#define DTHDAYCASECH 0x2110
#define DTFANAPPCH 0x2116
#define DTFANTYCH 0x211c
#define DTMOTTYCH 0x2125
#define DTMOTPOSCH 0x2128
#define DTFANPLCH 0x212c
#define DTCOILAPPCH 0x2134
#define DTCOILTYCH 0x2138
#define DTECOTYCH 0x2141
#define DTDUCTTYCH 0x2146
#define DTOAMNCH 0x2149
#define DTEXSHDCH 0x214c
#define DTINSHDCH 0x214f
#define DTHUMTHCH 0x2152
#define DTOFFONCH 0x2155
#define DTTPSTGCH 0x2158
#define DTCTTYCH 0x215b
#define DTCCHCM 0x215f
#define DTDIFFSAMECH 0x2166
#define DTAHSCHVC 0x4169
#define DTOFFAVAILVC 0x416c
#define DTTFANSCHVC 0x416f
#define DTOFFONVC 0x4174
#define DTNOYESVC 0x4177
#define DTOFFAVAILONVC 0x417a
#define DTVENTAVAILVC 0x417e
#define DTTSCMNC 0x4182
#define DTRANC 0x4188



/* Data type MAX defines         names are  DT<typeName>MAX

   Maximum absolute value for some types, for general use, optionally
   specifyable in dtypes.def. */

#define DTSIMAX 32767
#define DTUSIMAX 65535
#define DTANAMEMAX 63


/* Choice data type CHOICE INDEX defines

   These are values stored in fields and variables of choice
   (choicb, see srd.h) data types; the C_ defines are used in code to set
   and test choice data.

   The value is the sequence number of the choice in dtypes.def.  If changed,
   grep and recompile all pertinent code.

   The value is used in accessing the choice's text pointer at
	Dttab + (dt & DTBMASK) + masked value

   The names in these defines are  C_<typeName>_<choiceName> */

#define C_NOYESCH_NO 1
#define C_NOYESCH_YES 2
#define C_SKYMODCH_ISO 1
#define C_SKYMODCH_ANISO 2
#define C_SKYMODLWCH_DEFAULT 1
#define C_SKYMODLWCH_BERDAHLMARTIN 2
#define C_SKYMODLWCH_DRYBULB 3
#define C_SKYMODLWCH_BLAST 4
#define C_DOWCH_SUN 1
#define C_DOWCH_MON 2
#define C_DOWCH_TUE 3
#define C_DOWCH_WED 4
#define C_DOWCH_THU 5
#define C_DOWCH_FRI 6
#define C_DOWCH_SAT 7
#define C_MONCH_JAN 1
#define C_MONCH_FEB 2
#define C_MONCH_MAR 3
#define C_MONCH_APR 4
#define C_MONCH_MAY 5
#define C_MONCH_JUN 6
#define C_MONCH_JUL 7
#define C_MONCH_AUG 8
#define C_MONCH_SEP 9
#define C_MONCH_OCT 10
#define C_MONCH_NOV 11
#define C_MONCH_DEC 12
#define C_OSTYCH_WALL 1
#define C_OSTYCH_CEIL 2
#define C_OSTYCH_FLR 3
#define C_OSTYCH_DOOR 4
#define C_OSTYCH_PERIM 5
#define C_OSTYCH_GLZ 6
#define C_ZNMODELCH_CNE 1
#define C_ZNMODELCH_CZM 2
#define C_ZNMODELCH_UZM 3
#define C_IZNVTYCH_NONE 1
#define C_IZNVTYCH_ONEWAY 2
#define C_IZNVTYCH_TWOWAY 3
#define C_IZNVTYCH_ANEXT 4
#define C_IZNVTYCH_ANIZ 5
#define C_IZNVTYCH_ANHORIZ 6
#define C_IZNVTYCH_ANEXTFAN 7
#define C_IZNVTYCH_ANIZFAN 8
#define C_IZNVTYCH_ANEXTFLOW 9
#define C_IZNVTYCH_ANIZFLOW 10
#define C_IZNVTYCH_ANHERV 11
#define C_IZNVTYCH_ANDUCTLK 12
#define C_IZNVTYCH_ANSYSAIR 13
#define C_IZNVTYCH_ANOAVRLF 14
#define C_EXCNDCH_ADIABATIC 1
#define C_EXCNDCH_AMBIENT 2
#define C_EXCNDCH_SPECT 3
#define C_EXCNDCH_ADJZN 4
#define C_EXCNDCH_GROUND 5
#define C_SFMODELCH_QUICK 1
#define C_SFMODELCH_DELAYED 2
#define C_SFMODELCH_AUTO 3
#define C_SFMODELCH_DELAYED_HOUR 4
#define C_SFMODELCH_DELAYED_SUBHOUR 5
#define C_SFMODELCH_FD 6
#define C_GROUNDMODELCH_D2INP 1
#define C_GROUNDMODELCH_D2COR 2
#define C_CONVMODELCH_INPUT 1
#define C_CONVMODELCH_ASHRAE 2
#define C_CONVMODELCH_AKBARI 3
#define C_CONVMODELCH_WINKELMANN 4
#define C_CONVMODELCH_WALTON 5
#define C_CONVMODELCH_MILLS 6
#define C_CONVMODELCH_UNIFIED 7
#define C_RSYSMODECTRLCH_OFF 1
#define C_RSYSMODECTRLCH_HEAT 2
#define C_RSYSMODECTRLCH_COOL 3
#define C_RSYSMODECTRLCH_AUTO 4
#define C_RSYSTYCH_ACFURN 1
#define C_RSYSTYCH_ACRES 2
#define C_RSYSTYCH_ASHP 3
#define C_RSYSTYCH_ASHPHYD 4
#define C_RSYSTYCH_AC 5
#define C_RSYSTYCH_FURN 6
#define C_RSYSTYCH_RES 7
#define C_RSYSOAVTYCH_NONE 1
#define C_RSYSOAVTYCH_FIXEDFLOW 2
#define C_RSYSOAVTYCH_VARFLOW 3
#define C_WSCALCMODECH_PRERUN 1
#define C_WSCALCMODECH_SIM 2
#define C_WHHEATSRCCH_ELRES 1
#define C_WHHEATSRCCH_FUEL 2
#define C_WHHEATSRCCH_ASHP 3
#define C_WHHEATSRCCH_ASHPX 4
#define C_WHTYPECH_STRGSML 1
#define C_WHTYPECH_STRGLRG 2
#define C_WHTYPECH_INSTSML 3
#define C_WHTYPECH_INSTLRG 4
#define C_WHASHPTYCH_RESTANKNOUA 1
#define C_WHASHPTYCH_RESTANK 2
#define C_WHASHPTYCH_BASICINT 3
#define C_WHASHPTYCH_AOSMITHPHPT60 4
#define C_WHASHPTYCH_AOSMITHPHPT80 5
#define C_WHASHPTYCH_AOSMITHHPTU50 6
#define C_WHASHPTYCH_AOSMITHHPTU66 7
#define C_WHASHPTYCH_AOSMITHHPTU80 8
#define C_WHASHPTYCH_SANDEN40 9
#define C_WHASHPTYCH_SANDEN80 10
#define C_WHASHPTYCH_GE2012 11
#define C_WHASHPTYCH_GE2014STDMODE 12
#define C_WHASHPTYCH_GE2014 13
#define C_WHASHPTYCH_RHEEMHB50 14
#define C_WHASHPTYCH_STIEBEL220E 15
#define C_WHASHPTYCH_GENERIC1 16
#define C_WHASHPTYCH_GENERIC2 17
#define C_WHASHPTYCH_GENERIC3 18
#define C_DHWEUCH_FAUCET 1
#define C_DHWEUCH_SHOWER 2
#define C_DHWEUCH_BATH 3
#define C_DHWEUCH_CLOTHESWASHER 4
#define C_DHWEUCH_DISHWASHER 5
#define C_DHWEUCH_COUNT 6
#define C_DHWMODELCH_T24DHW 1
#define C_DHWMODELCH_2013 2
#define C_DHWMODELCH_2016 3
#define C_DHWLSEGTYCH_RET 1
#define C_DHWLSEGTYCH_SUP 2
#define C_PVMODCH_STD 1
#define C_PVMODCH_PRM 2
#define C_PVMODCH_THF 3
#define C_PVMODCH_CST 4
#define C_PVARRCH_FXDOR 1
#define C_PVARRCH_FXDRF 2
#define C_PVARRCH_1AXT 3
#define C_PVARRCH_1AXBT 4
#define C_PVARRCH_2AXT 5
#define C_FENMODELCH_SHGC 1
#define C_FENMODELCH_ASHWAT 2
#define C_SIDECH_INTERIOR 1
#define C_SIDECH_EXTERIOR 2
#define C_FILESTATCH_OVERWRITE 1
#define C_FILESTATCH_NEW 2
#define C_FILESTATCH_APPEND 3
#define C_IVLCH_Y 1
#define C_IVLCH_M 2
#define C_IVLCH_D 3
#define C_IVLCH_H 4
#define C_IVLCH_HS 5
#define C_IVLCH_S 6
#define C_RPTYCH_ERR 1
#define C_RPTYCH_LOG 2
#define C_RPTYCH_INP 3
#define C_RPTYCH_SUM 4
#define C_RPTYCH_ZDD 5
#define C_RPTYCH_ZST 6
#define C_RPTYCH_ZEB 7
#define C_RPTYCH_MTR 8
#define C_RPTYCH_UDT 9
#define C_RPTYCH_AH 10
#define C_RPTYCH_AHSIZE 11
#define C_RPTYCH_AHLOAD 12
#define C_RPTYCH_TUSIZE 13
#define C_RPTYCH_TULOAD 14
#define C_RPTHDCH_NO 1
#define C_RPTHDCH_YES 2
#define C_RPTHDCH_COL 3
#define C_JUSTCH_L 1
#define C_JUSTCH_R 2
#define C_ENDUSECH_CLG 1
#define C_ENDUSECH_HTG 2
#define C_ENDUSECH_HPHTG 3
#define C_ENDUSECH_DHW 4
#define C_ENDUSECH_FANC 5
#define C_ENDUSECH_FANH 6
#define C_ENDUSECH_FANV 7
#define C_ENDUSECH_FAN 8
#define C_ENDUSECH_AUX 9
#define C_ENDUSECH_PROC 10
#define C_ENDUSECH_LIT 11
#define C_ENDUSECH_RCP 12
#define C_ENDUSECH_EXT 13
#define C_ENDUSECH_REFR 14
#define C_ENDUSECH_DISH 15
#define C_ENDUSECH_DRY 16
#define C_ENDUSECH_WASH 17
#define C_ENDUSECH_COOK 18
#define C_ENDUSECH_USR1 19
#define C_ENDUSECH_USR2 20
#define C_ENDUSECH_PV 21
#define C_HDAYCASECH_FIRST 1
#define C_HDAYCASECH_SECOND 2
#define C_HDAYCASECH_THIRD 3
#define C_HDAYCASECH_FOURTH 4
#define C_HDAYCASECH_LAST 5
#define C_FANAPPCH_SFAN 1
#define C_FANAPPCH_RFAN 2
#define C_FANAPPCH_TFAN 3
#define C_FANAPPCH_XFAN 4
#define C_FANAPPCH_ZFAN 5
#define C_FANTYCH_NONE 1
#define C_FANTYCH_DRAWTHRU 2
#define C_FANTYCH_BLOWTHRU 3
#define C_FANTYCH_RETURN 4
#define C_FANTYCH_RELIEF 5
#define C_FANTYCH_SERIES 6
#define C_FANTYCH_PARALLEL 7
#define C_FANTYCH_EXHAUST 8
#define C_MOTTYCH_PSC 1
#define C_MOTTYCH_BPM 2
#define C_MOTPOSCH_INFLO 1
#define C_MOTPOSCH_INRETURN 2
#define C_MOTPOSCH_EXTERNAL 3
#define C_FANPLCH_VARSPEED 1
#define C_FANPLCH_CYCLE 2
#define C_FANPLCH_AF_VANES 3
#define C_FANPLCH_AF_DAMPERS 4
#define C_FANPLCH_FC_VANES 5
#define C_FANPLCH_FC_DAMPERS 6
#define C_FANPLCH_VARPITCH_AXIAL 7
#define C_COILAPPCH_AHCC 1
#define C_COILAPPCH_AHHC 2
#define C_COILAPPCH_TUHC 3
#define C_COILTYCH_NONE 1
#define C_COILTYCH_ELEC 2
#define C_COILTYCH_DX 3
#define C_COILTYCH_CHW 4
#define C_COILTYCH_HW 5
#define C_COILTYCH_GAS 6
#define C_COILTYCH_OIL 7
#define C_COILTYCH_AHP 8
#define C_ECOTYCH_NONE 1
#define C_ECOTYCH_NONINTEGRATED 2
#define C_ECOTYCH_TWO_STAGE 3
#define C_ECOTYCH_INTEGRATED 4
#define C_DUCTTYCH_RET 1
#define C_DUCTTYCH_SUP 2
#define C_OAMNCH_VOL 1
#define C_OAMNCH_FRAC 2
#define C_EXSHDCH_NONE 1
#define C_EXSHDCH_INSCRN 2
#define C_INSHDCH_NONE 1
#define C_INSHDCH_DRAPEMED 2
#define C_HUMTHCH_ROB 1
#define C_HUMTHCH_PHIL 2
#define C_OFFONCH_OFF 1
#define C_OFFONCH_ON 2
#define C_TPSTGCH_TOGETHER 1
#define C_TPSTGCH_LEAD 2
#define C_CTTYCH_ONESPEED 1
#define C_CTTYCH_TWOSPEED 2
#define C_CTTYCH_VARIABLE 3
#define C_CCHCM_NONE 1
#define C_CCHCM_CONSTANT 2
#define C_CCHCM_CONSTANT_CLO 3
#define C_CCHCM_PTC 4
#define C_CCHCM_PTC_CLO 5
#define C_CCHCM_TSTAT 6
#define C_DIFFSAMECH_DIFF 1
#define C_DIFFSAMECH_SAME 2


/* Number-Choice data type CHOICE VALUES

   These are used with "choicn" data types that can hold a (float) number
   or a choice.  They differ from choicb indeces in that hi bits have
   been added so as to form a NAN when stored in the hi word of a float.
   See cnglob.h for a macro (CHN(n)) to fetch the appropriate part of
   a float and cast it for comparison to these constants.

   The names in these defines are  C_<typeName>_<choiceName> */

#define C_AHSCHVC_OFF 0x7f81
#define C_AHSCHVC_ON 0x7f82
#define C_OFFAVAILVC_OFF 0x7f81
#define C_OFFAVAILVC_AVAIL 0x7f82
#define C_TFANSCHVC_OFF 0x7f81
#define C_TFANSCHVC_ON 0x7f82
#define C_TFANSCHVC_HEATING 0x7f83
#define C_TFANSCHVC_VAV 0x7f84
#define C_OFFONVC_OFF 0x7f81
#define C_OFFONVC_ON 0x7f82
#define C_NOYESVC_NO 0x7f81
#define C_NOYESVC_YES 0x7f82
#define C_OFFAVAILONVC_OFF 0x7f81
#define C_OFFAVAILONVC_AVAIL 0x7f82
#define C_OFFAVAILONVC_ON 0x7f83
#define C_VENTAVAILVC_NONE 0x7f81
#define C_VENTAVAILVC_WHOLEBLDG 0x7f82
#define C_VENTAVAILVC_RSYSOAV 0x7f83
#define C_TSCMNC_RA 0x7f81
#define C_TSCMNC_WZ 0x7f82
#define C_TSCMNC_CZ 0x7f83
#define C_TSCMNC_ZN 0x7f84
#define C_TSCMNC_ZN2 0x7f85
#define C_RANC_RA 0x7f81

// end of choice definitions


/* units definitions automatically generated by rcdef.exe. */


/* Unit systems */
#define UNSYSIP 0
#define UNSYSSI 1


/* Unit types					  (comments written 10-4-88)

   These are for .untype for fields (srd.h) as well as general use.
   They are subscripts into Untab (untab.cpp); also generated by rcdef.
   Untab is of type UNIT (srd.h) and contains a text and a conversion
   factor for each of the 2 unit systems. */

#define UNNONE 0
#define UNTEMP 1
#define UNABSTEMP 2
#define UNTDIFF 3
#define UNENERGY 4
#define UNENERGY1 5
#define UNENERGY2 6
#define UNENERGYDEN 7
#define UNELPWR1 8
#define UNELPWR2 9
#define UNPOWER 10
#define UNPOWER1 11
#define UNPOWER2 12
#define UNPOWER3 13
#define UNPOWER4 14
#define UNPOWER5 15
#define UNBHP 16
#define UNLENGTH 17
#define UNLENGTH2 18
#define UNLENGTH3 19
#define UNAREA 20
#define UNAREA2 21
#define UNVOLUME 22
#define UNANGLE 23
#define UNPOWERDEN 24
#define UNWATTDEN 25
#define UNPOWERDEN2 26
#define UNDENPOWER 27
#define UNELECDEN 28
#define UNANNUSE 29
#define UNDENSITY 30
#define UNDENSITY2 31
#define UNSPECHEAT 32
#define UNHEATCAP 33
#define UNHEATCAP_AREA 34
#define UNHEATCAP_VOL 35
#define UNENTH 36
#define UNUVAL 37
#define UNCNDVY 38
#define UNUA 39
#define UNRVAL 40
#define UNRESVY 41
#define UNF2 42
#define UNAIRCHANGE 43
#define UNELA 44
#define UNAIRFLOW 45
#define UNMFLOW 46
#define UNPRESAIR 47
#define UNPRESH2O 48
#define UNH2OFLOW 49
#define UNVELOCITY 50
#define UNHOURS 51
#define UNLATITUDE 52
#define UNLONGITUDE 53
#define UNPITCH 54
#define UNWINDVEL 55
#define UNLVOLUME 56
#define UNTON 57
#define UNCFMXPOW 58
#define UNGPMXPOW 59
#define UNCFMPSF 60
#define UNGPMPSF 61
#define UNCOST 62
#define UNCOSTF 63
#define UNCOSTSF 64
#define UNWXFLOW 65
#define UNEER 66
#define UNTIME 67
#define UNTIME2 68
#define UNTIME3 69

// end of units definitions


// Limit definitions
//   These specify the type of limit checking, e.g. for cvpak.cpp functions.
//   There is no limits table.

#define LMNONE 0
#define LMFR 1
#define LMFGZ 2
#define LMGEZ 3
#define LMGZ 4
#define LMDAYHR 5
#define LMDOY 6
#define LMLEZ 7
#define LMLZ 8
#define LMNZ 9
#define LMGE1 10
#define LMG1 11

// end of limits


/* RECORD TYPE DEFINES

   COMMENTS OBSOLETE FOR CSE: rcdef revisions required.

   The record type allows access to the record descriptor in Rcdtab.
   The rctype is stored in the first word of every record and is extensively
   used by routines in rcpak.cpp and via macros defined in rcpak.h.

   Each name consists of "RT" and the <typeName> given in records.def.

   Each record type value consists of an Rcdtab offset plus hi bits (rcpak.h):
      (no bits left in CN, 21-1-91)

   The low order bits (mask RCTMASK) are an offset into the pointer table
   at the start of Rcdtab (cpdbinit.cpp); the accessed pointer points to an
   RCD (rcpak.h) containing record and field information.

   The offset is determined by the record handle specified in
   records.def; these should not be changed as existing saved files
   (as well as compiled code) would be invalidated. */

#define RTNONE	0000
#define RTTOPRAT	0x2001
#define RTPYLINEAR	0x8002
#define RTPYCUBIC	0x8003
#define RTPYCUBIC2	0x8004
#define RTPYBIQUAD	0x8005
#define RTPY4	0x8006
#define RTWFILE	0x2007
#define RTWDHR	0x8008
#define RTWFDATA	0x2009
#define RTSGDIST	0x800a
#define RTHCI	0x800b
#define RTSGTARG	0x800c
#define RTSBCBASE	0xc00d
#define RTSBC	0x800e
#define RTXSURF	0x800f
#define RTPRI	0x2010
#define RTSFI	0x2011
#define RTXSRAT	0x2012
#define RTWSHADRAT	0x2013
#define RTMASSBC	0x8014
#define RTMSRAT	0x2015
#define RTSGI	0x2016
#define RTSGRAT	0x6017
#define RTGT	0x2018
#define RTCON	0x2019
#define RTLR	0x201a
#define RTMAT	0x201b
#define RTRFI	0x201c
#define RTRI	0x201d
#define RTCOL	0x201e
#define RTDVRI	0x601f
#define RTIMPF	0x2020
#define RTIFFNM	0x2021
#define RTAUSZ	0x8022
#define RTCONVERGER	0x8023
#define RTAIRSTATE	0x8024
#define RTAIRFLOW	0x8025
#define RTFAN	0x8026
#define RTZNISUB	0x8027
#define RTZNI	0x2028
#define RTZNR	0x2029
#define RTZNRES_IVL_SUB	0x802a
#define RTZNRES_SUB	0x802b
#define RTZNRES	0x202c
#define RTANDAT	0x802d
#define RTIZXRAT	0x202e
#define RTAIRNET	0xc02f
#define RTDBC	0x8030
#define RTDUCTSEG	0x2031
#define RTRSYS	0x2032
#define RTRSYSRES_IVL_SUB	0x8033
#define RTRSYSRES	0x2034
#define RTDHWMTR_IVL_SUB	0x8035
#define RTMTR	0x2036
#define RTDHWSYS	0x2037
#define RTDHWHEATER	0x2038
#define RTDHWTANK	0x2039
#define RTDHWPUMP	0x203a
#define RTDHWLOOP	0x203b
#define RTPIPESEG	0x203c
#define RTDHWLOOPSEG	0x203d
#define RTDHWLOOPBRANCH	0x203e
#define RTDHWLOOPPUMP	0x203f
#define RTDHWDAYUSE	0x2040
#define RTDHWUSE	0x2041
#define RTPVARRAY	0x2042
#define RTGAIN	0x2043
#define RTMTR_IVL_SUB	0x8044

// end of record type definitions



/* Record class and substruct typedefs

   These are here to put the typedefs in all compilations (dtypes.h is
   included in cnglob.h) so function prototypes and external variable
   declarations using the types will always compile.

   (Note: this section of dtypes.h is generated from information in
   records.def, not dtypes.def.) */

class TOPRAT;
struct PYLINEAR;
struct PYCUBIC;
struct PYCUBIC2;
struct PYBIQUAD;
struct PY4;
class WFILE;
struct WDHR;
class WFDATA;
struct SGDIST;
struct HCI;
struct SGTARG;
struct SBCBASE;
struct SBC;
struct XSURF;
class PRI;
class SFI;
class XSRAT;
class WSHADRAT;
struct MASSBC;
class MSRAT;
class SGI;
class SGRAT;
class GT;
class CON;
class LR;
class MAT;
class RFI;
class RI;
class COL;
class DVRI;
class IMPF;
class IFFNM;
struct AUSZ;
struct CONVERGER;
struct AIRSTATE;
struct AIRFLOW;
struct FAN;
struct ZNISUB;
class ZNI;
class ZNR;
struct ZNRES_IVL_SUB;
struct ZNRES_SUB;
class ZNRES;
struct ANDAT;
class IZXRAT;
struct AIRNET;
struct DBC;
class DUCTSEG;
class RSYS;
struct RSYSRES_IVL_SUB;
class RSYSRES;
struct DHWMTR_IVL_SUB;
class MTR;
class DHWSYS;
class DHWHEATER;
class DHWTANK;
class DHWPUMP;
class DHWLOOP;
class PIPESEG;
class DHWLOOPSEG;
class DHWLOOPBRANCH;
class DHWLOOPPUMP;
class DHWDAYUSE;
class DHWUSE;
class PVARRAY;
class GAIN;
struct MTR_IVL_SUB;

// end of dtypes.h