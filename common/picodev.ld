




MEMORY
{
    /* program memory, first 64kB of OCM */
    m_prog      (rx)    : ORIGIN = 0x00000000, LENGTH = 128k
    /* private memory for each core, 32kB of OCM, address translation in hardware */
    m_data      (rw)    : ORIGIN = 0xFF000000, LENGTH = 64k
    /* shared memory, first 63MB of DDR RAM */
    m_shared    (rw)    : ORIGIN = 0x00100000, LENGTH = 63M
}

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(__reset)

SECTIONS
{
    /*--------------------------------------------------------------------*/
    /* Code and read-only segment                                         */
    /*--------------------------------------------------------------------*/

    /* Begining of code and text segment */
    . = ORIGIN(m_prog);
    _ftext = .;
    PROVIDE( eprol = . );
    
    /* text: Program code section */
    .text : 
    {
		KEEP(*(.text.reset))
		. = ALIGN(16);
		KEEP(*(.text.irq))
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
    } > m_prog

    /* init: Code to execute before main (called by crt0.S) */
    .init : 
    {
        KEEP( *(.init) )
    } > m_prog
    
    /* fini: Code to execute after main (called by crt0.S) */
    .fini : 
    {
        KEEP( *(.fini) )
    } > m_prog
    
    /* rodata: Read-only data */
    .rodata : 
    {
        *(.rdata)
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
    } > m_prog
    
    /*--------------------------------------------------------------------*/
    /* Global constructor/destructor segement                             */
    /*--------------------------------------------------------------------*/

    .preinit_array     :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > m_prog
    
    .init_array     :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array ))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > m_prog
    
    .fini_array     :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array ))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > m_prog

    /*--------------------------------------------------------------------*/
    /* Other misc gcc segments (this was in idt32.ld)                     */
    /*--------------------------------------------------------------------*/
    /* I am not quite sure about these sections but it seems they are for
        C++ exception handling. I think .jcr is for "Java Class
        Registration" but it seems to end up in C++ binaries as well. */
    
    .eh_frame_hdr     : { *(.eh_frame_hdr)     } > m_prog
    .eh_frame         : { KEEP( *(.eh_frame) ) } > m_prog
    .gcc_except_table : { *(.gcc_except_table) } > m_prog
    .jcr              : { KEEP (*(.jcr))       } > m_prog

    /*--------------------------------------------------------------------*/
    /* Initialized data segment                                           */
    /*--------------------------------------------------------------------*/
    
	. = ALIGN(8);
    PROVIDE( etext = . );
    _etext = .;
	_fidata = .;
	
    . = ORIGIN(m_data);
    _fdata = .;

    .data : AT(_etext)
    {
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(8);
        *(.sdata)
        *(.sdata.*)
        *(.srodata.*)
        *(.gnu.linkonce.s.*)
    } > m_data

    PROVIDE( edata = . );
    _edata = .;

    /*--------------------------------------------------------------------*/
    /* Uninitialized data segment                                         */
    /*--------------------------------------------------------------------*/
    
    . = ALIGN(8);
    _fbss = .;
    .bss (NOLOAD) :
    {
        *(.sbss)
        *(.sbss.*)
        *(.gnu.linkonce.sb.*)
        . = ALIGN(8);
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
    } > m_data
    
    PROVIDE( ebss = . );
    _ebss = .;
	
	. = ALIGN(8);
    PROVIDE( sheap = . );
    _sheap = .;
    .heap (NOLOAD) :
    {
        KEEP(*(.heap))
        KEEP(*(.heap.*))
    } > m_data

    PROVIDE( eheap = . );
    _eheap = .;
	
	.stack (NOLOAD) :
	{
		KEEP(*(.stack))
		KEEP(*(.stack.*))
	} > m_data
    
    . = ORIGIN(m_data) + LENGTH(m_data);
    PROVIDE( estack = . );
    _estack = .;
	. = . - SIZEOF(.stack);
	PROVIDE( fstack = .);
	_fstack = .;
	
	ASSERT( _fstack > _eheap , "ERROR: heap and stack overlapping.")
    
    /*--------------------------------------------------------------------*/
    /* Shared data segment                                                */
    /*--------------------------------------------------------------------*/
    
    . = ORIGIN(m_shared);
    PROVIDE( sshared = . );
    _sshared = .;
    
    .shared (NOLOAD) :
    {
        *(.shared)
        *(.shared.*)
    } > m_shared
    
    . = ALIGN(4);
    PROVIDE( eshared = . );
    _eshared = .;
  
}
