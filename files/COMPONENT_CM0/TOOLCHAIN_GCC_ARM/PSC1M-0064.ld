/**
 * @file PSC1M-0064.ld
 *
 * @cond
 *****************************************************************************
 * Linker file for the GNU C Compiler
 * Supported devices: PSC1M2DBABQ0AT
 *                    PSC1M2DBLEQ0AT
 *                    PSC1M2DBLGQ0AT
 *                    PSC1M2DBTBQ0AT
 *                    PSC1M3DBABQ0AT
 *                    PSC1M3DBLGQ0AT
 *
 * \copyright
 * (c) 2016-2026, Infineon Technologies AG or an affiliate of
 * Infineon Technologies AG.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************
 * @endcond 
 *
 */

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
GROUP(-lgcc -lc -lnosys)
ENTRY(Reset_Handler)

MEMORY
{
    FLASH(RX)    : ORIGIN = 0x10001000, LENGTH = 0x10000
    SRAM(!RX)    : ORIGIN = 0x20000000, LENGTH = 0x3C00
    RAM_CODE(RX) : ORIGIN = 0x20003C00, LENGTH = 0x400
}

stack_size = DEFINED(stack_size) ? stack_size : 1024;
no_init_size = 4;

SECTIONS
{
    /* TEXT section */

    .text : 
    {
      sText = .;
      KEEP(*(.reset));
      *(.text .text.* .gnu.linkonce.t.*);

      /* C++ Support */
      KEEP(*(.init))
      KEEP(*(.fini))

      /* .ctors */
      *crtbegin.o(.ctors)
      *crtbegin?.o(.ctors)
      *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
      *(SORT(.ctors.*))
      *(.ctors)

      /* .dtors */
      *crtbegin.o(.dtors)
      *crtbegin?.o(.dtors)
      *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
      *(SORT(.dtors.*))
      *(.dtors)

      *(.rodata .rodata.*)
      *(.gnu.linkonce.r*)

      *(vtable)        

      . = ALIGN(4);
    } > FLASH

    .eh_frame_hdr : ALIGN (4)
    {
      KEEP (*(.eh_frame_hdr))
    } > FLASH

    .eh_frame : ALIGN (4)
    {
      KEEP (*(.eh_frame))
    } > FLASH

    /* Exception handling, exidx needs a dedicated section */
    .ARM.extab : ALIGN(4)
    {
      *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    . = ALIGN(4);
    __exidx_start = .;
    .ARM.exidx : ALIGN(4)
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;
    . = ALIGN(4);

    /* DSRAM layout (Lowest to highest)*/
    /* Veneer <-> Stack <-> DATA <-> BSS <-> HEAP <-> RAM_CODE */

    .VENEER_Code ABSOLUTE(0x2000000C):
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        VeneerStart = .;
        KEEP(*(.Psc1mVeneerCode));
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        VeneerEnd = .;
    } > SRAM AT > FLASH
    eROData = LOADADDR (.VENEER_Code);
    VeneerSize = ABSOLUTE(VeneerEnd) - ABSOLUTE(VeneerStart);

    /* Dummy section for stack */
    Stack (NOLOAD) : AT(0)
    {
        . = ALIGN(8);
        . = . + stack_size;
        __initial_sp = .;
    } > SRAM

     /* Standard DATA and user defined DATA/BSS/CONST sections */
    .data :
    {
      . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
      __data_start = .;
      * (.data);
      * (.data*);
      *(*.data);
      *(.gnu.linkonce.d*)
      
      . = ALIGN(4);
      /* preinit data */
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP(*(.preinit_array))
      PROVIDE_HIDDEN (__preinit_array_end = .);

      . = ALIGN(4);
      /* init data */
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP(*(SORT(.init_array.*)))
      KEEP(*(.init_array))
      PROVIDE_HIDDEN (__init_array_end = .);


      . = ALIGN(4);
      /* finit data */
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP(*(SORT(.fini_array.*)))
      KEEP(*(.fini_array))
      PROVIDE_HIDDEN (__fini_array_end = .);

      . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
      __data_end = .;
    } > SRAM AT > FLASH
    DataLoadAddr = LOADADDR (.data);
    __data_size = __data_end - __data_start;

    .ram_code :
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __ram_code_start = .;
        /* functions with __attribute__ ((section (".ram_code")))*/
        *(.ram_code)   
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __ram_code_end = .;
    } > RAM_CODE AT > FLASH
    __ram_code_load = LOADADDR (.ram_code);
    __ram_code_size = __ram_code_end - __ram_code_start;
    
    __text_size = (__exidx_end - sText) + VeneerSize + __data_size + __ram_code_size;
    eText = sText + __text_size;

    /* BSS section */
    .bss (NOLOAD) :
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __bss_start = .;
        * (.bss);
        * (.bss*);
        * (COMMON);
        *(.gnu.linkonce.b*)
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __bss_end = .;
        . = ALIGN(8);
        Heap_Bank1_Start = .;
    } > SRAM
    __bss_size = __bss_end - __bss_start;
    
    /* .no_init section contains SystemCoreClock. See system.c file */
    .no_init ORIGIN(SRAM) + LENGTH(SRAM) - no_init_size (NOLOAD) : 
    {
        Heap_Bank1_End = .;
        * (.no_init);
    } > SRAM
    
    /* Heap - Bank1*/
    Heap_Bank1_Size  = Heap_Bank1_End - Heap_Bank1_Start;

    ASSERT(Heap_Bank1_Start <= Heap_Bank1_End, "region SRAM overflowed no_init section")

    /DISCARD/ :
    {
        *(.comment)
    }

    .stab              0 (NOLOAD) : { *(.stab) }
    .stabstr           0 (NOLOAD) : { *(.stabstr) }

    /* DWARF 1 */
    .debug             0 : { *(.debug) }
    .line              0 : { *(.line) }

    /* GNU DWARF 1 extensions */
    .debug_srcinfo     0 : { *(.debug_srcinfo) }
    .debug_sfnames     0 : { *(.debug_sfnames) }

    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges     0 : { *(.debug_aranges) }
    .debug_pubnames    0 : { *(.debug_pubnames) }
    .debug_pubtypes    0 : { *(.debug_pubtypes) }

    /* DWARF 2 */
    .debug_info        0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev      0 : { *(.debug_abbrev) }
    .debug_line        0 : { *(.debug_line) }
    .debug_frame       0 : { *(.debug_frame) }
    .debug_str         0 : { *(.debug_str) }
    .debug_loc         0 : { *(.debug_loc) }
    .debug_macinfo     0 : { *(.debug_macinfo) }

    /* DWARF 2.1 */
    .debug_ranges      0 : { *(.debug_ranges) }

    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames   0 : { *(.debug_weaknames) }
    .debug_funcnames   0 : { *(.debug_funcnames) }
    .debug_typenames   0 : { *(.debug_typenames) }
    .debug_varnames    0 : { *(.debug_varnames) }

    /* Build attributes */
    .build_attributes  0 : { *(.ARM.attributes) }
}
