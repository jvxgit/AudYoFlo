/*
  ===========================================================================
   File: TYPEDEF.H                                       v.2.0 - 15.Nov.2004
  ===========================================================================

            ITU-T STL  BASIC OPERATORS

            TYPE DEFINITION PROTOTYPES

   History:
   26.Jan.00   v1.0     Incorporated to the STL from updated G.723.1/G.729 
                        basic operator library (based on basic_op.h)

   03 Nov 04   v2.0     Incorporation of new 32-bit / 40-bit / control
                        operators for the ITU-T Standard Tool Library as 
                        described in Geneva, 20-30 January 2004 WP 3/16 Q10/16
                        TD 11 document and subsequent discussions on the
                        wp3audio@yahoogroups.com email reflector.

  ============================================================================
*/


/*_____________________
 |                     |
 | Basic types.        |
 |_____________________|
*/


#ifndef _TYPEDEF_H
#define _TYPEDEF_H "$Id $"


typedef signed char Word8;
typedef rtpInt16 Word16;
typedef rtpInt32 Word32;
typedef rtpInt64 Word40;
typedef rtpUInt16 UWord16;
typedef rtpUInt32 UWord32;
typedef int Flag;


#endif /* ifndef _TYPEDEF_H */


/* end of file */
