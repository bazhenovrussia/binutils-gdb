/* Parameters for execution on any Hewlett-Packard PA-RISC machine.
   Copyright 1986, 1987, 1989, 1990, 1991, 1992, 1993, 1995, 1999, 2000
   Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

struct type;
struct value;
struct frame_info;

/* PA 64-bit specific definitions.  Override those which are in
   tm-hppa.h */

/* jimb: this must go.  I'm just using it to disable code I haven't
   gotten working yet.  */
#define GDB_TARGET_IS_HPPA_20W

/* NOTE: cagney/2003-07-27: Using CC='cc +DA2.0W -Ae' configure
   hppa64-hp-hpux11.00; GDB managed to build / start / break main /
   run with multi-arch enabled.  Not sure about much else as there
   appears to be an unrelated problem in the SOM symbol table reader
   causing GDB to lose line number information.  Since prior to this
   switch and a other recent tweaks, 64 bit PA hadn't been building
   for some months, this is probably the lesser of several evils.  */
#define GDB_MULTI_ARCH GDB_MULTI_ARCH_PARTIAL

#include "pa/tm-hppah.h"

#define HPUX_1100 1

/* The low two bits of the IA are the privilege level of the instruction.  */
#define ADDR_BITS_REMOVE(addr) ((CORE_ADDR)addr & (CORE_ADDR)~3)

/* Say how long (ordinary) registers are.  This is used in
   push_word and a few other places, but REGISTER_RAW_SIZE is
   the real way to know how big a register is.  */

#undef DEPRECATED_REGISTER_SIZE
#define DEPRECATED_REGISTER_SIZE 8

/* Number of bytes of storage in the actual machine representation
   for register N.  On the PA-RISC 2.0, all regs are 8 bytes, including
   the FP registers (they're accessed as two 4 byte halves).  */

#undef REGISTER_RAW_SIZE
#define REGISTER_RAW_SIZE(N) 8

/* Largest value REGISTER_RAW_SIZE can have.  */

#undef DEPRECATED_MAX_REGISTER_RAW_SIZE
#define DEPRECATED_MAX_REGISTER_RAW_SIZE 8

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */

#undef DEPRECATED_REGISTER_BYTES
#define DEPRECATED_REGISTER_BYTES (NUM_REGS * 8)

/* Index within `registers' of the first byte of the space for
   register N.  */

#undef REGISTER_BYTE
#define REGISTER_BYTE(N) ((N) * 8)

#undef REGISTER_VIRTUAL_TYPE
#define REGISTER_VIRTUAL_TYPE(N) \
 ((N) < FP4_REGNUM ? builtin_type_unsigned_long_long : builtin_type_double)


/* Number of machine registers */
#undef NUM_REGS
#define NUM_REGS 96

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.
   They are in rows of eight entries  */
extern const char *hppa64_register_name (int i);
#define REGISTER_NAME hppa64_register_name

#undef FP0_REGNUM
#undef FP4_REGNUM
#define FP0_REGNUM 64		/* floating point reg. 0 (fspr)*/
#define FP4_REGNUM 68

/* Redefine some target bit sizes from the default.  */

/* Number of bits in a long or unsigned long for the target machine. */

#define TARGET_LONG_BIT 64

/* Number of bits in a long long or unsigned long long for the 
   target machine.  */

#define TARGET_LONG_LONG_BIT 64

/* Number of bits in a pointer for the target machine */

#define TARGET_PTR_BIT 64

/* Argument Pointer Register */
#define AP_REGNUM 29

#define DP_REGNUM 27

#define FP5_REGNUM 70

#define SR5_REGNUM 48

/* We access locals from SP. This may not work for frames which call
   alloca; for those, we may need to consult unwind tables.
   jimb: FIXME.  */
#undef DEPRECATED_FRAME_LOCALS_ADDRESS
#define DEPRECATED_FRAME_LOCALS_ADDRESS(fi) (get_frame_base (fi))

/* For a number of horrible reasons we may have to adjust the location
   of variables on the stack.  Ugh.  jimb: why? */
#define HPREAD_ADJUST_STACK_ADDRESS(ADDR) hpread_adjust_stack_address(ADDR)

extern int hpread_adjust_stack_address (CORE_ADDR);


/* jimb: omitted dynamic linking stuff here */

/* This sequence of words is the instructions

; Call stack frame has already been built by gdb. Since we could be calling
; a varargs function, and we do not have the benefit of a stub to put things in
; the right place, we load the first 8 word of arguments into both the general
; and fp registers.
call_dummy
	nop
        copy %r4,%r29
        copy %r5,%r22
        copy %r6,%r27
        fldd -64(0,%r29),%fr4
        fldd -56(0,%r29),%fr5
        fldd -48(0,%r29),%fr6
        fldd -40(0,%r29),%fr7
        fldd -32(0,%r29),%fr8
        fldd -24(0,%r29),%fr9
        fldd -16(0,%r29),%fr10
        fldd -8(0,%r29),%fr11
        copy %r22,%r1
        ldd -64(%r29), %r26
        ldd -56(%r29), %r25
        ldd -48(%r29), %r24
        ldd -40(%r29), %r23
        ldd -32(%r29), %r22
        ldd -24(%r29), %r21
        ldd -16(%r29), %r20
        bve,l (%r1),%r2
        ldd -8(%r29), %r19
        break 4, 8
	mtsp %r21, %sr0
	ble 0(%sr0, %r22)
        nop
*/

/* Call dummys are sized and written out in word sized hunks.  So we have
   to pack the instructions into words.  Ugh.  */
#undef CALL_DUMMY
#define CALL_DUMMY {0x08000240349d0000LL, 0x34b6000034db0000LL, \
                    0x53a43f8353a53f93LL, 0x53a63fa353a73fb3LL,\
                    0x53a83fc353a93fd3LL, 0x2fa1100a2fb1100bLL,\
                    0x36c1000053ba3f81LL, 0x53b93f9153b83fa1LL,\
                    0x53b73fb153b63fc1LL, 0x53b53fd10fa110d4LL,\
                    0xe820f0000fb110d3LL, 0x0001000400151820LL,\
                    0xe6c0000008000240LL}

#define DEPRECATED_CALL_DUMMY_BREAKPOINT_OFFSET 22 * 4

/* DEPRECATED_CALL_DUMMY_LENGTH is computed based on the size of a
   word on the target machine, not the size of an instruction.  Since
   a word on this target holds two instructions we have to divide the
   instruction size by two to get the word size of the dummy.  */
#undef DEPRECATED_CALL_DUMMY_LENGTH
#define DEPRECATED_CALL_DUMMY_LENGTH (INSTRUCTION_SIZE * 26 / 2)

/* The PA64 ABI mandates a 16 byte stack alignment.  */
#undef STACK_ALIGN
#define STACK_ALIGN(arg) ( ((arg)%16) ? (((arg)+15)&-16) : (arg))

/* The PA64 ABI reserves 64 bytes of stack space for outgoing register
   parameters.  */
#undef REG_PARM_STACK_SPACE
#define REG_PARM_STACK_SPACE 64

/* Use the 64-bit calling conventions designed for the PA2.0 in wide mode.  */
#define PA20W_CALLING_CONVENTIONS

#undef FUNC_LDIL_OFFSET
#undef FUNC_LDO_OFFSET
#undef SR4EXPORT_LDIL_OFFSET
#undef SR4EXPORT_LDO_OFFSET

#undef REG_STRUCT_HAS_ADDR

#undef DEPRECATED_EXTRACT_RETURN_VALUE
/* RM: floats are returned in FR4R, doubles in FR4
 *     integral values are in r28, padded on the left 
 *     aggregates less that 65 bits are in r28, right padded 
 *     aggregates upto 128 bits are in r28 and r29, right padded
 */ 
#define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  { \
    if (TYPE_CODE (TYPE) == TYPE_CODE_FLT) \
      memcpy ((VALBUF), \
	      ((char *)(REGBUF)) + REGISTER_BYTE (FP4_REGNUM) + \
              (DEPRECATED_REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
	      TYPE_LENGTH (TYPE)); \
    else if  (is_integral_type(TYPE))   \
       memcpy ((VALBUF), \
               (char *)(REGBUF) + REGISTER_BYTE (28) + \
               (DEPRECATED_REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 8)   \
       memcpy ((VALBUF), \
               (char *)(REGBUF) + REGISTER_BYTE (28), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 16)   \
      { \
        memcpy ((VALBUF), \
                (char *)(REGBUF) + REGISTER_BYTE (28), \
                8); \
        memcpy (((char *) VALBUF + 8), \
                (char *)(REGBUF) + REGISTER_BYTE (29), \
                TYPE_LENGTH (TYPE) - 8); \
       } \
  }

/* RM: struct upto 128 bits are returned in registers */
#undef USE_STRUCT_CONVENTION
#define USE_STRUCT_CONVENTION(gcc_p, value_type)\
  (TYPE_LENGTH (value_type) > 16)                

/* RM: for return command */
#undef DEPRECATED_STORE_RETURN_VALUE
#define DEPRECATED_STORE_RETURN_VALUE(TYPE,VALBUF) \
  { \
    if (TYPE_CODE (TYPE) == TYPE_CODE_FLT) \
      deprecated_write_register_bytes \
	      (REGISTER_BYTE (FP4_REGNUM) + \
              (DEPRECATED_REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
              (VALBUF), \
	      TYPE_LENGTH (TYPE)); \
    else if (is_integral_type(TYPE))   \
       deprecated_write_register_bytes \
              (REGISTER_BYTE (28) + \
                 (DEPRECATED_REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
               (VALBUF), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 8)   \
       deprecated_write_register_bytes \
             ( REGISTER_BYTE (28), \
               (VALBUF), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 16)   \
      { \
        deprecated_write_register_bytes \
               (REGISTER_BYTE (28), \
                (VALBUF), \
                8); \
        deprecated_write_register_bytes \
               (REGISTER_BYTE (29), \
                ((char *) VALBUF + 8), \
                TYPE_LENGTH (TYPE) - 8); \
       } \
  }

/* RM: these are the PA64 equivalents of the macros in tm-hppah.h --
 * see comments there.  For PA64, the save_state structure is at an
 * offset of 24 32-bit words from the sigcontext structure. The 64 bit
 * general registers are at an offset of 640 bytes from the beginning of the
 * save_state structure, and the floating pointer register are at an offset
 * of 256 bytes from the beginning of the save_state structure.
 */
#undef FRAME_SAVED_PC_IN_SIGTRAMP
#define FRAME_SAVED_PC_IN_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer (get_frame_base (FRAME) + (24 * 4) + 640 + (33 * 8), 8); \
}

#undef FRAME_BASE_BEFORE_SIGTRAMP
#define FRAME_BASE_BEFORE_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer (get_frame_base (FRAME) + (24 * 4) + 640 + (30 * 8), 8); \
}

#undef FRAME_FIND_SAVED_REGS_IN_SIGTRAMP
#define FRAME_FIND_SAVED_REGS_IN_SIGTRAMP(FRAME, FSR) \
{ \
  int i; \
  CORE_ADDR TMP1, TMP2; \
  TMP1 = get_frame_base (FRAME) + (24 * 4) + 640; \
  TMP2 = get_frame_base (FRAME) + (24 * 4) + 256; \
  for (i = 0; i < NUM_REGS; i++) \
    { \
      if (i == SP_REGNUM) \
        (FSR)[SP_REGNUM] = read_memory_integer (TMP1 + SP_REGNUM * 8, 8); \
      else if (i >= FP0_REGNUM) \
        (FSR)[i] = TMP2 + (i - FP0_REGNUM) * 8; \
      else \
        (FSR)[i] = TMP1 + i * 8; \
    } \
}

/* jimb: omitted purify call support */
