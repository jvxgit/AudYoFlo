;******************************************************************************
;* TMS320C6x C/C++ Codegen                                          PC v7.4.2 *
;* Date/Time created: Mon May 16 23:56:16 2016                                *
;******************************************************************************
	.compiler_opts --abi=eabi --c64p_l1d_workaround=off --endian=little --hll_source=on --long_precision_bits=32 --mem_model:code=near --mem_model:const=data --mem_model:data=far --object_format=elf --silicon_version=6740 --symdebug:none 

;******************************************************************************
;* GLOBAL FILE PARAMETERS                                                     *
;*                                                                            *
;*   Architecture      : TMS320C674x                                          *
;*   Optimization      : Enabled at level 3                                   *
;*   Optimizing for    : Speed                                                *
;*                       Based on options: -o3, no -ms                        *
;*   Endian            : Little                                               *
;*   Interrupt Thrshld : Disabled                                             *
;*   Data Access Model : Far                                                  *
;*   Pipelining        : Enabled                                              *
;*   Speculate Loads   : Enabled with threshold = 0                           *
;*   Memory Aliases    : Presume are aliases (pessimistic)                    *
;*   Debug Info        : No Debug Info                                        *
;*                                                                            *
;******************************************************************************

	.asg	A15, FP
	.asg	B14, DP
	.asg	B15, SP
	.global	$bss

	.global	a
	.sect	".fardata:a", RW
	.clink
	.align	8
	.elfsym	a,SYM_SIZE(80)
a:
	.word	0d4b92a59h,040319e52h		; a[0] @ 0
	.word	0cb85a4f0h,04046892ah		; a[1] @ 64
	.word	0e5c5c217h,0c0537053h		; a[2] @ 128
	.word	058d2ffedh,04043f9b1h		; a[3] @ 192
	.word	07eefca28h,0c05885a6h		; a[4] @ 256
	.word	0766333ach,0c04324bbh		; a[5] @ 320
	.word	044e0daa1h,0c052df74h		; a[6] @ 384
	.word	0482d963bh,0c04d34e9h		; a[7] @ 448
	.word	057fec497h,0404dc9c2h		; a[8] @ 512
	.word	0d976287bh,0c0458554h		; a[9] @ 576

;	C:\MATHLIB_Tools\CCSV5_5_0\ccsv5\tools\compiler\c6000_7.4.2\bin\opt6x.exe C:\\Users\\gtbldadm\\AppData\\Local\\Temp\\016404 C:\\Users\\gtbldadm\\AppData\\Local\\Temp\\016406 
	.sect	".text"
	.clink
	.global	main

;******************************************************************************
;* FUNCTION NAME: main                                                        *
;*                                                                            *
;*   Regs Modified     : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,   *
;*                           A15,B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,   *
;*                           B13,SP,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,  *
;*                           A26,A27,A28,A29,A30,A31,B16,B17,B18,B19,B20,B21, *
;*                           B22,B23,B24,B25,B26,B27,B28,B29,B30,B31          *
;*   Regs Used         : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,   *
;*                           A15,B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,   *
;*                           B13,DP,SP,A16,A17,A18,A19,A20,A21,A22,A23,A24,   *
;*                           A25,A26,A27,A28,A29,A30,A31,B16,B17,B18,B19,B20, *
;*                           B21,B22,B23,B24,B25,B26,B27,B28,B29,B30,B31      *
;*   Local Frame Size  : 8 Args + 156 Auto + 56 Save = 220 byte               *
;******************************************************************************
main:
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 14
           STW     .D2T1   A11,*SP--(8)      ; |92| 
           STW     .D2T1   A10,*SP--(8)      ; |92| 
           STDW    .D2T2   B13:B12,*SP--     ; |92| 
           STDW    .D2T2   B11:B10,*SP--     ; |92| 
           STDW    .D2T1   A15:A14,*SP--     ; |92| 
           STDW    .D2T1   A13:A12,*SP--     ; |92| 
           STW     .D2T2   B3,*SP--(8)       ; |92| 

           MVKL    .S1     $C$SL1+0,A4
||         ADDK    .S2     -168,SP           ; |92| 

           CALLP   .S2     driver_init,B3
||         MVKH    .S1     $C$SL1+0,A4

$C$RL0:    ; CALL OCCURS {driver_init} {0}   ; |100| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 7

           MVKL    .S1     a,A11
||         MVKL    .S2     output,B4

           MVKH    .S1     a,A11
||         MVKH    .S2     output,B4

           MV      .L1     A11,A13
||         CALL    .S1     sin               ; |109| 
||         LDW     .D2T1   *B4,A12

           LDDW    .D1T1   *A13++,A5:A4      ; |109| 
           MVK     .L2     0xa,B4
           NOP             1
           MV      .L1X    B4,A10
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L1:    
;          EXCLUSIVE CPU CYCLES: 1
           ADDKPC  .S2     $C$RL1,B3,0       ; |109| 
$C$RL1:    ; CALL OCCURS {sin} {0}           ; |109| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 7

           SUB     .L1     A10,1,A0          ; |108| 
||         STDW    .D1T1   A5:A4,*A12++      ; |109| 
||         SUB     .S1     A10,1,A10         ; |108| 

   [ A0]   B       .S1     $C$L1             ; |108| 
|| [!A0]   MVKL    .S2     output,B4
|| [ A0]   LDDW    .D1T1   *A13++,A5:A4      ; |109| 
|| [!A0]   MV      .L1     A11,A14
|| [!A0]   ZERO    .L2     B10

   [ A0]   CALL    .S1     sin               ; |109| 
|| [!A0]   MVKH    .S2     output,B4

   [!A0]   ADD     .L2     4,B4,B5
   [!A0]   MVK     .L2     0xa,B4
   [!A0]   STW     .D2T2   B5,*+SP(20)
   [!A0]   MV      .L1X    B4,A10
           ; BRANCHCC OCCURS {$C$L1}         ; |108| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 1
           CALL    .S1     sindp             ; |112| 
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L2:    
;          EXCLUSIVE CPU CYCLES: 5
           LDDW    .D1T1   *A11++,A5:A4      ; |112| 
           ADDKPC  .S2     $C$RL2,B3,3       ; |112| 
$C$RL2:    ; CALL OCCURS {sindp} {0}         ; |112| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 12
           LDW     .D2T2   *+SP(20),B4       ; |112| 
           SUB     .L1     A10,1,A0          ; |111| 
           SUB     .L1     A10,1,A10         ; |111| 
           NOP             2
           LDW     .D2T2   *B4,B4            ; |112| 
   [ A0]   BNOP    .S1     $C$L2,3           ; |111| 

           ADD     .L2     B10,B4,B4         ; |112| 
||         ADD     .S2     8,B10,B10         ; |111| 

   [ A0]   CALL    .S1     sindp             ; |112| 
||         STDW    .D2T1   A5:A4,*B4         ; |112| 

           ; BRANCHCC OCCURS {$C$L2}         ; |111| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 8
           LDW     .D2T2   *+SP(20),B4
           ZERO    .L2     B10
           MV      .L1     A14,A10
           NOP             2

           ADD     .L2     4,B4,B5
||         MVK     .S2     0xa,B4

           STW     .D2T2   B5,*+SP(24)

           MV      .L1X    B4,A11
||         CALL    .S1     sindp_c           ; |115| 

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L3:    
;          EXCLUSIVE CPU CYCLES: 5
           LDDW    .D1T1   *A10++,A5:A4      ; |115| 
           ADDKPC  .S2     $C$RL3,B3,3       ; |115| 
$C$RL3:    ; CALL OCCURS {sindp_c} {0}       ; |115| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 12

           LDW     .D2T2   *+SP(24),B4       ; |115| 
||         SUB     .L1     A11,1,A0          ; |114| 
||         SUB     .S1     A11,1,A11         ; |114| 

   [!A0]   LDW     .D2T1   *+SP(24),A23      ; |85| 
|| [!A0]   MVKL    .S1     0xbfc55555,A3
|| [!A0]   MVKL    .S2     0x686ad430,B27
|| [!A0]   ZERO    .L1     A24

   [!A0]   MVKH    .S1     0xbfc55555,A3
|| [!A0]   MVKL    .S2     0x3de6123c,B28

   [!A0]   MVKL    .S2     0xa524f063,B29
|| [!A0]   MVKL    .S1     0x6dc9c883,A25

   [!A0]   MVKL    .S2     0x3ec71de3,B30
|| [!A0]   MVKL    .S1     0x3fd45f30,A26

           LDW     .D2T2   *B4,B4            ; |115| 
|| [!A0]   MVKL    .S2     0x111110b0,B31
|| [!A0]   MVKL    .S1     0x4b5dc0ab,A27

   [!A0]   MVKL    .S1     0xbe5ae645,A28
|| [ A0]   B       .S2     $C$L3             ; |114| 

   [!A0]   MVKL    .S1     0x1a013e1a,A29
   [!A0]   MVKL    .S1     0xbf2a01a0,A30
   [!A0]   MVKL    .S1     0x55555555,A31

           ADD     .L2     B10,B4,B4         ; |115| 
||         ADD     .S2     8,B10,B10         ; |114| 

           STDW    .D2T1   A5:A4,*B4         ; |115| 
|| [!A0]   MVKL    .S2     0x3f811111,B4
|| [ A0]   CALL    .S1     sindp_c           ; |115| 

           ; BRANCHCC OCCURS {$C$L3}         ; |114| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 18

           STW     .D2T1   A3,*+SP(44)       ; |118| 
||         MVKL    .S1     0xbd6ae420,A11
||         MVKL    .S2     0x3ce880ff,B11
||         ADD     .L1     4,A23,A4
||         ADD     .D1     4,A23,A3

           MVKL    .S1     0xdc08499c,A10
||         MVKL    .S2     0xbee2aeef,B13
||         LDW     .D1T2   *A4,B20
||         STW     .D2T1   A3,*+SP(16)
||         MV      .L1     A14,A3

           MVKL    .S1     0x40092200,A13
||         MVKL    .S2     0x4b9ee59e,B12
||         MVK     .L1     0xa,A1
||         ZERO    .D1     A9                ; |85| 

           MVKH    .S1     0x55555555,A31
||         MVKH    .S2     0x111110b0,B31
||         ZERO    .L1     A12               ; |85| 

           MVKH    .S1     0xbf2a01a0,A30
||         MVKH    .S2     0x3ec71de3,B30
||         STW     .D2T1   A31,*+SP(40)      ; |118| 

           MVKH    .S1     0x1a013e1a,A29
||         MVKH    .S2     0xa524f063,B29
||         STW     .D2T1   A30,*+SP(52)      ; |118| 

           MVKH    .S1     0xbe5ae645,A28
||         MVKH    .S2     0x3de6123c,B28
||         STW     .D2T2   B31,*+SP(64)      ; |118| 

           MVKH    .S1     0x4b5dc0ab,A27
||         MVKH    .S2     0x686ad430,B27
||         STW     .D2T1   A29,*+SP(48)      ; |118| 

           MVKH    .S1     0x3fd45f30,A26
||         MVKL    .S2     0x6993df95,B10
||         STW     .D2T2   B30,*+SP(76)      ; |118| 

           MVKH    .S1     0x6dc9c883,A25
||         MVKH    .S2     0x3f811111,B4
||         STW     .D2T1   A28,*+SP(60)      ; |118| 

           MVKH    .S1     0x41d00000,A24
||         MVKH    .S2     0x3ce880ff,B11
||         STW     .D2T2   B29,*+SP(72)      ; |118| 

           MVKH    .S1     0xbd6ae420,A11
||         MVKH    .S2     0xbee2aeef,B13
||         STW     .D2T1   A27,*+SP(56)      ; |118| 

           MVKH    .S1     0xdc08499c,A10
||         MVKH    .S2     0x4b9ee59e,B12
||         STW     .D2T2   B28,*+SP(84)      ; |118| 

           MVKH    .S1     0x40092200,A13
||         STW     .D2T1   A26,*+SP(36)      ; |85| 
||         MVKH    .S2     0x6993df95,B10

           STW     .D2T2   B27,*+SP(80)      ; |118| 
           STW     .D2T1   A25,*+SP(32)      ; |85| 
           STW     .D2T1   A24,*+SP(28)      ; |85| 
           STW     .D2T2   B4,*+SP(68)       ; |118| 
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Disqualified loop: Did not find partition
;*----------------------------------------------------------------------------*
$C$L4:    
;          EXCLUSIVE CPU CYCLES: 5
           LDDW    .D1T1   *A3++,A7:A6       ; |85| 
           NOP             4
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 173
           LDW     .D2T1   *+SP(28),A5
           ABSDP   .S1     A7:A6,A17:A16     ; |85| 
           MV      .L1     A9,A4
           SUB     .L1     A1,1,A1           ; |117| 
           NOP             1

           CMPGTDP .S1     A17:A16,A5:A4,A0  ; |85| 
||         LDW     .D2T1   *+SP(36),A5       ; |85| 

           LDW     .D2T1   *+SP(32),A4       ; |85| 
   [ A0]   ZERO    .L1     A7:A6             ; |85| 
           NOP             3
           MPYDP   .M1     A5:A4,A7:A6,A5:A4 ; |85| 
           NOP             9
           DPINT   .L1     A5:A4,A8          ; |85| 
           NOP             3
           INTDP   .L1     A8,A5:A4          ; |85| 
           AND     .L1     1,A8,A0           ; |118| 
           NOP             3
           MPYDP   .M1     A13:A12,A5:A4,A17:A16 ; |85| 
           MPYDP   .M2X    B13:B12,A5:A4,B5:B4 ; |85| 
           NOP             8
           SUBDP   .L1     A7:A6,A17:A16,A5:A4 ; |85| 
           NOP             7
           SUBDP   .L2X    A5:A4,B5:B4,B9:B8 ; |85| 
           LDW     .D2T1   *+SP(60),A5
           LDW     .D2T1   *+SP(56),A4
           LDW     .D2T2   *+SP(84),B5       ; |118| 
           LDW     .D2T2   *+SP(80),B4       ; |118| 
           NOP             2
           MPYDP   .M2     B9:B8,B9:B8,B17:B16 ; |85| 
           NOP             9
           MPYDP   .M2     B17:B16,B17:B16,B7:B6 ; |118| 
           NOP             9
           NOP             1
           MPYDP   .M1X    A11:A10,B7:B6,A7:A6 ; |118| 
           MPYDP   .M2     B11:B10,B7:B6,B19:B18 ; |118| 
           NOP             8
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |118| 
           ADDDP   .L2     B5:B4,B19:B18,B5:B4 ; |118| 
           NOP             5
           MPYDP   .M1X    B7:B6,A5:A4,A7:A6 ; |118| 
           MPYDP   .M2     B7:B6,B5:B4,B19:B18 ; |118| 
           LDW     .D2T2   *+SP(76),B5       ; |118| 
           LDW     .D2T1   *+SP(52),A5
           LDW     .D2T1   *+SP(48),A4
           LDW     .D2T2   *+SP(72),B4       ; |118| 
           NOP             4
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |118| 
           ADDDP   .L2     B5:B4,B19:B18,B5:B4 ; |118| 
           NOP             5
           MPYDP   .M1X    B7:B6,A5:A4,A7:A6 ; |118| 
           MPYDP   .M2     B7:B6,B5:B4,B19:B18 ; |118| 
           LDW     .D2T2   *+SP(68),B5       ; |118| 
           LDW     .D2T1   *+SP(44),A5
           LDW     .D2T1   *+SP(40),A4
           LDW     .D2T2   *+SP(64),B4       ; |118| 
           NOP             4
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |118| 
           ADDDP   .L2     B5:B4,B19:B18,B5:B4 ; |118| 
           NOP             5
           MPYDP   .M1X    B17:B16,A5:A4,A5:A4 ; |118| 
           MPYDP   .M2     B7:B6,B5:B4,B5:B4 ; |118| 
           NOP             9
           ADDDP   .L2X    A5:A4,B5:B4,B5:B4 ; |118| 
   [ A0]   ZERO    .S2     B7
   [!A0]   MV      .L2X    A12,B6            ; |118| 
   [!A0]   ZERO    .L2     B7
   [ A0]   MV      .L2X    A12,B6            ; |118| 
   [ A0]   MVKH    .S2     0xbff00000,B7
   [!A0]   SET     .S2     B7,0x14,0x1d,B7
           MPYDP   .M2     B9:B8,B5:B4,B5:B4 ; |118| 
           NOP             9
           ADDDP   .L2     B5:B4,B9:B8,B5:B4 ; |118| 
           NOP             6
           MPYDP   .M2     B7:B6,B5:B4,B5:B4 ; |118| 
           NOP             4
   [ A1]   BNOP    .S1     $C$L4,4           ; |117| 

           STDW    .D2T2   B5:B4,*B20++      ; |118| 
|| [!A1]   MVKL    .S2     output,B4

           ; BRANCHCC OCCURS {$C$L4}         ; |117| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 6

           CALL    .S1     sindp_v           ; |120| 
||         MVKH    .S2     output,B4

           LDW     .D2T2   *+B4(16),B4       ; |120| 
           ADDKPC  .S2     $C$RL4,B3,2       ; |120| 

           MV      .L1     A14,A4            ; |120| 
||         MVK     .S1     0xa,A6            ; |120| 

$C$RL4:    ; CALL OCCURS {sindp_v} {0}       ; |120| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 56
           MVKL    .S2     output,B5
           MVKL    .S2     output,B4
           MVKL    .S2     0xd9d7bdbb,B6
           MVKL    .S2     0x3ddb7cdf,B7
           MVKH    .S2     output,B5
           MVKH    .S2     output,B4
           MVKH    .S2     0xd9d7bdbb,B6

           LDW     .D2T1   *B5,A4            ; |66| 
||         MVKH    .S2     0x3ddb7cdf,B7

           CALLP   .S2     isequal,B3
||         LDW     .D2T2   *+B4(4),B4        ; |66| 
||         MVK     .L1     0xa,A6            ; |66| 
||         MVK     .S1     0x1,A8            ; |66| 

$C$RL5:    ; CALL OCCURS {isequal} {0}       ; |66| 
           MVKL    .S2     output,B4
           MVKL    .S2     fcn_pass,B6
           MVKH    .S2     output,B4

           LDW     .D2T2   *B4,B5            ; |67| 
||         MVKL    .S2     0x3ddb7cdf,B7

           MVKH    .S2     fcn_pass,B6
           MVKH    .S2     0x3ddb7cdf,B7

           STW     .D2T1   A4,*+B6(4)        ; |66| 
||         MVKL    .S2     0xd9d7bdbb,B6

           MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D2T2   *+B4(8),B4        ; |67| 
||         MV      .L1X    B5,A4             ; |67| 
||         MVK     .S1     0xa,A6            ; |67| 
||         MVK     .D1     0x1,A8            ; |67| 

$C$RL6:    ; CALL OCCURS {isequal} {0}       ; |67| 
           MVKL    .S2     output,B5
           MVKL    .S2     fcn_pass,B6
           MVKL    .S2     0x3ddb7cdf,B7
           MVKH    .S2     output,B5
           MVKH    .S2     fcn_pass,B6
           MVKH    .S2     0x3ddb7cdf,B7

           MV      .L2     B5,B4
||         STW     .D2T1   A4,*+B6(8)        ; |67| 
||         MVKL    .S2     0xd9d7bdbb,B6

           LDW     .D2T2   *+B4(12),B4       ; |68| 
||         MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D2T1   *B5,A4            ; |68| 
||         MVK     .L1     0xa,A6            ; |68| 
||         MVK     .S1     0x1,A8            ; |68| 

$C$RL7:    ; CALL OCCURS {isequal} {0}       ; |68| 
           MVKL    .S2     fcn_pass,B4

           LDW     .D2T1   *+SP(16),A3       ; |68| 
||         MVKL    .S2     0xd9d7bdbb,B6

           MVKL    .S2     0x3ddb7cdf,B7
           MVKH    .S2     fcn_pass,B4
           MVKL    .S2     output,B31
           MVKH    .S2     0xd9d7bdbb,B6

           ADD     .L1     4,A3,A3
||         STW     .D2T1   A4,*+B4(12)       ; |68| 
||         MVKH    .S2     0x3ddb7cdf,B7

           MVKH    .S2     output,B31
||         STW     .D2T1   A3,*+SP(88)

           CALLP   .S2     isequal,B3
||         LDW     .D1T2   *A3,B4            ; |69| 
||         LDW     .D2T1   *B31,A4           ; |69| 
||         MVK     .L1     0xa,A6            ; |69| 
||         MVK     .S1     0x1,A8            ; |69| 

$C$RL8:    ; CALL OCCURS {isequal} {0}       ; |69| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 14

           MVKL    .S1     fcn_pass,A3
||         MVKL    .S2     fcn_pass,B5

           MVKH    .S1     fcn_pass,A3
||         MVKH    .S2     fcn_pass,B5

           ADD     .L1     4,A3,A3
           LDW     .D1T1   *A3,A0            ; |72| 
           STW     .D2T1   A4,*+B5(16)       ; |69| 
           ADD     .L2     8,B5,B4
           STW     .D2T2   B4,*+SP(96)       ; |69| 
           STW     .D2T1   A3,*+SP(92)

   [!A0]   B       .S1     $C$L5             ; |72| 
|| [ A0]   LDW     .D2T2   *B4,B0            ; |72| 

   [!A0]   CALL    .S1     print_test_results ; |124| 
   [ A0]   ADD     .L2     4,B4,B4
   [!A0]   MVKL    .S1     all_pass,A3
   [!A0]   MV      .L2X    A12,B4
   [!A0]   MVKH    .S1     all_pass,A3
           ; BRANCHCC OCCURS {$C$L5}         ; |72| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 10
   [ B0]   LDW     .D2T2   *B4,B4            ; |72| 
           CMPEQ   .L1     A4,0,A3           ; |72| 
           XOR     .L1     1,A3,A3           ; |72| 
   [!B0]   MV      .L1     A12,A0            ; |72| 
           NOP             1
   [ B0]   CMPEQ   .L2     B4,0,B4           ; |72| 
   [ B0]   XOR     .L2     1,B4,B4           ; |72| 
           NOP             1

   [ B0]   AND     .L1X    A3,B4,A0          ; |72| 
||         MVK     .L2     0x1,B4            ; |72| 

   [!A0]   MV      .L2X    A12,B4
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 5
           CALL    .S1     print_test_results ; |124| 
           MVKL    .S1     all_pass,A3
           MVKH    .S1     all_pass,A3
           NOP             2
;** --------------------------------------------------------------------------*
$C$L5:    
;          EXCLUSIVE CPU CYCLES: 1

           MVK     .L1     0x1,A4            ; |124| 
||         STW     .D1T2   B4,*A3            ; |72| 
||         ADDKPC  .S2     $C$RL9,B3,0       ; |124| 

$C$RL9:    ; CALL OCCURS {print_test_results} {0}  ; |124| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 41
           LDW     .D2T1   *+SP(28),A2       ; |72| 
           LDW     .D2T1   *+SP(32),A18      ; |72| 
           LDW     .D2T1   *+SP(36),A19      ; |72| 
           LDW     .D2T1   *+SP(80),A20      ; |84| 
           LDW     .D2T1   *+SP(84),A21      ; |84| 
           LDW     .D2T1   *+SP(72),A22      ; |84| 
           LDW     .D2T1   *+SP(76),A23      ; |84| 
           LDW     .D2T1   *+SP(64),A24      ; |84| 
           LDW     .D2T1   *+SP(68),A25      ; |84| 
           LDW     .D2T1   *+SP(56),A26      ; |84| 
           LDW     .D2T1   *+SP(60),A27      ; |84| 
           LDW     .D2T1   *+SP(48),A28      ; |84| 
           LDW     .D2T1   *+SP(52),A29      ; |84| 
           LDW     .D2T1   *+SP(40),A30      ; |84| 
           LDW     .D2T1   *+SP(44),A31      ; |84| 

           LDW     .D2T1   *+SP(16),A3       ; |132| 
||         MVK     .L1     0x7,A1

           STW     .D2T1   A1,*+SP(12)       ; |132| 
||         ZERO    .L2     B31               ; |72| 

           STW     .D2T2   B31,*+SP(100)     ; |132| 
           STW     .D2T1   A2,*+SP(104)      ; |72| 
           STW     .D2T1   A18,*+SP(108)     ; |72| 
           STW     .D2T1   A19,*+SP(112)     ; |72| 
           STW     .D2T1   A20,*+SP(160)     ; |84| 
           STW     .D2T1   A21,*+SP(164)     ; |84| 
           STW     .D2T1   A22,*+SP(152)     ; |84| 
           STW     .D2T1   A23,*+SP(156)     ; |84| 
           STW     .D2T1   A24,*+SP(144)     ; |84| 
           STW     .D2T1   A25,*+SP(148)     ; |84| 
           STW     .D2T1   A26,*+SP(136)     ; |84| 
           STW     .D2T1   A27,*+SP(140)     ; |84| 
           STW     .D2T1   A28,*+SP(128)     ; |84| 
           STW     .D2T1   A29,*+SP(132)     ; |84| 
           STW     .D2T1   A30,*+SP(120)     ; |84| 

           STW     .D2T1   A31,*+SP(124)     ; |84| 
||         MVKL    .S2     a_sc,B6
||         MVK     .L2     0xffffffff,B5     ; |137| 

           STW     .D2T1   A3,*+SP(116)      ; |84| 
||         MV      .L2     B5,B4             ; |137| 
||         MVKH    .S2     a_sc,B6
||         ZERO    .L1     A9

           MV      .L2     B6,B4             ; |131| 
||         STDW    .D2T2   B5:B4,*+B6(48)    ; |137| 
||         MVKH    .S1     0x80000000,A9
||         ZERO    .S2     B9
||         ZERO    .L1     A8                ; |132| 

           STDW    .D2T1   A9:A8,*+B4(8)     ; |132| 
||         SET     .S2     B9,0x14,0x1e,B9
||         ZERO    .L1     A7
||         ZERO    .L2     B8                ; |133| 

           STDW    .D2T2   B9:B8,*+B4(16)    ; |133| 
||         MVKH    .S1     0xfff00000,A7
||         ZERO    .L1     A5
||         ZERO    .D1     A6                ; |134| 

           STDW    .D2T1   A7:A6,*+B4(24)    ; |134| 
||         MVKH    .S1     0xbff00000,A5
||         ZERO    .L1     A4                ; |136| 

           STDW    .D2T1   A5:A4,*+B4(40)    ; |136| 
||         ZERO    .L2     B7
||         ZERO    .L1     A17:A16           ; |131| 

           STDW    .D2T1   A17:A16,*B4       ; |131| 
||         ZERO    .L2     B6                ; |135| 
||         MV      .L1X    B6,A15            ; |131| 
||         SET     .S2     B7,0x14,0x1d,B7

           CALL    .S1     sin               ; |139| 
||         STDW    .D2T2   B7:B6,*+B4(32)    ; |135| 
||         ZERO    .L1     A12               ; |84| 
||         ZERO    .D1     A14

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L6:    
;          EXCLUSIVE CPU CYCLES: 5
           LDDW    .D1T1   *A15,A5:A4        ; |139| 
           ADDKPC  .S2     $C$RL10,B3,3      ; |139| 
$C$RL10:   ; CALL OCCURS {sin} {0}           ; |139| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 30
           MVKL    .S2     output,B4
           MVKH    .S2     output,B4
           LDW     .D2T2   *B4,B4            ; |139| 
           NOP             4
           ADD     .L2X    A14,B4,B4         ; |139| 
           STDW    .D2T1   A5:A4,*B4         ; |139| 

           CALLP   .S2     sindp,B3
||         LDDW    .D1T1   *A15,A5:A4        ; |140| 

$C$RL11:   ; CALL OCCURS {sindp} {0}         ; |140| 
           MVKL    .S2     output,B4
           MVKH    .S2     output,B4
           LDW     .D2T2   *+B4(4),B4        ; |140| 
           NOP             4
           ADD     .L1X    A14,B4,A3         ; |140| 
           STDW    .D1T1   A5:A4,*A3         ; |140| 

           CALLP   .S2     sindp_c,B3
||         LDDW    .D1T1   *A15,A5:A4        ; |141| 

$C$RL12:   ; CALL OCCURS {sindp_c} {0}       ; |141| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 185
           MVKL    .S2     output,B4
           MVKH    .S2     output,B4
           LDW     .D2T2   *+B4(8),B4        ; |141| 
           LDW     .D2T2   *+SP(104),B5
           LDW     .D2T2   *+SP(164),B19     ; |142| 
           LDW     .D2T2   *+SP(160),B18     ; |142| 
           LDW     .D2T1   *+SP(12),A30      ; |142| 
           ADD     .L1X    A14,B4,A3         ; |141| 
           STDW    .D1T1   A5:A4,*A3         ; |141| 
           LDDW    .D1T1   *A15++,A7:A6      ; |70| 
           MV      .L2X    A12,B4            ; |72| 
           LDW     .D2T1   *+SP(116),A31
           SUB     .L1     A30,1,A29         ; |138| 
           STW     .D2T1   A29,*+SP(12)      ; |142| 
           ABSDP   .S1     A7:A6,A5:A4       ; |72| 
           NOP             2

           CMPGTDP .S2X    A5:A4,B5:B4,B0    ; |72| 
||         LDW     .D2T1   *+SP(112),A5      ; |73| 

           LDW     .D2T1   *+SP(108),A4      ; |73| 
   [ B0]   ZERO    .L1     A7:A6             ; |73| 
           NOP             3
           MPYDP   .M1     A5:A4,A7:A6,A5:A4 ; |78| 
           NOP             9
           DPINT   .L1     A5:A4,A3          ; |78| 
           NOP             3
           INTDP   .L1     A3,A5:A4          ; |78| 
           AND     .L1     1,A3,A0           ; |81| 
           LDW     .D1T1   *A31,A3           ; |142| 
           NOP             2
           MPYDP   .M1     A13:A12,A5:A4,A9:A8 ; |84| 
           MPYDP   .M2X    B13:B12,A5:A4,B5:B4 ; |84| 
           ADD     .L1     A14,A3,A3         ; |142| 
           ADD     .L1     8,A14,A14         ; |138| 
           NOP             6
           SUBDP   .L1     A7:A6,A9:A8,A5:A4 ; |84| 
           NOP             7
           SUBDP   .L2X    A5:A4,B5:B4,B9:B8 ; |84| 
           LDW     .D2T1   *+SP(140),A5
           LDW     .D2T1   *+SP(136),A4
           NOP             4
           MPYDP   .M2     B9:B8,B9:B8,B5:B4 ; |142| 
           NOP             9
           MPYDP   .M2     B5:B4,B5:B4,B7:B6 ; |142| 
           NOP             9
           NOP             1
           MPYDP   .M1X    A11:A10,B7:B6,A7:A6 ; |142| 
           MPYDP   .M2     B11:B10,B7:B6,B17:B16 ; |142| 
           NOP             8
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |142| 
           ADDDP   .L2     B19:B18,B17:B16,B17:B16 ; |142| 
           LDW     .D2T2   *+SP(156),B19     ; |142| 
           LDW     .D2T2   *+SP(152),B18     ; |142| 
           NOP             3
           MPYDP   .M1X    B7:B6,A5:A4,A7:A6 ; |142| 
           MPYDP   .M2     B7:B6,B17:B16,B17:B16 ; |142| 
           LDW     .D2T1   *+SP(132),A5
           LDW     .D2T1   *+SP(128),A4
           NOP             6
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |142| 
           ADDDP   .L2     B19:B18,B17:B16,B17:B16 ; |142| 
           LDW     .D2T2   *+SP(148),B19     ; |142| 
           LDW     .D2T2   *+SP(144),B18     ; |142| 
           NOP             3
           MPYDP   .M1X    B7:B6,A5:A4,A7:A6 ; |142| 
           MPYDP   .M2     B7:B6,B17:B16,B17:B16 ; |142| 
           LDW     .D2T1   *+SP(124),A5
           LDW     .D2T1   *+SP(120),A4
           NOP             6
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |142| 
           ADDDP   .L2     B19:B18,B17:B16,B17:B16 ; |142| 
           NOP             5
           MPYDP   .M1X    B5:B4,A5:A4,A5:A4 ; |142| 
           MPYDP   .M2     B7:B6,B17:B16,B5:B4 ; |142| 
           NOP             9
           ADDDP   .L2X    A5:A4,B5:B4,B5:B4 ; |142| 
   [!A0]   ZERO    .S2     B7
   [!A0]   MV      .L2X    A12,B6            ; |81| 
   [ A0]   ZERO    .L2     B7
   [ A0]   MV      .L2X    A12,B6            ; |81| 
   [!A0]   SET     .S2     B7,0x14,0x1d,B7
   [ A0]   MVKH    .S2     0xbff00000,B7
           MPYDP   .M2     B9:B8,B5:B4,B5:B4 ; |142| 
           SUB     .L1     A30,1,A0          ; |138| 
           NOP             8
           ADDDP   .L2     B5:B4,B9:B8,B5:B4 ; |142| 
           NOP             6
           MPYDP   .M2     B7:B6,B5:B4,B5:B4 ; |142| 
           NOP             4
   [ A0]   BNOP    .S1     $C$L6,4           ; |138| 

   [ A0]   CALL    .S1     sin               ; |139| 
||         STDW    .D1T2   B5:B4,*A3         ; |142| 
|| [!A0]   MVKL    .S2     output,B4

           ; BRANCHCC OCCURS {$C$L6}         ; |138| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 6

           CALL    .S1     sindp_v           ; |144| 
||         MVKH    .S2     output,B4

           LDW     .D2T2   *+B4(16),B4       ; |144| 
           MVKL    .S1     a_sc,A4
           MVKH    .S1     a_sc,A4
           ADDKPC  .S2     $C$RL13,B3,0      ; |144| 
           MVK     .L1     0x7,A6            ; |144| 
$C$RL13:   ; CALL OCCURS {sindp_v} {0}       ; |144| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 54
           MVKL    .S2     output,B5
           MVKL    .S2     output,B4
           MVKL    .S2     0xd9d7bdbb,B6
           MVKL    .S2     0x3ddb7cdf,B7
           MVKH    .S2     output,B5
           MVKH    .S2     output,B4
           MVKH    .S2     0xd9d7bdbb,B6

           LDW     .D2T1   *B5,A4            ; |66| 
||         MVKH    .S2     0x3ddb7cdf,B7

           CALLP   .S2     isequal,B3
||         LDW     .D2T2   *+B4(4),B4        ; |66| 
||         MVK     .L1     0x7,A6            ; |66| 
||         MVK     .S1     0x1,A8            ; |66| 

$C$RL14:   ; CALL OCCURS {isequal} {0}       ; |66| 
           MVKL    .S2     output,B4
           MVKL    .S2     fcn_pass,B6
           MVKH    .S2     output,B4

           LDW     .D2T2   *B4,B5            ; |67| 
||         MVKL    .S2     0x3ddb7cdf,B7

           MVKH    .S2     fcn_pass,B6
           MVKH    .S2     0x3ddb7cdf,B7

           STW     .D2T1   A4,*+B6(4)        ; |66| 
||         MVKL    .S2     0xd9d7bdbb,B6

           MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D2T2   *+B4(8),B4        ; |67| 
||         MV      .L1X    B5,A4             ; |67| 
||         MVK     .S1     0x7,A6            ; |67| 
||         MVK     .D1     0x1,A8            ; |67| 

$C$RL15:   ; CALL OCCURS {isequal} {0}       ; |67| 
           MVKL    .S2     fcn_pass,B5
           MVKL    .S2     output,B8
           MVKL    .S2     0xd9d7bdbb,B6
           MVKH    .S2     fcn_pass,B5
           MVKL    .S2     0x3ddb7cdf,B7
           MVKH    .S2     output,B8

           MV      .L2     B8,B4
||         STW     .D2T1   A4,*+B5(8)        ; |67| 
||         MVKH    .S2     0xd9d7bdbb,B6

           LDW     .D2T2   *+B4(12),B4       ; |68| 
||         MVKH    .S2     0x3ddb7cdf,B7

           CALLP   .S2     isequal,B3
||         LDW     .D2T1   *B8,A4            ; |68| 
||         MVK     .L1     0x7,A6            ; |68| 
||         MVK     .S1     0x1,A8            ; |68| 

$C$RL16:   ; CALL OCCURS {isequal} {0}       ; |68| 
           MVKL    .S2     fcn_pass,B4

           LDW     .D2T1   *+SP(88),A3       ; |68| 
||         MVKL    .S2     0xd9d7bdbb,B6

           MVKL    .S2     0x3ddb7cdf,B7

           MVKL    .S1     output,A31
||         MVKH    .S2     fcn_pass,B4

           MVKH    .S1     output,A31
||         STW     .D2T1   A4,*+B4(12)       ; |68| 
||         MVKH    .S2     0xd9d7bdbb,B6

           LDW     .D1T1   *A31,A4           ; |69| 
||         MVKH    .S2     0x3ddb7cdf,B7

           CALLP   .S2     isequal,B3
||         LDW     .D1T2   *A3,B4            ; |69| 
||         STW     .D2T1   A3,*+SP(100)      ; |68| 
||         MVK     .L1     0x7,A6            ; |69| 
||         MVK     .S1     0x1,A8            ; |69| 

$C$RL17:   ; CALL OCCURS {isequal} {0}       ; |69| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 17
           LDW     .D2T1   *+SP(92),A3       ; |69| 
           MVKL    .S2     fcn_pass,B31
           MVKH    .S2     fcn_pass,B31
           STW     .D2T1   A4,*+B31(16)      ; |69| 
           NOP             1

           MV      .L2X    A3,B4             ; |69| 
||         STW     .D2T1   A3,*+SP(104)      ; |69| 

           LDW     .D2T2   *B4,B0            ; |72| 
           LDW     .D2T1   *+SP(96),A3
           NOP             3

   [!B0]   B       .S2     $C$L7             ; |72| 
|| [!B0]   MVKL    .S1     all_pass,A4
||         MV      .L2     B0,B1             ; guard predicate rewrite

   [!B0]   CALL    .S2     print_test_results ; |148| 
|| [ B0]   MV      .L2X    A3,B4
|| [!B0]   MVKH    .S1     all_pass,A4
||         STW     .D2T1   A3,*+SP(108)      ; |72| 
|| [!B0]   MV      .L1     A12,A3

   [ B0]   LDW     .D2T2   *B4,B0            ; |72| 
   [ B1]   ADD     .L2     4,B4,B4
           NOP             2
           ; BRANCHCC OCCURS {$C$L7}         ; |72| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 11
           CMPEQ   .L1     A4,0,A3           ; |72| 
   [ B0]   LDW     .D2T2   *B4,B4            ; |72| 
           XOR     .L1     1,A3,A3           ; |72| 
   [!B0]   MV      .L1     A12,A0            ; |72| 
           NOP             2
   [ B0]   CMPEQ   .L2     B4,0,B4           ; |72| 
   [ B0]   XOR     .L2     1,B4,B4           ; |72| 
           NOP             1

   [ B0]   AND     .L1X    A3,B4,A0          ; |72| 
||         MVK     .S1     0x1,A3            ; |72| 

   [!A0]   MV      .L1     A12,A3
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 5
           CALL    .S1     print_test_results ; |148| 
           MVKL    .S1     all_pass,A4
           MVKH    .S1     all_pass,A4
           NOP             2
;** --------------------------------------------------------------------------*
$C$L7:    
;          EXCLUSIVE CPU CYCLES: 1

           STW     .D1T1   A3,*A4            ; |72| 
||         MVK     .L1     0x3,A4            ; |148| 
||         ADDKPC  .S2     $C$RL18,B3,0      ; |148| 

$C$RL18:   ; CALL OCCURS {print_test_results} {0}  ; |148| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 13
           STW     .D2T1   A10,*+SP(116)     ; |84| 
           LDW     .D2T1   *+SP(16),A3       ; |157| 
           STW     .D2T1   A11,*+SP(120)     ; |84| 

           LDW     .D2T1   *+SP(32),A10      ; |72| 
||         MVK     .S1     0x6f4,A31

           STW     .D2T1   A31,*+SP(12)      ; |151| 
||         MV      .L2X    A12,B30           ; |151| 

           STW     .D2T2   B30,*+SP(16)      ; |151| 
||         ZERO    .L2     B5

           LDW     .D2T1   *+SP(36),A11      ; |72| 
||         MVKH    .S2     0xbff80000,B5

           STW     .D2T2   B5,*+SP(124)      ; |157| 
||         MVKL    .S2     a_ext,B4
||         MV      .L2X    A12,B31           ; |151| 

           STW     .D2T2   B31,*+SP(32)      ; |151| 
||         MVKH    .S2     a_ext,B4
||         ZERO    .L1     A5

           STW     .D2T1   A3,*+SP(112)      ; |84| 
||         SET     .S1     A5,0x14,0x14,A5
||         MV      .L1     A12,A4            ; |151| 

           CALL    .S1     sin               ; |153| 
||         STDW    .D2T1   A5:A4,*B4         ; |151| 
||         MV      .L1X    B4,A14            ; |151| 

           LDDW    .D1T1   *A14,A5:A4        ; |153| 
           MV      .L1     A12,A15           ; |151| 
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L8:    
;          EXCLUSIVE CPU CYCLES: 3
           ADDKPC  .S2     $C$RL19,B3,2      ; |153| 
$C$RL19:   ; CALL OCCURS {sin} {0}           ; |153| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 30
           MVKL    .S2     output,B4
           MVKH    .S2     output,B4
           LDW     .D2T2   *B4,B4            ; |153| 
           NOP             4
           ADD     .L1X    A15,B4,A3         ; |153| 
           STDW    .D1T1   A5:A4,*A3         ; |153| 

           CALLP   .S2     sindp,B3
||         LDDW    .D1T1   *A14,A5:A4        ; |154| 

$C$RL20:   ; CALL OCCURS {sindp} {0}         ; |154| 
           MVKL    .S2     output,B4
           MVKH    .S2     output,B4
           LDW     .D2T2   *+B4(4),B4        ; |154| 
           NOP             4
           ADD     .L1X    A15,B4,A3         ; |154| 
           STDW    .D1T1   A5:A4,*A3         ; |154| 

           CALLP   .S2     sindp_c,B3
||         LDDW    .D1T1   *A14,A5:A4        ; |155| 

$C$RL21:   ; CALL OCCURS {sindp_c} {0}       ; |155| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 200
           MVKL    .S2     output,B4
           MVKH    .S2     output,B4
           LDW     .D2T2   *+B4(8),B4        ; |155| 
           LDW     .D2T2   *+SP(28),B5
           LDW     .D2T2   *+SP(84),B19      ; |156| 
           LDW     .D2T2   *+SP(80),B18      ; |156| 
           LDW     .D2T1   *+SP(112),A31
           ADD     .L1X    A15,B4,A3         ; |155| 
           STDW    .D1T1   A5:A4,*A3         ; |155| 
           LDDW    .D1T1   *A14,A7:A6        ; |70| 
           MV      .L2X    A12,B4            ; |72| 
           LDW     .D2T1   *+SP(12),A30
           NOP             2
           ABSDP   .S1     A7:A6,A5:A4       ; |72| 
           NOP             2
           CMPGTDP .S2X    A5:A4,B5:B4,B0    ; |72| 
           NOP             1
   [ B0]   ZERO    .L1     A7:A6             ; |73| 
           MPYDP   .M1     A11:A10,A7:A6,A5:A4 ; |78| 
           NOP             9
           DPINT   .L1     A5:A4,A3          ; |78| 
           NOP             3
           INTDP   .L1     A3,A5:A4          ; |78| 
           AND     .L1     1,A3,A0           ; |81| 
           LDW     .D1T1   *A31,A3           ; |156| 
           NOP             2
           MPYDP   .M1     A13:A12,A5:A4,A9:A8 ; |84| 
           MPYDP   .M2X    B13:B12,A5:A4,B5:B4 ; |84| 
           ADD     .L1     A15,A3,A3         ; |156| 
           ADD     .L1     8,A15,A15         ; |152| 
           NOP             6
           SUBDP   .L1     A7:A6,A9:A8,A5:A4 ; |84| 
           NOP             7
           SUBDP   .L2X    A5:A4,B5:B4,B9:B8 ; |84| 
           LDW     .D2T1   *+SP(120),A5
           LDW     .D2T1   *+SP(116),A4
           NOP             4
           MPYDP   .M2     B9:B8,B9:B8,B5:B4 ; |156| 
           NOP             9
           MPYDP   .M2     B5:B4,B5:B4,B7:B6 ; |156| 
           NOP             9
           NOP             1
           MPYDP   .M1X    A5:A4,B7:B6,A7:A6 ; |156| 
           MPYDP   .M2     B11:B10,B7:B6,B17:B16 ; |156| 
           LDW     .D2T1   *+SP(60),A5
           LDW     .D2T1   *+SP(56),A4
           NOP             6
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |156| 
           ADDDP   .L2     B19:B18,B17:B16,B17:B16 ; |156| 
           LDW     .D2T2   *+SP(76),B19      ; |156| 
           LDW     .D2T2   *+SP(72),B18      ; |156| 
           NOP             3
           MPYDP   .M1X    B7:B6,A5:A4,A7:A6 ; |156| 
           MPYDP   .M2     B7:B6,B17:B16,B17:B16 ; |156| 
           LDW     .D2T1   *+SP(52),A5
           LDW     .D2T1   *+SP(48),A4
           NOP             6
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |156| 
           ADDDP   .L2     B19:B18,B17:B16,B17:B16 ; |156| 
           LDW     .D2T2   *+SP(68),B19      ; |156| 
           LDW     .D2T2   *+SP(64),B18      ; |156| 
           NOP             3
           MPYDP   .M1X    B7:B6,A5:A4,A7:A6 ; |156| 
           MPYDP   .M2     B7:B6,B17:B16,B17:B16 ; |156| 
           LDW     .D2T1   *+SP(44),A5
           LDW     .D2T1   *+SP(40),A4
           NOP             6
           ADDDP   .L1     A5:A4,A7:A6,A5:A4 ; |156| 
           ADDDP   .L2     B19:B18,B17:B16,B17:B16 ; |156| 
           NOP             5
           MPYDP   .M1X    B5:B4,A5:A4,A5:A4 ; |156| 
           MPYDP   .M2     B7:B6,B17:B16,B5:B4 ; |156| 
           NOP             9
           ADDDP   .L2X    A5:A4,B5:B4,B5:B4 ; |156| 
   [ A0]   ZERO    .S2     B7
   [ A0]   MV      .L2X    A12,B6            ; |81| 
   [!A0]   ZERO    .L2     B7
   [!A0]   MV      .L2X    A12,B6            ; |81| 
   [ A0]   MVKH    .S2     0xbff00000,B7
   [!A0]   SET     .S2     B7,0x14,0x1d,B7
           MPYDP   .M2     B9:B8,B5:B4,B5:B4 ; |156| 
           SUB     .L1     A30,1,A0          ; |152| 
   [!A0]   MVKL    .S2     output,B10
   [!A0]   MVKH    .S2     output,B10
           NOP             6
           ADDDP   .L2     B5:B4,B9:B8,B5:B4 ; |156| 
           NOP             6
           MPYDP   .M2     B7:B6,B5:B4,B5:B4 ; |156| 
           NOP             9
           STDW    .D1T2   B5:B4,*A3         ; |156| 

           LDDW    .D1T1   *A14,A5:A4        ; |157| 
||         LDW     .D2T2   *+SP(124),B5

           MV      .L2X    A12,B4            ; |157| 
           SUB     .L1     A30,1,A3          ; |152| 
           NOP             2
           MPYDP   .M2X    B5:B4,A5:A4,B5:B4 ; |157| 
           NOP             6
   [ A0]   BNOP    .S1     $C$L8,2           ; |152| 

   [ A0]   CALL    .S1     sin               ; |153| 
||         STDW    .D1T2   B5:B4,*++A14      ; |157| 

   [ A0]   LDDW    .D1T1   *A14,A5:A4        ; |153| 

   [!A0]   CALL    .S1     sindp_v           ; |159| 
||         STW     .D2T1   A3,*+SP(12)       ; |157| 

           ; BRANCHCC OCCURS {$C$L8}         ; |152| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 5
           LDW     .D2T2   *+B10(16),B4      ; |159| 
           MVKL    .S1     a_ext,A4
           MVKH    .S1     a_ext,A4
           MVK     .S1     0x6f4,A6          ; |159| 
           ADDKPC  .S2     $C$RL22,B3,0      ; |159| 
$C$RL22:   ; CALL OCCURS {sindp_v} {0}       ; |159| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 43
           MVKL    .S2     0x3ddb7cdf,B7
           MVKL    .S2     0xd9d7bdbb,B6
           MVKH    .S2     0x3ddb7cdf,B7

           LDW     .D2T1   *B10,A4           ; |66| 
||         MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D2T2   *+B10(4),B4       ; |66| 
||         MVK     .S1     0x6f4,A6          ; |66| 
||         MVK     .L1     0x1,A8            ; |66| 

$C$RL23:   ; CALL OCCURS {isequal} {0}       ; |66| 
           MVKL    .S2     fcn_pass,B11
           MVKL    .S2     0x3ddb7cdf,B7
           MVKH    .S2     fcn_pass,B11
           MVKL    .S2     0xd9d7bdbb,B6

           LDW     .D2T2   *+B10(8),B4       ; |67| 
||         MVKH    .S2     0x3ddb7cdf,B7

           STW     .D2T1   A4,*+B11(4)       ; |66| 
||         MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D2T1   *B10,A4           ; |67| 
||         MVK     .S1     0x6f4,A6          ; |67| 
||         MVK     .L1     0x1,A8            ; |67| 

$C$RL24:   ; CALL OCCURS {isequal} {0}       ; |67| 
           MVKL    .S2     0x3ddb7cdf,B7
           MVKL    .S2     0xd9d7bdbb,B6
           MVKH    .S2     0x3ddb7cdf,B7

           MV      .L1X    B10,A3            ; |67| 
||         STW     .D2T1   A4,*+B11(8)       ; |67| 
||         MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D1T1   *A3,A4            ; |68| 
||         LDW     .D2T2   *+B10(12),B4      ; |68| 
||         MVK     .S1     0x6f4,A6          ; |68| 
||         MVK     .L1     0x1,A8            ; |68| 

$C$RL25:   ; CALL OCCURS {isequal} {0}       ; |68| 
           LDW     .D2T1   *+SP(88),A3       ; |68| 
           MVKL    .S2     0x3ddb7cdf,B7
           MVKL    .S2     0xd9d7bdbb,B6
           MVKH    .S2     0x3ddb7cdf,B7

           MV      .L2     B10,B4            ; |68| 
||         STW     .D2T1   A4,*+B11(12)      ; |68| 
||         MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D2T1   *B4,A4            ; |69| 
||         LDW     .D1T2   *A3,B4            ; |69| 
||         MVK     .S1     0x6f4,A6          ; |69| 
||         MVK     .L1     0x1,A8            ; |69| 

$C$RL26:   ; CALL OCCURS {isequal} {0}       ; |69| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 16
           LDW     .D2T1   *+SP(92),A3       ; |69| 
           LDW     .D2T2   *+SP(96),B4
           STW     .D2T1   A4,*+B11(16)      ; |69| 
           NOP             2
           LDW     .D1T1   *A3,A0            ; |72| 
           NOP             4

   [!A0]   B       .S1     $C$L9             ; |72| 
|| [ A0]   LDW     .D2T2   *B4,B0            ; |72| 

   [!A0]   CALL    .S1     print_test_results ; |163| 
   [!A0]   MV      .L1     A12,A4
   [ A0]   ADD     .L2     4,B4,B4
   [!A0]   MVKL    .S1     all_pass,A3
   [!A0]   MVKH    .S1     all_pass,A3
           ; BRANCHCC OCCURS {$C$L9}         ; |72| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 10
   [ B0]   LDW     .D2T2   *B4,B4            ; |72| 
           CMPEQ   .L1     A4,0,A3           ; |72| 
           XOR     .L1     1,A3,A3           ; |72| 
   [!B0]   MV      .L1     A12,A0            ; |72| 
           MVK     .L1     0x1,A4            ; |72| 
   [ B0]   CMPEQ   .L2     B4,0,B4           ; |72| 
   [ B0]   XOR     .L2     1,B4,B4           ; |72| 
           NOP             1
   [ B0]   AND     .L1X    A3,B4,A0          ; |72| 
   [!A0]   MV      .L1     A12,A4
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 5
           CALL    .S1     print_test_results ; |163| 
           MVKL    .S1     all_pass,A3
           MVKH    .S1     all_pass,A3
           NOP             2
;** --------------------------------------------------------------------------*
$C$L9:    
;          EXCLUSIVE CPU CYCLES: 1

           STW     .D1T1   A4,*A3            ; |72| 
||         MVK     .L1     0x4,A4            ; |163| 
||         ADDKPC  .S2     $C$RL27,B3,0      ; |163| 

$C$RL27:   ; CALL OCCURS {print_test_results} {0}  ; |163| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 16
           MV      .L2X    A12,B12           ; |114| 

           MV      .L2X    A12,B13           ; |114| 
||         MVKL    .S1     cycle_counts+24,A3

           MVKH    .S1     cycle_counts+24,A3
||         MV      .L2X    A12,B4            ; |114| 

           MVC     .S2     B4,TSCL           ; |115| 
||         STDW    .D1T2   B13:B12,*A3       ; |114| 

           MVC     .S2     TSCL,B6           ; |116| 
           MVC     .S2     TSCH,B4           ; |116| 
           MVKL    .S1     t_start,A3

           MVKH    .S1     t_start,A3
||         MV      .L2     B4,B7             ; |116| 

           MVC     .S2     TSCL,B4           ; |117| 
||         STDW    .D1T2   B7:B6,*A3         ; |116| 

           MVC     .S2     TSCH,B8           ; |117| 

           MVKL    .S1     t_offset,A3
||         SUBU    .L2     B4,B6,B5:B4       ; |117| 

           SUB     .L2     B8,B7,B6          ; |117| 
||         EXT     .S2     B5,24,24,B5       ; |117| 
||         MVK     .S1     0x80,A12

           ADD     .L2     B6,B5,B5          ; |117| 
||         MVKH    .S1     t_offset,A3

           MVKL    .S1     input,A3
||         STDW    .D1T2   B5:B4,*A3         ; |117| 

           MVKH    .S1     input,A3
||         ZERO    .L2     B5

           CALL    .S1     gimme_random      ; |173| 
||         MVKH    .S2     0xc0590000,B5
||         MV      .L2     B12,B4            ; |173| 
||         MV      .D2     B12,B10           ; |117| 
||         ZERO    .L1     A5

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L10:    
;          EXCLUSIVE CPU CYCLES: 5
           LDW     .D1T1   *A3,A10           ; |173| 
           MV      .L1X    B13,A4            ; |173| 
           ADDKPC  .S2     $C$RL28,B3,1      ; |173| 
           MVKH    .S1     0x40690000,A5
$C$RL28:   ; CALL OCCURS {gimme_random} {0}  ; |173| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 197
           ADD     .L2X    A10,B10,B4        ; |173| 

           STDW    .D2T1   A5:A4,*B4         ; |173| 
||         MVC     .S2     TSCL,B8           ; |122| 

           MVC     .S2     TSCH,B9           ; |122| 

           ADD     .L1X    A10,B10,A3        ; |72| 
||         ZERO    .L2     B5
||         MV      .S2     B12,B4            ; |72| 
||         MVKL    .S1     0x6dc9c883,A6

           LDDW    .D1T1   *A3,A9:A8         ; |72| 
           MVKL    .S1     0x3fd45f30,A7
           MVKH    .S2     0x41d00000,B5
           MVKH    .S1     0x6dc9c883,A6
           MVKH    .S1     0x3fd45f30,A7
           ABSDP   .S1     A9:A8,A9:A8       ; |72| 
           MVKL    .S2     0x686ad430,B20
           CMPGTDP .S1X    A9:A8,B5:B4,A0    ; |72| 
           MV      .L2X    A13,B5            ; |78| 
   [ A0]   ZERO    .L1     A5:A4             ; |73| 
           MPYDP   .M1     A7:A6,A5:A4,A7:A6 ; |78| 
           MVKL    .S2     0x3de6123c,B21
           MVKH    .S2     0x686ad430,B20
           MVKH    .S2     0x3de6123c,B21
           MVKL    .S2     output+12,B31
           MVKH    .S2     output+12,B31
           MVKL    .S2     t_start,B30
           MVKH    .S2     t_start,B30
           STDW    .D2T2   B9:B8,*B30
           NOP             1
           DPINT   .L1     A7:A6,A3          ; |78| 
           NOP             3
           INTDP   .L1     A3,A7:A6          ; |78| 
           AND     .L1     1,A3,A0           ; |81| 
           NOP             4
           MPYDP   .M2X    B5:B4,A7:A6,B7:B6 ; |84| 
           NOP             2
           MVKL    .S2     0xbee2aeef,B5
           MVKL    .S2     0x4b9ee59e,B4
           MVKH    .S2     0xbee2aeef,B5
           MVKH    .S2     0x4b9ee59e,B4
           NOP             1
           MPYDP   .M1X    B5:B4,A7:A6,A7:A6 ; |84| 
           NOP             3
           SUBDP   .L2X    A5:A4,B7:B6,B5:B4 ; |84| 
           MVKL    .S1     0xdc08499c,A4
           MVKL    .S1     0xbd6ae420,A5
           MVKH    .S1     0xdc08499c,A4
           MVKH    .S1     0xbd6ae420,A5
           NOP             2
           SUBDP   .L2X    B5:B4,A7:A6,B7:B6 ; |84| 
           MVKL    .S1     0x6993df95,A6
           MVKL    .S1     0x3ce880ff,A7
           MVKH    .S1     0x6993df95,A6
           MVKH    .S1     0x3ce880ff,A7
           NOP             2
           MPYDP   .M2     B7:B6,B7:B6,B17:B16 ; |177| 
           NOP             9
           MPYDP   .M2     B17:B16,B17:B16,B5:B4 ; |177| 
           NOP             9
           MPYDP   .M2X    A5:A4,B5:B4,B19:B18 ; |177| 
           MPYDP   .M1X    A7:A6,B5:B4,A7:A6 ; |177| 
           NOP             5
           MVKL    .S1     0x4b5dc0ab,A4
           MVKL    .S1     0xbe5ae645,A5
           MVKH    .S1     0x4b5dc0ab,A4
           MVKH    .S1     0xbe5ae645,A5
           ADDDP   .L1X    A5:A4,B19:B18,A5:A4 ; |177| 
           ADDDP   .L2X    B21:B20,A7:A6,B19:B18 ; |177| 
           MVKL    .S1     0xa524f063,A6
           MVKL    .S1     0x3ec71de3,A7
           MVKH    .S1     0xa524f063,A6
           MVKH    .S1     0x3ec71de3,A7
           NOP             1
           MPYDP   .M1X    B5:B4,A5:A4,A5:A4 ; |177| 
           MPYDP   .M2     B5:B4,B19:B18,B21:B20 ; |177| 
           NOP             5
           MVKL    .S2     0x1a013e1a,B18
           MVKL    .S2     0xbf2a01a0,B19
           MVKH    .S2     0x1a013e1a,B18
           MVKH    .S2     0xbf2a01a0,B19
           ADDDP   .L2X    B19:B18,A5:A4,B19:B18 ; |177| 
           ADDDP   .L1X    A7:A6,B21:B20,A5:A4 ; |177| 
           MVKL    .S2     0x111110b0,B20
           MVKL    .S2     0x3f811111,B21
           MVKH    .S2     0x111110b0,B20
           MVKH    .S2     0x3f811111,B21
           NOP             1
           MPYDP   .M2     B5:B4,B19:B18,B19:B18 ; |177| 
           MPYDP   .M1X    B5:B4,A5:A4,A7:A6 ; |177| 
           NOP             5
           MVKL    .S1     0x55555555,A4
           MVKL    .S1     0xbfc55555,A5
           MVKH    .S1     0x55555555,A4
           MVKH    .S1     0xbfc55555,A5
           ADDDP   .L1X    A5:A4,B19:B18,A5:A4 ; |177| 
           ADDDP   .L2X    B21:B20,A7:A6,B19:B18 ; |177| 
           NOP             5
           MPYDP   .M1X    B17:B16,A5:A4,A5:A4 ; |177| 
           MPYDP   .M2     B5:B4,B19:B18,B5:B4 ; |177| 
           NOP             9
           ADDDP   .L2X    A5:A4,B5:B4,B5:B4 ; |177| 
           NOP             6
           MPYDP   .M2     B7:B6,B5:B4,B5:B4 ; |177| 
           NOP             9
           ADDDP   .L2     B5:B4,B7:B6,B7:B6 ; |177| 
   [ A0]   ZERO    .S2     B5
   [!A0]   ZERO    .L2     B5
   [!A0]   MV      .L2     B12,B4            ; |81| 
   [ A0]   MV      .L2     B12,B4            ; |81| 
   [ A0]   MVKH    .S2     0xbff00000,B5
   [!A0]   SET     .S2     B5,0x14,0x1d,B5
           MPYDP   .M2     B5:B4,B7:B6,B7:B6 ; |177| 
           LDW     .D2T2   *B31,B5           ; |177| 
           NOP             7
           ADD     .L2     B10,B5,B4         ; |177| 

           STDW    .D2T2   B7:B6,*B4         ; |177| 
||         MVC     .S2     TSCL,B6           ; |128| 

           MVC     .S2     TSCH,B4           ; |128| 

           MVKL    .S1     t_offset,A3
||         SUB     .L1     A12,1,A0          ; |171| 
||         MV      .L2     B4,B7             ; |128| 
||         MVKL    .S2     t_stop,B11
||         ADD     .D2     8,B10,B10         ; |171| 
||         SUB     .D1     A12,1,A12         ; |171| 

           MVKH    .S1     t_offset,A3
|| [ A0]   ZERO    .L2     B5
||         MVKH    .S2     t_stop,B11

           STDW    .D2T2   B7:B6,*B11        ; |128| 
||         LDDW    .D1T1   *A3,A7:A6         ; |130| 
||         MVKL    .S1     cycle_counts+24,A31

           MVKH    .S1     cycle_counts+24,A31
           LDDW    .D1T1   *A31,A9:A8        ; |130| 
   [ A0]   MVKH    .S2     0xc0590000,B5
           MV      .L1     A31,A11           ; |128| 
           ADDU    .L1X    A6,B8,A5:A4       ; |130| 

           ADD     .S1     A5,A7,A3          ; |130| 
||         SUBU    .L1X    B6,A4,A5:A4       ; |130| 

           EXT     .S1     A5,24,24,A30      ; |130| 
||         ADDU    .L1     A8,A4,A5:A4       ; |130| 

           ADD     .L2X    A3,B9,B4          ; |130| 
|| [ A0]   B       .S1     $C$L10            ; |171| 

           SUB     .L2     B7,B4,B4          ; |130| 
           ADD     .L1     A5,A9,A3          ; |130| 

           ADD     .L1X    B4,A30,A5         ; |130| 
|| [ A0]   MV      .L2     B12,B4            ; |173| 

           ADD     .L1     A3,A5,A5          ; |130| 
|| [ A0]   MVKL    .S1     input,A3

   [ A0]   CALL    .S2     gimme_random      ; |173| 
|| [ A0]   MVKH    .S1     input,A3
||         STDW    .D1T1   A5:A4,*A11        ; |130| 
|| [ A0]   ZERO    .L1     A5

           ; BRANCHCC OCCURS {$C$L10}        ; |171| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 6
           CALL    .S1     __c6xabi_fltllif  ; |135| 
           LDDW    .D1T1   *A11,A5:A4        ; |135| 
           ADDKPC  .S2     $C$RL29,B3,3      ; |135| 
$C$RL29:   ; CALL OCCURS {__c6xabi_fltllif} {0}  ; |135| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 16
           ZERO    .L1     A3
           SET     .S1     A3,0x1a,0x1d,A3
           MPYSP   .M1     A3,A4,A3          ; |135| 
           ZERO    .L1     A13
           SET     .S1     A13,0x15,0x1d,A13
           MV      .L1X    B13,A12           ; |135| 
           SPDP    .S1     A3,A5:A4          ; |135| 
           NOP             2
           ADDDP   .L1     A13:A12,A5:A4,A5:A4 ; |135| 
           CALLP   .S2     __c6xabi_fixdlli,B3
$C$RL30:   ; CALL OCCURS {__c6xabi_fixdlli} {0}  ; |135| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 16

           MVK     .S2     24,B10
||         MV      .L2X    A11,B4

           SUB     .L2X    A11,B10,B10
||         MV      .S2     B12,B5            ; |114| 
||         STDW    .D2T1   A5:A4,*B4         ; |135| 

           STDW    .D2T2   B13:B12,*B10      ; |114| 
||         MVC     .S2     B5,TSCL           ; |115| 

           MVC     .S2     TSCL,B4           ; |116| 
           MVC     .S2     TSCH,B5           ; |116| 
           MV      .L1X    B4,A14            ; |116| 

           MV      .L1X    B5,A12            ; |116| 
||         MVC     .S2     TSCL,B5           ; |117| 

           MVC     .S2     TSCH,B4           ; |117| 

           SUBU    .L2X    B5,A14,B7:B6      ; |117| 
||         MVKL    .S1     output,A31

           SUB     .L1X    B4,A12,A4         ; |117| 
||         MVK     .S2     0x80,B31
||         MVKH    .S1     output,A31

           MV      .L1X    B6,A3             ; |117| 
||         EXT     .S2     B7,24,24,B4       ; |117| 
||         STW     .D2T2   B31,*+SP(12)      ; |117| 
||         LDW     .D1T1   *A31,A11

           STW     .D2T1   A3,*+SP(16)       ; |117| 
||         CALL    .S1     sin               ; |185| 

           ADD     .L1X    A4,B4,A15         ; |117| 
||         LDDW    .D1T1   *A10++,A5:A4      ; |185| 

           NOP             3
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L11:    
;          EXCLUSIVE CPU CYCLES: 1
           ADDKPC  .S2     $C$RL31,B3,0      ; |185| 
$C$RL31:   ; CALL OCCURS {sin} {0}           ; |185| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 12
           LDW     .D2T2   *+SP(12),B4       ; |185| 
           STDW    .D1T1   A5:A4,*A11++      ; |185| 
           NOP             3

           SUB     .L1X    B4,1,A0           ; |184| 
||         SUB     .L2     B4,1,B4           ; |184| 

   [ A0]   B       .S1     $C$L11            ; |184| 
||         STW     .D2T2   B4,*+SP(12)       ; |185| 

   [ A0]   CALL    .S1     sin               ; |185| 
   [ A0]   LDDW    .D1T1   *A10++,A5:A4      ; |185| 
           NOP             3
           ; BRANCHCC OCCURS {$C$L11}        ; |184| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 14
           MVC     .S2     TSCL,B6           ; |128| 
           MVC     .S2     TSCH,B4           ; |128| 
           LDW     .D2T1   *+SP(16),A3       ; |128| 
           MV      .L2     B4,B7             ; |128| 
           LDDW    .D2T2   *B10,B5:B4        ; |130| 
           NOP             2
           ADDU    .L1     A3,A14,A5:A4      ; |130| 

           ADD     .S1     A5,A15,A3         ; |130| 
||         SUBU    .L1X    B6,A4,A5:A4       ; |130| 
||         CALL    .S2     __c6xabi_fltllif  ; |130| 

           ADD     .L1     A3,A12,A3         ; |130| 
||         EXT     .S1     A5,24,24,A31      ; |130| 

           ADDU    .L2X    B4,A4,B9:B8       ; |130| 
||         SUB     .L1X    B7,A3,A3          ; |130| 
||         MV      .S2     B11,B4            ; |130| 

           ADD     .L2     B9,B5,B5          ; |130| 
||         ADD     .L1     A3,A31,A3         ; |130| 
||         STDW    .D2T2   B7:B6,*B4         ; |128| 

           MV      .L1X    B8,A4             ; |130| 

           ADD     .L1X    B5,A3,A5          ; |130| 
||         ADDKPC  .S2     $C$RL32,B3,0      ; |130| 

$C$RL32:   ; CALL OCCURS {__c6xabi_fltllif} {0}  ; |130| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 15
           ZERO    .L1     A3
           SET     .S1     A3,0x1a,0x1d,A3
           MPYSP   .M1     A3,A4,A3          ; |130| 
           MV      .L1X    B13,A12           ; |130| 
           NOP             2
           SPDP    .S1     A3,A5:A4          ; |130| 
           NOP             1
           ADDDP   .L1     A13:A12,A5:A4,A5:A4 ; |130| 
           CALLP   .S2     __c6xabi_fixdlli,B3
$C$RL33:   ; CALL OCCURS {__c6xabi_fixdlli} {0}  ; |130| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 13

           ZERO    .L1     A11
||         STDW    .D2T2   B13:B12,*+B10(8)  ; |114| 
||         MV      .L2     B12,B4            ; |114| 

           MVC     .S2     B4,TSCL           ; |115| 
||         SET     .S1     A11,0x1a,0x1d,A11
||         STDW    .D2T1   A5:A4,*B10        ; |130| 

           MVC     .S2     TSCL,B4           ; |116| 
           MVC     .S2     TSCH,B5           ; |116| 
           MVKL    .S1     t_start,A14
           MVKH    .S1     t_start,A14

           MVC     .S2     TSCL,B6           ; |117| 
||         STDW    .D1T2   B5:B4,*A14        ; |116| 

           MVC     .S2     TSCH,B8           ; |117| 

           SUBU    .L2     B6,B4,B7:B6       ; |117| 
||         MVKL    .S2     t_offset,B31

           EXT     .S2     B7,24,24,B4       ; |117| 
||         SUB     .L2     B8,B5,B5          ; |117| 

           MVK     .S2     0x80,B4
||         ADD     .L2     B5,B4,B7          ; |117| 

           MVKH    .S2     t_offset,B31
||         LDW     .D2T2   *+SP(20),B11      ; |117| 
||         MVKL    .S1     input,A10

           MV      .L1X    B4,A12            ; |117| 
||         STDW    .D2T2   B7:B6,*B31        ; |117| 
||         MVKH    .S1     input,A10
||         MV      .D1     A12,A15

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L12:    
;          EXCLUSIVE CPU CYCLES: 11
           LDW     .D1T1   *A10,A3           ; |193| 
           NOP             4

           ADD     .L1     A15,A3,A3         ; |193| 
||         CALL    .S1     sindp             ; |193| 

           LDDW    .D1T1   *A3,A5:A4         ; |193| 
           ADDKPC  .S2     $C$RL34,B3,3      ; |193| 
$C$RL34:   ; CALL OCCURS {sindp} {0}         ; |193| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 7

           LDW     .D2T2   *B11,B4           ; |193| 
||         SUB     .L1     A12,1,A0          ; |192| 
||         SUB     .S1     A12,1,A12         ; |192| 

   [ A0]   BNOP    .S1     $C$L12,3          ; |192| 

           ADD     .L1X    A15,B4,A3         ; |193| 
||         ADD     .S1     8,A15,A15         ; |192| 

           STDW    .D1T1   A5:A4,*A3         ; |193| 
           ; BRANCHCC OCCURS {$C$L12}        ; |192| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 19
           MVC     .S2     TSCL,B6           ; |128| 
           MVC     .S2     TSCH,B5           ; |128| 

           MVKL    .S2     t_offset,B4
||         LDDW    .D1T1   *A14,A7:A6        ; |130| 

           MVKH    .S2     t_offset,B4
           LDDW    .D2T2   *B4,B9:B8         ; |130| 
           MV      .L2     B5,B7             ; |128| 
           LDDW    .D2T2   *+B10(8),B5:B4    ; |130| 
           MVKL    .S2     t_stop,B31
           MVKH    .S2     t_stop,B31
           ADDU    .L1X    B8,A6,A5:A4       ; |130| 
           STDW    .D2T2   B7:B6,*B31        ; |128| 

           ADD     .L1X    A5,B9,A4          ; |130| 
||         SUBU    .L2X    B6,A4,B9:B8       ; |130| 

           ADD     .L1     A4,A7,A3          ; |130| 
||         EXT     .S2     B9,24,24,B16      ; |130| 
||         ADDU    .L2     B4,B8,B9:B8       ; |130| 

           CALL    .S1     __c6xabi_fltllif  ; |130| 
||         ADD     .L2     B9,B5,B5          ; |130| 

           SUB     .L2X    B7,A3,B4          ; |130| 
||         MV      .L1X    B8,A4             ; |130| 

           ADD     .L2     B4,B16,B8         ; |130| 
           ADD     .L2     B5,B8,B4          ; |130| 
           ADDKPC  .S2     $C$RL35,B3,0      ; |130| 
           MV      .L1X    B4,A5             ; |130| 
$C$RL35:   ; CALL OCCURS {__c6xabi_fltllif} {0}  ; |130| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 13
           MPYSP   .M1     A11,A4,A3         ; |130| 
           MV      .L1X    B13,A12           ; |130| 
           NOP             2
           SPDP    .S1     A3,A5:A4          ; |130| 
           NOP             1
           ADDDP   .L1     A13:A12,A5:A4,A5:A4 ; |130| 
           CALLP   .S2     __c6xabi_fixdlli,B3
$C$RL36:   ; CALL OCCURS {__c6xabi_fixdlli} {0}  ; |130| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 15
           STDW    .D2T2   B13:B12,*+B10(16) ; |114| 

           ADDAD   .D2     B10,1,B11
||         MV      .L2     B12,B4            ; |114| 

           STDW    .D2T1   A5:A4,*B11        ; |130| 
||         MVC     .S2     B4,TSCL           ; |115| 

           MVC     .S2     TSCL,B8           ; |116| 
           MVC     .S2     TSCH,B4           ; |116| 
           MV      .L2     B4,B9             ; |116| 

           STDW    .D1T2   B9:B8,*A14        ; |116| 
||         MVC     .S2     TSCL,B5           ; |117| 

           MVC     .S2     TSCH,B4           ; |117| 
           LDW     .D2T1   *+SP(24),A3       ; |117| 
           SUBU    .L2     B5,B8,B7:B6       ; |117| 
           MVKL    .S2     t_offset,B31

           SUB     .L2     B4,B9,B5          ; |117| 
||         EXT     .S2     B7,24,24,B4       ; |117| 

           ADD     .L2     B5,B4,B7          ; |117| 
||         MVK     .S2     0x80,B4

           MVKH    .S2     t_offset,B31
||         STW     .D2T1   A3,*+SP(12)

           STDW    .D2T2   B7:B6,*B31        ; |117| 
||         MV      .L1     A12,A15
||         MV      .S1X    B4,A12            ; |117| 

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Loop contains a call
;*      Disqualified loop: Loop contains non-pipelinable instructions
;*----------------------------------------------------------------------------*
$C$L13:    
;          EXCLUSIVE CPU CYCLES: 11
           LDW     .D1T1   *A10,A3           ; |201| 
           NOP             4

           ADD     .L1     A15,A3,A3         ; |201| 
||         CALL    .S1     sindp_c           ; |201| 

           LDDW    .D1T1   *A3,A5:A4         ; |201| 
           ADDKPC  .S2     $C$RL37,B3,3      ; |201| 
$C$RL37:   ; CALL OCCURS {sindp_c} {0}       ; |201| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 12
           LDW     .D2T2   *+SP(12),B4       ; |201| 
           SUB     .L1     A12,1,A0          ; |200| 
           SUB     .L1     A12,1,A12         ; |200| 
           NOP             2
           LDW     .D2T2   *B4,B4            ; |201| 
   [ A0]   BNOP    .S1     $C$L13,3          ; |200| 

           ADD     .L1X    A15,B4,A3         ; |201| 
||         ADD     .S1     8,A15,A15         ; |200| 

           STDW    .D1T1   A5:A4,*A3         ; |201| 
           ; BRANCHCC OCCURS {$C$L13}        ; |200| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 17
           MVC     .S2     TSCL,B6           ; |128| 
           MVC     .S2     TSCH,B4           ; |128| 

           MVKL    .S2     t_offset,B5
||         LDDW    .D1T1   *A14,A5:A4        ; |130| 
||         MV      .L2     B4,B7             ; |128| 
||         MV      .D2     B10,B4            ; |130| 

           MVKH    .S2     t_offset,B5
           LDDW    .D2T2   *B5,B17:B16       ; |130| 
           LDDW    .D2T2   *+B4(16),B9:B8    ; |130| 
           MVKL    .S2     t_stop,B31
           MVKH    .S2     t_stop,B31
           STDW    .D2T2   B7:B6,*B31        ; |128| 
           ADDU    .L2X    B16,A4,B5:B4      ; |130| 

           ADD     .S2     B5,B17,B16        ; |130| 
||         SUBU    .L2     B6,B4,B5:B4       ; |130| 

           ADD     .L2X    B16,A5,B16        ; |130| 
||         CALL    .S1     __c6xabi_fltllif  ; |130| 

           SUB     .D2     B7,B16,B8         ; |130| 
||         ADDU    .L2     B8,B4,B5:B4       ; |130| 
||         EXT     .S2     B5,24,24,B16      ; |130| 

           ADD     .L2     B5,B9,B5          ; |130| 
||         ADD     .S2     B8,B16,B8         ; |130| 

           MV      .L1X    B4,A4             ; |130| 
||         ADD     .L2     B5,B8,B4          ; |130| 

           MV      .L1X    B10,A15           ; |130| 

           MV      .L1X    B4,A5             ; |130| 
||         ADDKPC  .S2     $C$RL38,B3,0      ; |130| 

$C$RL38:   ; CALL OCCURS {__c6xabi_fltllif} {0}  ; |130| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 107
           MPYSP   .M1     A11,A4,A3         ; |130| 
           MV      .L1X    B13,A12           ; |130| 
           NOP             3
           SPDP    .S2X    A3,B5:B4          ; |130| 
           NOP             2
           ADDDP   .L1X    A13:A12,B5:B4,A5:A4 ; |130| 
           CALLP   .S2     __c6xabi_fixdlli,B3
$C$RL39:   ; CALL OCCURS {__c6xabi_fixdlli} {0}  ; |130| 

           ADDAD   .D2     B11,1,B10
||         MV      .L2X    A15,B4

           MV      .L2     B12,B31           ; |114| 
||         STDW    .D2T2   B13:B12,*+B4(32)  ; |114| 

           STDW    .D2T1   A5:A4,*B10        ; |130| 
||         MVC     .S2     B31,TSCL          ; |115| 

           MVC     .S2     TSCL,B4           ; |116| 
           MVC     .S2     TSCH,B5           ; |116| 

           STDW    .D1T2   B5:B4,*A14        ; |116| 
||         MVC     .S2     TSCL,B6           ; |117| 

           MVC     .S2     TSCH,B8           ; |117| 

           SUBU    .L2     B6,B4,B7:B6       ; |117| 
||         MVKL    .S2     t_offset,B11

           EXT     .S2     B7,24,24,B5       ; |117| 
||         SUB     .L2     B8,B5,B7          ; |117| 

           MVKL    .S1     output,A15
||         ADD     .L2     B7,B5,B5          ; |117| 
||         MV      .D2     B6,B4             ; |117| 
||         MVKH    .S2     t_offset,B11

           MVKH    .S1     output,A15
||         MV      .L2X    A10,B9            ; |208| 
||         STDW    .D2T2   B5:B4,*B11        ; |117| 

           CALLP   .S2     sindp_v,B3
||         LDW     .D1T2   *+A15(16),B4      ; |208| 
||         LDW     .D2T1   *B9,A4            ; |208| 
||         MVK     .S1     0x80,A6           ; |208| 

$C$RL40:   ; CALL OCCURS {sindp_v} {0}       ; |208| 
           MVC     .S2     TSCL,B8           ; |128| 
           MVC     .S2     TSCH,B4           ; |128| 

           LDDW    .D1T1   *A14,A17:A16      ; |130| 
||         LDDW    .D2T2   *B11,B7:B6        ; |130| 

           MVK     .S1     16,A10
           ADD     .L1X    B10,A10,A10
           MVKL    .S1     t_stop,A3

           LDDW    .D1T1   *A10,A9:A8        ; |130| 
||         MVKH    .S1     t_stop,A3

           ADDU    .L1X    B6,A16,A7:A6      ; |130| 

           MV      .L2     B4,B9             ; |128| 
||         MV      .S2X    A3,B4             ; |128| 
||         ADD     .L1X    A7,B7,A3          ; |130| 

           SUBU    .L1X    B8,A6,A5:A4       ; |130| 

           ADD     .L1     A3,A17,A3         ; |130| 
||         EXT     .S1     A5,24,24,A6       ; |130| 

           SUB     .S1X    B9,A3,A3          ; |130| 
||         ADDU    .L1     A8,A4,A5:A4       ; |130| 

           ADD     .L1     A5,A9,A3          ; |130| 
||         ADD     .S1     A3,A6,A5          ; |130| 

           CALLP   .S2     __c6xabi_fltllif,B3
||         ADD     .L1     A3,A5,A5          ; |130| 
||         STDW    .D2T2   B9:B8,*B4         ; |128| 

$C$RL41:   ; CALL OCCURS {__c6xabi_fltllif} {0}  ; |130| 
           MPYSP   .M1     A11,A4,A3         ; |130| 
           NOP             3
           SPDP    .S1     A3,A5:A4          ; |130| 
           NOP             1
           ADDDP   .L1     A13:A12,A5:A4,A5:A4 ; |130| 
           CALLP   .S2     __c6xabi_fixdlli,B3
$C$RL42:   ; CALL OCCURS {__c6xabi_fixdlli} {0}  ; |130| 
           MVKL    .S2     0x3ddb7cdf,B7
           MVKL    .S2     0xd9d7bdbb,B6

           STDW    .D1T1   A5:A4,*A10        ; |130| 
||         MVKH    .S2     0x3ddb7cdf,B7

           LDW     .D1T2   *+A15(4),B4       ; |66| 
||         MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D1T1   *A15,A4           ; |66| 
||         MVK     .S1     0x80,A6           ; |66| 
||         MVK     .L1     0x1,A8            ; |66| 

$C$RL43:   ; CALL OCCURS {isequal} {0}       ; |66| 
           MVKL    .S2     0x3ddb7cdf,B7
           MVKL    .S2     0xd9d7bdbb,B6
           MVKL    .S2     fcn_pass,B10
           MVKH    .S2     0x3ddb7cdf,B7
           MVKH    .S2     0xd9d7bdbb,B6

           MV      .L1     A4,A3             ; |66| 
||         LDW     .D1T1   *A15,A4           ; |67| 
||         MVKH    .S2     fcn_pass,B10

           CALLP   .S2     isequal,B3
||         LDW     .D1T2   *+A15(8),B4       ; |67| 
||         STW     .D2T1   A3,*+B10(4)       ; |66| 
||         MVK     .S1     0x80,A6           ; |67| 
||         MVK     .L1     0x1,A8            ; |67| 

$C$RL44:   ; CALL OCCURS {isequal} {0}       ; |67| 
           MVKL    .S2     0x3ddb7cdf,B7
           MVKL    .S2     0xd9d7bdbb,B6
           MVKH    .S2     0x3ddb7cdf,B7

           STW     .D2T1   A4,*+B10(8)       ; |67| 
||         LDW     .D1T2   *+A15(12),B4      ; |68| 
||         MVKH    .S2     0xd9d7bdbb,B6

           CALLP   .S2     isequal,B3
||         LDW     .D1T1   *A15,A4           ; |68| 
||         MVK     .S1     0x80,A6           ; |68| 
||         MVK     .L1     0x1,A8            ; |68| 

$C$RL45:   ; CALL OCCURS {isequal} {0}       ; |68| 
           LDW     .D2T1   *+SP(100),A3      ; |68| 
           MVKL    .S2     0xd9d7bdbb,B6
           MVKL    .S2     0x3ddb7cdf,B7

           STW     .D2T1   A4,*+B10(12)      ; |68| 
||         MVKH    .S2     0xd9d7bdbb,B6

           LDW     .D1T1   *A15,A4           ; |69| 
||         MVKH    .S2     0x3ddb7cdf,B7

           CALLP   .S2     isequal,B3
||         LDW     .D1T2   *A3,B4            ; |69| 
||         MVK     .S1     0x80,A6           ; |69| 
||         MVK     .L1     0x1,A8            ; |69| 

$C$RL46:   ; CALL OCCURS {isequal} {0}       ; |69| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 16
           LDW     .D2T2   *+SP(104),B5      ; |69| 
           MV      .L2X    A4,B4             ; |69| 
           STW     .D2T2   B4,*+B10(16)      ; |69| 
           NOP             2
           LDW     .D2T2   *B5,B0            ; |72| 
           LDW     .D2T2   *+SP(108),B5
           NOP             3

   [!B0]   B       .S1     $C$L14            ; |72| 
||         MV      .L2     B0,B1             ; guard predicate rewrite

   [ B0]   LDW     .D2T2   *B5,B0            ; |72| 
   [ B1]   ADD     .L2     4,B5,B5
           NOP             3
           ; BRANCHCC OCCURS {$C$L14}        ; |72| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 14
   [ B0]   LDW     .D2T2   *B5,B5            ; |72| 
           CMPEQ   .L2     B4,0,B6           ; |72| 
           XOR     .L2     1,B6,B31          ; |72| 
           MVK     .L1     0x1,A3            ; |72| 
           NOP             1
   [ B0]   CMPEQ   .L2     B5,0,B4           ; |72| 
   [ B0]   XOR     .L2     1,B4,B4           ; |72| 

   [ B0]   AND     .L2     B31,B4,B0         ; |72| 
|| [!B0]   ZERO    .S2     B0                ; |72| 

   [ B0]   B       .S1     $C$L15            ; |72| 
   [ B0]   CALL    .S1     print_test_results ; |214| 
   [ B0]   MVKL    .S1     all_pass,A4
   [ B0]   MVKH    .S1     all_pass,A4
           NOP             2
           ; BRANCHCC OCCURS {$C$L15}        ; |72| 
;** --------------------------------------------------------------------------*
$C$L14:    
;          EXCLUSIVE CPU CYCLES: 5
           CALL    .S1     print_test_results ; |214| 
           MVKL    .S1     all_pass,A4
           MVKH    .S1     all_pass,A4
           MV      .L1     A12,A3
           NOP             1
;** --------------------------------------------------------------------------*
$C$L15:    
;          EXCLUSIVE CPU CYCLES: 1

           STW     .D1T1   A3,*A4            ; |72| 
||         MVK     .L1     0x2,A4            ; |214| 
||         ADDKPC  .S2     $C$RL47,B3,0      ; |214| 

$C$RL47:   ; CALL OCCURS {print_test_results} {0}  ; |214| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 30
           MVKL    .S2     $C$SL2+0,B4
           MVKH    .S2     $C$SL2+0,B4

           CALLP   .S2     printf,B3
||         STW     .D2T2   B4,*+SP(4)        ; |215| 

$C$RL48:   ; CALL OCCURS {printf} {0}        ; |215| 
           MVKL    .S2     $C$SL3+0,B4
           MVKH    .S2     $C$SL3+0,B4

           CALLP   .S2     printf,B3
||         STW     .D2T2   B4,*+SP(4)        ; |215| 

$C$RL49:   ; CALL OCCURS {printf} {0}        ; |215| 
           MVKL    .S1     $C$SL1+0,A10
           MVKH    .S1     $C$SL1+0,A10

           CALLP   .S2     print_profile_results,B3
||         MV      .L1     A10,A4            ; |222| 

$C$RL50:   ; CALL OCCURS {print_profile_results} {0}  ; |222| 

           CALLP   .S2     print_memory_results,B3
||         MV      .L1     A10,A4            ; |225| 

$C$RL51:   ; CALL OCCURS {print_memory_results} {0}  ; |225| 
;** --------------------------------------------------------------------------*
;          EXCLUSIVE CPU CYCLES: 12
           ADDK    .S2     168,SP            ; |226| 
           LDW     .D2T2   *++SP(8),B3       ; |226| 
           LDDW    .D2T1   *++SP,A13:A12     ; |226| 
           LDDW    .D2T1   *++SP,A15:A14     ; |226| 
           LDDW    .D2T2   *++SP,B11:B10     ; |226| 
           LDDW    .D2T2   *++SP,B13:B12     ; |226| 

           LDW     .D2T1   *++SP(8),A10      ; |226| 
||         RET     .S2     B3                ; |226| 

           LDW     .D2T1   *++SP(8),A11      ; |226| 
           NOP             4
           ; BRANCH OCCURS {B3}              ; |226| 
;******************************************************************************
;* STRINGS                                                                    *
;******************************************************************************
	.sect	".const:.string"
$C$SL1:	.string	"sinDP",0
$C$SL2:	.string	"----------------------------------------",0
$C$SL3:	.string	"----------------------------------------",10,0
;*****************************************************************************
;* UNDEFINED EXTERNAL REFERENCES                                             *
;*****************************************************************************
	.global	printf
	.global	sin
	.global	driver_init
	.global	print_profile_results
	.global	print_memory_results
	.global	print_test_results
	.global	gimme_random
	.global	isequal
	.global	sindp
	.global	sindp_c
	.global	sindp_v
	.global	t_start
	.global	t_stop
	.global	t_offset
	.global	cycle_counts
	.global	fcn_pass
	.global	all_pass
	.global	a_ext
	.global	a_sc
	.global	output
	.global	input
	.global	__c6xabi_fltllif
	.global	__c6xabi_fixdlli

;******************************************************************************
;* BUILD ATTRIBUTES                                                           *
;******************************************************************************
	.battr "TI", Tag_File, 1, Tag_Long_Precision_Bits(2)
	.battr "TI", Tag_File, 1, Tag_Bitfield_layout(2)
	.battr "TI", Tag_File, 1, Tag_ABI_enum_size(3)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_wchar_t(1)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_array_object_alignment(0)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_array_object_align_expected(0)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_PIC(0)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_PID(0)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_DSBT(0)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_stack_align_needed(0)
	.battr "c6xabi", Tag_File, 1, Tag_ABI_stack_align_preserved(0)
	.battr "TI", Tag_File, 1, Tag_Tramps_Use_SOC(1)
