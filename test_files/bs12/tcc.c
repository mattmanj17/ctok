
// massage things so we can compile with msvc + stock tcc --------------------
//  note that we treat pp direcives as fancy line comments,
//  so these ONLY do anything when compiled with not-ourself

// CTOK : we comment these out and manualy perform these macros replacements 
// to avoid doing any pp from clang.
// this makes this file no longer self hosting, since it
// doesnt have the code for __attribute__((***)) parsing
// but, for turning ctok into a tool for munging bootstrip, I think it is fine.

//#ifdef _MSC_VER
//#define __stdcall__ __stdcall
//#pragma warning (disable : 5045) // disable inescapable spectre warning
//#endif

//#ifdef __TINYC__
//#define __stdcall__ __attribute__((__stdcall__))
//#endif

//----------------------------------------------------------------------------

// XXX make these keywords
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned u32;
typedef long long i64;
typedef unsigned long long u64;

// msvcrt.dll
// XXX remove these, just use kernel32
extern void exit(i32 _Code);
extern void * malloc(u32 _Size);
extern void free(void * _Memory);
extern u32 fwrite(void * _Str, u32 _Size, u32 _Count, void * _File);
extern i32 ftell(void * _File);
extern void * fopen(char * _Filename, char * _Mode);
extern i32 fseek(void * _File, i32 _Offset, i32 _Origin);
extern u32 fread(void * _DstBuf, u32 _ElementSize, u32 _Count, void * _File);
extern i32 ferror(void * stream);

// kernel32.dll
extern char * __attribute__((__stdcall__)) 
GetCommandLineA(void);

extern void * __attribute__((__stdcall__)) 
GetStdHandle(u32 nStdHandle);

extern i32 __attribute__((__stdcall__)) 
WriteFile(
    void * hFile,
    void * lpBuffer,
    u32 nNumberOfBytesToWrite,
    void * lpNumberOfBytesWritten,
    void * lpOverlapped);

// -------- ENUMS
// XXX create a 'define' keyword, that declares an enum of one member?
//  that is, 'define(NULL, 0)' --> enum { NULL = 0 };
typedef enum
{
    NULL = 0,
    SEEK_SET = 0,
    SEEK_END = 2,
    CHAR_BUF_LEN = 1024,
} misc_;

// pretty names for the registers
typedef enum TREG
{
    TREG_EAX,
    TREG_ECX,
    TREG_EDX,
    TREG_EBX,
    TREG_ESP,
} TREG;

// XXX grumble grumble keep in sync with tcc_keywords
typedef enum tcc_token
{
    // 'name' tokens (interned strings) start at 256
    TOK_NAME_MIN = 256, //tokfixme

    // keywords
    TOK_INT = TOK_NAME_MIN,
    TOK_VOID,
    TOK_CHAR,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_BREAK,
    TOK_RETURN,
    TOK_FOR,
    TOK_EXTERN,
    TOK_STATIC,
    TOK_UNSIGNED,
    TOK_GOTO,
    TOK_DO,
    TOK_CONTINUE,
    TOK_SWITCH,
    TOK_CASE,

    TOK_LONG,
    TOK_SIGNED1,
    TOK_INLINE2,

    TOK_BOOL,
    TOK_SHORT,
    TOK_STRUCT,
    TOK_UNION,
    TOK_TYPEDEF,
    TOK_DEFAULT,
    TOK_ENUM,
    TOK_SIZEOF,
    TOK_ATTRIBUTE2,

    // attribute identifiers
    TOK_STDCALL3,

    // builtin functions or variables
    TOK_intrin_memmove,

    // TOK_intrin_memmove is a 'fake' keyword, we let you use it as a normal name
    TOK_KEYWORD_MAX = TOK_intrin_memmove, //tokfixme
} tcc_token;

typedef enum
{
    // --------------------------------------------
    // include the target specific definitions
    // number of available registers
    NB_REGS = 4,

    // a register can belong to several classes. The classes must be
    // sorted from more general to more precise (see gv2() code which does
    // assumptions on it).
    RC_INT = 0x0001, // generic integer register
    RC_EAX = 0x0004,
    RC_ECX = 0x0010,
    RC_EDX = 0x0020,

    RC_IRET = RC_EAX, // function return: integer register
    RC_LRET = RC_EDX, // function return: second integer register

    // pointer size, in bytes
    PTR_SIZE = 4,
    PTR_SIZE_IS_4 = (PTR_SIZE == 4),

    // --------------------------------------------
    LONG_SIZE_IS_4 = 1,

    // --------------------------------------------
    VSTACK_SIZE = 256,
    STRING_MAX_SIZE = 1024,

    TOK_HASH_SIZE = 16384, // must be a power of two
    TOK_ALLOC_INCR = 512,  // must be a power of two
    TOK_MAX_SIZE = 4, // token max size in int unit when stored in string

    // --------------------------------------------------
    SYM_STRUCT = 0x40000000, // struct/union/enum symbol space
    SYM_FIELD = 0x20000000 ,// struct/union field symbol space
    SYM_FIRST_ANOM = 0x10000000, // first anonymous sym

    // stored in 'Sym->f.func_call' field
    FUNC_CDECL = 0, // standard c call
    FUNC_STDCALL = 1, // pascal c call

    // field 'Sym.r' for C labels
    LABEL_DEFINED = 0, // label is defined
    LABEL_FORWARD = 1, // label is forward defined
    LABEL_DECLARED = 2, // label is declared but never used

    // type_decl() types
    TYPE_ABSTRACT = 1, // type without variable
    TYPE_DIRECT = 2, // type with variable

    IO_BUF_SIZE = 8192 * 80,

    CH_EOF = (-1),   // end of file */

    // The current value can be:
    VT_VALMASK = 0x003f,  // mask for value location, register or:
    VT_CONST = 0x0030,  // constant in vc (must be first non register value)
    VT_LLOCAL = 0x0031,  // lvalue, offset on stack
    VT_LOCAL = 0x0032,  // offset on stack
    VT_CMP = 0x0033,  // the value is stored in processor flags (in vc)
    VT_JMP = 0x0034,  // value is the consequence of jmp true (even)
    VT_JMPI = 0x0035,  // value is the consequence of jmp false (odd)
    VT_LVAL = 0x0100,  // var is an lvalue
    VT_SYM = 0x0200,  // a symbol value is added
    VT_MUSTCAST = 0x0400, // value must be casted to be correct (used for
                            // char/short stored in integer registers)
    VT_BOUNDED = 0x8000, //  value is bounded. The address of the
                            // bounding function call point is in vc
    VT_LVAL_BYTE = 0x1000,  // lvalue is a byte
    VT_LVAL_SHORT = 0x2000,  // lvalue is a short
    VT_LVAL_UNSIGNED = 0x4000,  // lvalue is unsigned
    VT_LVAL_TYPE = (VT_LVAL_BYTE | VT_LVAL_SHORT | VT_LVAL_UNSIGNED),

    // types
    VT_BTYPE = 0x000f,  // mask for basic type
    VT_VOID = 0,  // void type
    VT_BYTE = 1,  // signed byte type
    VT_SHORT = 2,  // short type
    VT_INT = 3,  // integer type
    VT_LLONG = 4,  // 64 bit integer
    VT_PTR = 5,  // pointer
    VT_FUNC = 6,  // function type
    VT_STRUCT = 7,  // struct/union definition
    VT_BOOL = 11,  // ISOC99 boolean type

    VT_UNSIGNED = 0x0010,  // unsigned type
    VT_DEFSIGN = 0x0020,  // explicitly signed or unsigned
    VT_ARRAY = 0x0040,  // array type (also has VT_PTR)
    VT_CONSTANT = 0x0100,  // modifier
    VT_VOLATILE = 0x0200,  // volatile modifier
    VT_LONG = 0x0800,  // long type (also has VT_INT rsp. VT_LLONG)

    // storage
    VT_EXTERN = 0x00001000,  // extern definition
    VT_STATIC = 0x00002000,  // static variable
    VT_TYPEDEF = 0x00004000,  // typedef definition
    VT_INLINE = 0x00008000,  // inline definition
    // currently unused: 0x000[1248]0000
    VT_STRUCT_SHIFT = 20,     // shift for union/enum flags (32 - 2*6) */
    VT_STRUCT_MASK = (((1 << (6 + 6)) - 1) << VT_STRUCT_SHIFT), //!!! simplify this...

    VT_UNION = ((1 << VT_STRUCT_SHIFT) | VT_STRUCT),
    VT_ENUM = (2 << VT_STRUCT_SHIFT), // integral type is an enum really
    VT_ENUM_VAL = (3 << VT_STRUCT_SHIFT), // integral type is an enum constant really

    // type mask (except storage)
    VT_STORAGE = (VT_EXTERN | VT_STATIC | VT_TYPEDEF | VT_INLINE),
    VT_TYPE = (~(VT_STORAGE | VT_STRUCT_MASK)),

    LONG_BTYPE = (!LONG_SIZE_IS_4 ? VT_LLONG : VT_INT),

    // token values
    // warning: the following compare tokens depend on i386 asm code
    TOK_ULT = 0x92,
    TOK_UGE = 0x93,
    TOK_EQ = 0x94,
    TOK_NE = 0x95,
    TOK_ULE = 0x96,
    TOK_UGT = 0x97,
    TOK_LT = 0x9c,
    TOK_GE = 0x9d,
    TOK_LE = 0x9e,
    TOK_GT = 0x9f,

    TOK_LAND = 0xa0,
    TOK_LOR = 0xa1,
    TOK_DEC = 0xa2,
    TOK_MID = 0xa3, // inc/dec, to void constant
    TOK_INC = 0xa4,
    TOK_UDIV = 0xb0, // unsigned division
    TOK_UMOD = 0xb1, // unsigned modulo
    TOK_PDIV = 0xb2, // fast division with undefined rounding for pointers

    // tokens that carry values (in additional token string space / tokc) -->
    TOK_CCHAR = 0xb3, // char constant in tokc
    TOK_CINT = 0xb5, // number in tokc
    TOK_CUINT = 0xb6, // u32 constant
    TOK_CLLONG = 0xb7, // i64 constant
    TOK_CULLONG = 0xb8, // u64 constant
    TOK_STR = 0xb9, // pointer to string in tokc
    TOK_PPNUM = 0xbe, // preprocessor number
    TOK_PPSTR = 0xbf, // preprocessor string
    TOK_LINENUM = 0xc0, // line number info
    // <--
    TOK_UMULL = 0xc2, // unsigned 32x32 -> 64 mul
    TOK_ADDC1 = 0xc3, // add with carry generation
    TOK_ADDC2 = 0xc4, // add with carry use
    TOK_SUBC1 = 0xc5, // add with carry generation
    TOK_SUBC2 = 0xc6, // add with carry use
    TOK_ARROW = 0xc7,
    TOK_SHR = 0xc9, // unsigned shift right
    TOK_CLONG = 0xce, // long constant
    TOK_CULONG = 0xcf, // u32 constant

    TOK_SHL = 0x01, // shift left
    TOK_SAR = 0x02, // signed shift right

    // assignment operators : normal operator or 0x80
    TOK_A_MOD = 0xa5,
    TOK_A_AND = 0xa6,
    TOK_A_MUL = 0xaa,
    TOK_A_ADD = 0xab,
    TOK_A_SUB = 0xad,
    TOK_A_DIV = 0xaf,
    TOK_A_XOR = 0xde,
    TOK_A_OR = 0xfc,
    TOK_A_SHL = 0x81,
    TOK_A_SAR = 0x82,

    TOK_EOF = (-1),  // end of file
    TOK_LINEFEED = 10,    // line feed

    TOK_END_OF_REPLAY = 0,

    // -------------------------------------------------------------------------
    // Custom allocator for tiny objects
    TOKSYM_TAL_SIZE = (768 * 1024), // allocator for tiny TokenSym in table_ident
    TOKSTR_TAL_SIZE = (768 * 1024), // allocator for tiny TokenString instances
    CSTR_TAL_SIZE = (256 * 1024), // allocator for tiny CString instances
    TOKSYM_TAL_LIMIT = 256, // prefer unique limits to distinguish allocators debug msgs
    TOKSTR_TAL_LIMIT = 128, // 32 * sizeof(int) */
    CSTR_TAL_LIMIT = 1024,

    //---------------------

    TOK_HASH_INIT = 1,

    // we use 64 bit numbers
    BN_SIZE = 2,

    EXPR_CONST = 1,
    EXPR_ANY = 2,

    FUNC_PROLOG_SIZE = (10),

    IMAGE_FILE_MACHINE_I386 = 332, // 0x014C

    IMAGE_DIRECTORY_ENTRY_EXPORT = 0,   // Export Directory
    IMAGE_DIRECTORY_ENTRY_IMPORT = 1,   // Import Directory
    IMAGE_DIRECTORY_ENTRY_IAT = 12 ,  // Import Address Table

    WD_ALL = 0x0001, // warning is activated when using -Wall
    FD_INVERT = 0x0002, // invert value before storing

    TCC_OPTION_HAS_ARG = 0x0001,

    // ...

    CH_NEWLINE = 0x0a, // backslash n

    CH_BACKSLASH = 0x5c, // Reverse Solidus

} defines_;


// -------- structs
typedef struct
{
    i32 low;
    i32 high;
} DWstruct;

typedef union
{
    DWstruct s;
    i64 ll;
} DWunion;

typedef struct _IMAGE_DOS_HEADER
{
    u8  e_magic[2];
    u8  junk[58];
    i32 e_lfanew;
} IMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER
{
    u16 Machine;
    u16 NumberOfSections;
    u32 TimeDateStamp;
    u32 PointerToSymbolTable;
    u32 NumberOfSymbols;
    u16 SizeOfOptionalHeader;
    u16 Characteristics;
} IMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY
{
    u32 VirtualAddress;
    u32 Size;
} IMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER
{
    u16 Magic;
    u8 MajorLinkerVersion;
    u8 MinorLinkerVersion;
    u32 SizeOfCode;
    u32 SizeOfInitializedData;
    u32 SizeOfUninitializedData;
    u32 AddressOfEntryPoint;
    u32 BaseOfCode;
    u32 BaseOfData;
    u32 ImageBase;
    u32 SectionAlignment;
    u32 FileAlignment;
    u16 MajorOperatingSystemVersion;
    u16 MinorOperatingSystemVersion;
    u16 MajorImageVersion;
    u16 MinorImageVersion;
    u16 MajorSubsystemVersion;
    u16 MinorSubsystemVersion;
    u32 Win32VersionValue;
    u32 SizeOfImage;
    u32 SizeOfHeaders;
    u32 CheckSum;
    u16 Subsystem;
    u16 DllCharacteristics;
    u32 SizeOfStackReserve;
    u32 SizeOfStackCommit;
    u32 SizeOfHeapReserve;
    u32 SizeOfHeapCommit;
    u32 LoaderFlags;
    u32 NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_SECTION_HEADER
{
    u8 Name[8];
    union
    {
        u32 PhysicalAddress;
        u32 VirtualSize;
    } Misc;
    u32 VirtualAddress;
    u32 SizeOfRawData;
    u32 PointerToRawData;
    u32 PointerToRelocations;
    u32 PointerToLinenumbers;
    u16 NumberOfRelocations;
    u16 NumberOfLinenumbers;
    u32 Characteristics;
} IMAGE_SECTION_HEADER;

typedef struct _IMAGE_IMPORT_DESCRIPTOR
{
    union
    {
        u32 Characteristics;
        u32 OriginalFirstThunk;
    }ch_or_ft;
    u32 TimeDateStamp;
    u32 ForwarderChain;
    u32 Name;
    u32 FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_EXPORT_DIRECTORY
{
    u32 Characteristics;
    u32 TimeDateStamp;
    u16 MajorVersion;
    u16 MinorVersion;
    u32 Name;
    u32 Base;
    u32 NumberOfFunctions;
    u32 NumberOfNames;
    u32 AddressOfFunctions;
    u32 AddressOfNames;
    u32 AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER64
{
    u16 Magic;
    u8 MajorLinkerVersion;
    u8 MinorLinkerVersion;
    u32 SizeOfCode;
    u32 SizeOfInitializedData;
    u32 SizeOfUninitializedData;
    u32 AddressOfEntryPoint;
    u32 BaseOfCode;
    u64 ImageBase;
    u32 SectionAlignment;
    u32 FileAlignment;
    u16 MajorOperatingSystemVersion;
    u16 MinorOperatingSystemVersion;
    u16 MajorImageVersion;
    u16 MinorImageVersion;
    u16 MajorSubsystemVersion;
    u16 MinorSubsystemVersion;
    u32 Win32VersionValue;
    u32 SizeOfImage;
    u32 SizeOfHeaders;
    u32 CheckSum;
    u16 Subsystem;
    u16 DllCharacteristics;
    u64 SizeOfStackReserve;
    u64 SizeOfStackCommit;
    u64 SizeOfHeapReserve;
    u64 SizeOfHeapCommit;
    u32 LoaderFlags;
    u32 NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER64;

typedef struct Span
{
    u8 * p;
    u8 * end;
} Span;

typedef struct Bytes
{
    u8 * p;
    u32 cap;
    u32 size;
} Bytes;

typedef struct Items
{
    u32 c;
    Bytes _bytes;
    u32 item_size;
} Items;

typedef struct Relocation
{
    u32 offset;
    i32 sym;
    i32 is_pc_relative;
} Relocation;

typedef struct Relocations
{
    union
    {
        struct
        {
            u32 c;
            Relocation * p;
        }r;
        Items _items;
    }u;
} Relocations;

typedef struct Symbols
{
    u32 c_syms;
    u8 * syms;// Elf32_Sym *
    u32 c_syms_allocated;

    u32 c_names;
    u8 * names;
    u32 c_names_allocated;

    u32 c_hash;
    u8 * p_hash; // i32 *
    u32 c_hash_allocated;

    i32 nb_hashed_syms;
    i32 nb_buckets;
    i32 nb_syms;
} Symbols;

// section definition
typedef struct Section
{
    Bytes bytes;
    Relocations relocs;
    u32 reloc_base_addr;
} Section;

typedef struct
{
    u32 got_offset;
    u32 plt_offset;
    i32 plt_sym;
    i32 dyn_index;
} sym_attr;

// function attributes or temporary attributes for parsing
typedef struct
{
    u32 func_call; // calling convention (0..5), see below
    u32 func_args; // PE __stdcall args
} FuncAttr;

// GNUC attribute definition
typedef struct
{
    FuncAttr f;
} AttributeDef;

typedef struct _tag_TYPE_VAL
{
    i32 bits;
} TYPE_VAL;

// type definition
typedef struct CType
{
    TYPE_VAL _tv_;
    struct Sym * ref;
} CType;

// symbol management
typedef struct Sym
{
    i64 enum_val; // enum constant if IS_ENUM_VAL

    i32 v; // symbol token
    u32 r; // associated register or VT_CONST/VT_LOCAL and LVAL type
    i32 c; // associated number or Elf symbol index
    i32 jnext; // next jump label
    FuncAttr f; // function attributes

    CType type; // associated type
    struct Sym * next; // next related symbol (for fields and anoms)
    struct Sym * prev; // prev symbol in stack
} Sym;

typedef struct _TCCState
{
    i32 symbolic; // if true, resolve symbols in the current module first

    char * soname; // as specified on the command line (-soname)
    char * rpath; // as specified on the command line (-Wl,-rpath=)
    i32 enable_new_dtags; // ditto, (-Wl,--enable-new-dtags)

    // output format, see TCC_OUTPUT_FORMAT_xxx
    i32 output_format;

    u32 text_addr; // address of text section
    i32 has_text_addr;

    u32 section_align; // section alignment

    char * init_symbol; // symbols to call at load-time (not used currently)
    char * fini_symbol; // symbols to call at unload-time (not used currently)

    char _pad[4];

    // got & plt handling
    Section * got;
    Section * plt;

    // PE info
    i32 pe_subsystem;
    u32 pe_characteristics;
    u32 pe_file_align;
    u32 pe_stack_size;
    u32 pe_imagebase;

    //

    Sym * * sym_pools;
    i32 nb_sym_pools;
    Sym * sym_free_first;

    char _pad1[4];
} TCCState;

// Symbol table entry.
typedef struct _tag_Elf32_Sym
{
    Section * section; // Section
    u32 st_name; // Symbol name (string tbl index)
    u32 st_value; // Symbol value

    i32 is_func; // function?
} Elf32_Sym;

// token symbol management
typedef struct Name
{
    struct Name * next;  // next in hash table
    
    Sym * label;         // symbol if we are a label?
    Sym * structure;     // symbol if we are a structure
    Sym * identifier;    // symbol if we are an identifier
    
    i32 i;               // index in 'names'
    
    i32 len;             // name length
    char str[4];         // name string (len 4 to squawsh padding warning)
} Name;

typedef struct CString
{
    i32 size; // size in bytes
    void * data; // 'char *' */
    i32 size_allocated;
} CString;

// constant value
typedef union CValue
{
    u64 i;
    u32 i_as_ints[2];
    struct
    {
        i32 size;
        void * data;
    } str;
} CValue;

// value on stack
typedef struct SValue
{
    CType type;      // type */
    u16 r;      // register + flags */
    u16 r2;     ///* second register, used for 'i64'
                            //type. If not used, set to VT_CONST */
    
    u8 _pad0[4];
    CValue c;              // constant, if VT_CONST */
    struct Sym * sym;       // symbol, if (VT_SYM | VT_CONST), or if
                        //result of unary() for an identifier. */
    u8 _pad1[4];
} SValue;

// used to record tokens
typedef struct TokenString
{
    i32 * str;
    i32 len;
    i32 lastlen;
    i32 allocated_len;
    i32 last_line_num;
    i32 save_line_num;
    // used to chain token-strings with begin/end_replay()
    struct TokenString * prev;
    i32 * prev_ptr;
    char alloc;
    char _pad[3];
} TokenString;

typedef struct ExprValue
{
    u64 v;
    Sym * sym;
    i32 pcrel;
} ExprValue;

typedef struct TinyAlloc
{
    u32  limit;
    u32  size;
    u8 * buffer;
    u8 * p;
    u32  nb_allocs;
    struct TinyAlloc * next; 
    struct TinyAlloc * top;
} TinyAlloc;

typedef struct tal_header_t
{
    u32  size;
} tal_header_t;

typedef struct SectionMergeInfo
{
    Section * s;            // corresponding existing section
    u32 offset;  // offset of the new section in the existing section
    u8 new_section;       // true if section 's' was added
    u8 link_once;         // true if link once section
    u8 _pad[2];
} SectionMergeInfo;

typedef struct FlagDef
{
    u16 offset;
    u16 flags;
    char * name;
} FlagDef;

typedef struct case_t
{
    i32 value_;
    i32 symbol;
} case_t;

typedef struct switch_t
{
    case_t * * cases;
    i32 nb_cases; // list of case ranges

    i32 default_symbol; // default symbol
} switch_t;

typedef struct pe_header_t
{
    IMAGE_DOS_HEADER doshdr;
    u8 dosstub[64];
    u32 nt_sig;
    IMAGE_FILE_HEADER filehdr;
    IMAGE_OPTIONAL_HEADER32 opthdr;
} pe_header_t;

typedef struct section_info
{
    Section * s;
    u32 sh_addr;
    u32 sh_size;
    u8 * data;
    u32 data_size;
    IMAGE_SECTION_HEADER ish;
} section_info;

typedef struct pe_info
{
    TCCState * s1;
    Section * reloc;
    char * filename;
    u32 sizeofheaders;
    u32 imagebase;
    char * start_symbol;
    u32 start_addr;
    u32 imp_offs;
    u32 imp_size;
    u32 iat_offs;
    u32 iat_size;
    u32 exp_offs;
    u32 exp_size;
    i32 subsystem;
    u32 section_align;
    u32 file_align;
    struct section_info sec_info[2];
    i32 sec_count;
} pe_info;

typedef struct _put_elf_sym_ARGS
{
    u32 value;
    i32 is_func;
    Section * section;
} put_elf_sym_ARGS;

typedef struct ImportSymbol
{
    char * name;
    i32 iat_index;
} ImportSymbol;

typedef struct ImportDll
{
    char * name;
    ImportSymbol * syms;
    i32 nb_syms_used;
} ImportDll;

typedef struct ImportId
{
    i32 i_dll;
    i32 i_sym;
} ImportId;

typedef struct
{
    char * str;
    i32 tok;
} operator_token;

// -------- PURE FUNCTIONS (before any global state)
// note, allowed to do 'system calls' etc
// also note, may have local static state (which we try to minimize...)
static char * strcpy_(char * _Dest, char * _Source)
{
    char * _Dest_orig = _Dest;
    while (*_Source)
    {
        *_Dest = *_Source;
        ++_Dest;
        ++_Source;
    }
    *_Dest = 0;
    return _Dest_orig;
}

static u32 strlen_(char * _Str)
{
    u32 i = 0;
    while (_Str[i])
    {
        ++i;
    }
    return i;
}

static char * strcat_(char * dest, char * src)
{
    strcpy_(dest + strlen_(dest), src);
    return dest;
}

static void * memset(void * s, i32 c, u32 n)
{
    u8 * p = (u8 *)s;

    while (n--)
    {
        *p++ = (u8)c;
    }

    return s;
}

static void * memcpy(void * s1, void * s2, u32 n)
{
    char * dest = (char *)s1;
    char * src = (char *)s2;

    while (n--)
    {
        *dest++ = *src++;
    }

    return s1;
}

static i32 memcmp_(void * s1, void * s2, u32 n)
{
    u8 * p1 = (u8 *)s1;
    u8 * p2 = (u8 *)s2;

    while (n--)
    {
        if (*p1 != *p2)
        {
            return (*p1 - *p2);
        }

        ++p1;
        ++p2;
    }

    return 0;
}

// xxx rename
static void * __intrin_memmove(void * _Dst, void * _Src, u32 _Size)
{
    u8 * dst = (u8 *)_Dst;
    u8 * src = (u8 *)_Src;

    if (dst < src)
    {
        while (_Size--)
        {
            *dst = *src;
            ++dst;
            ++src;
        }
    }
    else if (dst > src)
    {
        // Overlapping regions, copy backwards

        dst += _Size;
        src += _Size;
        while (_Size--)
        {
            --dst;
            --src;
            *dst = *src;
        }
    }

    return _Dst;
}

static i32 strcmp_(char * _Str1, char * _Str2)
{
    while (1)
    {
        u8 c1 = *_Str1;
        u8 c2 = *_Str2;

        if ((c1 != 0) && (c1 == c2))
        {
            ++_Str1;
            ++_Str2;
            continue;
        }

        return (c1 - c2);
    }
}

static i32 strncmp_(char * _Str1, char * _Str2, u32 n)
{
    u32 i = 0;
    while (i < n)
    {
        u8 c1 = *_Str1;
        u8 c2 = *_Str2;

        if ((c1 != 0) && (c1 == c2))
        {
            ++_Str1;
            ++_Str2;
            ++i;
            continue;
        }

        return (c1 - c2);
    }

    return 0;
}

static void sputu(char * buf_out, u32 u)
{
    char buf[12];
    char * s = &buf[11];
    *s = 0;
    for (;;)
    {
        --s;
        *s = (u % 10) + '0';
        u /= 10;
        if (!u)
            break;
    }
    strcpy_(buf_out, s);
}

static void sputd(char * buf_out, i32 d)
{
    // Cache sign
    i32 is_negative = 0;
    if (d < 0)
    {
        is_negative = 1;
        d = -d;
    }

    // 'render' to buffer.
    //  12 is enough for '-' + 32bits + '\x00'.
    //  Note that do/while handles d == 0.
    char buf[12];
    char * s = buf + 1;
    sputu(s, (u32)d);

    if (is_negative)
    {
        --s;
        *s = '-';
    }

    strcpy_(buf_out, s);
}

// wrapper around compiler specific stderr
static inline void * get_stderr__(void)
{
    return GetStdHandle((u32)(-12));
}

static void do_exit(i32 _Code)
{
    exit(_Code);
}

static void stderr_write(char * str)
{
    i32 ret = WriteFile(get_stderr__(), str, strlen_(str), NULL, NULL);
    if (!ret)
    {
        do_exit(69);
    }
}

static void stderr_write_ln(char * str)
{
    stderr_write(str);
    char newline[] = {CH_NEWLINE, 0};
    stderr_write(newline);
}

// 'pure' bail out function
void die(char * str)
{
    stderr_write(str);
    stderr_write_ln("");

    do_exit(1);
}

// TYPE_VAL fiddle (xxx better place?)
static i32 * P_TV_BITS(TYPE_VAL * tv)
{
    return &tv->bits;
}

static i32 BTYPES_MATCH(TYPE_VAL t, i32 bt)
{
    switch (bt)
    {
    case VT_VOID:
    case VT_BYTE:
    case VT_SHORT:
    case VT_INT:
    case VT_LLONG:
    case VT_PTR:
    case VT_FUNC:
    case VT_STRUCT:
    case VT_BOOL:
        break;

    default:
        die("expected a valid BTYPE!");
        break;
    }

    return ((*P_TV_BITS(&t) & VT_BTYPE) == bt);
}

static i32 HAS_TFLAG(TYPE_VAL t, i32 flag)
{
    switch (flag)
    {
    case VT_UNSIGNED:
    case VT_DEFSIGN:
    case VT_ARRAY:
    case VT_CONSTANT:
    case VT_VOLATILE:
    case VT_LONG:
    case VT_EXTERN:
    case VT_STATIC:
    case VT_TYPEDEF:
    case VT_INLINE:
        break;

    default:
        die("expected a valid type flag!");
        break;
    }

    return (*P_TV_BITS(&t) & flag);
}

static u64 twopow(i32 n)
{
    if ((n < 0) || (n >= 64))
    {
        die("ack! twopow");
        return 0;
    }

    u64 result = 1;
    while (n)
    {
        result *= 2;
        --n;
    }
    return result;
}

static u64 u64div(
    u64 numerator,
    u64 denominator)
{
    if (denominator == 0)
    {
        die("divide by 0!!!");
        return 0;
    }

    if ((numerator < 0x100000000ull) &&
        (denominator < 0x100000000ull))
    {
        return (
            (u32)numerator /
            (u32)denominator
            );
    }

    u64 scratch = 0;
    u64 quotient = 0;

    i32 i = 63;
    for (; i >= 0; --i)
    {
        u64 bit = twopow(i);

        scratch *= 2;
        scratch |= (numerator & bit) ? 1 : 0;

        if (scratch >= denominator)
        {
            scratch -= denominator;
            quotient |= bit;
        }
    }

    return quotient;
}

// XXX: fix tcc's code generator to do this instead
static i64 i64shl(i64 a, i32 b)
{
    DWunion u;
    u.ll = a;
    if (b >= 32)
    {
        u.s.high = (u32)u.s.low << (b - 32);
        u.s.low = 0;
    }
    else if (b != 0)
    {
        u.s.high = ((u32)u.s.high << b) | ((u32)u.s.low >> (32 - b));
        u.s.low = (u32)u.s.low << b;
    }
    return u.ll;
}

// alloc
void * mallocz_or_die(u32 size)
{
    void * ptr = malloc(size);
    if (!ptr)
    {
        die("memory full (malloc)");
        return NULL;
    }

    memset(ptr, 0, size);
    return ptr;
}

void * realloc_or_die(
    void * ptr,
    u32 old_size,
    u32 size)
{
    void * ptr2 = mallocz_or_die(size);
    __intrin_memmove(ptr2, ptr, old_size);
    free(ptr);
    return ptr2;
}

// dynary
static void dynarray_add_(void * ptab, i32 * nb_ptr, void * data)
{
    i32 nb;
    i32 nb_alloc;
    void * * pp;

    nb = *nb_ptr;
    pp = *(void * * *)ptab;
    // every power of two we 2X array size
    if ((nb & (nb - 1)) == 0)
    {
        if (!nb)
        {
            nb_alloc = 1;
        }
        else
        {
            nb_alloc = nb * 2;
        }

        pp = realloc_or_die(
            pp,
            nb * sizeof(void *),
            nb_alloc * sizeof(void *)
        );
        *(void * * *)ptab = pp;
    }
    pp[nb++] = data;
    *nb_ptr = nb;
}

static void dynarray_reset(void * pp, i32 * n)
{
    void * * p = *(void * * *)pp;
    while (*n)
    {
        if (*p)
        {
            free(*p);
        }

        ++p; *n -= 1;
    }
    free(*(void * *)pp);
    *(void * *)pp = NULL;
}

// ch classes

static inline i32 isid(i32 c)
{
    return 
    (
        ((c >= 'a') && (c <= 'z')) || 
        ((c >= 'A') && (c <= 'Z')) || 
        (c == '_')
    );
}

static i32 is_in_range(i32 n, i32 low, i32 high)
{
    if (n < low)
        return 0;

    if (n > high)
        return 0;

    return 1;
}

static inline i32 toup(i32 c)
{
    return (((c >= 'a') && (c <= 'z')) ? (c - 'a' + 'A') : c);
}

static u8 is_id_num(i32 c)
{
    if (isid(c))
        return 1;

    if (is_in_range(c, '0', '9'))
        return 1;

    return 0;
}

static u8 is_operator_char(i32 c)
{
    switch (c)
    {
    case '!': case '%': case '&': case '*': case '+': 
    case '-': case '.': case ':': case '<': case '=': 
    case '>': case '?': case '^': case '|': case '~':
        return 1;

    default:
        return 0;
    }
}

static u8 is_sentinel_char(i32 c)
{
    switch (c)
    {
    case '(': case ')':
    case '[': case ']':
    case '{': case '}':
    case ',': case ';':
        return 1;

    default:
        return 0;
    }
}

// read/write to/from pointers
static inline u16 read16le(u8 * p)
{
    return (p[0] | ((u16)p[1] << 8));
}
static inline void write16le(u8 * p, u16 x)
{
    p[0] = x & 255;  p[1] = (x >> 8) & 255;
}
static inline u32 read32le(u8 * p)
{
    return (read16le(p) | ((u32)read16le(p + 2) << 16));
}
static inline void write32le(u8 * p, u32 x)
{
    write16le(p, (u16)(x));
    write16le(p + 2, (u16)(x >> 16));
}
static inline void add32le(u8 * p, i32 x)
{
    write32le(p, read32le(p) + x);
}

// -------- TinyAlloc
static TinyAlloc * tal_new(TinyAlloc * * pal, u32 limit, u32 size)
{
    TinyAlloc * al = mallocz_or_die(sizeof(TinyAlloc));
    al->p = mallocz_or_die(size);
    al->buffer = al->p;
    al->limit = limit;
    al->size = size;
    if (pal)
    {
        *pal = al;
    }
    return al;
}

static void * tal_realloc_impl(
    TinyAlloc * * pal,
    void * p,
    u32 old_size,
    u32 size)
{
    tal_header_t * header;
    void * ret;
    i32 is_own;
    u32 adj_size = (size + 3) & -4;
    TinyAlloc * al = *pal;

tail_call:
    is_own = (
        (al->buffer <= (u8 *)p) &&
        ((u8 *)p < (al->buffer + al->size))
        );
    if ((!p || is_own) && (size <= al->limit))
    {
        if ((al->p + adj_size + sizeof(tal_header_t)) < (al->buffer + al->size))
        {
            header = (tal_header_t *)al->p;
            header->size = adj_size;
            ret = al->p + sizeof(tal_header_t);
            al->p += adj_size + sizeof(tal_header_t);
            if (is_own)
            {
                header = (((tal_header_t *)p) - 1);
                memcpy(ret, p, header->size);
            }
            else
            {
                al->nb_allocs++;
            }
            return ret;
        }
        else if (is_own)
        {
            al->nb_allocs--;
            ret = tal_realloc_impl(pal, 0, 0, size);
            header = (((tal_header_t *)p) - 1);
            memcpy(ret, p, header->size);
            return ret;
        }
        if (al->next)
        {
            al = al->next;
        }
        else
        {
            TinyAlloc * bottom = al;
            TinyAlloc * next = al->top ? al->top : al;

            al = tal_new(pal, next->limit, next->size * 2);
            al->next = next;
            bottom->top = al;
        }
        goto tail_call;
    }
    if (is_own)
    {
        al->nb_allocs--;
        ret = mallocz_or_die(size);
        header = (((tal_header_t *)p) - 1);
        memcpy(ret, p, header->size);
    }
    else if (al->next)
    {
        al = al->next;
        goto tail_call;
    }
    else
    {
        ret = realloc_or_die(
            p,
            old_size,
            size
        );
    }
    return ret;
}

static void tal_free_impl(TinyAlloc * al, void * p)
{
    if (!p)
    {
        return;
    }
tail_call:
    if (
        (al->buffer <= (u8 *)p) &&
        ((u8 *)p < (al->buffer + al->size))
        )
    {
        al->nb_allocs--;
        if (!al->nb_allocs)
        {
            al->p = al->buffer;
        }
    }
    else if (al->next)
    {
        al = al->next;
        goto tail_call;
    }
    else
    {
        free(p);
    }
}

static void tal_delete(TinyAlloc * al)
{
    TinyAlloc * next;

tail_call:
    if (!al)
    {
        return;
    }
    next = al->next;
    free(al->buffer);
    free(al);
    al = next;
    goto tail_call;
}

// -------- CString
static void cstr_new(CString * cstr)
{
    memset(cstr, 0, sizeof(CString));
}

static void cstr_realloc(CString * cstr, i32 new_size)
{
    static TinyAlloc * cstr_alloc = 0;
    if (!cstr_alloc)
    {
        tal_new(&cstr_alloc, CSTR_TAL_LIMIT, CSTR_TAL_SIZE);
    }

    if (new_size == 0)
    {
        tal_free_impl(cstr_alloc, cstr->data);
        cstr_new(cstr);
        return;
    }

    i32 size;

    size = cstr->size_allocated;
    if (size < 8)
    {
        size = 8; // no need to allocate a too small first string
    }
    while (size < new_size)
    {
        size = size * 2;
    }
    cstr->data = tal_realloc_impl(&cstr_alloc, cstr->data, cstr->size_allocated, size);
    cstr->size_allocated = size;
}

static void cstr_free(CString * cstr)
{
    cstr_realloc(cstr, 0);
}

static void cstr_reset(CString * cstr)
{
    cstr->size = 0;
}

static void cstr_cat(CString * cstr, char * str, i32 len)
{
    i32 size;
    if (len <= 0)
    {
        len = strlen_(str) + 1 + len;
    }
    size = cstr->size + len;
    if (size > cstr->size_allocated)
    {
        cstr_realloc(cstr, size);
    }
    __intrin_memmove(((u8 *)cstr->data) + cstr->size, str, len);
    cstr->size = size;
}

static inline void cstr_ccat(CString * cstr, i32 ch)
{
    i32 size;
    size = cstr->size + 1;
    if (size > cstr->size_allocated)
    {
        cstr_realloc(cstr, size);
    }
    ((u8 *)cstr->data)[size - 1] = (u8)ch;
    cstr->size = size;
}

// ...
static u32 TOK_HASH_FUNC(u32 h, i32 c)
{
    return (h + (h << 5) + (h >> 27) + c);
}

static void AUDIT_CH(i32 ch)
{
    if (ch == 0x0d)
    {
        die("CARRIAGE_RETURN");
    }
}

static i32 PEEK_CH(Span * span)
{
    if (span->p < span->end)
    {
        i32 ch = span->p[0];
        AUDIT_CH(ch);
        return ch;
    }
    else if (span->p > span->end)
    {
        die("(span->p > span->end) !!!");
        return CH_EOF;
    }
    else
    {
        return CH_EOF;
    }
}

// XXX change lex/parse to return lengs, instead of dropping inline
static void DROP_CH(Span * span)
{
    if (span->p < span->end)
    {
        AUDIT_CH(*span->p);
        ++(span->p);
    }
}

// single line C++ comments
static void parse_line_comment__(
    Span * span)
{
    // skip leading '/'
    DROP_CH(span);

    // eat the chars
    for (;;)
    {
        i32 c = PEEK_CH(span);

        // hit end?
        if ((c == CH_NEWLINE) || (c == CH_EOF))
            break;

        // advance
        DROP_CH(span);
    }
}

// PEEK_CH, but '\\' not allowed
static inline i32 PEEK_CH_NO_STRAY(Span * span)
{
    i32 ch = PEEK_CH(span);

    if (ch == '\\')
    {
        die("stray '\\' in program");
    }

    return ch;
}

// error logging
static i32 g_line_num = 0;
static i32 g_nb_errors = 0;
static void print_error_with_line_number(char * str)
{
    char buf[CHAR_BUF_LEN];
    sputd(buf, g_line_num);
    strcat_(buf, " : ");
    stderr_write(buf);
    stderr_write_ln(str);

    g_nb_errors++;
}
static void die_with_line_number(char * str)
{
    print_error_with_line_number(str);
    do_exit(1);
}
static void tcc_error_puts(char * fmt, char * msg)
{
    print_error_with_line_number(fmt);
    die_with_line_number(msg);
}
static void expect(char * msg)
{
    tcc_error_puts("%s expected", msg);
}

static i32 tok_from_ch_before_assign(i32 c)
{
    switch (c)
    {
    case '!':
        return TOK_NE;
    case '=':
        return TOK_EQ;
    case '*':
        return TOK_A_MUL;
    case '%':
        return TOK_A_MOD;
    case '^':
        return TOK_A_XOR;
    }

    // SHOULD NEVER HAPPEN
    return 0;
}

// parse a string without interpreting escapes
static void parse_pp_string_(
    Span * span,
    i32 ch_quote,
    CString * str)
{
    cstr_reset(str);
    cstr_ccat(str, ch_quote);

    // drop leading quote
    DROP_CH(span);

    // eat all the chars
    for (;;)
    {
        i32 ch = PEEK_CH(span);

        // hit end?
        if (ch == ch_quote)
            break;

        // Deal with escapes
        i32 leading_backslash = (ch == '\\');
        if (leading_backslash)
        {
            DROP_CH(span);
            ch = PEEK_CH(span);
        }

        // eof?
        if (ch == CH_EOF)
        {
            die_with_line_number("unexpected EOF in string");
        }

        // newline?
        if (ch == CH_NEWLINE)
        {
            die_with_line_number("unexpected newline in string");
        }

        // Append to str (if we have one)
        if (str)
        {
            if (leading_backslash)
            {
                cstr_ccat(str, '\\');
            }
            cstr_ccat(str, ch);
        }

        // advance
        DROP_CH(span);
    }

    // drop trailing quote
    DROP_CH(span);

    cstr_ccat(str, ch_quote);
    cstr_ccat(str, 0);
}

void parse_ppnum(Span * span, CString * str)
{
    // after the first digit, accept digits or alpha
    cstr_reset(str);
    for (;;)
    {
        i32 c = PEEK_CH_NO_STRAY(span);
        if (!is_id_num(c))
            break;

        cstr_ccat(str, c);
        DROP_CH(span);
    }
    // We add a trailing '\x00' to ease parsing
    cstr_ccat(str, 0);
}

static void skip_spaces_and_newlines(Span * span)
{
    for (;;)
    {
        i32 c = PEEK_CH_NO_STRAY(span);

        if (c == ' ')
        {
            for (;;)
            {
                DROP_CH(span);
                c = PEEK_CH_NO_STRAY(span);
                if (c != ' ')
                    break;
            }
        }
        else if (c == CH_NEWLINE)
        {
            g_line_num++;
            DROP_CH(span);
        }
        else
        {
            break;
        }
    }
}

// NAMES
static i32 g_nb_names = 0;
static Name * * g_names = 0;
static Name * intern_name(char * str, i32 len)
{
    // hash the str
    u32 h = TOK_HASH_INIT;
    for (int i = 0; i < len; i++)
    {
        h = TOK_HASH_FUNC(h, ((u8 *)str)[i]);
    }
    h &= (TOK_HASH_SIZE - 1);

    // see if we already have it
    static Name * h_to_name[TOK_HASH_SIZE] = {0};
    for (
        Name * name = h_to_name[h];
        name;
        name = name->next
    )
    {
        if (name->len != len)
            continue;

        if (memcmp_(name->str, str, len) != 0)
            continue;

        return name;
    }

    // check for overflow
    if (g_nb_names >= (SYM_FIRST_ANOM - TOK_NAME_MIN))
    {
        die_with_line_number(
            "name overflow! " 
            "interned strings overflowed into "
            "anonymous symbols.");
    }

    // expand token table if needed (stores allocated TokenSym pointers)
    if ((g_nb_names % TOK_ALLOC_INCR) == 0)
    {
        g_names = realloc_or_die(
            g_names,
            g_nb_names * sizeof(Name *),
            (g_nb_names + TOK_ALLOC_INCR) * sizeof(Name *)
        );
    }

    // set up name allocator
    static TinyAlloc * name_alloc = 0;
    if (!name_alloc)
    {
        tal_new(&name_alloc, TOKSYM_TAL_LIMIT, TOKSYM_TAL_SIZE);
    }

    // alloc new name
    u32 size = sizeof(Name) + len;
    Name * name = tal_realloc_impl(&name_alloc, 0, 0, size);
    memset(name, 0, size);
   
    // copy str
    name->len = len;
    memcpy(name->str, str, len);

    // place in table and return
    
    name->i = g_nb_names;
    g_names[g_nb_names] = name;
    ++g_nb_names;
    
    Name * prev = h_to_name[h];
    if (prev)
    {
        prev->next = name;
    }
    else
    {
        h_to_name[h] = name;
    }
    
    return name;
}

Name * intern_leading_name(Span * span)
{
    i32 c = PEEK_CH_NO_STRAY(span);
    u8 * ident_start = span->p;
    while (is_id_num(c))
    {
        DROP_CH(span);
        c = PEEK_CH_NO_STRAY(span);
    }
    i32 len = span->p - ident_start;
    return intern_name((char *)ident_start, len);
}

static void audit_tok_(i32 tok, i32 tok_i_name)
{
    if (tok < -1)
    {
        die_with_line_number("(tok < -1)");
    }

    if (tok >= (TOK_NAME_MIN + g_nb_names))
    {
        die_with_line_number("(tok >= (TOK_NAME_MIN + g_nb_names))");
    }

    if (tok >= TOK_NAME_MIN)
    {
        if (tok_i_name != (tok - TOK_NAME_MIN))
        {
            die_with_line_number("1886");
        }

        return;
    }

    switch (tok)
    {
    case TOK_END_OF_REPLAY:
    case TOK_LINENUM:
    case TOK_EOF:
    case ' ':
    case TOK_PPNUM:
    case TOK_PPSTR:
    case TOK_LE:
    case TOK_A_SHL:
    case TOK_SHL:
    case TOK_LT:
    case TOK_GE:
    case TOK_A_SAR:
    case TOK_SAR:
    case TOK_GT:
    case TOK_LAND:
    case TOK_A_AND:
    case '&':
    case TOK_LOR:
    case TOK_A_OR:
    case '|':
    case TOK_INC:
    case TOK_A_ADD:
    case '+':
    case TOK_DEC:
    case TOK_A_SUB:
    case TOK_ARROW:
    case '-':
    case TOK_NE:
    case TOK_EQ:
    case TOK_A_MUL:
    case TOK_A_MOD:
    case TOK_A_XOR:
    case '!':
    case '=':
    case '*':
    case '%':
    case '^':
    case TOK_A_DIV:
    case '/':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
    case ';':
    case ':':
    case '?':
    case '~':
    case '.':
    case TOK_CCHAR:
    case TOK_STR:
    case TOK_CINT:
    case TOK_CLONG:
    case TOK_CLLONG:
    case TOK_CUINT:
    case TOK_CULONG:
    case TOK_CULLONG:
        return;
    }

    die_with_line_number("unknown tok!");
}

static i32 g_tok_ = 0;
static i32 g_tok_i_name = 0;
static i32 g_tok(void) //!!!1 audit uses
{
    audit_tok_(g_tok_, g_tok_i_name);
    return g_tok_;
}

static void audit_g_tok(void)
{
    audit_tok_(g_tok_, g_tok_i_name);
}

static void set_g_tok_tag(i32 tag)
{
    g_tok_ = tag;
    g_tok_i_name = -1;
    audit_g_tok();
}

static i32 g_tok_is(i32 tag)
{
    audit_tok_(tag, -1);
    audit_g_tok();

    return (g_tok_ == tag);
}

static i32 g_tok_is_kw(i32 tag)
{
    if (tag < -1)
    {
        die_with_line_number("1989");
    }

    if (tag >= (TOK_NAME_MIN + g_nb_names))
    {
        die_with_line_number("1994");
    }

    if (tag < TOK_NAME_MIN)
    {
        die_with_line_number("1999");
    }

    if (tag >= TOK_KEYWORD_MAX)
    {
        die_with_line_number("2004");
    }

    audit_tok_(tag, tag - TOK_NAME_MIN);
    audit_g_tok();

    return (g_tok_ == tag);
}

static Span * g_span = 0;
static TCCState * tcc_state = {0};
static CString tokcstr = {0}; // current parsed string, if any
static CValue tokc = {0};
static inline void next_pp_noreplay(void)
{
    skip_spaces_and_newlines(g_span);

    i32 c = PEEK_CH_NO_STRAY(g_span);
    if (c == CH_EOF)
    {
        set_g_tok_tag(TOK_EOF);
    }
    else if (c == 0x09)
    {
        die_with_line_number("TAB!!!");
    }
    else if (c == '#')
    {
        // we treat pp directives as fancy line comments
        parse_line_comment__(g_span);
        set_g_tok_tag(' ');
    }
    else if (isid(c))
    {
        Name * name = intern_leading_name(g_span);
        g_tok_ = name->i + TOK_NAME_MIN;
        g_tok_i_name = name->i;
        audit_g_tok();
    }
    else if (is_in_range(c, '0', '9'))
    {
        parse_ppnum(g_span, &tokcstr);
        tokc.str.size = tokcstr.size;
        tokc.str.data = tokcstr.data;
        set_g_tok_tag(TOK_PPNUM);
    }
    else if ((c == '\'') || (c == '"'))
    {
        parse_pp_string_(g_span, c, &tokcstr);
        tokc.str.size = tokcstr.size;
        tokc.str.data = tokcstr.data;
        set_g_tok_tag(TOK_PPSTR);
    }
    else if (c == '/')
    {
        DROP_CH(g_span);
        c = PEEK_CH_NO_STRAY(g_span);
        if (c == '/')
        {
            parse_line_comment__(g_span);
            set_g_tok_tag(' ');
        }
        else if (c == '=')
        {
            DROP_CH(g_span);
            set_g_tok_tag(TOK_A_DIV);
        }
        else
        {
            set_g_tok_tag('/');
        }
    }
    else if (is_sentinel_char(c))
    {
        set_g_tok_tag(c);
        DROP_CH(g_span);
    }
    else
    {
        u8 * operator_start = g_span->p;
        while (is_operator_char(PEEK_CH_NO_STRAY(g_span)))
        {
            DROP_CH(g_span);
        }
        u8 * operator_end = g_span->p;
        if (operator_start == operator_end)
        {
            die_with_line_number("unrecognized character");
        }
        u32 len_leading_operator = operator_end - operator_start;

        operator_token operators[] = 
        {
            {"<=", TOK_LE },
            {"<<=", TOK_A_SHL },
            {"<<", TOK_SHL },
            {"<", TOK_LT },
            {">=", TOK_GE },
            {">>=", TOK_A_SAR },
            {">>", TOK_SAR },
            {">", TOK_GT },
            {"&&", TOK_LAND },
            {"&=", TOK_A_AND },
            {"&", '&' },
            {"||", TOK_LOR },
            {"|=", TOK_A_OR },
            {"|", '|' },
            {"++", TOK_INC },
            {"+=", TOK_A_ADD },
            {"+", '+' },
            {"--", TOK_DEC },
            {"-=", TOK_A_SUB },
            {"->", TOK_ARROW },
            {"-", '-' },
            {"!=", TOK_NE },
            {"!", '!' },
            {"==", TOK_EQ },
            {"=", '=' },
            {"*=", TOK_A_MUL },
            {"*", '*' },
            {"%=", TOK_A_MOD },
            {"%", '%' },
            {"^=", TOK_A_XOR },
            {"^", '^' },
            {":", ':' },
            {"?", '?' },
            {"~", '~' },
            {".", '.' },
            {"", 0}, // the end
        };
       
        int i_operator = 0;
        for (;;)
        {
            char * str_operator = operators[i_operator].str;
            if (!str_operator[0])
            {
                break;
            }

            if (strlen_(str_operator) != len_leading_operator)
            {
                ++i_operator;
                continue;
            }

            if (strncmp_(str_operator, operator_start, len_leading_operator) == 0)
            {
                set_g_tok_tag(operators[i_operator].tok);
                audit_g_tok();
                return;
            }

            ++i_operator;
        }

        print_error_with_line_number("unrecognized operator");
    }

    audit_g_tok();
}

// return next pp token. Can re_ad input from
// replay ptr buffer
static i32 * replay_ptr = 0;
static void next_pp_replay(void)
{
    if (replay_ptr)
    {
    redo:

        g_tok_i_name = -1;
        g_tok_ = *replay_ptr;
        if (g_tok_ >= TOK_NAME_MIN)
        {
            ++replay_ptr;
            g_tok_i_name = *replay_ptr;
        }
        
        audit_g_tok();
        if (g_tok())
        {
            ++replay_ptr;
            switch (g_tok())
            {
            case TOK_CLONG:
            case TOK_CINT:
            case TOK_CCHAR:
            case TOK_LINENUM:
                tokc.i = *replay_ptr;
                ++replay_ptr;
                break;
            case TOK_CULONG:
            case TOK_CUINT:
                tokc.i = (u32)*replay_ptr;
                ++replay_ptr;
                break;
            case TOK_STR:
            case TOK_PPNUM:
            case TOK_PPSTR:
                tokc.str.size = *replay_ptr;
                ++replay_ptr;
                tokc.str.data = replay_ptr;
                replay_ptr += (tokc.str.size + sizeof(i32) - 1) / sizeof(i32);
                break;
            case TOK_CLLONG:
            case TOK_CULLONG:;
                i32 n = 2;
                u32 * i_as_ints = tokc.i_as_ints;
                for (;;)
                {
                    *i_as_ints = *replay_ptr;
                    ++i_as_ints;
                    ++replay_ptr;
                    --n;
                    if (!n)
                        break;
                }
                break;
            default:
                break;
            }
            
            if (g_tok_is(TOK_LINENUM))
            {
                if (tokc.i > 100000)
                {
                    die("ACK linenum");
                }

                g_line_num = (i32)tokc.i;
                goto redo;
            }
        }
    }
    else
    {
        next_pp_noreplay();
    }
}

static void next_pp_replay_skip_spaces(void)
{
    for (;;)
    {
        next_pp_replay();
        if (!g_tok_is(' '))
            break;
    }
}

// token string handling
static inline void tok_str_new(TokenString * s)
{
    s->str = NULL;
    s->len = 0;
    s->lastlen = 0;
    s->allocated_len = 0;
    s->last_line_num = -1;
}

static TinyAlloc * tokstr_alloc = {0};
static TokenString * tok_str_alloc(void)
{
    TokenString * str = tal_realloc_impl(&tokstr_alloc, 0, 0, sizeof(TokenString));
    tok_str_new(str);
    return str;
}

static void tok_str_free_str(i32 * str)
{
    tal_free_impl(tokstr_alloc, str);
}

static void tok_str_free(TokenString * str)
{
    tok_str_free_str(str->str);
    tal_free_impl(tokstr_alloc, str);
}

static i32 * tok_str_realloc(TokenString * s, i32 new_size)
{
    i32 * str;
    i32 size;

    size = s->allocated_len;
    if (size < 16)
    {
        size = 16;
    }
    while (size < new_size)
    {
        size = size * 2;
    }
    if (size > s->allocated_len)
    {
        str = tal_realloc_impl(
            &tokstr_alloc,
            s->str,
            s->allocated_len * sizeof(i32),
            size * sizeof(i32)
        );
        s->allocated_len = size;
        s->str = str;
    }
    return s->str;
}

static void tok_str_add(TokenString * s, i32 t)
{
    i32 len;
    i32 *str;

    len = s->len;
    str = s->str;
    if (len >= s->allocated_len)
    {
        str = tok_str_realloc(s, len + 1);
    }
    str[len++] = t;
    s->len = len;
}

static TokenString * replay_stack = {0};
static void begin_replay(TokenString * str, i32 alloc)
{
    // XXX this whole 'replay' system is only used for
    //  1. string literals
    //  2. some lazy lable parsing
    //  3. inferred array sizes (i32 foo[] = {1,2,3}, type of foo == i32[3])
    //  would probably be better to get rid of,
    //  read whole damn file ito mem, and just legit reparse
    //  (or parse in a smarter way) to deal with those cases

    str->alloc = (char)alloc;
    str->prev = replay_stack;
    str->prev_ptr = replay_ptr;
    str->save_line_num = g_line_num;
    replay_ptr = str->str;
    replay_stack = str;
}

static void end_replay(void)
{
    TokenString * str = replay_stack;
    replay_stack = str->prev;
    replay_ptr = str->prev_ptr;
    g_line_num = str->save_line_num;
    if (str->alloc == 2)
    {
        str->alloc = 3; // just mark as finished
    }
    else
    {
        tok_str_free(str);
    }
}

static void tok_str_add2(
    TokenString * s, 
    i32 tok_tag,
    i32 tok_i_name,
    CValue * cv)
{
    i32 len;
    i32 *str;

    len = s->len;
    s->lastlen = len;
    str = s->str;

    // allocate space for worst case
    if ((len + TOK_MAX_SIZE) >= s->allocated_len)
    {
        str = tok_str_realloc(s, len + TOK_MAX_SIZE + 1);
    }
    str[len++] = tok_tag;
    switch (tok_tag)
    {
    case TOK_CINT:
    case TOK_CUINT:
    case TOK_CCHAR:
    case TOK_LINENUM:
    case TOK_CLONG:
    case TOK_CULONG:
        str[len++] = cv->i_as_ints[0];
        break;
    case TOK_PPNUM:
    case TOK_PPSTR:
    case TOK_STR:
        {
            // Insert the string into the i32 array.
            u32 nb_words =
                1 + ((cv->str.size + sizeof(i32) - 1) / sizeof(i32));
            if ((i32)(len + nb_words) >= s->allocated_len)
            {
                str = tok_str_realloc(s, len + nb_words + 1);
            }
            str[len] = cv->str.size;
            memcpy(&str[len + 1], cv->str.data, cv->str.size);
            len += nb_words;
        }
        break;
    case TOK_CLLONG:
    case TOK_CULLONG:
        str[len++] = cv->i_as_ints[0];
        str[len++] = cv->i_as_ints[1];
        break;
    default:
        break;
    }

    if (tok_tag >= TOK_NAME_MIN)
    {
        if (tok_i_name != (tok_tag - TOK_NAME_MIN))
        {
            die_with_line_number("2551");
        }

        str[len++] = tok_i_name;
    }

    s->len = len;
}

// add the current parse token in token string 's'
static void tok_str_add_tok(TokenString * s)
{
    CValue cval;

    // save line number info
    if (g_line_num != s->last_line_num)
    {
        s->last_line_num = g_line_num;
        cval.i = s->last_line_num;
        tok_str_add2(s, TOK_LINENUM, 0, &cval);
    }
    tok_str_add2(s, g_tok(), g_tok_i_name, &tokc);
}

// label lookup

static Sym * label_find(i32 v)
{
    v -= TOK_NAME_MIN;
    if ((u32)v >= (u32)g_nb_names)
    {
        return NULL;
    }
    return g_names[v]->label;
}

static Sym * sym_push2(Sym * * ps, i32 v, i32 t, i32 c);
static Sym * global_label_stack = 0;
static Sym * label_push(Sym * * ptop, i32 v, i32 flags)
{
    Sym * s;
    s = sym_push2(ptop, v, 0, 0);
    s->r = (u16)flags;
    Name * name = g_names[v - TOK_NAME_MIN];
    if (name->label)
    {
        die_with_line_number("lable shadowed");
    }
    name->label = s;
    return s;
}

// pop labels until element last is reached. Look if any labels are
// undefined. Define symbols if '&&label' was used.
static inline void sym_free(Sym * sym);
static void put_extern_sym(Sym * sym, Section * section, u32 value);
static Section * cur_text_section = {0}; // current section where function code is generated
static char * get_tok_str(i32 v, CValue * cv);
static void tcc_error_puts(char * fmt, char * msg);
static void label_pop(Sym * * ptop, Sym * slast)
{
    Sym * s;
    Sym *s1;
    s = *ptop;
    for (; s != slast; s = s1)
    {
        s1 = s->prev;
        if (s->r == LABEL_DECLARED)
        {
            print_error_with_line_number("label declared but not used");
            print_error_with_line_number(get_tok_str(s->v, NULL));
        }
        else if (s->r == LABEL_FORWARD)
        {
            tcc_error_puts("label '%s' used but not defined",
                get_tok_str(s->v, NULL));
        }
        else
        {
            if (s->c)
            {
                // define corresponding symbol.
                if (!cur_text_section)
                {
                    die_with_line_number("3090");
                }

                put_extern_sym(s, cur_text_section, s->jnext);
            }
        }
        // remove label
        g_names[s->v - TOK_NAME_MIN]->label = NULL;
        sym_free(s);
    }
    *ptop = slast;
}

static void next(void);

static void skip(i32 c);

// evaluate escape codes in a string.
static void parse_escape_string(CString * outstr, u8 * p)
{
    for (i32 i = 0; 1; ++i)
    {
        i32 c = p[i];
        if (c == 0)
            break;

        if (c == '\\')
        {
            ++i;
            c = p[i];
            if ((c != '\\') && (c != '\'') && (c != '\"'))
                die_with_line_number("'\\' expected");
        }

        cstr_ccat(outstr, c);
    }
    // add a trailing '\x00'
    cstr_ccat(outstr, 0);
}

static void parse_string(char * s, i32 len)
{
    enum
    {
        LEN_BUF = 1000,
    };

    u8 buf[LEN_BUF];
    u8 * p = buf;
    i32 sep;

    if (*s == 'L')
    {
        die_with_line_number("L str");
        return;
    }
    sep = *s++;
    len -= 2;
    if (len >= LEN_BUF)
    {
        p = mallocz_or_die(len + 1);
    }
    memcpy(p, s, len);
    p[len] = 0;

    cstr_reset(&tokcstr);
    parse_escape_string(&tokcstr, p);
    if (p != buf)
    {
        free(p);
    }

    if (sep == "'"[0])
    {
        i32 char_size;
        i32 i;
        i32 n;
        i32 c;
        // XXX: make it portable
        set_g_tok_tag(TOK_CCHAR);
        char_size = 1;
        n = (tokcstr.size / char_size) - 1;
        if (n < 1)
        {
            die_with_line_number("empty character constant");
        }
        if (n > 1)
        {
            print_error_with_line_number("multi-character character constant");
        }

        i = 0;
        c = 0;
        for (; i < n; ++i)
        {
            c = (c << 8) | ((char *)tokcstr.data)[i];
        }
        tokc.i = c;
    }
    else
    {
        tokc.str.size = tokcstr.size;
        tokc.str.data = tokcstr.data;
        set_g_tok_tag(TOK_STR);
    }

    audit_g_tok();
}

// parse number in null terminated string 'p' and return it in the
// current token
static char token_buf[STRING_MAX_SIZE + 1] = {0};
static void parse_number(char * p)
{
    i32 b;
    i32 t;
    i32 ch;
    char * q;

    // number
    q = token_buf;
    ch = *p++;
    t = ch;
    ch = *p++;
    *q++ = (char)t;
    b = 10;
    if (t == '0')
    {
        if ((ch == 'x') || (ch == 'X'))
        {
            q--;
            ch = *p++;
            b = 16;
        }
    }
    // parse all digits.
    while (1)
    {
        if ((ch >= 'a') && (ch <= 'f'))
        {
            t = ch - 'a' + 10;
        }
        else if ((ch >= 'A') && (ch <= 'F'))
        {
            t = ch - 'A' + 10;
        }
        else if (is_in_range(ch, '0', '9'))
        {
            t = ch - '0';
        }
        else
        {
            break;
        }
        if (t >= b)
        {
            break;
        }
        if (q >= (token_buf + STRING_MAX_SIZE))
        {
            die_with_line_number("number too long");
        }
        *q++ = (char)ch;
        ch = *p++;
    }

    u64 n;
    u64 n1;
    i32 lcount;
    i32 ucount;
    i32 ov = 0;
    char * p1;

    // integer number
    *q = 0;
    q = token_buf;
    n = 0;
    while (1)
    {
        t = *q++;
        // no need for checks except for base 10 / 8 errors
        if (t == 0)
        {
            break;
        }
        else if (t >= 'a')
        {
            t = t - 'a' + 10;
        }
        else if (t >= 'A')
        {
            t = t - 'A' + 10;
        }
        else
        {
            t = t - '0';
        }
        if (t >= b)
        {
            die_with_line_number("invalid digit");
        }
        n1 = n;
        n = (n * b) + t;
        // detect overflow
        u64 n_div_b = u64div(n, b);
        if ((n1 >= 0x1000000000000000ULL) && (n_div_b != n1))
        {
            ov = 1;
        }
    }

    // Determine the characteristics (unsigned and/or 64bit) the type of
    // the constant must have according to the constant suffix(es)
    ucount = 0;
    lcount = 0;
    p1 = p;
    for (;;)
    {
        t = toup(ch);
        if (t == 'L')
        {
            if (lcount >= 2)
            {
                die_with_line_number("three 'l's in integer constant");
            }
            if (lcount && (*(p - 1) != ch))
            {
                tcc_error_puts("incorrect integer suffix: %s", p1);
            }
            lcount++;
            ch = *p++;
        }
        else if (t == 'U')
        {
            if (ucount >= 1)
            {
                die_with_line_number("two 'u's in integer constant");
            }
            ucount++;
            ch = *p++;
        }
        else
        {
            break;
        }
    }

    // how many trailing Ls coorispond to 64 bits
    enum
    {
        lcount_64_bits = (LONG_SIZE_IS_4) ? 2 : 1,
    };

    // Determine if it needs 64 bits and/or unsigned in order to fit
    if ((ucount == 0) && (b == 10))
    {
        if (lcount < lcount_64_bits)
        {
            if (n >= 0x80000000U)
            {
                lcount = lcount_64_bits;
            }
        }
        if (n >= 0x8000000000000000ULL)
        {
            ov = 1;
            ucount = 1;
        }
    }
    else
    {
        if (lcount < lcount_64_bits)
        {
            if (n >= 0x100000000ULL)
            {
                lcount = lcount_64_bits;
            }
            else if (n >= 0x80000000U)
            {
                ucount = 1;
            }
        }
        if (n >= 0x8000000000000000ULL)
        {
            ucount = 1;
        }
    }

    if (ov)
    {
        print_error_with_line_number("integer constant overflow");
    }

    if ((lcount < 0) || (lcount > 2))
    {
        die_with_line_number("lcount!");
    }

    if ((ucount < 0) || (ucount > 1))
    {
        die_with_line_number("ucount!");
    }

    static i32 tok_from_ul[3][2] =
    {
        // ucount == 0, ucount == 1
        {  TOK_CINT   , TOK_CUINT   }, // lcount == 0
        {  TOK_CLONG  , TOK_CULONG  }, // lcount == 1
        {  TOK_CLLONG , TOK_CULLONG }, // lcount == 2
    };

    set_g_tok_tag(tok_from_ul[lcount][ucount]);

    tokc.i = n;

    if (b == 10)
    {

    }
    else if (b == 16)
    {
        if (n > 255)
        {
            //print_error_with_line_number("hex too big");
        }
    }
    else
    {
        die_with_line_number("HUH???");
    }

    if (ch)
    {
        die_with_line_number("invalid number");
    }

    audit_g_tok();
}

// return next token (and replay bookkeeping)
static void next(void)
{
redo:
    next_pp_replay_skip_spaces();

    // XXX the fact that we only end_replay from next (not next_pp_replay)
    //  is disturbing...

    if (replay_ptr)
    {
        if (g_tok_is(TOK_END_OF_REPLAY))
        {
            // end of replay
            end_replay();
            goto redo;
        }
    }

    // convert preprocessor tokens into C tokens
    if (g_tok_is(TOK_PPNUM))
    {
        parse_number((char *)tokc.str.data);
    }
    else if (g_tok_is(TOK_PPSTR))
    {
        parse_string((char *)tokc.str.data, tokc.str.size - 1);
    }
}

// push back current token and set current token to 'last_tok'. Only
// identifier case handled for labels.
static inline void unget_tok(i32 last_tok, i32 last_tok_i_name)
{
    TokenString * str = tok_str_alloc();
    tok_str_add2(str, g_tok(), g_tok_i_name, &tokc);
    tok_str_add(str, TOK_END_OF_REPLAY);
    begin_replay(str, 1);
    g_tok_ = last_tok;
    g_tok_i_name = last_tok_i_name;
    audit_g_tok();
}

static SValue __vstack[1 + VSTACK_SIZE] = {{0}};
static SValue * vstack(void)
{
    return (__vstack + 1);
}

 // global variables
 // loc : local variable index 
 // ind : output code index 
 // rsym: return symbol 
 // anon_sym: anonymous symbol index
static i32 rsym = {0};
static i32 anon_sym = {0}; //!!!
static i32 ind = {0};
static i32 loc = {0};

static i32 in_sizeof = {0};
static i32 section_sym = {0};

static CType ptrdiff_type = {0};

// -------------------------------------------------------------------------
static void gen_cast(CType * type);
static void gen_cast_s(i32 t);
static inline CType * pointed_type(CType * type);
static i32 is_compatible_types(CType * type1, CType * type2);
static i32 parse_btype(CType * type, AttributeDef * ad);
static CType * type_decl(CType * type, AttributeDef * ad, i32 * v, i32 td);
static void init_putv(CType * type, Section * sec, u32 c);
static void decl_initializer(CType * type, Section * sec, u32 c, i32 first, i32 size_only);

static void block(i32 * bsym, i32 * csym, switch_t * cur_switch);

static void decl_initializer_alloc(CType * type, AttributeDef * ad, i32 r, i32 has_init, i32 v);
static void decl(i32 l);
static void decl0(i32 l, Sym * sym);
static i32 is_compatible_unqualified_types(CType * type1, CType * type2);

static SValue * vtop = 0;
static void test_lvalue(void)
{
    if (!(vtop->r & VT_LVAL))
    {
        expect("lvalue");
    }
}

static SValue * pvtop = 0;
static void check_vstack(void)
{
    if (pvtop != vtop)
    {
        die_with_line_number("internal compiler error: vstack leak");
    }
}

// -------------------------------------------------------------------------
// parse a function defined by symbol 'sym' and generate its code in// 'cur_text_section'
static void sym_pop(Sym * * ptop, Sym * b);
static void gfunc_prolog(
    CType * func_type,
    u32 * func_sub_sp_offset,
    i32 * func_ret_sub);
static void gsym(i32 t);
static void gfunc_epilog(
    u32 func_sub_sp_offset,
    i32 func_ret_sub);
static Section text_section = {0};
static i32 nocode_wanted = 0; // true if no code generation wanted for an expression
static char * funcname = {0};
static Sym * local_stack = 0;
static CType func_vt = {0}; // current function return type (used by return instruction)
static TYPE_VAL T_VOID = {VT_VOID};
static void gen_function(Sym * sym)
{
    if (cur_text_section)
    {
        die_with_line_number("huh, nested gen_function???");
    }

    cur_text_section = &text_section;
    {
        nocode_wanted = 0;
        ind = cur_text_section->bytes.size;
        // NOTE: we patch the symbol size later
        put_extern_sym(sym, cur_text_section, ind);
        funcname = get_tok_str(sym->v, NULL);
        // push a dummy symbol to enable local sym storage
        sym_push2(&local_stack, SYM_FIELD, 0, 0);

        u32 func_sub_sp_offset = 0;
        i32 func_ret_sub = 0;

        gfunc_prolog(&sym->type, &func_sub_sp_offset, &func_ret_sub);

        rsym = 0;
        block(NULL, NULL, NULL);
        nocode_wanted = 0;
        gsym(rsym);
        gfunc_epilog(func_sub_sp_offset, func_ret_sub);
        cur_text_section->bytes.size = ind;
        label_pop(&global_label_stack, NULL);
        // reset local stack
        sym_pop(&local_stack, NULL);
        // end of function
    }
    cur_text_section = NULL;

    funcname = ""; // for safety
    func_vt._tv_ = T_VOID; // for safety
    ind = 0; // for safety
    nocode_wanted = 0x80000000;
    check_vstack();
}

// -------------------------------------------------------------------------
static Symbols symtab_section = {0};
static Elf32_Sym * elfsym(Sym * s)
{
    if (!s || !s->c)
    {
        return NULL;
    }
    return &((Elf32_Sym *)symtab_section.syms)[s->c];
}

static char * symtab_name_of_sym(Elf32_Sym * sym)
{
    char * name = (char *)symtab_section.names + sym->st_name;
    return name;
}

static u32 UINT32_MAX = 0xffffffffU;

static u32 align_upwards(
    u32 address,
    u32 alignment)
{
    // die if alignment is zero,
    //  since it is not obvious what the
    //  correct semantics are
    if (alignment == 0)
    {
        die_with_line_number("zero alignment");
    }

    // no-op check
    u32 remainder = address % alignment;
    if (remainder == 0)
        return address;

    // compute adjustment up to next aligned block
    u32 adjustment = alignment - remainder;

    // check for overflow
    u32 available = UINT32_MAX - address;
    if (adjustment > available)
    {
        die_with_line_number("alignment overflow");
    }

    // return adjusted address
    return (address + adjustment);
}

static void data_ensure_cap(
    u8 * * p_data,
    u32 * p_data_allocated,
    u32 new_size)
{
    if (new_size <= *p_data_allocated)
        return;

    u32 size;
    u8 * data;

    size = *p_data_allocated;
    if (size == 0)
    {
        size = 1;
    }
    while (size < new_size)
    {
        size = size * 2;
    }
    data = realloc_or_die(
        *p_data,
        *p_data_allocated,
        size
    );
    memset(data + *p_data_allocated, 0, size - *p_data_allocated);
    *p_data = data;
    *p_data_allocated = size;
}

static u32 data_add(
    u32 * p_data_offset,
    u8 * * p_data,
    u32 * p_data_allocated,
    u32 size,
    i32 align)
{
    u32 begin = align_upwards(*p_data_offset, align);
    u32 end = begin + size;

    data_ensure_cap(p_data, p_data_allocated, end);

    *p_data_offset = end;
    return begin;
}

static void * data_ptr_add(
    u32 * p_data_offset,
    u8 * * p_data,
    u32 * p_data_allocated,
    u32 size)
{
    u32 offset = data_add(
        p_data_offset,
        p_data,
        p_data_allocated,
        size,
        1);

    return (*p_data + offset);
}

// return the symbol number
static i32 put_data_str(
    u32 * p_data_offset,
    u8 * * p_data,
    u32 * p_data_allocated,
    char * sym);
static i32 put_elf_sym(
    Symbols * s,
    char * name,
    put_elf_sym_ARGS args)
{
    Elf32_Sym * sym = data_ptr_add(
        &s->c_syms,
        &s->syms,
        &s->c_syms_allocated,
        sizeof(Elf32_Sym)
    );

    i32 name_offset = put_data_str(
        &s->c_names,
        &s->names,
        &s->c_names_allocated,
        name
    );

    sym->st_name = name_offset;
    sym->st_value = args.value;
    sym->is_func = args.is_func;
    sym->section = args.section;

    i32 sym_index = sym - (Elf32_Sym *)s->syms;
    return sym_index;
}

// -------------------------------------------------------------------------
// update sym->c so that it points to an external symbol in section// 'section' with value 'value'
static void put_extern_sym(
    Sym * sym,
    Section * section,
    u32 value)
{
    i32 is_func = 0;
    TYPE_VAL _tv_;
    Elf32_Sym * esym;
    char * name;
    //char buf1[256];

    if (!sym->c)
    {
        name = get_tok_str(sym->v, NULL);
        _tv_ = sym->type._tv_;
        if (BTYPES_MATCH(_tv_, VT_FUNC))
        {
            is_func = 1;
        }

        put_elf_sym_ARGS args;
        memset(&args, 0, sizeof(put_elf_sym_ARGS));

        args.value = value;
        args.is_func = is_func;
        args.section = section;

        sym->c = put_elf_sym(&symtab_section, name, args);
    }
    else
    {
        esym = elfsym(sym);
        esym->st_value = value;
        esym->section = section;
    }
}

static void bytes_ensure_size(
    Bytes * bytes, // too many things call this... maybe it should be wrapped more?
    u32 size)
{
    // already have enough size?
    // XXX this is weird...
    if (size <= bytes->size)
        return;

    // set the size
    bytes->size = size;

    // do we need to grow?
    if (bytes->size <= bytes->cap)
        return;

    // compute new cap. we handle cap == 0 initialy.
    u32 old_cap = bytes->cap;
    if (bytes->cap == 0)
    {
        bytes->cap = 1;
    }
    while (bytes->cap < size)
    {
        bytes->cap *= 2;
    }

    // realloc
    bytes->p = realloc_or_die(
        bytes->p,
        old_cap,
        bytes->cap
    );

    // clear newly allocated bytes
    memset(
        bytes->p + old_cap,
        0,
        bytes->cap - old_cap
    );
}

static void * append_new_bytes(Bytes * bytes, u32 add_size)
{
    u32 old_size = bytes->size;
    bytes_ensure_size(bytes, old_size + add_size);
    void * p = bytes->p + old_size;
    memset(p, 0, add_size);
    return p;
}

static void * items_add_new(
    Items * items,
    u32 item_size)
{
    if (!items->c && items->item_size)
    {
        die_with_line_number("items->item_size is supposed to start at 0");
    }

    if (items->c && (items->item_size != item_size))
    {
        die_with_line_number("mismatched item_size in items_add_new");
    }

    items->item_size = item_size;

    void * item = append_new_bytes(&items->_bytes, item_size);
    ++items->c;

    return item;
}

static Relocation * add_relocation(Section * s)
{
    return items_add_new(
        &s->relocs.u._items,
        sizeof(Relocation)
    );
}

// add a new relocation entry to symbol 'sym' in section 's'
// put relocation
static void put_elf_reloc(
    Section * s,
    u32 offset,
    i32 symbol,
    i32 is_pc_relative)
{
    Relocation * rel;
    rel = add_relocation(s);
    rel->offset = offset;
    rel->sym = symbol;
    rel->is_pc_relative = is_pc_relative;
}

static void put_elf_reloc_direct(
    Section * s,
    u32 offset,
    i32 symbol)
{
    put_elf_reloc(s, offset, symbol, 0);
}

static void greloc(Section * s, Sym * sym, u32 offset, i32 is_pc_relative)
{
    i32 c = 0;

    if (nocode_wanted && (s == cur_text_section))
    {
        return;
    }

    if (sym)
    {
        if (0 == sym->c)
        {
            put_extern_sym(sym, NULL, 0);
        }
        c = sym->c;
    }

    // now we can add ELF relocation info
    put_elf_reloc(s, offset, c, is_pc_relative);
}

static void greloc_direct(Section * s, Sym * sym, u32 offset)
{
    greloc(s, sym, offset, 0);
}

static void greloc_pc_relative(Section * s, Sym * sym, u32 offset)
{
    greloc(s, sym, offset, 1);
}

// -------------------------------------------------------------------------
// symbol allocator
static Sym * __sym_malloc(void)
{
    static i32 done_it = 0;
    if (done_it)
    {
        die_with_line_number("__sym_malloc!!!");
    }
    done_it = 1;

    enum
    {
        SYM_POOL_NB = 128 * 28
    };

    Sym * sym_pool;
    Sym * sym;
    Sym *last_sym;
    i32 i;

    sym_pool = mallocz_or_die(SYM_POOL_NB * sizeof(Sym));
    dynarray_add_(&tcc_state->sym_pools, &tcc_state->nb_sym_pools, sym_pool);

    last_sym = tcc_state->sym_free_first;
    sym = sym_pool;
    i = 0;
    for (; i < SYM_POOL_NB; i++)
    {
        sym->next = last_sym;
        last_sym = sym;
        sym++;
    }
    tcc_state->sym_free_first = last_sym;
    return last_sym;
}

static inline Sym * sym_malloc(void)
{
    Sym * sym;
    sym = tcc_state->sym_free_first;
    if (!sym)
    {
        sym = __sym_malloc();
    }
    tcc_state->sym_free_first = sym->next;
    return sym;
}

static inline void sym_free(Sym * sym)
{
    sym->next = tcc_state->sym_free_first;
    tcc_state->sym_free_first = sym;
}

// push, without hashing
static Sym * sym_push2(Sym * * ps, i32 v, i32 t, i32 c)
{
    Sym * s;

    s = sym_malloc();
    memset(s, 0, sizeof(Sym));
    s->v = v;
    s->type._tv_.bits = t;
    s->c = c;
    // add in stack
    s->prev = *ps;
    *ps = s;
    return s;
}

// structure lookup
static inline Sym * struct_find(i32 v)
{
    v -= TOK_NAME_MIN;
    if ((u32)v >= (u32)g_nb_names)
    {
        return NULL;
    }
    return g_names[v]->structure;
}

// find an identifier
static inline Sym * sym_find(i32 v)
{
    v -= TOK_NAME_MIN;
    if ((u32)v >= (u32)g_nb_names)
    {
        return NULL;
    }
    return g_names[v]->identifier;
}

// push a given symbol on the symbol stack
static Sym * global_stack = 0;
static Sym * sym_push(i32 v, CType * type, i32 r, i32 c)
{
    Sym * s;
    Sym * *ps;

    if (local_stack)
    {
        ps = &local_stack;
    }
    else
    {
        ps = &global_stack;
    }
    s = sym_push2(ps, v, type->_tv_.bits, c);
    s->type.ref = type->ref;
    s->r = (u16)r;
    // don't record fields or anonymous symbols
    // XXX: simplify
    if (!(v & SYM_FIELD) && ((v & ~SYM_STRUCT) < SYM_FIRST_ANOM))
    {
        // record symbol in token array
        Name * name = g_names[(v & ~SYM_STRUCT) - TOK_NAME_MIN];
        if (v & SYM_STRUCT)
        {
            ps = &name->structure;
        }
        else
        {
            ps = &name->identifier;
        }
        if (*ps)
        {
            tcc_error_puts("redeclaration of '%s'",
                get_tok_str(v & ~SYM_STRUCT, NULL));
        }
        *ps = s;
    }
    return s;
}

// push a global identifier
static Sym * global_identifier_push(i32 v, i32 t, i32 c)
{
    Sym * s;
    s = sym_push2(&global_stack, v, t, c);
    // don't record anonymous symbol
    if (v < SYM_FIRST_ANOM)
    {
        Name * name = g_names[v - TOK_NAME_MIN];
        if (name->identifier)
        {
            die_with_line_number("identifier shadowed");
        }
        name->identifier = s;
    }
    return s;
}

// pop symbols until top reaches 'b'.  If KEEP is non-zero don't really
// pop them yet from the list, but do remove them from the token array.
static void sym_pop(Sym * * ptop, Sym * b)
{
    Sym * s;
    Sym * ss;
    Sym * *ps;
    i32 v;

    s = *ptop;
    while (s != b)
    {
        ss = s->prev;
        v = s->v;
        // remove symbol in token array
        // XXX: simplify
        if (!(v & SYM_FIELD) && ((v & ~SYM_STRUCT) < SYM_FIRST_ANOM))
        {
            Name * name = g_names[(v & ~SYM_STRUCT) - TOK_NAME_MIN];
            if (v & SYM_STRUCT)
            {
                ps = &name->structure;
            }
            else
            {
                ps = &name->identifier;
            }
            *ps = NULL;
        }
        sym_free(s);
        s = ss;
    }
    *ptop = b;
}

// -------------------------------------------------------------------------
static i32 gv(i32 rc);
static void vsetc(CType * type, i32 r, CValue * vc)
{
    i32 v;

    if (vtop >= (vstack() + (VSTACK_SIZE - 1)))
    {
        die_with_line_number("memory full (vstack())");
    }
    // cannot let cpu flags if other instruction are generated. 
    //  Also avoid leaving VT_JMP anywhere except on the top of the stack
    //  because it would complicate the code generator.
    //  Don't do this when nocode_wanted.  vtop might come from
    //  !nocode_wanted regions (see 88_codeopt.c) and transforming
    //  it to a register without actually generating code is wrong
    //  as their value might still be used for real.  All values
    //  we push under nocode_wanted will eventually be popped
    //  again, so that the VT_CMP/VT_JMP value will be in vtop
    //  when code is unsuppressed again.
    //  Same logic below in vswap();
    if ((vtop >= vstack()) && !nocode_wanted)
    {
        v = vtop->r & VT_VALMASK;
        if ((v == VT_CMP) || ((v & ~1) == VT_JMP))
        {
            gv(RC_INT);
        }
    }

    vtop++;
    vtop->type = *type;
    vtop->r = (u16)r;
    vtop->r2 = VT_CONST;
    vtop->c = *vc;
    vtop->sym = NULL;
}

static void vswap(void)
{
    SValue tmp;
    // cannot vswap cpu flags. See comment at vsetc() above
    if ((vtop >= vstack()) && !nocode_wanted)
    {
        i32 v = vtop->r & VT_VALMASK;
        if ((v == VT_CMP) || ((v & ~1) == VT_JMP))
        {
            gv(RC_INT);
        }
    }
    tmp = vtop[0];
    vtop[0] = vtop[-1];
    vtop[-1] = tmp;
}

// pop stack value
static void o(u32 c);
static void vpop(void)
{
    i32 v = vtop->r & VT_VALMASK;
    if ((v == VT_JMP) || (v == VT_JMPI))
    {
        // need to put correct jump if && or || without test
        // grumble grumble cast
        gsym((i32)vtop->c.i);
    }
    vtop--;
}

// push constant of type "type" with useless value
static void vset(CType * type, i32 r, i32 v);
static void vpush(CType * type)
{
    vset(type, VT_CONST, 0);
}

// push integer constant
static CType int_type = {0};
static void vpushi(i32 v)
{
    CValue cval;
    cval.i = v;
    vsetc(&int_type, VT_CONST, &cval);
}

// push a pointer sized constant
static CType size_type = {0};
static void vpushs(u32 v)
{
    CValue cval;
    cval.i = v;
    vsetc(&size_type, VT_CONST, &cval);
}

static void vset(CType * type, i32 r, i32 v)
{
    CValue cval;

    cval.i = v;
    vsetc(type, r, &cval);
}

static void vseti(i32 r, i32 v)
{
    CType type;
    type._tv_.bits = VT_INT;
    type.ref = NULL;
    vset(&type, r, v);
}

static void vpushv(SValue * v)
{
    if (vtop >= (vstack() + (VSTACK_SIZE - 1)))
    {
        die_with_line_number("memory full (vstack())");
    }
    vtop++;
    *vtop = *v;
}

static void vdup(void)
{
    vpushv(vtop);
}

// rotate n first stack elements to the bottom
// I1 ... In -> I2 ... In I1 [top is right]
//
static void vrotb(i32 n)
{
    i32 i;
    SValue tmp;

    i = -n + 1;
    tmp = vtop[i];
    for (; i != 0; i++)
    {
        i32 i_next = i + 1;
        vtop[i] = vtop[i_next];
    }
    vtop[0] = tmp;
}

// rotate the n elements before entry e towards the top
// I1 ... In ... -> In I1 ... I(n-1) ... [top is right]
//
static void vrote(SValue * e, i32 n)
{
    if (n < 2)
    {
        // rotation requires at least two elements
        die_with_line_number("vrote n < 2 ???");
    }

    // i_bottom : the 'lowest' value we will be rotating.
    // NOTE (matthewd) I find this to be a bit opaque, so here is a note:
    //  Consider the case of n = 2:
    //  e[0] is the top, and e[-1] is the value below that,
    //  so (1 - n) correctly gives us the 'bottom'.
    //  This generalizes to n > 2.
    //  Put another way, n is a 'count', but we want an 'index',
    //  so we need to offset by one. It just gets confusing
    //  because we want a NEGATIVE index.
    i32 i_bottom = 1 - n;

    // cache top value (will eventualy move to the bottom)
    SValue val_top = e[0];

    // move rest of the values up one
    i32 i_upper = 0;
    i32 i_lower = -1;
    while (i_upper > i_bottom)
    {
        e[i_upper] = e[i_lower];

        --i_upper;
        --i_lower;
    }

    // lastly, set bottom to the cached top
    e[i_bottom] = val_top;
}

// rotate n first stack elements to the top
// I1 ... In -> In I1 ... I(n-1)  [top is right]
//
static void vrott(i32 n)
{
    vrote(vtop, n);
}

// push a symbol value of TYPE
static inline void vpushsym(CType * type, Sym * sym)
{
    CValue cval;
    cval.i = 0;
    vsetc(type, VT_CONST | VT_SYM, &cval);
    vtop->sym = sym;
}

// Return a static symbol pointing to a section
static Sym * get_sym_ref(CType * type, Section * sec, u32 offset)
{
    i32 v;
    Sym * sym;

    v = anon_sym++;
    sym = global_identifier_push(v, type->_tv_.bits | VT_STATIC, 0);
    sym->type.ref = type->ref;
    sym->r = VT_CONST | VT_SYM;
    put_extern_sym(sym, sec, offset);
    return sym;
}

// define a new external reference to a symbol 'v' of type 'u'
static Sym * external_global_sym(
    i32 v,
    CType * type,
    i32 r,
    i32 * is_fresh)
{
    Sym * s;

    if (is_fresh)
    {
        *is_fresh = 0;
    }

    s = sym_find(v);
    if (!s)
    {
        if (is_fresh)
        {
            *is_fresh = 1;
        }
        // push forward reference
        s = global_identifier_push(v, type->_tv_.bits | VT_EXTERN, 0);
        s->type.ref = type->ref;
        s->r = (u16)(r | VT_CONST | VT_SYM);
    }
    return s;
}

// Merge some type attributes.
static void patch_type(Sym * sym, CType * type, i32 is_fresh)
{
    if (!is_fresh)
    {
        //!!! enable this
        //tcc_error_noabort("redefinitions not allowed");
    }

    if (!(type->_tv_.bits & VT_EXTERN))
    {
        if (!(sym->type._tv_.bits & VT_EXTERN))
        {
            tcc_error_puts("redefinition of '%s'", get_tok_str(sym->v, NULL));
        }
        sym->type._tv_.bits &= ~VT_EXTERN;
    }

    if (!is_compatible_types(&sym->type, type))
    {
        tcc_error_puts("incompatible types for redefinition of '%s'",
            get_tok_str(sym->v, NULL));
    }
    else if (BTYPES_MATCH(sym->type._tv_, VT_FUNC))
    {
        i32 static_proto = sym->type._tv_.bits & VT_STATIC;
        // warn if static follows non-static function declaration
        if ((type->_tv_.bits & VT_STATIC) && !static_proto && !(type->_tv_.bits & VT_INLINE))
        {
            print_error_with_line_number("static storage ignored for redefinition");
            die_with_line_number(get_tok_str(sym->v, NULL));
        }

        if (0 == (type->_tv_.bits & VT_EXTERN))
        {
            // put complete type, use static from prototype
            sym->type._tv_.bits = (type->_tv_.bits & ~VT_STATIC) | static_proto;
            if (type->_tv_.bits & VT_INLINE)
            {
                sym->type._tv_ = type->_tv_;
            }
            sym->type.ref = type->ref;
        }
    }
    else
    {
        if ((sym->type._tv_.bits & VT_ARRAY) && (type->ref->c >= 0))
        {
            // set array size if it was omitted in extern declaration
            if (sym->type.ref->c < 0)
            {
                sym->type.ref->c = type->ref->c;
            }
            else if (sym->type.ref->c != type->ref->c)
            {
                tcc_error_puts("conflicting type for '%s'", get_tok_str(sym->v, NULL));
            }
        }
        if ((type->_tv_.bits ^ sym->type._tv_.bits) & VT_STATIC)
        {
            print_error_with_line_number("storage mismatch for redefinition");
            print_error_with_line_number(get_tok_str(sym->v, NULL));
        }
    }
}

// Merge some storage attributes.
static void patch_storage(Sym * sym, AttributeDef * ad, CType * type, i32 is_fresh)
{
    (void)ad;

    if (type)
    {
        patch_type(sym, type, is_fresh);
    }
}

// define a new external reference to a symbol 'v'
static Sym * external_sym(i32 v, CType * type, i32 r, AttributeDef * ad)
{
    Sym * s;
    s = sym_find(v);
    if (!s)
    {
        // push forward reference
        s = sym_push(v, type, r | VT_CONST | VT_SYM, 0);
        s->type._tv_.bits |= VT_EXTERN;
    }
    else
    {
        patch_storage(s, ad, type, 0);
    }
    return s;
}

// save registers up to (vtop - n) stack entry
static void save_reg(i32 r);
static void save_regs(i32 n)
{
    SValue * p;
    SValue *p1;
    p = vstack();
    p1 = vtop - n;
    for (; p <= p1; p++)
    {
        save_reg(p->r);
    }
}

// save r to the memory stack, and mark it as being free
static void save_reg_upstack(i32 r, i32 n);
static void save_reg(i32 r)
{
    save_reg_upstack(r, 0);
}

// save r to the memory stack, and mark it as being free,
// if seen up to (vtop - n) stack entry
static i32 type_size(CType * type, i32 * a);
static i32 lvalue_type(i32 t);
static void store(i32 r, SValue * v);
static void save_reg_upstack(i32 r, i32 n)
{
    i32 l;
    i32 saved;
    i32 size;
    i32 align;
    SValue * p;
    SValue * p1;
    SValue sv;
    CType * type;

    r &= VT_VALMASK;
    if (r >= VT_CONST)
    {
        return;
    }
    if (nocode_wanted)
    {
        return;
    }

    // modify all stack values
    saved = 0;
    l = 0;
    p = vstack();
    p1 = vtop - n;
    for (; p <= p1; p++)
    {
        if (
            ((p->r & VT_VALMASK) == r) ||
            (
                (BTYPES_MATCH(p->type._tv_, VT_LLONG)) &&
                ((p->r2 & VT_VALMASK) == r)
            )
        )
        {
            // must save value on stack if not already done
            if (!saved)
            {
                // NOTE: must reload 'r' because r might be equal to r2
                r = p->r & VT_VALMASK;
                // store register in the stack
                type = &p->type;
                if ((p->r & VT_LVAL) ||
                    (!BTYPES_MATCH(type->_tv_, VT_LLONG)))
                {
                    type = &int_type;
                }
                size = type_size(type, &align);
                loc = (loc - size) & -align;
                sv.type._tv_ = type->_tv_;
                sv.r = VT_LOCAL | VT_LVAL;
                sv.c.i = loc;
                store(r, &sv);
                // special i64 case
                if (BTYPES_MATCH(type->_tv_, VT_LLONG))
                {
                    sv.c.i += 4;
                    store(p->r2, &sv);
                }
                l = loc;
                saved = 1;
            }
            // mark that stack entry as being saved on the stack
            if (p->r & VT_LVAL)
            {
                // also clear the bounded flag because the // relocation address of the function was stored in // p->c.i
                p->r = (p->r & ~(VT_VALMASK | VT_BOUNDED)) | VT_LLOCAL;
            }
            else
            {
                p->r = (u16)(lvalue_type(p->type._tv_.bits) | VT_LOCAL);
            }
            p->r2 = VT_CONST;
            p->c.i = l;
        }
    }
}

// find a free register of class 'rc'. If none, save one register
static i32 reg_classes[NB_REGS] = {
    RC_INT | RC_EAX, // eax
    RC_INT | RC_ECX, // ecx
    RC_INT | RC_EDX, // edx
    0, // ebx
};
static i32 get_reg(i32 rc)
{
    i32 r;
    SValue * p;

    // find a free register
    r = 0;
    for (; r < NB_REGS; r++)
    {
        if (reg_classes[r] & rc)
        {
            if (nocode_wanted)
            {
                return r;
            }
            p = vstack();
            for (; p <= vtop; p++)
            {
                if (((p->r & VT_VALMASK) == r) ||
                    ((p->r2 & VT_VALMASK) == r))
                {
                    goto notfound;
                }
            }
            return r;
        }
    notfound:;
    }

    // no register left : free the first one on the stack (VERY
    // IMPORTANT to start from the bottom to ensure that we don't
    // spill registers used in gen_opi())
    p = vstack();
    for (; p <= vtop; p++)
    {
        // look at second register (if i64)
        r = p->r2 & VT_VALMASK;
        if ((r < VT_CONST) && (reg_classes[r] & rc))
        {
            goto save_found;
        }
        r = p->r & VT_VALMASK;
        if ((r < VT_CONST) && (reg_classes[r] & rc))
        {
        save_found:
            save_reg(r);
            return r;
        }
    }
    // Should never comes here
    return -1;
}

// move register 's' (of type 't') to 'r', and flush previous value of r to memory
// if needed
static void load(i32 r, SValue * sv);
static void move_reg(i32 r, i32 s, i32 t)
{
    SValue sv;

    if (r != s)
    {
        save_reg(r);
        sv.type._tv_.bits = t;
        sv.type.ref = NULL;
        sv.r = (u16)s;
        sv.c.i = 0;
        load(r, &sv);
    }
}

// get address of vtop (vtop MUST BE an lvalue)
static void gaddrof(void)
{
    vtop->r &= ~VT_LVAL;
    // tricky: if saved lvalue, then we can go back to lvalue
    if ((vtop->r & VT_VALMASK) == VT_LLOCAL)
    {
        vtop->r = (vtop->r & ~(VT_VALMASK | VT_LVAL_TYPE)) | VT_LOCAL | VT_LVAL;
    }
}

// expand 64bit on stack in two ints
static void lexpand(void)
{
    i32 u;
    i32 v;
    u = vtop->type._tv_.bits & (VT_DEFSIGN | VT_UNSIGNED);
    v = vtop->r & (VT_VALMASK | VT_LVAL);
    if (v == VT_CONST)
    {
        vdup();
        vtop[0].c.i >>= 32;
    }
    else if (
        (v == (VT_LVAL | VT_CONST)) ||
        (v == (VT_LVAL | VT_LOCAL))
    )
    {
        vdup();
        vtop[0].c.i += 4;
    }
    else
    {
        gv(RC_INT);
        vdup();
        vtop[0].r = vtop[-1].r2;
        vtop[-1].r2 = VT_CONST;
        vtop[0].r2 = VT_CONST;
    }
    vtop[-1].type._tv_.bits = VT_INT | u;
    vtop[0].type._tv_ = vtop[-1].type._tv_;
}

// build a i64 from two ints
static void gv2(i32 rc1, i32 rc2);
static void lbuild(i32 t)
{
    gv2(RC_INT, RC_INT);
    vtop[-1].r2 = vtop[0].r;
    vtop[-1].type._tv_.bits = t;
    vpop();
}

// convert stack entry to register and duplicate its value in another
// register
static void gv_dup(void)
{
    i32 rc;
    TYPE_VAL _tv_;
    i32 r;
    i32 r1;
    SValue sv;

    _tv_ = vtop->type._tv_;
    if (BTYPES_MATCH(_tv_, VT_LLONG))
    {
        lexpand();
        gv_dup();
        vswap();
        vrotb(3);
        gv_dup();
        vrotb(4);
        // stack: H L L1 H1
        lbuild(_tv_.bits);
        vrotb(3);
        vrotb(3);
        vswap();
        lbuild(_tv_.bits);
        vswap();
    }
    else
    {
        // duplicate value
        rc = RC_INT;
        sv.type._tv_.bits = VT_INT;
        r = gv(rc);
        r1 = get_reg(rc);
        sv.r = (u16)r;
        sv.c.i = 0;
        load(r1, &sv); // move r to r1
        vdup();
        // duplicates value
        if (r != r1)
        {
            vtop->r = (u16)r1;
        }
    }
}

static inline i32 NODATA_WANTED(void)
{
    return (nocode_wanted > 0);
}

// store vtop a register belonging to class 'rc'. lvalues are
// converted to values. Cannot be used if cannot be converted to
// register value (such as structures).
static void gen_op(i32 op);
static i32 gv(i32 rc)
{
    i32 r;
    i32 rc2;

    // NOTE: get_reg can modify vstack[]
    {
        r = vtop->r & VT_VALMASK;
        rc2 = RC_INT;

        if (rc == RC_IRET)
        {
            rc2 = RC_LRET;
        }
        // need to reload if: // - constant // - lvalue (need to dereference pointer) // - already a register, but not in the right class
        if ((r >= VT_CONST)
            || (vtop->r & VT_LVAL)
            || !(reg_classes[r] & rc)
            || ((BTYPES_MATCH(vtop->type._tv_, VT_LLONG)) && !(reg_classes[vtop->r2] & rc2))
            )
        {
            r = get_reg(rc);
            if (BTYPES_MATCH(vtop->type._tv_, VT_LLONG))
            {
                i32 addr_type = VT_INT;
                i32 load_size = 4;
                i32 load_type = VT_INT;
                u64 ll;
                i32 r2;
                TYPE_VAL _tv_;
                _tv_ = vtop->type._tv_;
                // two register type load : expand to two words // temporarily
                if ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST)
                {
                    // load constant
                    ll = vtop->c.i;
                    vtop->c.i = ll; // first word
                    load(r, vtop);
                    vtop->r = (u16)r; // save register value
                    vpushi(ll >> 32); // second word
                }
                else
                    if (vtop->r & VT_LVAL)
                    {
                        // We do not want to modifier the i64 // pointer here, so the safest (and less // efficient) is to save all the other registers // in the stack. XXX: totally inefficient.
                        // lvalue_save: save only if used further down the stack
                        save_reg_upstack(vtop->r, 1);
                        // load from memory
                        vtop->type._tv_.bits = load_type;
                        load(r, vtop);
                        vdup();
                        vtop[-1].r = (u16)r; // save register value
                        // increment pointer to get second word
                        vtop->type._tv_.bits = addr_type;
                        gaddrof();
                        vpushi(load_size);
                        gen_op('+');
                        vtop->r |= VT_LVAL;
                        vtop->type._tv_.bits = load_type;
                    }
                    else
                    {
                        // move registers
                        load(r, vtop);
                        vdup();
                        vtop[-1].r = (u16)r; // save register value
                        vtop->r = vtop[-1].r2;
                    }
                // Allocate second register. Here we rely on the fact that // get_reg() tries first to free r2 of an SValue.
                r2 = get_reg(rc2);
                load(r2, vtop);
                vpop();
                // write second register
                vtop->r2 = (u16)r2;
                vtop->type._tv_ = _tv_;
            }
            else if ((vtop->r & VT_LVAL))
            {
                TYPE_VAL _tv1_;
                TYPE_VAL _tv_;
                // lvalue of scalar type : need to use lvalue type // because of possible cast
                _tv_ = vtop->type._tv_;
                _tv1_ = _tv_;
                // compute memory access type
                if (vtop->r & VT_LVAL_BYTE)
                {
                    _tv_.bits = VT_BYTE;
                }
                else if (vtop->r & VT_LVAL_SHORT)
                {
                    _tv_.bits = VT_SHORT;
                }
                if (vtop->r & VT_LVAL_UNSIGNED)
                {
                    _tv_.bits |= VT_UNSIGNED;
                }
                vtop->type._tv_ = _tv_;
                load(r, vtop);
                // restore wanted type
                vtop->type._tv_ = _tv1_;
            }
            else
            {
                // one register type load
                load(r, vtop);
            }
        }
        vtop->r = (u16)r;
    }
    return r;
}

// generate vtop[-1] and vtop[0] in resp. classes rc1 and rc2
static void gv2(i32 rc1, i32 rc2)
{
    i32 v;

    // generate more generic register first. But VT_JMP or VT_CMP
    // values must be generated first in all cases to avoid possible
    // reload errors
    v = vtop[0].r & VT_VALMASK;
    if ((v != VT_CMP) && ((v & ~1) != VT_JMP) && (rc1 <= rc2))
    {
        vswap();
        gv(rc1);
        vswap();
        gv(rc2);
        // test if reload is needed for first register
        if ((vtop[-1].r & VT_VALMASK) >= VT_CONST)
        {
            vswap();
            gv(rc1);
            vswap();
        }
    }
    else
    {
        gv(rc2);
        vswap();
        gv(rc1);
        vswap();
        // test if reload is needed for first register
        if ((vtop[0].r & VT_VALMASK) >= VT_CONST)
        {
            gv(rc2);
        }
    }
}

// Generate value test
//
// Generate a test for any value (jump, comparison and integers) */
static i32 gjmp(i32 t);
static i32 gtst(i32 inv, i32 t);
static i32 gvtst(i32 inv, i32 t)
{
    i32 v = vtop->r & VT_VALMASK;
    if ((v != VT_CMP) && (v != VT_JMP) && (v != VT_JMPI))
    {
        vpushi(0);
        gen_op(TOK_NE);
    }
    if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
    {
        // constant jmp optimization
        if ((vtop->c.i != 0) != inv)
        {
            t = gjmp(t);
        }
        vtop--;
        return t;
    }
    return gtst(inv, t);
}

// generate CPU independent (unsigned) i64 operations
static void gen_opl(i32 op)
{
    TYPE_VAL _tv_;
    i32 a;
    i32 b;
    i32 op1;
    i32 c;
    i32 i;
    SValue tmp;

    switch (op)
    {
    case '/':
    case TOK_PDIV:
        die_with_line_number("__divdi3!!!");
    case TOK_UDIV:
        die_with_line_number("TOK___udivdi3_"); // prevents compiling bs1, but ok
    case '%':
        die_with_line_number("__moddi3!!!");
    case TOK_UMOD:
        die_with_line_number("TOK___umoddi3_");

    case '^':
    case '&':
    case '|':
    case '*':
    case '+':
    case '-':
        //pv("gen_opl A",0,2);
        _tv_ = vtop->type._tv_;
        vswap();
        lexpand();
        vrotb(3);
        lexpand();
        // stack: L1 H1 L2 H2
        tmp = vtop[0];
        vtop[0] = vtop[-3];
        vtop[-3] = tmp;
        tmp = vtop[-2];
        vtop[-2] = vtop[-3];
        vtop[-3] = tmp;
        vswap();
        // stack: H1 H2 L1 L2
        //pv("gen_opl B",0,4);
        if (op == '*')
        {
            vpushv(vtop - 1);
            vpushv(vtop - 1);
            gen_op(TOK_UMULL);
            lexpand();
            // stack: H1 H2 L1 L2 ML MH
            i = 0;
            for (; i < 4; i++)
            {
                vrotb(6);
            }
            // stack: ML MH H1 H2 L1 L2
            tmp = vtop[0];
            vtop[0] = vtop[-2];
            vtop[-2] = tmp;
            // stack: ML MH H1 L2 H2 L1
            gen_op('*');
            vrotb(3);
            vrotb(3);
            gen_op('*');
            // stack: ML MH M1 M2
            gen_op('+');
            gen_op('+');
        }
        else if ((op == '+') || (op == '-'))
        {
            // XXX: add non carry method too (for MIPS or alpha)
            if (op == '+')
            {
                op1 = TOK_ADDC1;
            }
            else
            {
                op1 = TOK_SUBC1;
            }
            gen_op(op1);
            // stack: H1 H2 (L1 op L2)
            vrotb(3);
            vrotb(3);

            if (op1 == TOK_ADDC1)
            {
                gen_op(TOK_ADDC2);
            }
            else
            {
                gen_op(TOK_SUBC2);
            }
        }
        else
        {
            gen_op(op);
            // stack: H1 H2 (L1 op L2)
            vrotb(3);
            vrotb(3);
            // stack: (L1 op L2) H1 H2
            gen_op(op);
            // stack: (L1 op L2) (H1 op H2)
        }
        // stack: L H
        lbuild(_tv_.bits);
        break;
    case TOK_SAR:
    case TOK_SHR:
    case TOK_SHL:
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
        {
            _tv_ = vtop[-1].type._tv_;
            vswap();
            lexpand();
            vrotb(3);
            // stack: L H shift
            c = (i32)vtop->c.i;
            // constant: simpler
            // NOTE: all comments are for SHL. the other cases are // done by swapping words
            vpop();
            if (op != TOK_SHL)
            {
                vswap();
            }
            if (c >= 32)
            {
                // stack: L H
                vpop();
                if (c > 32)
                {
                    vpushi(c - 32);
                    gen_op(op);
                }
                if (op != TOK_SAR)
                {
                    vpushi(0);
                }
                else
                {
                    gv_dup();
                    vpushi(31);
                    gen_op(TOK_SAR);
                }
                vswap();
            }
            else
            {
                vswap();
                gv_dup();
                // stack: H L L
                vpushi(c);
                gen_op(op);
                vswap();
                vpushi(32 - c);
                if (op == TOK_SHL)
                {
                    gen_op(TOK_SHR);
                }
                else
                {
                    gen_op(TOK_SHL);
                }
                vrotb(3);
                // stack: L L H
                vpushi(c);
                if (op == TOK_SHL)
                {
                    gen_op(TOK_SHL);
                }
                else
                {
                    gen_op(TOK_SHR);
                }
                gen_op('|');
            }
            if (op != TOK_SHL)
            {
                vswap();
            }
            lbuild(_tv_.bits);
        }
        else
        {
            if (op == TOK_SAR)
            {
                die_with_line_number("TOK___ashrdi3");
            }
            else if (op == TOK_SHR)
            {
                die_with_line_number("TOK___lshrdi3_");
            }
            else if (op == TOK_SHL)
            {
                die_with_line_number("TOK___ashldi3_");
            }
        }
        break;
    default:
        // compare operations
        _tv_ = vtop->type._tv_;
        vswap();
        lexpand();
        vrotb(3);
        lexpand();
        // stack: L1 H1 L2 H2
        tmp = vtop[-1];
        vtop[-1] = vtop[-2];
        vtop[-2] = tmp;
        // stack: L1 L2 H1 H2
        // compare high
        op1 = op;
        // when values are equal, we need to compare low words. since // the jump is inverted, we invert the test too.
        if (op1 == TOK_LT)
        {
            op1 = TOK_LE;
        }
        else if (op1 == TOK_GT)
        {
            op1 = TOK_GE;
        }
        else if (op1 == TOK_ULT)
        {
            op1 = TOK_ULE;
        }
        else if (op1 == TOK_UGT)
        {
            op1 = TOK_UGE;
        }
        a = 0;
        b = 0;
        gen_op(op1);
        if (op == TOK_NE)
        {
            b = gvtst(0, 0);
        }
        else
        {
            a = gvtst(1, 0);
            if (op != TOK_EQ)
            {
                // generate non equal test
                vpushi(TOK_NE);
                vtop->r = VT_CMP;
                b = gvtst(0, 0);
            }
        }
        // compare low. Always unsigned
        op1 = op;
        if (op1 == TOK_LT)
        {
            op1 = TOK_ULT;
        }
        else if (op1 == TOK_LE)
        {
            op1 = TOK_ULE;
        }
        else if (op1 == TOK_GT)
        {
            op1 = TOK_UGT;
        }
        else if (op1 == TOK_GE)
        {
            op1 = TOK_UGE;
        }
        gen_op(op1);
        a = gvtst(1, a);
        gsym(b);
        vseti(VT_JMPI, a);
        break;
    }
}

static _Bool t_needs_opl(i32 t)
{
    if (t == VT_LLONG)
        return 1;

    // Besides LLONG, only pointer might be 64 bit

    if (t != VT_PTR)
        return 0;

    // Check pointer size

    //if (PTR_SIZE_IS_4)
    //    return 0;

    //return 1;

    // PTR_SIZE_IS_4 == 1
    return 0;
}

u64 twocomp_unary_neg(u64 n)
{
    return ((~n) + 1);
}

// handle integer constant optimizations and various machine
// independent opt
static void gen_opi(i32 op);
static i32 const_wanted = 0; // true if constant wanted
static void gen_opic(i32 op) //!!!
{
    SValue * v1 = vtop - 1;
    SValue * v2 = vtop;
    i32 t1 = v1->type._tv_.bits & VT_BTYPE;
    i32 t2 = v2->type._tv_.bits & VT_BTYPE;
    i32 c1 = (v1->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
    i32 c2 = (v2->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
    u64 l1 = c1 ? v1->c.i : 0;
    u64 l2 = c2 ? v2->c.i : 0;
    i32 shm = (t1 == VT_LLONG) ? 63 : 31;

    if ((t1 != VT_LLONG) && (PTR_SIZE_IS_4 || (t1 != VT_PTR)))
    {
        l1 = ((u32)l1 |
            ((v1->type._tv_.bits & VT_UNSIGNED) ? 0 : twocomp_unary_neg(l1 & 0x80000000)));
    }
    if ((t2 != VT_LLONG) && (PTR_SIZE_IS_4 || (t2 != VT_PTR)))
    {
        l2 = ((u32)l2 |
            ((v2->type._tv_.bits & VT_UNSIGNED) ? 0 : twocomp_unary_neg(l2 & 0x80000000)));
    }

    if (c1 && c2)
    {
        switch (op)
        {
        case '+': l1 += l2; break;
        case '-': l1 -= l2; break;
        case '&': l1 &= l2; break;
        case '^': l1 ^= l2; break;
        case '|': l1 |= l2; break;
        case '*': l1 *= l2; break;

        case TOK_PDIV:
        case '/':
        case '%':
        case TOK_UDIV:
        case TOK_UMOD:
            die_with_line_number("whatever the fuck you are doing here, we do not support it!");
            break;
        case TOK_SHL:
            l1 = i64shl(l1, (l2 & shm));
            break;
        case TOK_SHR:
        case TOK_SAR:
            die_with_line_number("INTERNAL__lshrdi3");
            break;
            // tests
        case TOK_ULT: l1 = l1 < l2; break;
        case TOK_UGE: l1 = l1 >= l2; break;
        case TOK_EQ: l1 = l1 == l2; break;
        case TOK_NE: l1 = l1 != l2; break;
        case TOK_ULE: l1 = l1 <= l2; break;
        case TOK_UGT: l1 = l1 > l2; break;

        case TOK_LT:
        case TOK_GE:
        case TOK_LE:
        case TOK_GT:
            die_with_line_number("gen_opic_lt");
            break;

            // logical
        case TOK_LAND: l1 = l1 && l2; break;
        case TOK_LOR: l1 = l1 || l2; break;
        default:
            goto general_case;
        }
        if ((t1 != VT_LLONG) && (PTR_SIZE_IS_4 || (t1 != VT_PTR)))
        {
            l1 = ((u32)l1 |
                ((v1->type._tv_.bits & VT_UNSIGNED) ? 0 : twocomp_unary_neg(l1 & 0x80000000)));
        }
        v1->c.i = l1;
        vtop--;
    }
    else
    {
        // if commutative ops, put c2 as constant
        if (
            c1 &&
            (
                (op == '+') ||
                (op == '&') ||
                (op == '^') ||
                (op == '|') ||
                (op == '*')
            )
        )
        {
            vswap();
            c2 = c1; //c = c1, c1 = c2, c2 = c;
            l2 = l1; //l = l1, l1 = l2, l2 = l;
        }
        if (
            !const_wanted &&
            c1 &&
            (
                (
                    (l1 == 0) &&
                    (
                        (op == TOK_SHL) ||
                        (op == TOK_SHR) ||
                        (op == TOK_SAR)
                    )
                ) ||
                (
                    (l1 == -1) &&
                    (op == TOK_SAR)
                )
            )
        )
        {
            // treat (0 << x), (0 >> x) and (-1 >> x) as constant
            vtop--;
        }
        else if (
            !const_wanted &&
            c2 &&
            (
                (
                    (l2 == 0) &&
                    (
                        (op == '&') ||
                        (op == '*')
                    )
                ) ||
                (
                    (op == '|') &&
                    (
                        (l2 == -1) ||
                        (
                            (l2 == 0xFFFFFFFF) &&
                            (t2 != VT_LLONG)
                        )
                    )
                ) ||
                (
                    (l2 == 1) &&
                    (
                        (op == '%') ||
                        (op == TOK_UMOD)
                    )
                )
            )
        )
        {
            // treat (x & 0), (x * 0), (x | -1) and (x % 1) as constant */
            if (l2 == 1)
            {
                vtop->c.i = 0;
            }
            vswap();
            vtop--;
        }
        else if (
            c2 &&
            (
                (
                    (
                        (op == '*') ||
                        (op == '/') ||
                        (op == TOK_UDIV) ||
                        (op == TOK_PDIV)
                    ) &&
                    (l2 == 1)
                ) ||
                (
                    (
                        (op == '+') ||
                        (op == '-') ||
                        (op == '|') ||
                        (op == '^') ||
                        (op == TOK_SHL) ||
                        (op == TOK_SHR) ||
                        (op == TOK_SAR)
                    ) &&
                    (l2 == 0)
                ) ||
                (
                    (op == '&') &&
                    (
                        (l2 == -1) ||
                        (
                            (l2 == 0xFFFFFFFF) &&
                            (t2 != VT_LLONG)
                        )
                    )
                )
            )
        )
        {
            // filter out NOP operations like x*1, x-0, x&-1... */
            vtop--;
        }
        else if (c2 && ((op == '*') || (op == TOK_PDIV) || (op == TOK_UDIV)))
        {
            // try to use shifts instead of muls or divs
            if ((l2 > 0) && ((l2 & (l2 - 1)) == 0))
            {
                i32 n = -1;
                while (l2)
                {
                    l2 >>= 1;
                    n++;
                }
                vtop->c.i = n;
                if (op == '*')
                {
                    op = TOK_SHL;
                }
                else if (op == TOK_PDIV)
                {
                    op = TOK_SAR;
                }
                else
                {
                    op = TOK_SHR;
                }
            }
            goto general_case;
        }
        else if (
            c2 &&
            ((op == '+') || (op == '-')) &&
            (
                (((vtop[-1].r & (VT_VALMASK | VT_LVAL | VT_SYM)) == (VT_CONST | VT_SYM))) ||
                ((vtop[-1].r & (VT_VALMASK | VT_LVAL)) == VT_LOCAL)
            )
        )
        {
            // symbol + constant case
            if (op == '-')
            {
                l2 = twocomp_unary_neg(l2);
            }
            l2 += vtop[-1].c.i;
            // The backends can't always deal with addends to symbols // larger than +-1<<31.  Don't construct such.
            if ((i32)l2 != l2)
            {
                goto general_case;
            }
            vtop--;
            vtop->c.i = l2;
        }
        else
        {
        general_case:
            // call low level op generator
            if (t_needs_opl(t1) || t_needs_opl(t2))
            {
                gen_opl(op);
            }
            else
            {
                gen_opi(op);
            }
        }
    }
}

static i32 pointed_size(CType * type)
{
    i32 align;
    return type_size(pointed_type(type), &align);
}

static inline i32 is_null_pointer(SValue * p)
{
    if ((p->r & (VT_VALMASK | VT_LVAL | VT_SYM)) != VT_CONST)
    {
        return 0;
    }
    return (
        (
            (BTYPES_MATCH(p->type._tv_, VT_INT)) &&
            ((u32)p->c.i == 0)
        ) ||
        (
            (BTYPES_MATCH(p->type._tv_, VT_LLONG)) &&
            (p->c.i == 0)
        ) ||
        (
            (BTYPES_MATCH(p->type._tv_, VT_PTR)) &&
            (PTR_SIZE_IS_4 ? ((u32)p->c.i == 0) : (p->c.i == 0))
        )
    );
}

static inline i32 is_integer_btype(i32 bt)
{
    return
        (
            (bt == VT_BYTE) ||
            (bt == VT_SHORT) ||
            (bt == VT_INT) ||
            (bt == VT_LLONG)
        );
}

// check types for comparison or subtraction of pointers
static void check_comparison_pointer_types(SValue * p1, SValue * p2, i32 op)
{
    CType * type1;
    CType * type2;
    CType tmp_type1;
    CType tmp_type2;
    i32 bt1;
    i32 bt2;

    // null pointers are accepted for all comparisons as gcc
    if (is_null_pointer(p1) || is_null_pointer(p2))
    {
        return;
    }
    type1 = &p1->type;
    type2 = &p2->type;
    bt1 = type1->_tv_.bits & VT_BTYPE;
    bt2 = type2->_tv_.bits & VT_BTYPE;
    // accept comparison between pointer and integer with a warning
    if ((is_integer_btype(bt1) || is_integer_btype(bt2)) && (op != '-'))
    {
        if ((op != TOK_LOR) && (op != TOK_LAND))
        {
            print_error_with_line_number("comparison between pointer and integer");
        }
        return;
    }

    // both must be pointers or implicit function pointers
    if (bt1 == VT_PTR)
    {
        type1 = pointed_type(type1);
    }
    else if (bt1 != VT_FUNC)
    {
        goto invalid_operands;
    }

    if (bt2 == VT_PTR)
    {
        type2 = pointed_type(type2);
    }
    else if (bt2 != VT_FUNC)
    {
    invalid_operands:
        tcc_error_puts("invalid operands to binary %s", get_tok_str(op, NULL));
    }
    if ((BTYPES_MATCH(type1->_tv_, VT_VOID)) ||
        (BTYPES_MATCH(type2->_tv_, VT_VOID)))
    {
        return;
    }
    tmp_type1 = *type1;
    tmp_type2 = *type2;
    tmp_type1._tv_.bits &= ~(VT_DEFSIGN | VT_UNSIGNED | VT_CONSTANT | VT_VOLATILE);
    tmp_type2._tv_.bits &= ~(VT_DEFSIGN | VT_UNSIGNED | VT_CONSTANT | VT_VOLATILE);
    if (!is_compatible_types(&tmp_type1, &tmp_type2))
    {
        // gcc-like error if '-' is used
        if (op == '-')
        {
            goto invalid_operands;
        }
        else
        {
            print_error_with_line_number("comparison of distinct pointer types lacks a cast");
        }
    }
}

int is_rel_op(i32 op)
{
    switch (op)
    {
    case TOK_ULT:
    case TOK_UGE:
    case TOK_EQ:
    case TOK_NE:
    case TOK_ULE:
    case TOK_UGT:
    case TOK_LT:
    case TOK_GE:
    case TOK_LE:
    case TOK_GT:
        return 1;
    }

    return 0;
}

static void op_error(i32 op, char * msg)
{
    char buf[256];
    sputd(buf, op);
    strcat_(buf, " : unexpected op in ");
    strcat_(buf, msg);
    die_with_line_number(buf);
}

// generic gen_op: handles types problems
static void mk_pointer(CType * type);
static void gen_op(i32 op)
{
    switch (op)
    {
    case '*':
    case '-':
    case '|':
    case '+':
    case '^':
    case '&':
    case '%':
    case '/':
    case TOK_EQ:
    case TOK_SHL:
    case TOK_NE:
    case TOK_LT:
    case TOK_GT:
    case TOK_LE:
    case TOK_GE:
    case TOK_SHR:
    case TOK_ULE:
    case TOK_ULT:
    case TOK_SAR:
    case TOK_UGE:
    case TOK_SUBC1:
    case TOK_SUBC2:
    case TOK_PDIV:
    case TOK_UGT:
    case TOK_UMULL:
    case TOK_ADDC1:
    case TOK_ADDC2:
        break;

    default:
        op_error(op, "gen_op");
        break;
    }

    i32 u;
    TYPE_VAL _tv1_;
    TYPE_VAL _tv2_;
    i32 bt1;
    i32 bt2;
    i32 t;
    CType type1;

redo:
    _tv1_ = vtop[-1].type._tv_;
    _tv2_ = vtop[0].type._tv_;
    bt1 = _tv1_.bits & VT_BTYPE;
    bt2 = _tv2_.bits & VT_BTYPE;

    if ((bt1 == VT_STRUCT) || (bt2 == VT_STRUCT))
    {
        die_with_line_number("operation on a struct");
    }
    else if ((bt1 == VT_FUNC) || (bt2 == VT_FUNC))
    {
        if (bt2 == VT_FUNC)
        {
            mk_pointer(&vtop->type);
            gaddrof();
        }
        if (bt1 == VT_FUNC)
        {
            vswap();
            mk_pointer(&vtop->type);
            gaddrof();
            vswap();
        }
        goto redo;
    }
    else if ((bt1 == VT_PTR) || (bt2 == VT_PTR))
    {
        // at least one operand is a pointer
        // relational op: must be both pointers
        if (is_rel_op(op))
        {
            check_comparison_pointer_types(vtop - 1, vtop, op);
            // pointers are handled are unsigned
            t = VT_INT | VT_UNSIGNED;
            goto std_op;
        }
        // if both pointers, then it must be the '-' op
        if ((bt1 == VT_PTR) && (bt2 == VT_PTR))
        {
            if (op != '-')
            {
                die_with_line_number("cannot use pointers here");
            }
            check_comparison_pointer_types(vtop - 1, vtop, op);
            // XXX: check that types are compatible
            {
                vpushi(pointed_size(&vtop[-1].type));
            }
            vrott(3);
            gen_opic(op);
            vtop->type._tv_ = ptrdiff_type._tv_;
            vswap();
            gen_op(TOK_PDIV);
        }
        else
        {
            // exactly one pointer : must be '+' or '-'.
            if ((op != '-') && (op != '+'))
            {
                die_with_line_number("cannot use pointers here");
            }
            // Put pointer as first operand
            if (bt2 == VT_PTR)
            {
                vswap();
                t = _tv1_.bits; _tv1_.bits = _tv2_.bits; _tv2_.bits = t;
            }
            if (BTYPES_MATCH(vtop[0].type._tv_, VT_LLONG))
            {
                // XXX: truncate here because gen_opl can't handle ptr + i64
                gen_cast_s(VT_INT);
            }
            type1 = vtop[-1].type;
            type1._tv_.bits &= ~VT_ARRAY;
            {
                u = pointed_size(&vtop[-1].type);
                if (u < 0)
                {
                    die_with_line_number("unknown array element size");
                }
                // XXX: cast to i32 ? (i64 case)
                vpushi(u);
            }
            gen_op('*');

            gen_opic(op);

            // put again type if gen_opic() swaped operands
            vtop->type = type1;
        }
    }
    else if ((op == TOK_SHR) || (op == TOK_SAR) || (op == TOK_SHL))
    {
        t = (bt1 == VT_LLONG) ? VT_LLONG : VT_INT;
        if ((_tv1_.bits & (VT_BTYPE | VT_UNSIGNED)) == (t | VT_UNSIGNED))
        {
            t |= VT_UNSIGNED;
        }
        t |= (VT_LONG & _tv1_.bits);
        goto std_op;
    }
    else if ((bt1 == VT_LLONG) || (bt2 == VT_LLONG))
    {
        // cast to biggest op
        t = VT_LLONG | VT_LONG;
        if (bt1 == VT_LLONG)
        {
            t &= _tv1_.bits;
        }
        if (bt2 == VT_LLONG)
        {
            t &= _tv2_.bits;
        }
        // convert to unsigned if it does not fit in a i64
        if (((_tv1_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED)) ||
            ((_tv2_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED)))
        {
            t |= VT_UNSIGNED;
        }
        goto std_op;
    }
    else
    {
        // integer operations
        t = VT_INT | (VT_LONG & (_tv1_.bits | _tv2_.bits));
        // convert to unsigned if it does not fit in an integer
        if (((_tv1_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED)) ||
            ((_tv2_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED)))
        {
            t |= VT_UNSIGNED;
        }
    std_op:
        // XXX: currently, some unsigned operations are explicit, so // we modify them here
        if (t & VT_UNSIGNED)
        {
            if (op == TOK_SAR)
            {
                op = TOK_SHR;
            }
            else if (op == '/')
            {
                op = TOK_UDIV;
            }
            else if (op == '%')
            {
                op = TOK_UMOD;
            }
            else if (op == TOK_LT)
            {
                op = TOK_ULT;
            }
            else if (op == TOK_GT)
            {
                op = TOK_UGT;
            }
            else if (op == TOK_LE)
            {
                op = TOK_ULE;
            }
            else if (op == TOK_GE)
            {
                op = TOK_UGE;
            }
        }
        vswap();
        type1._tv_.bits = t;
        type1.ref = NULL;
        gen_cast(&type1);
        vswap();
        // special case for shifts and i64: we keep the shift as // an integer
        if ((op == TOK_SHR) || (op == TOK_SAR) || (op == TOK_SHL))
        {
            type1._tv_.bits = VT_INT;
        }
        gen_cast(&type1);
        {
            gen_opic(op);
        }
        if (is_rel_op(op))
        {
            // relational op: the result is an i32
            vtop->type._tv_.bits = VT_INT;
        }
        else
        {
            vtop->type._tv_.bits = t;
        }
    }
    // Make sure that we have converted to an rvalue:
    if (vtop->r & VT_LVAL)
    {
        gv(RC_INT);
    }
}

// force char or short cast
static void force_charshort_cast(i32 t)
{
    i32 bits;
    i32 dbt;

    // cannot cast static initializers
    i32 STATIC_DATA_WANTED = (nocode_wanted & 0xC0000000);
    if (STATIC_DATA_WANTED)
    {
        return;
    }

    dbt = t & VT_BTYPE;
    // XXX: add optimization if lvalue : just change type and offset
    if (dbt == VT_BYTE)
    {
        bits = 8;
    }
    else
    {
        bits = 16;
    }
    if (t & VT_UNSIGNED)
    {
        vpushi((1 << bits) - 1);
        gen_op('&');
    }
    else
    {
        if (BTYPES_MATCH(vtop->type._tv_, VT_LLONG))
        {
            bits = 64 - bits;
        }
        else
        {
            bits = 32 - bits;
        }
        vpushi(bits);
        gen_op(TOK_SHL);
        // result must be signed or the SAR is converted to an SHL 
        // // This was not the case when "t" was a signed short 
        // // and the last value on the stack was a u32
        vtop->type._tv_.bits &= ~VT_UNSIGNED;
        vpushi(bits);
        gen_op(TOK_SAR);
    }
}

// cast 'vtop' to 'type'
static void gen_cast_s(i32 t)
{
    CType type;
    type._tv_.bits = t;
    type.ref = NULL;
    gen_cast(&type);
}

static void gen_cast(CType * type)
{
    i32 sbt;
    i32 dbt;
    i32 c;
    i32 p;

    // special delayed cast for char/short
    // XXX: in some cases (multiple cascaded casts), it may still // be incorrect
    if (vtop->r & VT_MUSTCAST)
    {
        vtop->r &= ~VT_MUSTCAST;
        force_charshort_cast(vtop->type._tv_.bits);
    }

    dbt = type->_tv_.bits & (VT_BTYPE | VT_UNSIGNED);
    sbt = vtop->type._tv_.bits & (VT_BTYPE | VT_UNSIGNED);

    if (sbt != dbt)
    {
        c = (vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
        p = (vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == (VT_CONST | VT_SYM);
        if (c)
        {
            // constant case: we can do it now
            {
                if (sbt == (VT_LLONG | VT_UNSIGNED))
                {
                    ;
                }
                else if (sbt & VT_UNSIGNED)
                {
                    vtop->c.i = (u32)vtop->c.i;
                }
                else if (sbt != VT_LLONG)
                {
                    vtop->c.i = ((u32)vtop->c.i |
                        twocomp_unary_neg(vtop->c.i & 0x80000000));
                }

                if (dbt == (VT_LLONG | VT_UNSIGNED))
                {
                    ;
                }
                else if (dbt == VT_BOOL)
                {
                    vtop->c.i = (vtop->c.i != 0);
                }
                else if (dbt != VT_LLONG)
                {
                    u32 m = (
                        ((dbt & VT_BTYPE) == VT_BYTE) ?
                            0xff :
                            (
                                ((dbt & VT_BTYPE) == VT_SHORT) ?
                                    0xffff :
                                    0xffffffff
                            )
                    );
                    vtop->c.i &= m;
                    if (!(dbt & VT_UNSIGNED))
                    {
                        vtop->c.i |= twocomp_unary_neg(vtop->c.i & ((m >> 1) + 1));
                    }
                }
            }
        }
        else if (p && (dbt == VT_BOOL))
        {
            vtop->r = VT_CONST;
            vtop->c.i = 1;
        }
        else
        {
            // non constant case: generate code
            if ((dbt & VT_BTYPE) == VT_LLONG)
            {
                if ((sbt & VT_BTYPE) != VT_LLONG)
                {
                    // scalar to i64
                    // machine independent conversion
                    gv(RC_INT);
                    // generate high word
                    if (sbt == (VT_INT | VT_UNSIGNED))
                    {
                        vpushi(0);
                        gv(RC_INT);
                    }
                    else
                    {
                        if (sbt == VT_PTR)
                        {
                            // cast from pointer to int before we apply // shift operation, which pointers don't support
                            gen_cast_s(VT_INT);
                        }
                        gv_dup();
                        vpushi(31);
                        gen_op(TOK_SAR);
                    }
                    // patch second register
                    vtop[-1].r2 = vtop->r;
                    vpop();
                }
            }
            else if (dbt == VT_BOOL)
            {
                // scalar to bool
                vpushi(0);
                gen_op(TOK_NE);
            }
            else if (
                ((dbt & VT_BTYPE) == VT_BYTE) ||
                ((dbt & VT_BTYPE) == VT_SHORT)
            )
            {
                if (sbt == VT_PTR)
                {
                    vtop->type._tv_.bits = VT_INT;
                    print_error_with_line_number("nonportable conversion from pointer to char/short");
                }
                force_charshort_cast(dbt);
            }
            else if ((dbt & VT_BTYPE) == VT_INT)
            {
                // scalar to int
                if ((sbt & VT_BTYPE) == VT_LLONG)
                {
                    // from i64: just take low order word
                    lexpand();
                    vpop();
                }
                // if lvalue and single word type, nothing to do because // the lvalue already contains the real type size (see // VT_LVAL_xxx constants)
            }
        }
    }
    else if (((dbt & VT_BTYPE) == VT_PTR) && !(vtop->r & VT_LVAL))
    {
        // if we are casting between pointer types, // we must update the VT_LVAL_xxx size
        vtop->r = (vtop->r & ~VT_LVAL_TYPE)
            | (lvalue_type(type->ref->type._tv_.bits) & VT_LVAL_TYPE);
    }
    vtop->type = *type;
}

static inline i32 IS_ENUM(i32 t)
{
    return ((t & VT_STRUCT_MASK) == VT_ENUM);
}

static inline i32 IS_ENUM_VAL(i32 t)
{
    return ((t & VT_STRUCT_MASK) == VT_ENUM_VAL);
}

static inline i32 IS_UNION(i32 t)
{
    return ((t & (VT_STRUCT_MASK | VT_BTYPE)) == VT_UNION);
}

// return type size as known at compile time. Put alignment at 'a'
static i32 type_size(CType * type, i32 * a)
{
    Sym * s;
    i32 bt;

    bt = type->_tv_.bits & VT_BTYPE;
    if (bt == VT_STRUCT)
    {
        // struct/union
        s = type->ref;
        *a = s->r;
        return s->c;
    }
    else if (bt == VT_PTR)
    {
        if (type->_tv_.bits & VT_ARRAY)
        {
            i32 ts;

            s = type->ref;
            ts = type_size(&s->type, a);

            if ((ts < 0) && (s->c < 0))
            {
                ts = -ts;
            }

            return (ts * s->c);
        }
        else
        {
            *a = PTR_SIZE;
            return PTR_SIZE;
        }
    }
    else if (IS_ENUM(type->_tv_.bits) && (type->ref->c == -1))
    {
        return -1; // incomplete enum
    }
    else if (bt == VT_LLONG)
    {
        *a = 8;
        return 8;
    }
    else if (bt == VT_INT)
    {
        *a = 4;
        return 4;
    }
    else if (bt == VT_SHORT)
    {
        *a = 2;
        return 2;
    }
    else
    {
        // char, void, function, _Bool
        *a = 1;
        return 1;
    }
}

// return the pointed type of t
static inline CType * pointed_type(CType * type)
{
    return &type->ref->type;
}

// modify type so that its it is a pointer to type.
static void mk_pointer(CType * type)
{
    Sym * s;
    s = sym_push(SYM_FIELD, type, 0, -1);
    type->_tv_.bits = VT_PTR | (type->_tv_.bits & VT_STORAGE);
    type->ref = s;
}

// compare function types. OLD functions match any new functions
static i32 is_compatible_func(CType * type1, CType * type2)
{
    Sym * s1;
    Sym * s2;

    s1 = type1->ref;
    s2 = type2->ref;
    if (!is_compatible_types(&s1->type, &s2->type))
    {
        return 0;
    }
    // check func_call
    if (s1->f.func_call != s2->f.func_call)
    {
        return 0;
    }
    while (s1 != NULL)
    {
        if (s2 == NULL)
        {
            return 0;
        }
        if (!is_compatible_unqualified_types(&s1->type, &s2->type))
        {
            return 0;
        }
        s1 = s1->next;
        s2 = s2->next;
    }
    if (s2)
    {
        return 0;
    }
    return 1;
}

// return true if type1 and type2 are the same.  If unqualified is
// true, qualifiers on the types are ignored.
// - enums are not checked as gcc __builtin_types_compatible_p ()
//
static i32 compare_types(CType * type1, CType * type2, i32 unqualified)
{
    i32 bt1;
    i32 t1;
    i32 t2;

    t1 = type1->_tv_.bits & VT_TYPE;
    t2 = type2->_tv_.bits & VT_TYPE;
    if (unqualified)
    {
        // strip qualifiers before comparing
        t1 &= ~(VT_CONSTANT | VT_VOLATILE);
        t2 &= ~(VT_CONSTANT | VT_VOLATILE);
    }

    // Default Vs explicit signedness only matters for char
    if ((t1 & VT_BTYPE) != VT_BYTE)
    {
        t1 &= ~VT_DEFSIGN;
        t2 &= ~VT_DEFSIGN;
    }
    if (t1 != t2)
    {
        return 0;
    }
    // test more complicated cases
    bt1 = t1 & VT_BTYPE;
    if (bt1 == VT_PTR)
    {
        type1 = pointed_type(type1);
        type2 = pointed_type(type2);
        return is_compatible_types(type1, type2);
    }
    else if (bt1 == VT_STRUCT)
    {
        return (type1->ref == type2->ref);
    }
    else if (bt1 == VT_FUNC)
    {
        return is_compatible_func(type1, type2);
    }
    else
    {
        return 1;
    }
}

// return true if type1 and type2 are exactly the same (including
// qualifiers).
//
static i32 is_compatible_types(CType * type1, CType * type2)
{
    return compare_types(type1, type2, 0);
}

// return true if type1 and type2 are the same (ignoring qualifiers).
//
static i32 is_compatible_unqualified_types(CType * type1, CType * type2)
{
    return compare_types(type1, type2, 1);
}

// verify type compatibility to store vtop in 'dt' type, and generate
// casts if needed.
static void gen_assign_cast(CType * dt)
{
    CType * st;
    CType * type1;
    CType *type2;
    i32 dbt;
    i32 sbt;

    st = &vtop->type; // source type
    dbt = dt->_tv_.bits & VT_BTYPE;
    sbt = st->_tv_.bits & VT_BTYPE;
    if ((sbt == VT_VOID) || (dbt == VT_VOID))
    {
        if ((sbt == VT_VOID) && (dbt == VT_VOID))
        {
            ;
            ///*
            // It is Ok if both are void
            //A test program:
                //void func1() {}
            //void func2() {
            //return func1();
            //}
            //gcc accepts this program
            //*/
        }
        else
        {
            die_with_line_number("cannot cast from/to void");
        }
    }
    if (dt->_tv_.bits & VT_CONSTANT)
    {
        print_error_with_line_number("assignment of re_ad-only location");
    }
    switch (dbt)
    {
    case VT_PTR:
        // special cases for pointers
        // '0' can also be a pointer
        if (is_null_pointer(vtop))
        {
            goto type_ok;
        }
        // accept implicit pointer to integer cast with warning
        if (is_integer_btype(sbt))
        {
            print_error_with_line_number("assignment makes pointer from integer without a cast");
            goto type_ok;
        }
        type1 = pointed_type(dt);
        // a function is implicitly a function pointer
        if (sbt == VT_FUNC)
        {
            if ((!BTYPES_MATCH(type1->_tv_, VT_VOID)) &&
                !is_compatible_types(pointed_type(dt), st))
            {
                print_error_with_line_number("assignment from incompatible pointer type");
            }
            goto type_ok;
        }
        if (sbt != VT_PTR)
        {
            goto error;
        }
        type2 = pointed_type(st);
        if ((BTYPES_MATCH(type1->_tv_, VT_VOID)) ||
            (BTYPES_MATCH(type2->_tv_, VT_VOID)))
        {
            // void * can match anything */
        }
        else
        {
            // exact type match, except for qualifiers
            if (!is_compatible_unqualified_types(type1, type2))
            {
                // Like GCC don't warn by default for merely changes 
                // // in pointer target signedness.  Do warn for different 
                // // base types, though, in particular for unsigned enums 
                // // and signed int targets.
                if (
                    ((type1->_tv_.bits & (VT_BTYPE | VT_LONG)) != (type2->_tv_.bits & (VT_BTYPE | VT_LONG))) ||
                    IS_ENUM(type1->_tv_.bits) ||
                    IS_ENUM(type2->_tv_.bits)
                )
                {
                    print_error_with_line_number("assignment from incompatible pointer type");
                }
            }
        }
        // check and volatile
        if ((!(type1->_tv_.bits & VT_CONSTANT) && (type2->_tv_.bits & VT_CONSTANT)) ||
            (!(type1->_tv_.bits & VT_VOLATILE) && (type2->_tv_.bits & VT_VOLATILE)))
        {
            print_error_with_line_number("assignment discards qualifiers from pointer target type");
        }
        break;
    case VT_BYTE:
    case VT_SHORT:
    case VT_INT:
    case VT_LLONG:
        if ((sbt == VT_PTR) || (sbt == VT_FUNC))
        {
            print_error_with_line_number("assignment makes integer from pointer without a cast");
        }
        else if (sbt == VT_STRUCT)
        {
            goto case_VT_STRUCT;
        }
        // XXX: more tests
        break;
    case VT_STRUCT:
    case_VT_STRUCT:
        if (!is_compatible_unqualified_types(dt, st))
        {
        error:
            die_with_line_number("cannot cast between these struct types");
        }
        break;
    }
type_ok:
    gen_cast(dt);
}

// store vtop in lvalue pushed on stack
static void gfunc_call(i32 nb_args);
static void vstore(void)
{
    i32 sbt;
    i32 dbt;
    TYPE_VAL _tv_;
    i32 r;
    i32 t;
    i32 size;
    i32 align;
    i32 rc;
    i32 delayed_cast;

    _tv_ = vtop[-1].type._tv_;
    sbt = vtop->type._tv_.bits & VT_BTYPE;
    dbt = _tv_.bits & VT_BTYPE;
    if (
        (
            (
                (sbt == VT_INT) ||
                (sbt == VT_SHORT)
            ) &&
            (dbt == VT_BYTE)
        ) ||
        (
            (sbt == VT_INT) &&
            (dbt == VT_SHORT)
        )
    )
    {
        // optimize char/short casts
        delayed_cast = VT_MUSTCAST;
        vtop->type._tv_.bits = _tv_.bits & VT_TYPE;
        // XXX: factorize
        if (_tv_.bits & VT_CONSTANT)
        {
            print_error_with_line_number("assignment of re_ad-only location");
        }
    }
    else
    {
        delayed_cast = 0;
        {
            gen_assign_cast(&vtop[-1].type);
        }
    }

    if (sbt == VT_STRUCT)
    {
        // if structure, only generate pointer
        // structure assignment : generate memcpy
        // XXX: optimize if small size
        size = type_size(&vtop->type, &align);

        // destination
        vswap();
        vtop->type._tv_.bits = VT_PTR;
        gaddrof();

        // use memmove
        Sym * s = sym_find(TOK_intrin_memmove);
        if (!s)
        {
            die_with_line_number("Needed TOK_intrin_memmove!");
            return;
        }

        vpushsym(&s->type, s);

        vswap();
        // source
        vpushv(vtop - 2);
        vtop->type._tv_.bits = VT_PTR;
        gaddrof();
        // type size
        vpushi(size);
        gfunc_call(3);

        // leave source on stack
    }
    else if (dbt == VT_VOID)
    {
        --vtop;
    }
    else
    {

        rc = RC_INT;
        r = gv(rc);  // generate value
        // if lvalue was saved on stack, must re_ad it
        if ((vtop[-1].r & VT_VALMASK) == VT_LLOCAL)
        {
            SValue sv;
            t = get_reg(RC_INT);
            sv.type._tv_.bits = VT_INT;
            sv.r = VT_LOCAL | VT_LVAL;
            sv.c.i = vtop[-1].c.i;
            load(t, &sv);
            vtop[-1].r = (u16)(t | VT_LVAL);
        }
        // two word case handling : store second register at word + 4 (or +8 for x86-64)
        if (BTYPES_MATCH(_tv_, VT_LLONG))
        {
            i32 addr_type = VT_INT;
            i32 load_size = 4;
            i32 load_type = VT_INT;
            vtop[-1].type._tv_.bits = load_type;
            store(r, vtop - 1);
            vswap();
            // convert to int to increment easily
            vtop->type._tv_.bits = addr_type;
            gaddrof();
            vpushi(load_size);
            gen_op('+');
            vtop->r |= VT_LVAL;
            vswap();
            vtop[-1].type._tv_.bits = load_type;
            // XXX: it works because r2 is spilled last !
            store(vtop->r2, vtop - 1);
        }
        else
        {
            store(r, vtop - 1);
        }

        vswap();
        vtop--; // NOT vpop() because on x86 it would flush the fp stack
        vtop->r |= delayed_cast;
    }
}

// post defines POST/PRE add. c is the token ++ or --
static void inc(i32 post, i32 c)
{//!!! kill later
    test_lvalue();
    vdup(); // save lvalue
    if (post)
    {
        gv_dup(); // duplicate value
        vrotb(3);
        vrotb(3);
    }
    // add constant
    i32 v = 0;
    if (c == TOK_INC)
    {
        v = 1;
    }
    else if (c == TOK_DEC)
    {
        v = -1;
    }
    else
    {
        die_with_line_number("INTERNAL ERROR CALLING 'inc'");
    }

    vpushi(v);
    gen_op('+');
    vstore(); // store value
    if (post)
    {
        vpop(); // if post op, return saved value
    }
}

// Parse __attribute__((...)) GNUC extension.
static void parse_attribute(AttributeDef * ad)
{
redo:
    if (g_tok_is_kw(TOK_STDCALL3))
    {
        next();
        ad->f.func_call = FUNC_STDCALL;
        goto redo;
    }
    if (!g_tok_is_kw(TOK_ATTRIBUTE2))
    {
        return;
    }
    next();
    skip('(');
    skip('(');
    if (!g_tok_is_kw(TOK_STDCALL3))
    {
        die_with_line_number("attribute ignored");
    }
    next();
    ad->f.func_call = FUNC_STDCALL;
    skip(')');
    skip(')');
    goto redo;
}

static Sym * find_field(CType * type, i32 v)
{
    Sym * s = type->ref;
    v |= SYM_FIELD;

    for (;;)
    {
        s = s->next;
        if (s == NULL)
        {
            break;
        }

        if (
            (s->v & SYM_FIELD) &&
            (BTYPES_MATCH(s->type._tv_, VT_STRUCT)) &&
            ((s->v & ~SYM_FIELD) >= SYM_FIRST_ANOM)
        )
        {
            Sym * ret = find_field(&s->type, v);
            if (ret)
            {
                return ret;
            }
        }
        if (s->v == v)
        {
            break;
        }
    }

    return s;
}

static void struct_add_offset(Sym * s, i32 offset)
{
    for (;;)
    {
        s = s->next;
        if (s == NULL)
        {
            break;
        }

        if (
            (s->v & SYM_FIELD) &&
            (BTYPES_MATCH(s->type._tv_, VT_STRUCT)) &&
            ((s->v & ~SYM_FIELD) >= SYM_FIRST_ANOM)
        )
        {
            struct_add_offset(s->type.ref, offset);
        }
        else
        {
            s->c += offset;
        }
    }
}

static void struct_layout(CType * type, AttributeDef * ad)
{
    (void)ad;

    i32 size;
    i32 align;
    i32 maxalign;
    i32 offset;
    i32 c;
    i32 packed;
    i32 a;
    i32 bt;
    Sym * f;

    maxalign = 1;
    offset = 0;
    c = 0;

    f = type->ref->next;
    for (; f; f = f->next)
    {
        size = type_size(&f->type, &align);
        a = 0;
        packed = 0;

        // some individual align was specified
        if (a)
        {
            align = a;
        }

        if (type->ref->type._tv_.bits == VT_UNION)
        {
            offset = 0;
            if (size > c)
            {
                c = size;
            }
        }
        else
        {
            c = (c + align - 1) & -align;
            offset = c;
            if (size > 0)
            {
                c += size;
            }
        }
        if (align > maxalign)
        {
            maxalign = align;
        }

        if (
            (f->v & SYM_FIRST_ANOM) &&
            (BTYPES_MATCH(f->type._tv_, VT_STRUCT))
        )
        {
            Sym * ass;
            // An anonymous struct/union.  Adjust member offsets // to reflect the real offset of our containing struct. // Also set the offset of this anon member inside // the outer struct to be zero.  Via this it // works when accessing the field offset directly // (from base object), as well as when recursing // members in initializer handling.
            i32 v2 = f->type.ref->v;
            if (!(v2 & SYM_FIELD) &&
                ((v2 & ~SYM_STRUCT) < SYM_FIRST_ANOM))
            {
                Sym * * pps;
                // This happens only with MS extensions.  The // anon member has a named struct type, so it // potentially is shared with other references. // We need to unshare members so we can modify // them.
                ass = f->type.ref;
                f->type.ref = sym_push(anon_sym++ | SYM_FIELD,
                    &f->type.ref->type, 0,
                    f->type.ref->c);
                pps = &f->type.ref->next;

                for (;;)
                {
                    ass = ass->next;
                    if (ass == NULL)
                    {
                        break;
                    }

                    *pps = sym_push(ass->v, &ass->type, 0, ass->c);
                    pps = &((*pps)->next);
                }

                *pps = NULL;
            }
            struct_add_offset(f->type.ref, offset);
            f->c = 0;
        }
        else
        {
            f->c = offset;
        }

        f->r = 0;
    }

    // store size and alignment
    bt = 1;
    a = bt;
    if (a < maxalign)
    {
        a = maxalign;
    }
    type->ref->r = (u16)a;
    c = (c + a - 1) & -a;
    type->ref->c = c;
}

static void sym_to_attr(AttributeDef * ad, Sym * s)
{
    if (s->f.func_call && (0 == ad->f.func_call))
    {
        ad->f.func_call = s->f.func_call;
    }
}

// convert a function parameter type (array to pointer and function to
// function pointer)
static inline void convert_parameter_type(CType * pt)
{
    // remove and volatile qualifiers (XXX: could be used // to indicate a function parameter
    pt->_tv_.bits &= ~(VT_CONSTANT | VT_VOLATILE);
    // array must be transformed to pointer according to ANSI C
    pt->_tv_.bits &= ~VT_ARRAY;
    if (BTYPES_MATCH(pt->_tv_, VT_FUNC))
    {
        mk_pointer(pt);
    }
}

static i32 expr_const(i32 req_unary);
static void unary(void);
static i32 post_type(CType * type, AttributeDef * ad, i32 storage, i32 td)
{
    i32 arg_size;
    i32 align;
    Sym * * plast;
    Sym * s;
    Sym *first;
    AttributeDef ad1;
    CType pt;
    memset(&pt, 0, sizeof(CType));

    if (g_tok_is('('))
    {
        // function type, or recursive declarator (return if so)
        next();
        if (td && !(td & TYPE_ABSTRACT))
        {
            return 0;
        }
        if (g_tok_is(')'))
        {
            die_with_line_number("no arguments in func decl");
        }
        else if (parse_btype(&pt, &ad1))
        {
            // func decl
        }
        else if (td)
        {
            return 0;
        }
        else
        {
            die_with_line_number("we do not support old style function decls");
        }
        first = NULL;
        plast = &first;
        arg_size = 0;
        
        for (;;)
        {
            // re_ad param name and compute offset
            if (
                (BTYPES_MATCH(pt._tv_, VT_VOID)) &&
                (g_tok_is(')'))
            )
            {
                break;
            }
            i32 n;
            type_decl(&pt, &ad1, &n, TYPE_DIRECT);
            if (BTYPES_MATCH(pt._tv_, VT_VOID))
            {
                die_with_line_number("parameter declared as void");
            }
            arg_size += (type_size(&pt, &align) + PTR_SIZE - 1) / PTR_SIZE;
            convert_parameter_type(&pt);
            s = sym_push(n | SYM_FIELD, &pt, 0, 0);
            *plast = s;
            plast = &s->next;
            if (g_tok_is(')'))
            {
                break;
            }
            skip(',');
            if (!parse_btype(&pt, &ad1))
            {
                die_with_line_number("invalid type");
            }
        }

        skip(')');
        // NOTE: is ignored in returned type as it has a special // meaning in gcc / C++
        type->_tv_.bits &= ~VT_CONSTANT;
        // some ancient pre-K&R C allows a function to return an array 
        // // and the array brackets to be put after the arguments, such 
        // // that "int c()[]" means something like "int[] c()"
        if (g_tok_is('['))
        {
            next();
            skip(']'); // only handle simple "[]"
            mk_pointer(type);
        }
        // we push a anonymous symbol which will contain the function prototype
        ad->f.func_args = arg_size;
        s = sym_push(SYM_FIELD, type, 0, 0);
        s->f = ad->f;
        s->next = first;
        type->_tv_.bits = VT_FUNC;
        type->ref = s;
    }
    else if (g_tok_is('['))
    {
        // array definition
        next();
        i32 n;
        n = -1;
        if (!g_tok_is(']'))
        {
            if (!local_stack || (storage & VT_STATIC))
            {
                vpushi(expr_const(0));
            }
            else
            {
                unary();
            }
            if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
            {
                n = (i32)vtop->c.i;
                if (n < 0)
                {
                    die_with_line_number("invalid array size");
                }
            }
            else
            {
                die_with_line_number("V L A");
            }
        }
        skip(']');
        // parse next post type
        post_type(type, ad, storage, 0);
        if (type->_tv_.bits == VT_FUNC)
        {
            die_with_line_number("declaration of an array of functions");
        }

        if (n != -1)
        {
            vpop();
        }

        // we push an anonymous symbol which will contain the array
        // element type
        s = sym_push(SYM_FIELD, type, 0, n);
        type->_tv_.bits = VT_ARRAY | VT_PTR;
        type->ref = s;
    }
    return 1;
}

// Parse a type declarator (except basic type), and return the type
// in 'type'. 'td' is a bitmask indicating which kind of type decl is
// expected. 'type' should contain the basic type. 'ad' is the
// attribute definition of the basic type. It can be modified by
// type_decl().  If this (possibly abstract) declarator is a pointer chain
// it returns the innermost pointed to type (equals *type, but is a different
// pointer), otherwise returns type itself, that's used for recursive calls.  */
static CType * type_decl(
    CType * type, 
    AttributeDef * ad, 
    i32 * v_, 
    i32 td)
{
    CType * post;
    CType *ret;
    i32 qualifiers;
    i32 storage;

    // recursive type, remove storage bits first, apply them later again
    storage = type->_tv_.bits & VT_STORAGE;
    type->_tv_.bits &= ~VT_STORAGE;
    ret = type;
    post = ret;

    while (g_tok_is('*'))
    {
        qualifiers = 0;

        next();
        switch (g_tok())
        {
            // XXX: clarify attribute handling
        case TOK_ATTRIBUTE2:
        case TOK_STDCALL3:
            parse_attribute(ad);
            break;
        }
        mk_pointer(type);
        type->_tv_.bits |= qualifiers;
        if (ret == type)
            // innermost pointed to type is the one for the first derivation
        {
            ret = pointed_type(type);
        }
    }

    if (g_tok_is('('))
    {
        die_with_line_number("nested (or abstract function) declarators not supported");
    }
    else if ((g_tok() >= TOK_NAME_MIN) && (td & TYPE_DIRECT))
    {
        // type identifier
        *v_ = g_tok_;//!!!1
        next();
    }
    else
    {
        if (!(td & TYPE_ABSTRACT))
        {
            expect("identifier");
        }
        *v_ = 0;
    }
    post_type(post, ad, storage, 0);
    parse_attribute(ad);
    type->_tv_.bits |= storage;
    return ret;
}

// compute the lvalue VT_LVAL_xxx needed to match type t.
static i32 lvalue_type(i32 t)
{
    i32 bt;
    i32 r;
    r = VT_LVAL;
    bt = t & VT_BTYPE;
    if ((bt == VT_BYTE) || (bt == VT_BOOL))
    {
        r |= VT_LVAL_BYTE;
    }
    else if (bt == VT_SHORT)
    {
        r |= VT_LVAL_SHORT;
    }
    else
    {
        return r;
    }
    if (t & VT_UNSIGNED)
    {
        r |= VT_LVAL_UNSIGNED;
    }
    return r;
}

// indirection with full error checking and bound check
static void indir(void)
{
    if (!BTYPES_MATCH(vtop->type._tv_, VT_PTR))
    {
        if (BTYPES_MATCH(vtop->type._tv_, VT_FUNC))
        {
            return;
        }
        expect("pointer");
    }
    if (vtop->r & VT_LVAL)
    {
        gv(RC_INT);
    }
    vtop->type = *pointed_type(&vtop->type);
    // Arrays and functions are never lvalues
    if (!(vtop->type._tv_.bits & VT_ARRAY)
        && (!BTYPES_MATCH(vtop->type._tv_, VT_FUNC)))
    {
        vtop->r |= lvalue_type(vtop->type._tv_.bits);
        // if bound checking, the referenced pointer must be checked
    }
}

// pass a parameter to a function and do type checking and casting
static void gfunc_param_typed(Sym * arg)
{
    if (arg == NULL)
    {
        die_with_line_number("too many arguments to function");
    }
    else
    {
        CType type = arg->type;
        type._tv_.bits &= ~VT_CONSTANT; // need to do that to avoid false warning
        gen_assign_cast(&type);
    }
}

// This skips over a stream of tokens containing balanced {} and ()
// pairs, stopping at outer ',' ';' and '}' (or matching '}' if we started
// with a '{').  If STR then allocates and stores the skipped tokens
// in *STR.  This doesn't check if () and {} are nested correctly,
// i.e. "({)}" is accepted.  */
static void skip_or_save_block(TokenString * * str)
{
    i32 braces = g_tok_is('{');
    i32 level = 0;
    if (str)
    {
        *str = tok_str_alloc();
    }

    while (
        (level > 0) ||
        (
            (!g_tok_is('}')) &&
            (!g_tok_is(',')) &&
            (!g_tok_is(';')) &&
            (!g_tok_is(')'))
        )
    )
    {
        if (g_tok_is(TOK_EOF))
        {
            if (str || (level > 0))
            {
                die_with_line_number("unexpected end of file");
            }
            else
            {
                break;
            }
        }
        if (str)
        {
            tok_str_add_tok(*str);
        }
        i32 t = g_tok_;//!!!1
        next();
        if ((t == '{') || (t == '('))
        {
            level++;
        }
        else if ((t == '}') || (t == ')'))
        {
            level--;
            if ((level == 0) && braces && (t == '}'))
            {
                break;
            }
        }
    }
    if (str)
    {
        tok_str_add(*str, TOK_EOF);
        tok_str_add(*str, TOK_END_OF_REPLAY);
    }
}

static void expr_prod(i32 req_unary, i32 * was_unary)
{
    *was_unary = 1;

    unary();

    i32 tok_first = g_tok_;//!!!1
    i32 nb_term = 1;
    while ((g_tok_is('*')) || (g_tok_is('/')) || (g_tok_is('%')))
    {
        *was_unary = 0;
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (!g_tok_is(tok_first))
        {
            print_error_with_line_number("ACK associative");
        }
        if ((nb_term > 1) && (!g_tok_is('*')))
        {
            print_error_with_line_number("ACK associative!!!");
        }
        ++nb_term;
        i32 t = g_tok_;//!!!1
        next();
        unary();
        gen_op(t);
    }
}

static i32 g_bootstrip = 0;
static void expr_sum(i32 req_unary, i32 * was_unary)
{
    i32 was_prod_unary;
    expr_prod(req_unary, &was_prod_unary);
    *was_unary = was_prod_unary;

    i32 tok_first = g_tok_;//!!!1
    while ((g_tok_is('+')) || (g_tok_is('-')))
    {
        *was_unary = 0;

        if (g_bootstrip && !was_prod_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }

        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }

        if (!g_tok_is(tok_first))
        {
            //!!!
            //tcc_error_noabort("ACK associative");
        }

        i32 t = g_tok_;//!!!1
        next();
        expr_prod(g_bootstrip, &was_prod_unary);
        gen_op(t);
    }
}

static void expr_shift(i32 req_unary, i32 * was_unary)
{
    i32 was_sum_unary;
    expr_sum(req_unary, &was_sum_unary);
    *was_unary = was_sum_unary;
    if ((g_tok_is(TOK_SHL)) || (g_tok_is(TOK_SAR)))
    {
        *was_unary = 0;
        if (g_bootstrip && !was_sum_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }

        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        i32 t;
        t = g_tok_;//!!!1
        next();
        expr_sum(g_bootstrip, &was_sum_unary);
        gen_op(t);
    }
}

static void expr_cmp(i32 req_unary, i32 * was_unary)
{
    i32 was_shift_unary;
    expr_shift(req_unary, &was_shift_unary);
    *was_unary = was_shift_unary;
    if (
        (g_tok_is(TOK_LT)) ||
        (g_tok_is(TOK_GE)) ||
        (g_tok_is(TOK_LE)) ||
        (g_tok_is(TOK_GT))
    )
    {
        *was_unary = 0;
        if (g_bootstrip && !was_shift_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        i32 t;
        t = g_tok_;//!!!1
        next();
        expr_shift(g_bootstrip, &was_shift_unary);
        gen_op(t);
    }
}

static void expr_cmpeq(i32 req_unary, i32 * was_unary)
{
    i32 was_cmp_unary;
    expr_cmp(req_unary, &was_cmp_unary);
    *was_unary = was_cmp_unary;
    if ((g_tok_is(TOK_EQ)) || (g_tok_is(TOK_NE)))
    {
        *was_unary = 0;
        if (g_bootstrip && !was_cmp_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        i32 t;
        t = g_tok_;//!!!1
        next();
        expr_cmp(g_bootstrip, &was_cmp_unary);
        gen_op(t);
    }
}

static void expr_and(i32 req_unary, i32 * was_unary)
{
    i32 was_cmpeq_unary;
    expr_cmpeq(req_unary, &was_cmpeq_unary);
    *was_unary = was_cmpeq_unary;
    while (g_tok_is('&'))
    {
        *was_unary = 0;
        if (g_bootstrip && !was_cmpeq_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        next();
        expr_cmpeq(g_bootstrip, &was_cmpeq_unary);
        gen_op('&');
    }
}

static void expr_xor(i32 req_unary, i32 * was_unary)
{
    i32 was_and_unary;
    expr_and(req_unary, &was_and_unary);
    *was_unary = was_and_unary;
    if (g_tok_is('^'))
    {
        *was_unary = 0;
        if (g_bootstrip && !was_and_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        next();
        expr_and(g_bootstrip, &was_and_unary);
        gen_op('^');
    }
}

static void expr_or(i32 req_unary, i32 * was_unary)
{
    i32 was_xor_unary;
    expr_xor(req_unary, &was_xor_unary);
    *was_unary = was_xor_unary;
    while (g_tok_is('|'))
    {
        *was_unary = 0;
        if (g_bootstrip && !was_xor_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        next();
        expr_xor(g_bootstrip, &was_xor_unary);
        gen_op('|');
    }
}

static void expr_land(i32 req_unary, i32 * was_unary)
{
    i32 was_or_unary;
    expr_or(req_unary, &was_or_unary);
    *was_unary = was_or_unary;
    if (g_tok_is(TOK_LAND))
    {
        *was_unary = 0;
        if (g_bootstrip && !was_or_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        i32 t = 0;
        for (;;)
        {
            if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
            {
                gen_cast_s(VT_BOOL);
                if (vtop->c.i)
                {
                    vpop();
                }
                else
                {
                    nocode_wanted++;
                    while (g_tok_is(TOK_LAND))
                    {
                        next();
                        expr_or(g_bootstrip, &was_or_unary);
                        vpop();
                    }
                    nocode_wanted--;
                    if (t)
                    {
                        gsym(t);
                    }
                    gen_cast_s(VT_INT);
                    break;
                }
            }
            else
            {
                if (!t)
                {
                    save_regs(1);
                }
                t = gvtst(1, t);
            }
            if (!g_tok_is(TOK_LAND))
            {
                if (t)
                {
                    vseti(VT_JMPI, t);
                }
                else
                {
                    vpushi(1);
                }
                break;
            }
            next();
            expr_or(g_bootstrip, &was_or_unary);
        }
    }
}

static void expr_lor(i32 req_unary, i32 * was_unary)
{
    i32 was_land_unary;
    expr_land(req_unary, &was_land_unary);
    *was_unary = was_land_unary;
    if (g_tok_is(TOK_LOR))
    {
        *was_unary = 0;
        if (g_bootstrip && !was_land_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        i32 t = 0;
        for (;;)
        {
            if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
            {
                gen_cast_s(VT_BOOL);
                if (!vtop->c.i)
                {
                    vpop();
                }
                else
                {
                    nocode_wanted++;
                    while (g_tok_is(TOK_LOR))
                    {
                        next();
                        expr_land(g_bootstrip, &was_land_unary);
                        vpop();
                    }
                    nocode_wanted--;
                    if (t)
                    {
                        gsym(t);
                    }
                    gen_cast_s(VT_INT);
                    break;
                }
            }
            else
            {
                if (!t)
                {
                    save_regs(1);
                }
                t = gvtst(0, t);
            }
            if (!g_tok_is(TOK_LOR))
            {
                if (t)
                {
                    vseti(VT_JMP, t);
                }
                else
                {
                    vpushi(0);
                }
                break;
            }
            next();
            expr_land(g_bootstrip, &was_land_unary);
        }
    }
}

// Assuming vtop is a value used in a conditional context
// (i.e. compared with zero) return 0 if it's false, 1 if
// true and -1 if it can't be statically determined.
static i32 condition_3way(void)
{
    i32 c = -1;
    if (
        ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) &&
        !(vtop->r & VT_SYM)
    )
    {
        vdup();
        gen_cast_s(VT_BOOL);
        c = (i32)vtop->c.i;
        vpop();
    }
    return c;
}

static void gexpr(i32 no_eq, i32 req_unary);
static void expr_cond(i32 req_unary)
{
    i32 tt;
    i32 u;
    i32 r1;
    i32 r2;
    i32 rc;
    TYPE_VAL _tv1_;
    TYPE_VAL _tv2_;
    i32 bt1;
    i32 bt2;
    i32 islv;
    i32 c;
    SValue sv;
    CType type;
    CType type1;
    CType type2;

    i32 was_lor_unary;
    expr_lor(req_unary, &was_lor_unary);
    if (g_tok_is('?'))
    {
        if (g_bootstrip && !was_lor_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        if (req_unary)
        {
            print_error_with_line_number("additional op not allowed here");
        }
        next();
        c = condition_3way();
        if (c < 0)
        {
            // needed to avoid having different registers saved in // each branch
            {
                rc = RC_INT;
            }
            gv(rc);
            save_regs(1);
            tt = gvtst(1, 0);
        }
        else
        {
            vpop();
            tt = 0;
        }

        if (c == 0)
        {
            nocode_wanted++;
        }

        gexpr(g_bootstrip, g_bootstrip);

        type1 = vtop->type;
        sv = *vtop; // save value to handle it later
        vtop--; // no vpop so that FP stack is not flushed
        skip(':');

        u = 0;
        if (c < 0)
        {
            u = gjmp(0);
        }
        gsym(tt);

        if (c == 0)
        {
            nocode_wanted--;
        }
        if (c == 1)
        {
            nocode_wanted++;
        }
        expr_cond(1);
        if (c == 1)
        {
            nocode_wanted--;
        }

        type2 = vtop->type;
        _tv1_ = type1._tv_;
        bt1 = _tv1_.bits & VT_BTYPE;
        _tv2_ = type2._tv_;
        bt2 = _tv2_.bits & VT_BTYPE;
        type.ref = NULL;

        // cast operands to correct type according to ISOC rules
        if ((bt1 == VT_LLONG) || (bt2 == VT_LLONG))
        {
            // cast to biggest op
            type._tv_.bits = VT_LLONG | VT_LONG;
            if (bt1 == VT_LLONG)
            {
                type._tv_.bits &= _tv1_.bits;
            }
            if (bt2 == VT_LLONG)
            {
                type._tv_.bits &= _tv2_.bits;
            }
            // convert to unsigned if it does not fit in a i64
            if (
                ((_tv1_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED)) ||
                ((_tv2_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED))
            )
            {
                type._tv_.bits |= VT_UNSIGNED;
            }
        }
        else if ((bt1 == VT_PTR) || (bt2 == VT_PTR))
        {
            // If one is a null ptr constant the result type // is the other.
            if (is_null_pointer(vtop))
            {
                type = type1;
            }
            else if (is_null_pointer(&sv))
            {
                type = type2;
            }
            // XXX: test pointer compatibility, C99 has more elaborate
            //  rules here.
            else
            {
                type = type1;
            }
        }
        else if ((bt1 == VT_FUNC) || (bt2 == VT_FUNC))
        {
            // XXX: test function pointer compatibility
            type = (bt1 == VT_FUNC) ? type1 : type2;
        }
        else if ((bt1 == VT_STRUCT) || (bt2 == VT_STRUCT))
        {
            // XXX: test structure compatibility
            type = (bt1 == VT_STRUCT) ? type1 : type2;
        }
        else if ((bt1 == VT_VOID) || (bt2 == VT_VOID))
        {
            // NOTE: as an extension, we accept void on only one side
            type._tv_.bits = VT_VOID;
        }
        else
        {
            // integer operations
            type._tv_.bits = VT_INT | (VT_LONG & (_tv1_.bits | _tv2_.bits));
            // convert to unsigned if it does not fit in an integer
            if (
                ((_tv1_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED)) ||
                ((_tv2_.bits & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED))
            )
            {
                type._tv_.bits |= VT_UNSIGNED;
            }
        }
        // keep structs lvalue by transforming '(expr ? a : b)' to '*(expr ? &a : &b)' so
        // that '(expr ? a : b).mem' does not error  with "lvalue expected" */
        islv = (vtop->r & VT_LVAL) && (sv.r & VT_LVAL) && (VT_STRUCT == (type._tv_.bits & VT_BTYPE));
        islv &= c < 0;

        // now we convert second operand
        if (c != 1)
        {
            gen_cast(&type);
            if (islv)
            {
                mk_pointer(&vtop->type);
                gaddrof();
            }
            else if (VT_STRUCT == (vtop->type._tv_.bits & VT_BTYPE))
            {
                gaddrof();
            }
        }

        rc = RC_INT;
        if (BTYPES_MATCH(type._tv_, VT_LLONG))
        {
            // for long longs, we use fixed registers to avoid having // to handle a complicated move
            rc = RC_IRET;
        }

        r2 = 0;
        tt = r2;
        if (c < 0)
        {
            r2 = gv(rc);
            tt = gjmp(0);
        }
        gsym(u);

        // this is horrible, but we must also convert first
        // operand
        if (c != 0)
        {
            *vtop = sv;
            gen_cast(&type);
            if (islv)
            {
                mk_pointer(&vtop->type);
                gaddrof();
            }
            else if (VT_STRUCT == (vtop->type._tv_.bits & VT_BTYPE))
            {
                gaddrof();
            }
        }

        if (c < 0)
        {
            r1 = gv(rc);
            move_reg(r2, r1, type._tv_.bits);
            vtop->r = (u16)r2;
            gsym(tt);
            if (islv)
            {
                indir();
            }
        }
    }
}

static i32 op_from_compound_assign(i32 op)
{
    switch (op)
    {
    case TOK_A_MOD:
        return '%';
    case TOK_A_AND:
        return '&';
    case TOK_A_MUL:
        return '*';
    case TOK_A_ADD:
        return '+';
    case TOK_A_SUB:
        return '-';
    case TOK_A_DIV:
        return '/';
    case TOK_A_XOR:
        return '^';
    case TOK_A_OR:
        return '|';
    case TOK_A_SHL:
        return TOK_SHL;
    case TOK_A_SAR:
        return TOK_SAR;
    }

    op_error(op, "op_from_compound_assign");
    return op;
}

static void expr_eq(i32 no_eq, i32 req_unary)
{
    expr_cond(req_unary);
    if (
        (g_tok_is('=')) ||
        (g_tok_is(TOK_A_MOD)) ||
        (g_tok_is(TOK_A_AND)) ||
        (g_tok_is(TOK_A_MUL)) ||
        (g_tok_is(TOK_A_ADD)) ||
        (g_tok_is(TOK_A_SUB)) ||
        (g_tok_is(TOK_A_DIV)) ||
        (g_tok_is(TOK_A_XOR)) ||
        (g_tok_is(TOK_A_OR)) ||
        (g_tok_is(TOK_A_SHL)) ||
        (g_tok_is(TOK_A_SAR))
    )
    {
        if (no_eq)
        {
            print_error_with_line_number("assign not allowed here");
        }

        test_lvalue();
        i32 t;
        t = g_tok_;//!!!1
        next();
        if (t == '=')
        {
            expr_eq(g_bootstrip, 0);
        }
        else
        {
            vdup();
            expr_eq(g_bootstrip, 0);
            gen_op(op_from_compound_assign(t));
        }
        vstore();
    }
}

static void gexpr(i32 no_eq, i32 req_unary)
{
    expr_eq(no_eq, req_unary);
}

// parse a constant expression and return value in vtop.
static void expr_const1(i32 req_unary)
{
    const_wanted++;
    nocode_wanted++;
    expr_cond(req_unary);
    nocode_wanted--;
    const_wanted--;
}

// parse an integer constant and return its value.
static inline i64 expr_const64(i32 req_unary)
{
    i64 c;
    expr_const1(req_unary);
    if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) != VT_CONST)
    {
        expect("constant expression");
    }
    c = vtop->c.i;
    vpop();
    return c;
}

// parse an integer constant and return its value.
// Complain if it doesn't fit 32bit (signed or unsigned).
static i32 expr_const(i32 req_unary)
{
    i32 c;
    i64 wc = expr_const64(req_unary);
    c = (i32)wc;
    if ((c != wc) && ((u32)c != wc))
    {
        die_with_line_number("constant exceeds 32 bit");
    }
    return c;
}

static i32 vt_from_tok(i32 tok)
{
    i32 t = 0;
    switch (tok)
    {
    case TOK_CINT:
    case TOK_CCHAR:
        t = VT_INT;
        break;
    case TOK_CUINT:
        t = VT_INT | VT_UNSIGNED;
        break;
    case TOK_CLLONG:
        t = VT_LLONG;
        break;
    case TOK_CULLONG:
        t = VT_LLONG | VT_UNSIGNED;
        break;
    case TOK_CLONG:
        t = LONG_BTYPE | VT_LONG;
        break;
    case TOK_CULONG:
        t = LONG_BTYPE | VT_LONG | VT_UNSIGNED;
        break;
    default:
        die_with_line_number("expected constant");
        break;
    }
    return t;
}

static i32 global_expr = 0;  // true if compound literals must be allocated globally (used during initializers parsing
static CType char_pointer_type = {0};
static void unary(void)
{
    if (g_bootstrip && in_sizeof && (!g_tok_is('(')))
    {
        print_error_with_line_number("raw unary in sizeof (size of expr) not supported");
    }

    i32 align;
    i32 size;
    i32 r;
    i32 sizeof_caller;
    CType type;
    Sym * s;
    AttributeDef ad_;

    sizeof_caller = in_sizeof;
    in_sizeof = 0;
    type.ref = NULL;
    // XXX: GCC 2.95.3 does not generate a table although it should be // better here
    switch (g_tok())
    {
    case TOK_CINT:
    case TOK_CCHAR:
    case TOK_CUINT:
    case TOK_CLLONG:
    case TOK_CULLONG:
    case TOK_CLONG:
    case TOK_CULONG:;
        i32 vt = vt_from_tok(g_tok_);//!!!1
        type._tv_.bits = vt;
        vsetc(&type, VT_CONST, &tokc);
        next();
        break;

    case TOK_STR:;
        // string parsing
        i32 t1 = VT_BYTE;
        type._tv_.bits = t1;
        mk_pointer(&type);
        type._tv_.bits |= VT_ARRAY;
        memset(&ad_, 0, sizeof(AttributeDef));
        decl_initializer_alloc(&type, &ad_, VT_CONST, 2, 0);
        break;

    case '(':
        next();
        // cast ?
        if (parse_btype(&type, &ad_))
        {
            i32 n_;
            type_decl(&type, &ad_, &n_, TYPE_ABSTRACT);
            skip(')');
            // check ISOC99 compound literal
            if (g_tok_is('{'))
            {
                if (g_bootstrip)
                {
                    die_with_line_number("ISOC99 compound literal not supported");
                }

                // data is allocated locally by default
                if (global_expr)
                {
                    r = VT_CONST;
                }
                else
                {
                    r = VT_LOCAL;
                }
                // all except arrays are lvalues
                if (!(type._tv_.bits & VT_ARRAY))
                {
                    r |= lvalue_type(type._tv_.bits);
                }
                memset(&ad_, 0, sizeof(AttributeDef));
                decl_initializer_alloc(&type, &ad_, r, 1, 0);
            }
            else
            {
                if (sizeof_caller)
                {
                    if (g_bootstrip)
                    {
                        die_with_line_number("how did we get here???");
                    }

                    vpush(&type);
                    return;
                }

                if (g_bootstrip)
                {
                    //!!!
                    //tcc_error_noabort("explicit casts not supported");
                }

                unary();
                gen_cast(&type);
            }
        }
        else if (g_tok_is('{'))
        {
            die_with_line_number("statement expressions not supported");
        }
        else
        {
            if (g_bootstrip && sizeof_caller)
            {
                print_error_with_line_number("size of expr not supported");
            }

            gexpr(g_bootstrip, 0);
            skip(')');
        }
        break;

    case '*':
        next();
        unary();
        indir();
        break;
    case '&':
        next();
        unary();
        // functions names must be treated as function pointers, // except for unary '&' and sizeof. Since we consider that // functions are not lvalues, we only have to handle it // there and in function calls.
        // arrays can also be used although they are not lvalues
        if ((!BTYPES_MATCH(vtop->type._tv_, VT_FUNC)) &&
            !(vtop->type._tv_.bits & VT_ARRAY))
        {
            test_lvalue();
        }
        mk_pointer(&vtop->type);
        gaddrof();
        break;
    case '!':
        next();
        unary();
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
        {
            gen_cast_s(VT_BOOL);
            vtop->c.i = !vtop->c.i;
        }
        else if ((vtop->r & VT_VALMASK) == VT_CMP)
        {
            vtop->c.i ^= 1;
        }
        else
        {
            save_regs(1);
            vseti(VT_JMP, gvtst(1, 0));
        }
        break;
    case '~':
        next();
        unary();
        vpushi(-1);
        gen_op('^');
        break;
    case '+':
        next();
        unary();
        if (BTYPES_MATCH(vtop->type._tv_, VT_PTR))
        {
            die_with_line_number("pointer not accepted for unary plus");
        }
        // In order to force cast, we add zero
        {
            vpushi(0);
            gen_op('+');
        }
        break;
    case TOK_SIZEOF:;
        next();
        in_sizeof++;

        nocode_wanted++;
        if (!g_bootstrip)
        {
            // sets in_sizeof = 0
            unary();
        }
        else
        {
            AttributeDef ad;
            CType type_parsed;
            type_parsed.ref = NULL;

            in_sizeof = 0;

            skip('(');
            if (parse_btype(&type_parsed, &ad))
            {
                i32 n;
                type_decl(&type_parsed, &ad, &n, TYPE_ABSTRACT);
                skip(')');
                vpush(&type_parsed);
            }
            else
            {
                die_with_line_number("expected a type name/decl");
            }
        }
        type = vtop->type;
        vpop();
        nocode_wanted--;

        s = vtop[1].sym; // hack: accessing previous vtop
        size = type_size(&type, &align);
        if (size < 0)
        {
            die_with_line_number("sizeof applied to an incomplete type");
        }
        vpushs(size);
        vtop->type._tv_.bits |= VT_UNSIGNED;
        break;
    case TOK_INC:
    case TOK_DEC:;
        // prefix inc/dec
        i32 t3 = g_tok_;//!!!1
        next();
        unary();
        inc(0, t3);
        break;
    case '-':
        next();
        unary();
        vpushi(0);
        vswap();
        gen_op('-');
        break;

    default:;
        i32 t5 = g_tok_;//!!!1
        next();
        if (t5 < TOK_KEYWORD_MAX)
        {
            expect("identifier");
        }
        s = sym_find(t5);
        if (!s)
        {
            char * name = get_tok_str(t5, NULL);
            print_error_with_line_number("implicit declaration of function");
            die_with_line_number(name);
        }

        r = s->r;
        // A symbol that has a register is a local register variable, // which starts out as VT_LOCAL value.
        if ((r & VT_VALMASK) < VT_CONST)
        {
            r = (r & ~VT_VALMASK) | VT_LOCAL;
        }

        vset(&s->type, r, s->c);
        // Point to s as backpointer (even without r&VT_SYM). // Will be used by at least the x86 inline asm parser for // regvars.
        vtop->sym = s;

        if (r & VT_SYM)
        {
            vtop->c.i = 0;
        }
        else if ((r == VT_CONST) && IS_ENUM_VAL(s->type._tv_.bits))
        {
            vtop->c.i = s->enum_val;
        }
        break;
    }

    // post operations
    while (1)
    {
        if ((g_tok_is(TOK_INC)) || (g_tok_is(TOK_DEC)))
        {
            inc(1, g_tok());
            next();
        }
        else if ((g_tok_is('.')) || (g_tok_is(TOK_ARROW)))
        {
            i32 qualifiers;
            // field
            if (g_tok_is(TOK_ARROW))
            {
                indir();
            }
            qualifiers = vtop->type._tv_.bits & (VT_CONSTANT | VT_VOLATILE);
            test_lvalue();
            gaddrof();
            // expect pointer on structure
            if (!BTYPES_MATCH(vtop->type._tv_, VT_STRUCT))
            {
                expect("struct or union");
            }
            next();
            if ((g_tok_is(TOK_CINT)) || (g_tok_is(TOK_CUINT)))
            {
                expect("field name");
            }
            s = find_field(&vtop->type, g_tok_);//!!!1
            if (!s)
            {
                tcc_error_puts("field not found: %s", get_tok_str(g_tok() & ~SYM_FIELD, &tokc));
            }
            // add field offset to pointer
            vtop->type = char_pointer_type; // change type to 'char *' */
            vpushi(s->c);
            gen_op('+');
            // change type to field type, and set to lvalue
            vtop->type = s->type;
            vtop->type._tv_.bits |= qualifiers;
            // an array is never an lvalue
            if (!(vtop->type._tv_.bits & VT_ARRAY))
            {
                vtop->r |= lvalue_type(vtop->type._tv_.bits);
            }
            next();
        }
        else if (g_tok_is('['))
        {
            next();
            expr_cond(0); // OK
            gen_op('+');
            indir();
            skip(']');
        }
        else if (g_tok_is('('))
        {
            SValue ret;
            memset(&ret, 0, sizeof(SValue));

            Sym * sa;
            i32 nb_args;
            i32 ret_nregs = 0;
            i32 regsize;

            // function call
            if (!BTYPES_MATCH(vtop->type._tv_, VT_FUNC))
            {
                // pointer test (no array accepted)
                if ((vtop->type._tv_.bits & (VT_BTYPE | VT_ARRAY)) == VT_PTR)
                {
                    vtop->type = *pointed_type(&vtop->type);
                    if (!BTYPES_MATCH(vtop->type._tv_, VT_FUNC))
                    {
                        goto error_func;
                    }
                }
                else
                {
                error_func:
                    expect("function pointer");
                }
            }
            else
            {
                vtop->r &= ~VT_LVAL; // no lvalue
            }
            // get return type
            s = vtop->type.ref;
            next();
            sa = s->next; // first parameter
            regsize = 0;
            nb_args = regsize;
            ret.r2 = VT_CONST;
            // compute first implicit argument if a structure is returned
            if (BTYPES_MATCH(s->type._tv_, VT_STRUCT))
            {
                die_with_line_number("returning struct");
            }
            else
            {
                ret_nregs = 1;
                ret.type = s->type;
            }

            if (ret_nregs)
            {
                // return in register
                
                enum
                {
                    // return registers for function
                    REG_IRET = TREG_EAX, // single word int return register
                    REG_LRET = TREG_EDX, // second word return register (for i64)
                };

                if (BTYPES_MATCH(ret.type._tv_, VT_LLONG))
                {
                    ret.r2 = REG_LRET;
                }
                ret.r = REG_IRET;
                
                ret.c.i = 0;
            }
            if (!g_tok_is(')'))
            {
                for (;;)
                {
                    expr_eq(g_bootstrip, 0);
                    gfunc_param_typed(sa);
                    nb_args++;
                    if (sa)
                    {
                        sa = sa->next;
                    }
                    if (g_tok_is(')'))
                    {
                        break;
                    }
                    skip(',');
                }
            }
            if (sa)
            {
                die_with_line_number("too few arguments to function");
            }
            skip(')');
            gfunc_call(nb_args);

            // return value
            r = ret.r + ret_nregs + !ret_nregs;
            while (r-- > ret.r)
            {
                vsetc(&ret.type, r, &ret.c);
                vtop->r2 = ret.r2; // Loop only happens when r2 is VT_CONST
            }

            // handle packed struct return
            if ((BTYPES_MATCH(s->type._tv_, VT_STRUCT)) && ret_nregs)
            {
                i32 addr;
                i32 offset;

                size = type_size(&s->type, &align);
                // We're writing whole regs often, make sure there's enough // space.  Assume register size is power of 2.
                if (regsize > align)
                {
                    align = regsize;
                }
                loc = (loc - size) & -align;
                addr = loc;
                offset = 0;
                for (;;)
                {
                    vset(&ret.type, VT_LOCAL | VT_LVAL, addr + offset);
                    vswap();
                    vstore();
                    vtop--;
                    if (--ret_nregs == 0)
                    {
                        break;
                    }
                    offset += regsize;
                }
                vset(&s->type, VT_LOCAL | VT_LVAL, addr);
            }
        }
        else
        {
            break;
        }
    }
}

// enum/struct/union declaration. u is VT_ENUM/VT_STRUCT/VT_UNION
static void struct_decl(CType * type, i32 u_)
{
    i32 v;
    i32 c;
    i32 size;
    i32 align;
    i32 flexible;
    Sym * s;
    Sym * ss;
    Sym * *ps;
    AttributeDef ad;
    AttributeDef ad1;
    CType type1;
    CType btype;

    memset(&ad, 0, sizeof(AttributeDef));
    next();
    parse_attribute(&ad);
    if (!g_tok_is('{'))
    {
        v = g_tok_;//!!!1
        next();
        // struct already defined ? return it
        if (v < TOK_NAME_MIN)
        {
            expect("struct/union/enum name");
        }
        s = struct_find(v);
        if (s)
        {
            if (u_ == s->type._tv_.bits)
            {
                goto do_decl;
            }
            if ((u_ == VT_ENUM) && IS_ENUM(s->type._tv_.bits))
            {
                goto do_decl;
            }
            tcc_error_puts("redefinition of '%s'", get_tok_str(v, NULL));
        }
    }
    else
    {
        v = anon_sym++;
    }
    // Record the original enum/struct/union token.
    type1._tv_.bits
        = (u_ == VT_ENUM)
        ? (u_ | VT_INT | VT_UNSIGNED)
        : (u_)
        ;
    type1.ref = NULL;
    // we put an undefined size for struct/union
    s = sym_push(v | SYM_STRUCT, &type1, 0, -1);
    s->r = 0; // default alignment is zero as gcc
do_decl:
    type->_tv_ = s->type._tv_;
    type->ref = s;

    if (g_tok_is('{'))
    {
        next();
        if (s->c != -1)
        {
            die_with_line_number("struct/union/enum already defined");
        }
        // cannot be empty
        // non empty enums are not allowed
        enum
        {
            _64_BIT_ENUM_TYPE = (!LONG_SIZE_IS_4 ? (VT_LLONG | VT_LONG) : VT_LLONG),
        };

        ps = &s->next;
        if (u_ == VT_ENUM)
        {
            i64 ll = 0;
            i64 pl = 0;
            i64 nl = 0;
            CType t;
            t.ref = s;
            // enum symbols have static storage
            t._tv_.bits = VT_INT | VT_STATIC | VT_ENUM_VAL;
            for (;;)
            {
                v = g_tok_;//!!!1
                if (v < TOK_KEYWORD_MAX)
                {
                    expect("identifier");
                }
                ss = sym_find(v);
                if (ss && !local_stack)
                {
                    tcc_error_puts("redefinition of enumerator '%s'",
                        get_tok_str(v, NULL));
                }
                next();
                if (g_tok_is('='))
                {
                    next();
                    ll = expr_const(0);//!!!
                }
                ss = sym_push(v, &t, VT_CONST, 0);
                ss->enum_val = ll;
                *ps = ss; ps = &ss->next;
                if (ll < nl)
                {
                    nl = ll;
                }
                if (ll > pl)
                {
                    pl = ll;
                }
                if (!g_tok_is(','))
                {
                    break;
                }
                next();
                ll++;
                // NOTE: we accept a trailing comma
                if (g_tok_is('}'))
                {
                    break;
                }
            }
            skip('}');
            // set integral type of the enum
            t._tv_.bits = VT_INT;
            if (nl >= 0)
            {
                if (pl != (u32)pl)
                {
                    t._tv_.bits = _64_BIT_ENUM_TYPE;
                }
                t._tv_.bits |= VT_UNSIGNED;
            }
            else if ((pl != (i32)pl) || (nl != (i32)nl))
            {
                t._tv_.bits = _64_BIT_ENUM_TYPE;
            }
            type->_tv_.bits = t._tv_.bits | VT_ENUM;
            s->type._tv_ = type->_tv_;
            s->c = 0;
            // set type for enum members
            ss = s->next;
            for (; ss; ss = ss->next)
            {
                ll = ss->enum_val;
                if (ll == (i32)ll) // default is int if it fits
                {
                    continue;
                }
                if (t._tv_.bits & VT_UNSIGNED)
                {
                    ss->type._tv_.bits |= VT_UNSIGNED;
                    if (ll == (u32)ll)
                    {
                        continue;
                    }
                }
                ss->type._tv_.bits = (ss->type._tv_.bits & ~VT_BTYPE) | _64_BIT_ENUM_TYPE;
            }
        }
        else
        {
            c = 0;
            flexible = 0;
            while (!g_tok_is('}'))
            {
                if (!parse_btype(&btype, &ad1))
                {
                    skip(';');
                    continue;
                }
                
                if (flexible)
                {
                    tcc_error_puts("flexible array member '%s' not at the end of struct",
                        get_tok_str(v, NULL));
                }
                v = 0;
                type1 = btype;
                if (g_tok_is(':'))
                {
                    die_with_line_number("bitfields not suuported");
                }
                
                type_decl(&type1, &ad1, &v, TYPE_DIRECT);
                if (type_size(&type1, &align) < 0)
                {
                    if ((u_ == VT_STRUCT) && (type1._tv_.bits & VT_ARRAY) && c)
                    {
                        flexible = 1;
                    }
                    else
                    {
                        tcc_error_puts("field '%s' has incomplete type",
                            get_tok_str(v, NULL));
                    }
                }
                if ((BTYPES_MATCH(type1._tv_, VT_FUNC)) ||
                    (type1._tv_.bits & VT_STORAGE))
                {
                    tcc_error_puts("invalid type for '%s'",
                        get_tok_str(v, NULL));
                }
                
                size = type_size(&type1, &align);
                if (
                    (v != 0) ||
                    (BTYPES_MATCH(type1._tv_, VT_STRUCT))
                )
                {
                    // Remember we've seen a real field to check // for placement of flexible array member.
                    c = 1;
                }
                // If member is a struct, enforce // placing into the struct (as anonymous).
                if ((v == 0) &&
                    (BTYPES_MATCH(type1._tv_, VT_STRUCT)))
                {
                    v = anon_sym++;
                }
                if (v)
                {
                    ss = sym_push(v | SYM_FIELD, &type1, 0, 0);
                    *ps = ss;
                    ps = &ss->next;
                }

                skip(';');
            }
            skip('}');
            parse_attribute(&ad);
            struct_layout(type, &ad);
        }
    }
}

// return 0 if no type declaration. otherwise, return the basic type
// and skip it.
//
static i32 parse_btype(CType * type, AttributeDef * ad)
{
    TYPE_VAL _tv_;
    TYPE_VAL _tvu_;
    i32 bt;
    i32 st;
    i32 type_found;
    i32 typespec_found;
    i32 g;
    Sym * s;
    CType type1;

    i32 from_typedef = 0;

    memset(ad, 0, sizeof(AttributeDef));
    type_found = 0;
    typespec_found = 0;
    _tv_.bits = VT_INT;
    st = -1;
    bt = -1;
    type->ref = NULL;

    while (1)
    {
        switch (g_tok())
        {
            // basic types
        case TOK_CHAR:
            _tvu_.bits = VT_BYTE;
        basic_type:
            next();
        basic_type1:
            if ((_tvu_.bits == VT_SHORT) || (_tvu_.bits == VT_LONG))
            {
                if ((st != -1) || ((bt != -1) && (bt != VT_INT)))
                {
                tmbt:
                    die_with_line_number("too many basic types");
                }
                st = _tvu_.bits;
            }
            else
            {
                if ((bt != -1) || ((st != -1) && (_tvu_.bits != VT_INT)))
                {
                    goto tmbt;
                }
                bt = _tvu_.bits;
            }
            if (_tvu_.bits != VT_INT)
            {
                _tv_.bits = (_tv_.bits & ~(VT_BTYPE | VT_LONG)) | _tvu_.bits;
            }
            typespec_found = 1;
            break;
        case TOK_VOID:
            _tvu_.bits = VT_VOID;
            goto basic_type;
        case TOK_SHORT:
            _tvu_.bits = VT_SHORT;
            goto basic_type;
        case TOK_INT:
            _tvu_.bits = VT_INT;
            goto basic_type;
        case TOK_LONG:
            if ((_tv_.bits & (VT_BTYPE | VT_LONG)) == VT_LONG)
            {
                _tv_.bits = (_tv_.bits & ~(VT_BTYPE | VT_LONG)) | VT_LLONG;
            }
            else
            {
                _tvu_.bits = VT_LONG;
                goto basic_type;
            }
            next();
            break;
        case TOK_BOOL:
            _tvu_.bits = VT_BOOL;
            goto basic_type;
        case TOK_ENUM:
            struct_decl(&type1, VT_ENUM);
        basic_type2:
            _tvu_ = type1._tv_;
            type->ref = type1.ref;
            goto basic_type1;
        case TOK_STRUCT:
            struct_decl(&type1, VT_STRUCT);
            goto basic_type2;
        case TOK_UNION:
            struct_decl(&type1, VT_UNION);
            goto basic_type2;

            // type modifiers
        case TOK_SIGNED1:
            if (g_bootstrip)
            {
                print_error_with_line_number("TOK_SIGNED1");
            }
            if ((_tv_.bits & (VT_DEFSIGN | VT_UNSIGNED)) == (VT_DEFSIGN | VT_UNSIGNED))
            {
                die_with_line_number("signed and unsigned modifier");
            }
            _tv_.bits |= VT_DEFSIGN;
            next();
            typespec_found = 1;
            break;
        case TOK_UNSIGNED:
            if ((_tv_.bits & (VT_DEFSIGN | VT_UNSIGNED)) == VT_DEFSIGN)
            {
                die_with_line_number("signed and unsigned modifier");
            }
            _tv_.bits |= VT_DEFSIGN | VT_UNSIGNED;
            next();
            typespec_found = 1;
            break;

            // storage
        case TOK_EXTERN:
            g = VT_EXTERN;
            goto storage;
        case TOK_STATIC:
            g = VT_STATIC;
            goto storage;
        case TOK_TYPEDEF:
            g = VT_TYPEDEF;
            goto storage;
        storage:
            if (_tv_.bits & (VT_EXTERN | VT_STATIC | VT_TYPEDEF) & ~g)
            {
                die_with_line_number("multiple storage classes");
            }
            _tv_.bits |= g;
            next();
            break;
        case TOK_INLINE2:
            _tv_.bits |= VT_INLINE;
            next();
            break;

            // GNUC attribute
        case TOK_ATTRIBUTE2:
        case TOK_STDCALL3:
            parse_attribute(ad);
            break;
        default:
            if (typespec_found)
            {
                goto the_end;
            }
            s = sym_find(g_tok());
            if (!s || !(s->type._tv_.bits & VT_TYPEDEF))
            {
                goto the_end;
            }
            if (from_typedef)
            {
                die_with_line_number("nested typedef ref???");
            }
            from_typedef = 1;
            _tv_.bits &= ~(VT_BTYPE | VT_LONG);
            _tvu_.bits = _tv_.bits & ~(VT_CONSTANT | VT_VOLATILE); _tv_.bits ^= _tvu_.bits;
            type->_tv_.bits = (s->type._tv_.bits & ~VT_TYPEDEF) | _tvu_.bits;
            type->ref = s->type.ref;
            if (_tv_.bits)
            {
                die_with_line_number("parse_btype_qualify");
            }
            _tv_.bits = type->_tv_.bits;
            // get attributes from typedef
            sym_to_attr(ad, s);
            next();
            typespec_found = 1;
            bt = -2;
            st = -2;
            break;
        }
        type_found = 1;
    }
the_end:
    // VT_LONG is used just as a modifier for VT_INT / VT_LLONG
    bt = _tv_.bits & (VT_BTYPE | VT_LONG);
    if (bt == VT_LONG)
    {
        _tv_.bits |= LONG_BTYPE;
    }

    type->_tv_ = _tv_;
    return type_found;
}

// return the label token if current token is a label, otherwise
// return zero
static i32 is_label(i32 * last_i_name)
{
    i32 last_tok;

    // fast test first
    if (g_tok() < TOK_KEYWORD_MAX)
    {
        return 0;
    }
    // no need to save tokc because tok is an identifier
    last_tok = g_tok_;//!!!1
    *last_i_name = g_tok_i_name;
    next();
    if (g_tok_is(':'))
    {
        return last_tok;
    }
    else
    {
        unget_tok(last_tok, *last_i_name);
        return 0;
    }
}

static void gfunc_return(CType * func_type)
{
    if (BTYPES_MATCH(func_type->_tv_, VT_STRUCT))
    {
        die_with_line_number("returning struct");
    }
    else
    {
        gv(RC_IRET);
    }
    vtop--; // NOT vpop() because on x86 it would flush the fp stack
}

static void gtst_addr(i32 inv, i32 a);
static void gjmp_addr(i32 a);
static void gcase(
    i32 default_symbol,
    struct case_t * * base,
    i32 len,
    i32 * bsym)
{
    struct case_t * p;
    i32 e;
    i32 ll = BTYPES_MATCH(vtop->type._tv_, VT_LLONG);
    gv(RC_INT);
    while (len > 4)
    {
        // binary search
        i32 half_len = len / 2;
        p = base[half_len];
        vdup();
        if (ll)
        {
            die_with_line_number("vpushll");
        }
        else
        {
            vpushi(p->value_);
        }
        gen_op(TOK_LE);
        e = gtst(1, 0);
        vdup();
        if (ll)
        {
            die_with_line_number("vpushll");
        }
        else
        {
            vpushi(p->value_);
        }
        gen_op(TOK_GE);
        gtst_addr(0, p->symbol); // v1 <= x <= v2
        // x < v1
        gcase(default_symbol, base, len / 2, bsym);
        if (default_symbol)
        {
            gjmp_addr(default_symbol);
        }
        else
        {
            *bsym = gjmp(*bsym);
        }
        // x > v2
        gsym(e);
        e = (len / 2) + 1;
        base += e; len -= e;
    }
    // linear scan
    while (len--)
    {
        p = *base++;
        vdup();
        if (ll)
        {
            die_with_line_number("vpushll");
        }
        else
        {
            vpushi(p->value_);
        }

        gen_op(TOK_EQ);
        gtst_addr(0, p->symbol);
    }
}

static void sort_cases(case_t * * cases, i32 nb_cases)
{
    // Bubble sort

    for (;;)
    {
        i32 swapped = 0;

        // check (i < (nb_cases - 1)), since we
        //  look at cases[i + 1]

        i32 i = 0;
        for (; i < (nb_cases - 1); ++i)
        {
            i32 i_next = i + 1;
            case_t * * case_a = &cases[i];
            case_t * * case_b = &cases[i_next];

            i32 value_a = (*case_a)->value_;
            i32 value_b = (*case_b)->value_;

            if (value_a > value_b)
            {
                case_t * temp = *case_a;
                *case_a = *case_b;
                *case_b = temp;
                swapped = 1;
            }
        }

        if (!swapped)
        {
            // If no elements were swapped,
            //  the array is sorted
            break;
        }
    }
}

static void gsym_addr(i32 t, i32 a);
static Sym * local_label_stack = 0;
static void block(
    i32 * break_sym,
    i32 * continue_sym,
    switch_t * cur_switch)
{
    if (g_tok_is_kw(TOK_IF))
    {
        // if test
        i32 saved_nocode_wanted = nocode_wanted;
        next();
        skip('(');
        gexpr(g_bootstrip, 0);
        skip(')');
        i32 cond;
        cond = condition_3way();
        i32 a;
        if (cond == 1)
        {
            a = 0; vpop();
        }
        else
        {
            a = gvtst(1, 0);
        }
        if (cond == 0)
        {
            nocode_wanted |= 0x20000000;
        }
        block(break_sym, continue_sym, cur_switch);
        if (cond != 1)
        {
            nocode_wanted = saved_nocode_wanted;
        }
        if (g_tok_is_kw(TOK_ELSE))
        {
            next();
            i32 d;
            d = gjmp(0);
            gsym(a);
            if (cond == 1)
            {
                nocode_wanted |= 0x20000000;
            }
            block(break_sym, continue_sym, cur_switch);
            gsym(d); // patch else jmp
            if (cond != 0)
            {
                nocode_wanted = saved_nocode_wanted;
            }
        }
        else
        {
            gsym(a);
        }
    }
    else if (g_tok_is_kw(TOK_WHILE))
    {
        i32 saved_nocode_wanted;
        nocode_wanted &= ~0x20000000;
        next();
        i32 d;
        d = ind;
        skip('(');
        gexpr(g_bootstrip, 0);
        skip(')');
        i32 a;
        a = gvtst(1, 0);
        i32 b;
        b = 0;
        saved_nocode_wanted = nocode_wanted;
        block(&a, &b, cur_switch);
        nocode_wanted = saved_nocode_wanted;
        gjmp_addr(d);
        gsym(a);
        gsym_addr(b, d);
    }
    else if (g_tok_is('{'))
    {
        Sym * llabel;

        next();
        // record local declaration stack position
        Sym * s;
        s = local_stack;
        llabel = local_label_stack;

        while (!g_tok_is('}'))
        {
            i32 last_i_name;
            i32 a;
            a = is_label(&last_i_name);
            if (a)
            {
                unget_tok(a, last_i_name);
            }
            else
            {
                decl(VT_LOCAL);
            }
            if (!g_tok_is('}'))
            {
                block(break_sym, continue_sym, cur_switch);
            }
        }
        // pop locally defined labels
        label_pop(&local_label_stack, llabel);
        // pop locally defined symbols
        sym_pop(&local_stack, s);

        next();
    }
    else if (g_tok_is_kw(TOK_RETURN))
    {
        next();
        if (!g_tok_is(';'))
        {
            expr_cond(1);
            gen_assign_cast(&func_vt);
            if (BTYPES_MATCH(func_vt._tv_, VT_VOID))
            {
                vtop--;
            }
            else
            {
                gfunc_return(&func_vt);
            }
        }
        skip(';');
        
        // jump to exit function
        rsym = gjmp(rsym);

        nocode_wanted |= 0x20000000;
    }
    else if (g_tok_is_kw(TOK_BREAK))
    {
        // compute jump
        if (!break_sym)
        {
            die_with_line_number("cannot break");
        }
        *break_sym = gjmp(*break_sym);
        next();
        skip(';');
        nocode_wanted |= 0x20000000;
    }
    else if (g_tok_is_kw(TOK_CONTINUE))
    {
        // compute jump
        if (!continue_sym)
        {
            die_with_line_number("cannot continue");
        }
        *continue_sym = gjmp(*continue_sym);
        next();
        skip(';');
    }
    else if (g_tok_is_kw(TOK_FOR))
    {
        i32 e;
        i32 saved_nocode_wanted;
        nocode_wanted &= ~0x20000000;
        next();
        skip('(');
        Sym * s;
        s = local_stack;
        if (!g_tok_is(';'))
        {
            decl0(VT_LOCAL, NULL);
        }
        else
        { 
            skip(';');
        }
        i32 d;
        d = ind;
        i32 c;
        c = ind;
        i32 a;
        a = 0;
        i32 b;
        b = 0;
        i32 has_cond = 0;
        if (!g_tok_is(';'))
        {
            has_cond = 1;
            gexpr(g_bootstrip, 0); //!!!
            a = gvtst(1, 0);
        }
        skip(';');
        if (!g_tok_is(')'))
        {
            e = gjmp(0);
            c = ind;
            gexpr(0, 0); //!!!
            vpop();
            gjmp_addr(d);
            gsym(e);
        }
        else if (g_bootstrip && has_cond)
        {
            print_error_with_line_number("this for loop is equivalent to a while loop, just use that");
        }
        skip(')');
        saved_nocode_wanted = nocode_wanted;
        block(&a, &b, cur_switch);
        nocode_wanted = saved_nocode_wanted;
        gjmp_addr(c);
        gsym(a);
        gsym_addr(b, c);
        sym_pop(&local_stack, s);
    }
    else if (g_tok_is_kw(TOK_DO))
    {
        if (g_bootstrip)
        {
            print_error_with_line_number("DO WHILE");
        }

        i32 saved_nocode_wanted;
        nocode_wanted &= ~0x20000000;
        next();
        i32 a;
        a = 0;
        i32 b;
        b = 0;
        i32 d;
        d = ind;
        saved_nocode_wanted = nocode_wanted;
        block(&a, &b, cur_switch);
        skip(TOK_WHILE);
        skip('(');
        gsym(b);
        gexpr(g_bootstrip, 0);
        i32 c;
        c = gvtst(0, 0);
        gsym_addr(c, d);
        nocode_wanted = saved_nocode_wanted;
        skip(')');
        gsym(a);
        skip(';');
    }
    else if (g_tok_is_kw(TOK_SWITCH))
    {
        if (g_bootstrip && cur_switch)
        {
            print_error_with_line_number("nested switch");
        }

        switch_t this_switch;
        i32 saved_nocode_wanted = nocode_wanted;
        SValue switchval;
        next();
        skip('(');
        gexpr(g_bootstrip, 0);
        skip(')');
        switchval = *vtop--;
        i32 a;
        a = 0;
        i32 b;
        b = gjmp(0); // jump to first case
        this_switch.cases = NULL;
        this_switch.nb_cases = 0;
        this_switch.default_symbol = 0;
        block(&a, continue_sym, &this_switch);
        nocode_wanted = saved_nocode_wanted;
        a = gjmp(a); // add implicit break
        // case lookup
        gsym(b);
        sort_cases(this_switch.cases, this_switch.nb_cases);
        b = 1;
        for (; b < this_switch.nb_cases; b++)
        {
            i32 b_prev = b - 1;
            if (this_switch.cases[b_prev]->value_ >= this_switch.cases[b]->value_)
            {
                die_with_line_number("duplicate case value");
            }
        }
        // Our switch table sorting is signed, so the compared // value needs to be as well when it's 64bit.
        if (BTYPES_MATCH(switchval.type._tv_, VT_LLONG))
        {
            switchval.type._tv_.bits &= ~VT_UNSIGNED;
        }
        vpushv(&switchval);
        gcase(this_switch.default_symbol, this_switch.cases, this_switch.nb_cases, &a);
        vpop();
        if (this_switch.default_symbol)
        {
            gjmp_addr(this_switch.default_symbol);
        }
        dynarray_reset(&this_switch.cases, &this_switch.nb_cases);
        // break label
        gsym(a);
    }
    else if (g_tok_is_kw(TOK_CASE))
    {
        struct case_t * cr = mallocz_or_die(sizeof(struct case_t));
        if (!cur_switch)
        {
            expect("switch");
        }
        nocode_wanted &= ~0x20000000;
        next();
        cr->value_ = expr_const(g_bootstrip);
        cr->symbol = ind;
        dynarray_add_(&cur_switch->cases, &cur_switch->nb_cases, cr);
        skip(':');
        goto block_after_label;
    }
    else if (g_tok_is_kw(TOK_DEFAULT))
    {
        next();
        skip(':');
        if (!cur_switch)
        {
            expect("switch");
        }
        if (cur_switch->default_symbol)
        {
            die_with_line_number("too many 'default'");
        }
        cur_switch->default_symbol = ind;
        goto block_after_label;
    }
    else if (g_tok_is_kw(TOK_GOTO))
    {
        next();
        if (g_tok() >= TOK_KEYWORD_MAX)
        {
            Sym * s;
            s = label_find(g_tok());
            // put forward definition if needed
            if (!s)
            {
                s = label_push(&global_label_stack, g_tok_, LABEL_FORWARD);//!!!1
            }
            else
            {
                if (s->r == LABEL_DECLARED)
                {
                    s->r = LABEL_FORWARD;
                }
            }
            if (s->r & LABEL_FORWARD)
            {
                s->jnext = gjmp(s->jnext);
            }
            else
            {
                gjmp_addr(s->jnext);
            }
            next();
        }
        else
        {
            expect("label identifier");
        }
        skip(';');
    }
    else
    {
        i32 last_i_name;
        i32 b;
        b = is_label(&last_i_name);
        if (b)
        {
            // label case
            next();
            Sym * s;
            s = label_find(b);
            if (s)
            {
                if (s->r == LABEL_DEFINED)
                {
                    tcc_error_puts("duplicate label '%s'", get_tok_str(s->v, NULL));
                }
                gsym(s->jnext);
                s->r = LABEL_DEFINED;
            }
            else
            {
                s = label_push(&global_label_stack, b, LABEL_DEFINED);
            }
            s->jnext = ind;
            // we accept this, but it is a mistake
        block_after_label:
            nocode_wanted &= ~0x20000000;
            if (g_tok_is('}'))
            {
                print_error_with_line_number("deprecated use of label at end of compound statement");
            }
            else
            {
                block(break_sym, continue_sym, cur_switch);
            }
        }
        else
        {
            // expression case
            if (!g_tok_is(';'))
            {
                gexpr(0, g_bootstrip);
                vpop();
            }
            skip(';');
        }
    }
}

static void parse_init_elem(i32 expr_type, i32 req_unary)
{
    i32 saved_global_expr;
    switch (expr_type)
    {
    case EXPR_CONST:
        // compound literals must be allocated globally in this case
        saved_global_expr = global_expr;
        global_expr = 1;
        expr_const1(req_unary);
        global_expr = saved_global_expr;
        // NOTE: symbols are accepted, as well as lvalue for anon symbols // (compound literals).
        if (
            (
                ((vtop->r & (VT_VALMASK | VT_LVAL)) != VT_CONST) &&
                (
                    ((vtop->r & (VT_SYM | VT_LVAL)) != (VT_SYM | VT_LVAL)) ||
                    (vtop->sym->v < SYM_FIRST_ANOM)
                )
            )
        )
        {
            die_with_line_number("initializer element is not constant");
        }
        break;
    case EXPR_ANY:
        expr_eq(g_bootstrip, 0);
        break;
    }
}

// put zeros for variable based init
static void init_putz(Section * sec, u32 c, i32 size)
{
    (void)size;
    (void)c;

    if (sec)
    {
        // nothing to do because globals are already set to zero
    }
    else
    {
        die_with_line_number("ACK! don't know how to zero init here.");
    }
}

// t is the array or struct type. c is the array or struct
// address. cur_field is the pointer to the current
// field, for arrays the 'c' member contains the current start
// index.  'size_only' is true if only size info is needed (only used
// in arrays).  al contains the already initialized length of the
// current container (starting at c).  This returns the new length of that.
static i32 decl_designator(CType * type, Section * sec, u32 c,
    Sym * * cur_field, i32 size_only, i32 al)
{
    Sym * s;
    Sym * f;
    i32 index;
    i32 index_last;
    i32 align;
    i32 nb_elems;
    i32 elem_size;
    u32 corig = c;

    elem_size = 0;
    nb_elems = 1;
    // NOTE: we only support ranges for last designator
    while (
        (nb_elems == 1) &&
        (
            (g_tok_is('[')) ||
            (g_tok_is('.'))
        )
    )
    {
        if (g_tok_is('['))
        {
            if (!(type->_tv_.bits & VT_ARRAY))
            {
                expect("array type");
            }
            next();
            index_last = expr_const(g_bootstrip);
            index = index_last;
            skip(']');
            s = type->ref;
            if ((index < 0) || ((s->c >= 0) && (index_last >= s->c)) ||
                (index_last < index))
            {
                die_with_line_number("invalid index");
            }
            if (cur_field)
            {
                (*cur_field)->c = index_last;
            }
            type = pointed_type(type);
            elem_size = type_size(type, &align);
            c += index * elem_size;
            nb_elems = index_last - index + 1;
        }
        else
        {
            next();
            i32 l;
            l = g_tok_;
            next();
            if (!BTYPES_MATCH(type->_tv_, VT_STRUCT))
            {
                expect("struct/union type");
            }
            f = find_field(type, l);
            if (!f)
            {
                expect("field");
            }
            if (cur_field)
            {
                *cur_field = f;
            }
            type = &f->type;
            c += f->c;
        }
        cur_field = NULL;
    }
    if (!cur_field)
    {
        if (g_tok_is('='))
        {
            next();
        }
        else
        {
            expect("=");
        }
    }
    else
    {
        if (type->_tv_.bits & VT_ARRAY)
        {
            index = (*cur_field)->c;
            if ((type->ref->c >= 0) && (index >= type->ref->c))
            {
                die_with_line_number("index too large");
            }
            type = pointed_type(type);
            c += index * type_size(type, &align);
        }
        else
        {
            f = *cur_field;
            if (!f)
            {
                die_with_line_number("too many field init");
            }
            type = &f->type;
            c += f->c;
        }
    }
    // must put zero in holes (note that doing it that way // ensures that it even works with designators)
    if (!size_only && ((i32)(c - corig) > al))
    {
        init_putz(sec, corig + al, c - corig - al);
    }
    decl_initializer(type, sec, c, 0, size_only);

    // XXX: make it more general
    if (!size_only && (nb_elems > 1))
    {
        u8 * src;
        u8 *dst;
        i32 i;

        if (!sec)
        {
            vset(type, VT_LOCAL | VT_LVAL, c);
            i = 1;
            for (; i < nb_elems; i++)
            {
                vset(type, VT_LOCAL | VT_LVAL, c + (elem_size * i));
                vswap();
                vstore();
            }
            vpop();
        }
        else if (!NODATA_WANTED())
        {
            bytes_ensure_size(
                &sec->bytes,
                c + (nb_elems * elem_size));
            src = sec->bytes.p + c;
            dst = src;
            i = 1;
            for (; i < nb_elems; i++)
            {
                dst += elem_size;
                memcpy(dst, src, elem_size);
            }
        }
    }
    c += nb_elems * type_size(type, &align);
    if ((i32)(c - corig) > al)
    {
        al = c - corig;
    }
    return al;
}

// store a value or an expression directly in global data or in local array
static void init_putv(CType * type, Section * sec, u32 c)
{
    i32 bt;
    void * ptr;
    CType dtype;

    dtype = *type;
    dtype._tv_.bits &= ~VT_CONSTANT; // need to do that to avoid false warning

    if (sec)
    {
        i32 size;
        i32 align;
        // XXX: not portable
        // XXX: generate error if incorrect relocation
        gen_assign_cast(&dtype);
        bt = type->_tv_.bits & VT_BTYPE;

        if ((vtop->r & VT_SYM)
            && (bt != VT_PTR)
            && (bt != VT_FUNC)
            && (bt != (PTR_SIZE_IS_4 ? VT_INT : VT_LLONG))
            && !((vtop->r & VT_CONST) && (vtop->sym->v >= SYM_FIRST_ANOM))
            )
        {
            die_with_line_number("initializer element is not computable at load time");
        }

        if (NODATA_WANTED())
        {
            vtop--;
            return;
        }

        size = type_size(type, &align);
        bytes_ensure_size(&sec->bytes, c + size);
        ptr = sec->bytes.p + c;

        // XXX: make code faster ?
        // XXX This ((vtop->type.t & VT_BTYPE) != VT_PTR)??? rejects compound literals like
        //   '(void *){ptr}'.  The problem is that '&sym' is
        //   represented the same way, which would be ruled out
        //   by the SYM_FIRST_ANOM check above, but also '"string"'
        //   in 'char *p = "string"' is represented the same
        //   with the type being VT_PTR and the symbol being an
        //   anonymous one.  That is, there's no difference in vtop
        //   between '(void *){x}' and '&(void *){x}'.  Ignore
        //   pointer typed entities here.  Hopefully no real code
        //   will every use compound literals with scalar type.  */
        if (
            ((vtop->r & (VT_SYM | VT_CONST)) == (VT_SYM | VT_CONST)) &&
            (vtop->sym->v >= SYM_FIRST_ANOM) &&
            (!BTYPES_MATCH(vtop->type._tv_, VT_PTR))
        )
        {
            // These come from compound literals, memcpy stuff over.
            Section * ssec;
            Elf32_Sym * esym;
            Relocation * rel;
            esym = elfsym(vtop->sym);
            ssec = esym->section;
            __intrin_memmove(ptr, ssec->bytes.p + esym->st_value, size);
            if (ssec->relocs.u.r.c)
            {
                // We need to copy over all memory contents, and that // includes relocations.  Use the fact that relocs are // created it order, so look from the end of relocs // until we hit one before the copied region.
                i32 num_relocs = ssec->relocs.u.r.c / sizeof(Relocation);
                rel = (Relocation *)(ssec->relocs.u.r.p + ssec->relocs.u.r.c);
                while (num_relocs--)
                {
                    rel--;
                    if (rel->offset >= (esym->st_value + size))
                    {
                        continue;
                    }
                    if (rel->offset < esym->st_value)
                    {
                        break;
                    }
                    // Note: if the same fields are initialized multiple 
                    // // times (possible with designators) then we possibly 
                    // // add multiple relocations for the same offset here. 
                    // // That would lead to wrong code, the last reloc needs 
                    // // to win.  We clean this up later after the whole 
                    // // initializer is parsed.
                    put_elf_reloc(
                        sec,
                        c + rel->offset - esym->st_value,
                        rel->is_pc_relative,
                        rel->sym
                    );
                }
            }
        }
        else
        {
            {
                switch (bt)
                {
                case VT_BOOL:
                    vtop->c.i = vtop->c.i != 0;
                case VT_BYTE:
                    *(char *)ptr |= vtop->c.i;
                    break;
                case VT_SHORT:
                    *(i16 *)ptr |= vtop->c.i;
                    break;
                case VT_LLONG:
                    *(i64 *)ptr |= vtop->c.i;
                    break;
                case VT_PTR:
                    {
                        u32 val = (u32)vtop->c.i;
                        if (vtop->r & VT_SYM)
                        {
                            greloc_direct(sec, vtop->sym, c);
                        }
                        *(u32 *)ptr |= val;
                        break;
                    }
                default:
                    {
                        i32 val = (i32)vtop->c.i;
                        if (vtop->r & VT_SYM)
                        {
                            greloc_direct(sec, vtop->sym, c);
                        }
                        *(i32 *)ptr |= val;
                        break;
                    }
                }
            }
        }
        vtop--;
    }
    else
    {
        vset(&dtype, VT_LOCAL | VT_LVAL, c);
        vswap();
        vstore();
        vpop();
    }
}

// 't' contains the type and storage info. 'c' is the offset of the
// object in section 'sec'. If 'sec' is NULL, it means stack based
// allocation. 'first' is true if array '{' must be re_ad (multi
// dimension implicit array init handling). 'size_only' is true if
// size only evaluation is wanted (only for arrays).
static void decl_initializer(
    CType * type, 
    Section * sec, 
    u32 c,
    i32 first, 
    i32 size_only)
{
    i32 len;
    i32 n;
    i32 no_oblock;
    i32 nb;
    i32 i;
    i32 size1;
    i32 align1;
    i32 have_elem;
    Sym * s;
    Sym * f;
    
    Sym indexsym;
    memset(&indexsym, 0, sizeof(Sym));

    CType * t1;

    // If we currently are at an '}' or ',' we have re_ad an initializer
    // element in one of our callers, and not yet consumed it.
    have_elem = (g_tok_is('}')) || (g_tok_is(','));
    if (!have_elem && (!g_tok_is('{')) &&
        // In case of strings we have special handling for arrays, so // don't consume them as initializer value (which would commit them // to some anonymous symbol).
        (!g_tok_is(TOK_STR)) &&
        !size_only)
    {
        parse_init_elem(!sec ? EXPR_ANY : EXPR_CONST, 0);
        have_elem = 1;
    }

    if (have_elem &&
        !(type->_tv_.bits & VT_ARRAY) &&
        // Use i_c_parameter_t, to strip toplevel qualifiers. // The source type might have VT_CONSTANT set, which is // of course assignable to non-elements.
        is_compatible_unqualified_types(type, &vtop->type))
    {
        init_putv(type, sec, c);
    }
    else if (type->_tv_.bits & VT_ARRAY)
    {
        s = type->ref;
        n = s->c;
        t1 = pointed_type(type);
        size1 = type_size(t1, &align1);

        no_oblock = 1;
        if ((first && (!g_tok_is(TOK_STR))) ||
            (g_tok_is('{')))
        {
            if (!g_tok_is('{'))
            {
                die_with_line_number("character array initializer must be a literal,"
                    " optionally enclosed in braces");
            }
            skip('{');
            no_oblock = 0;
        }

        // only parse strings here if correct type (otherwise: handle
        // them as ((w)char *) expressions */
        if ((g_tok_is(TOK_STR)) && (BTYPES_MATCH(t1->_tv_, VT_BYTE)))
        {
            len = 0;
            while (g_tok_is(TOK_STR))
            {
                i32 cstr_len;
                i32 ch;

                // compute maximum number of chars wanted
                cstr_len = tokc.str.size;
                cstr_len--;
                nb = cstr_len;
                if ((n >= 0) && (nb > (n - len)))
                {
                    nb = n - len;
                }
                if (!size_only)
                {
                    if (cstr_len > nb)
                    {
                        print_error_with_line_number("initializer-string for array is too long");
                    }
                    // in order to go faster for common case (char // string in global variable, we handle it // specifically
                    if (sec && (size1 == 1))
                    {
                        if (!NODATA_WANTED())
                        {
                            memcpy(sec->bytes.p + c + len, tokc.str.data, nb);
                        }
                    }
                    else
                    {
                        i = 0;
                        for (; i < nb; i++)
                        {
                            ch = ((u8 *)tokc.str.data)[i];
                            vpushi(ch);
                            init_putv(t1, sec, c + ((len + i) * size1));
                        }
                    }
                }
                len += nb;
                next();
            }
            // only add trailing zero if enough storage (no // warning in this case since it is standard)
            if ((n < 0) || (len < n))
            {
                if (!size_only)
                {
                    vpushi(0);
                    init_putv(t1, sec, c + (len * size1));
                }
                len++;
            }
            len *= size1;
        }
        else
        {
            indexsym.c = 0;
            f = &indexsym;

        do_init_list:
            len = 0;
            while ((!g_tok_is('}')) || have_elem)
            {
                len = decl_designator(type, sec, c, &f, size_only, len);
                have_elem = 0;
                if (type->_tv_.bits & VT_ARRAY)
                {
                    ++indexsym.c;
                    // special test for multi dimensional arrays (may not // be strictly correct if designators are used at the // same time)
                    if (no_oblock && (len >= (n * size1)))
                    {
                        break;
                    }
                }
                else
                {
                    if (s->type._tv_.bits == VT_UNION)
                    {
                        f = NULL;
                    }
                    else
                    {
                        f = f->next;
                    }
                    if (no_oblock && (f == NULL))
                    {
                        break;
                    }
                }

                if (g_tok_is('}'))
                {
                    break;
                }
                skip(',');
            }
        }
        // put zeros at the end
        if (!size_only && (len < (n * size1)))
        {
            init_putz(sec, c + len, (n * size1) - len);
        }
        if (!no_oblock)
        {
            skip('}');
        }
        // patch type size if needed, which happens only for array types
        if (n < 0)
        {
            s->c = (size1 == 1) ? len : ((len + size1 - 1) / size1);
        }
    }
    else if (BTYPES_MATCH(type->_tv_, VT_STRUCT))
    {
        size1 = 1;
        no_oblock = 1;
        if (first || (g_tok_is('{')))
        {
            skip('{');
            no_oblock = 0;
        }
        s = type->ref;
        f = s->next;
        n = s->c;
        goto do_init_list;
    }
    else if (g_tok_is('{'))
    {
        next();
        decl_initializer(type, sec, c, first, size_only);
        skip('}');
    }
    else if (size_only)
    {
        // If we supported only ISO C we wouldn't have to accept calling // this on anything than an array size_only==1 (and even then // only on the outermost level, so no recursion would be needed), // because initializing a flex array member isn't supported. // But GNU C supports it, so we need to recurse even into // subfields of structs and arrays when size_only is set.
        // just skip expression
        skip_or_save_block(NULL);
    }
    else
    {
        if (!have_elem)
        {
            // This should happen only when we haven't parsed // the init element above for fear of committing a // string constant to memory too early.
            if (!g_tok_is(TOK_STR))
            {
                expect("string constant");
            }
            parse_init_elem(!sec ? EXPR_ANY : EXPR_CONST, g_bootstrip);
        }
        init_putv(type, sec, c);
    }
}

// parse an initializer for type 't' if 'has_init' is non zero, and
// allocate space in local or global data space ('r' is either
// VT_LOCAL or VT_CONST). If 'v' is non zero, then an associated
// variable 'v' of scope 'scope' is declared before initializers
// are parsed. If 'v' is zero, then a reference to the new object
// is put in the value stack. If 'has_init' is 2, a special parsing
// is done to handle string constants.
static void squeeze_multi_relocs(Section * sec, u32 oldrelocoffset);
static Section data_section = {0}; // predefined sections
static void decl_initializer_alloc(CType * type, AttributeDef * ad, i32 r,
    i32 has_init, i32 v)
{
    i32 size;
    i32 align;
    i32 addr;
    TokenString * init_str = NULL;

    Section * sec = NULL;
    Sym * flexible_array;
    Sym * sym = NULL;
    i32 saved_nocode_wanted = nocode_wanted;

    if (type->_tv_.bits & VT_STATIC)
    {
        nocode_wanted |= NODATA_WANTED() ? 0x40000000 : 0x80000000;
    }

    flexible_array = NULL;
    if (BTYPES_MATCH(type->_tv_, VT_STRUCT))
    {
        Sym * field = type->ref->next;
        if (field)
        {
            while (field->next)
            {
                field = field->next;
            }
            if ((field->type._tv_.bits & VT_ARRAY) && (field->type.ref->c < 0))
            {
                flexible_array = field;
            }
        }
    }

    size = type_size(type, &align);
    // If unknown size, we must evaluate it before // evaluating initializers because // initializers can generate global data too // (e.g. string pointers or ISOC99 compound // literals). It also simplifies local // initializers handling
    if ((size < 0) || (flexible_array && has_init))
    {
        if (!has_init)
        {
            die_with_line_number("unknown type size");
        }
        // get all init string
        if (has_init == 2)
        {
            init_str = tok_str_alloc();
            // only get strings
            while (g_tok_is(TOK_STR))
            {
                tok_str_add_tok(init_str);
                next();
            }
            tok_str_add(init_str, TOK_EOF);
            tok_str_add(init_str, TOK_END_OF_REPLAY);
        }
        else
        {
            skip_or_save_block(&init_str);
        }
        
        //unget_tok(TOK_END_OF_REPLAY);
        TokenString * str_ = tok_str_alloc();
        tok_str_add2(str_, g_tok(), g_tok_i_name, &tokc);
        tok_str_add(str_, TOK_END_OF_REPLAY);
        begin_replay(str_, 1);

        // compute size
        begin_replay(init_str, 1);
        next();
        decl_initializer(type, NULL, 0, 1, 1);
        // prepare second initializer parsing
        replay_ptr = init_str->str;
        next();

        // if still unknown size, error
        size = type_size(type, &align);
        if (size < 0)
        {
            die_with_line_number("unknown type size");
        }
    }
    // If there's a flex member and it was used in the initializer // adjust size.
    if (flexible_array &&
        (flexible_array->type.ref->c > 0))
    {
        size +=
            flexible_array->type.ref->c *
            pointed_size(&flexible_array->type);
    }

    if (NODATA_WANTED())
    {
        size = 0; align = 1;
    }

    if ((r & VT_VALMASK) == VT_LOCAL)
    {
        loc = (loc - size) & -align;
        addr = loc;

        if (v)
        {
            // local variable
            sym = sym_push(v, type, r, addr);
        }
        else
        {
            // push local reference
            vset(type, r, addr);
        }
    }
    else
    {
        //!!! get rid of globals, gets rid of two whole sections in output!

        if (v)
        {
            // see if the symbol was already defined
            sym = sym_find(v);
            if (sym)
            {
                die_with_line_number("redefinition of global");
            }
        }

        // allocate symbol in corresponding section
        if (!has_init)
        {
            die_with_line_number("BSS!");
        }

        sec = &data_section;

        addr = align_upwards(sec->bytes.size, align);
        bytes_ensure_size(&sec->bytes, addr + size);

        if (v)
        {
            if (!sym)
            {
                sym = sym_push(v, type, r | VT_SYM, 0);
                patch_storage(sym, ad, NULL, 1);
            }
            // update symbol definition
            put_extern_sym(sym, sec, addr);
        }
        else
        {
            // push global reference
            sym = get_sym_ref(type, sec, addr);
            vpushsym(type, sym);
            vtop->r |= r;
        }
    }

    if (has_init)
    {
        u32 oldreloc_offset = 0;
        if (sec && sec->relocs.u.r.c)
        {
            oldreloc_offset = sec->relocs.u.r.c;
        }
        decl_initializer(type, sec, addr, 1, 0);
        if (sec && sec->relocs.u.r.c)
        {
            squeeze_multi_relocs(sec, oldreloc_offset);
        }
        // patch flexible array member size back to -1,
        // for possible subsequent similar declarations
        if (flexible_array)
        {
            flexible_array->type.ref->c = -1;
        }
    }

    // restore parse state if needed
    if (init_str)
    {
        end_replay();
        next();
    }

    nocode_wanted = saved_nocode_wanted;
}

// 'l' is VT_LOCAL or VT_CONST to define default storage type, or VT_CMP
// if parsing old style parameter decl list (and FUNC_SYM is set then)
static void decl0(i32 l, Sym * func_sym)
{
    i32 v0;
    i32 has_init;
    i32 r;
    CType type;
    CType btype;
    Sym * sym;
    AttributeDef ad;

    while (1)
    {
        if (!parse_btype(&btype, &ad))
        {
            // skip redundant ';' if not in old parameter decl scope
            if ((g_tok_is(';')) && (l != VT_CMP))
            {
                next();
                continue;
            }
            if (l != VT_CONST)
            {
                break;
            }
            if (g_tok() >= TOK_KEYWORD_MAX)
            {
                // special test for old K&R protos without explicit i32 // type. Only accepted when defining global data
                btype._tv_.bits = VT_INT;
            }
            else
            {
                if (!g_tok_is(TOK_EOF))
                {
                    expect("declaration");
                }
                break;
            }
        }
        if (g_tok_is(';'))
        {
            if (BTYPES_MATCH(btype._tv_, VT_STRUCT))
            {
                i32 v = btype.ref->v;
                if (!(v & SYM_FIELD) && ((v & ~SYM_STRUCT) >= SYM_FIRST_ANOM))
                {
                    print_error_with_line_number("unnamed struct/union that defines no instances");
                }
                next();
                continue;
            }
            if (IS_ENUM(btype._tv_.bits))
            {
                next();
                continue;
            }
        }
        while (1)
        { // iterate thru each declaration
            type = btype;
            // If the base type itself was an array type of unspecified // size (like in 'typedef int arr[]; arr x = {1};') then // we will overwrite the unknown size by the real one for // this decl.  We need to unshare the ref symbol holding // that size.
            if ((type._tv_.bits & VT_ARRAY) && (type.ref->c < 0))
            {
                type.ref = sym_push(SYM_FIELD, &type.ref->type, 0, type.ref->c);
            }
            type_decl(&type, &ad, &v0, TYPE_DIRECT);

            if (BTYPES_MATCH(type._tv_, VT_FUNC))
            {
                if ((type._tv_.bits & VT_STATIC) && (l == VT_LOCAL))
                {
                    die_with_line_number("function without file scope cannot be static");
                }
                sym = type.ref;
            }

            if (g_tok_is('{'))
            {
                if (l != VT_CONST)
                {
                    die_with_line_number("cannot use local functions");
                }
                if (!BTYPES_MATCH(type._tv_, VT_FUNC))
                {
                    expect("function definition");
                }

                // reject abstract declarators in function definition
                // make old style params without decl have int type
                sym = type.ref;

                for (;;)
                {
                    sym = sym->next;
                    if (sym == NULL)
                    {
                        break;
                    }

                    if (!(sym->v & ~SYM_FIELD))
                    {
                        expect("identifier");
                    }
                    if (sym->type._tv_.bits == VT_VOID)
                    {
                        sym->type = int_type;
                    }
                }

                if (type._tv_.bits & VT_EXTERN)
                {
                    die_with_line_number("extern functions may not have a body");
                }

                // XXX: cannot do better now: convert extern inline to static inline
                if ((type._tv_.bits & (VT_EXTERN | VT_INLINE)) == (VT_EXTERN | VT_INLINE))
                {
                    die_with_line_number("extern+inline, wtf?");

                    type._tv_.bits = (type._tv_.bits & ~VT_EXTERN) | VT_STATIC;
                }

                // put function symbol
                i32 is_fresh = 0;
                sym = external_global_sym(v0, &type, 0, &is_fresh);
                type._tv_.bits &= ~VT_EXTERN;
                patch_storage(sym, &ad, &type, is_fresh);

                // compute text section
                gen_function(sym);

                break;
            }
            else
            {
                if (l == VT_CMP)
                {
                    // find parameter in function parameter list
                    sym = func_sym->next;
                    for (; sym; sym = sym->next)
                    {
                        if ((sym->v & ~SYM_FIELD) == v0)
                        {
                            goto found;
                        }
                    }
                    tcc_error_puts("declaration for parameter '%s' but no such parameter",
                        get_tok_str(v0, NULL));
                found:
                    if (type._tv_.bits & VT_STORAGE) // 'register' is okay
                    {
                        tcc_error_puts("storage class specified for '%s'",
                            get_tok_str(v0, NULL));
                    }
                    if (sym->type._tv_.bits != VT_VOID)
                    {
                        tcc_error_puts("redefinition of parameter '%s'",
                            get_tok_str(v0, NULL));
                    }
                    convert_parameter_type(&type);
                    sym->type = type;
                }
                else if (type._tv_.bits & VT_TYPEDEF)
                {
                    // save typedefed type
                    // XXX: test storage specifiers ?
                    sym = sym_find(v0);
                    if (sym)
                    {
                        if (!is_compatible_types(&sym->type, &type)
                            || !(sym->type._tv_.bits & VT_TYPEDEF))
                        {
                            tcc_error_puts("incompatible redefinition of '%s'",
                                get_tok_str(v0, NULL));
                        }
                        sym->type = type;
                    }
                    else
                    {
                        sym = sym_push(v0, &type, 0, 0);
                    }
                    sym->f = ad.f;
                }
                else
                {
                    r = 0;
                    if (BTYPES_MATCH(type._tv_, VT_FUNC))
                    {
                        // external function definition
                        // specific case for func_call attribute
                        type.ref->f = ad.f;
                    }
                    else if (!(type._tv_.bits & VT_ARRAY))
                    {
                        // not lvalue if array
                        r |= lvalue_type(type._tv_.bits);
                    }
                    has_init = (g_tok_is('='));

                    i32 is_explicit_extern = (type._tv_.bits & VT_EXTERN);
                    if (is_explicit_extern && has_init)
                    {
                        die_with_line_number("extern decl may not have init");
                    }
                    if (is_explicit_extern && (l != VT_CONST))
                    {
                        // techinically too restrictive, but for my own sanity
                        die_with_line_number("extern decl must be top level");
                    }

                    i32 btype_is_func = (BTYPES_MATCH(type._tv_, VT_FUNC));
                    if (btype_is_func && has_init)
                    {
                        die_with_line_number("wtf, function decl with '='?");
                    }

                    if (btype_is_func && !is_explicit_extern)
                    {
                        //!!!
                        //tcc_error_noabort("function forward declrations are only for externs");
                    }

                    i32 is_noinit_global_static_array
                        = (!has_init)
                        && (l == VT_CONST)
                        && (type._tv_.bits & VT_STATIC)
                        && (type._tv_.bits & VT_ARRAY)
                        ;

                    i32 is_gcc_null_size_extern_array
                        = is_noinit_global_static_array
                        && (type.ref->c < 0)
                        ;

                    if (is_gcc_null_size_extern_array)
                    {
                        die_with_line_number(
                            "gcc would implicily make this 'extern', "
                            "did you want that?");
                    }

                    i32 should_infer_extern
                        = is_explicit_extern
                        || btype_is_func
                        || is_gcc_null_size_extern_array
                        ;

                    if (should_infer_extern)
                    {
                        // external variable or function
                        type._tv_.bits |= VT_EXTERN;
                        sym = external_sym(v0, &type, r, &ad);
                    }
                    else
                    {
                        if (type._tv_.bits & VT_STATIC)
                        {
                            r |= VT_CONST;
                        }
                        else
                        {
                            r |= l;
                        }
                        if (has_init)
                        {
                            next();
                        }
                        else if (l == VT_CONST)
                        {
                            die_with_line_number("uninitilized global");
                        }
                        decl_initializer_alloc(&type, &ad, r, has_init, v0);
                    }
                }

                if (g_bootstrip && (g_tok_is(',')))
                {
                    print_error_with_line_number("comma in decl!");
                }

                if (!g_tok_is(','))
                {
                    skip(';');
                    break;
                }
                next();
            }
        }
    }
}

static void decl(i32 l)
{
    decl0(l, NULL);
}

 // -------------------------------------------------------------------------
// elf symbol hashing function
static u32 elf_hash(u8 * name)
{
    u32 h = 0;
    u32 g;

    while (*name)
    {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        if (g)
        {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

// rebuild hash table of section s
// NOTE: we do factorize the hash table code to go faster
static void * hash_ptr_add_(Symbols * hs, u32 size);
static void rebuild_hash(
    Symbols * s,
    u32 nb_buckets)
{
    Elf32_Sym * sym;
    i32 * ptr;
    i32 * hash;
    i32 nb_syms;
    i32 sym_index;
    i32 h;
    u8 * strtab;

    strtab = s->names;
    nb_syms = s->c_syms / sizeof(Elf32_Sym);

    if (!nb_buckets)
    {
        nb_buckets = s->nb_buckets;
    }

    s->c_hash = 0;
    ptr = hash_ptr_add_(s, (nb_buckets + nb_syms) * sizeof(i32));
    s->nb_buckets = nb_buckets;
    s->nb_syms = nb_syms;
    hash = ptr;
    memset(hash, 0, (nb_buckets + 1) * sizeof(i32));
    ptr += nb_buckets + 1;

    sym = (Elf32_Sym *)s->syms + 1;
    sym_index = 1;
    for (; sym_index < nb_syms; sym_index++)
    {
        h = elf_hash(strtab + sym->st_name) % nb_buckets;
        *ptr = hash[h];
        hash[h] = sym_index;

        ptr++;
        sym++;
    }
}

static i32 define_elf_sym(
    Symbols * s,
    char * name,
    put_elf_sym_ARGS args)
{
    // new sym
    i32 sym_index = put_elf_sym(s, name, args);
    Elf32_Sym * sym = &((Elf32_Sym *)s->syms)[sym_index];
    s->nb_syms++;

    // get place to put hash (init to 0) //??? do I understand this?
    i32 * ptr = hash_ptr_add_(s, sizeof(i32));
    *ptr = 0;

    // add another hashing entry
    i32 * base = (i32 *)s->p_hash;
    i32 nbuckets = s->nb_buckets;
    u8 * names = (u8 *)s->names;
    i32 h = elf_hash(names + sym->st_name) % nbuckets;
    *ptr = base[h];
    base[h] = sym_index;
    s->nb_syms++;

    // we resize the hash table
    s->nb_hashed_syms++;
    if (s->nb_hashed_syms > (2 * nbuckets))
    {
        rebuild_hash(s, 2 * nbuckets);
    }

    return sym_index;
}

// add an elf symbol : check if it is already defined and patch
// it. Return symbol index. NOTE that sh_num can be SHN_UNDEF.
static i32 find_elf_sym(Symbols * s, char * name);
static i32 set_elf_sym(
    Symbols * s,
    char * name,
    put_elf_sym_ARGS args)
{
    i32 sym_index = find_elf_sym(s, name);
    if (sym_index)
    {
        print_error_with_line_number("defined twice");
        die_with_line_number(name);
    }

    return define_elf_sym(s, name, args);
}

static void * hash_ptr_add_(Symbols * hs, u32 size)
{
    return data_ptr_add(
        &hs->c_hash,
        &hs->p_hash,
        &hs->c_hash_allocated,
        size
    );
}

// -------------------------------------------------------------------------
static i32 bytes_append_str(
    Bytes * bytes,
    char * str)
{
    i32 len = strlen_(str) + 1;
    i32 offset = bytes->size;
    char * ptr = append_new_bytes(bytes, len);

    __intrin_memmove(ptr, str, len);
    return offset;
}

static i32 put_data_str(
    u32 * p_data_offset,
    u8 * * p_data,
    u32 * p_data_allocated,
    char * sym)
{
    i32 len = strlen_(sym) + 1;
    i32 offset = *p_data_offset;
    char * ptr = data_ptr_add(
        p_data_offset,
        p_data,
        p_data_allocated,
        len
    );

    __intrin_memmove(ptr, sym, len);
    return offset;
}

static i32 find_elf_sym(
    Symbols * s,
    char * name)
{
    Elf32_Sym * sym;
    i32 nbuckets;
    i32 sym_index;
    i32 h;
    char * name1;

    nbuckets = s->nb_buckets;
    h = elf_hash((u8 *)name) % nbuckets;
    sym_index = ((i32 *)s->p_hash)[h];
    while (sym_index != 0)
    {
        sym = &((Elf32_Sym *)s->syms)[sym_index];
        name1 = (char *)s->names + sym->st_name;
        if (!strcmp_(name, name1))
        {
            return sym_index;
        }
        i32 next_index = nbuckets + sym_index;
        sym_index = ((i32 *)s->p_hash)[next_index];
    }
    return 0;
}

static ImportSymbol crt_syms[] =
{
    {"_iob"},
    {"_environ"},
    {"__argv"},
    {"__argc"},
    {"_vsnprintf"},
    {"exit"},
    {"fflush"},
    {"fprintf"},
    {"memcpy"},
    {"memset"},
    {"fread"},
    {"ferror"},
    {"strrchr"},
    {"fwrite"},
    {"ftell"},
    {"fputc"},
    {"fopen"},
    {"strncpy"},
    {"fseek"},
    {"fclose"},
    {"free"},
    {"malloc"},
    {"realloc"},
    {"atoi"},
    {"__getmainargs"},
    {"_strlwr"},
    {0},
};

static ImportSymbol k32_syms[] =
{
    {"GetSystemDirectoryA"},
    {"GetModuleFileNameA"},
    {"GetStdHandle"},
    {"GetCommandLineA"},
    {"WriteFile"},
    {0},
};

static ImportDll import_dlls[] =
{
    {"msvcrt.dll", crt_syms},
    {"kernel32.dll", k32_syms},
    {0, 0}
};

static void find_import(char * name, ImportId * id)
{
    id->i_dll = -1;
    id->i_sym = -1;

    i32 i_dll = 0;
    for (;;)
    {
        ImportDll * dll = &import_dlls[i_dll];
        if (!dll->name)
            break;

        i32 i_sym = 0;
        for (;;)
        {
            ImportSymbol * sym = &dll->syms[i_sym];
            char * sym_name = sym->name;
            if (!sym_name)
                break;

            if (0 == strcmp_(sym_name, name))
            {
                id->i_dll = i_dll;
                id->i_sym = i_sym;
                return;
            }

            ++i_sym;
        }

        ++i_dll;
    }
}

// return elf symbol value, signal error if 'err' is nonzero
static u32 get_elf_sym_addr(TCCState * s, char * name, i32 err)
{
    (void)s;

    i32 sym_index;
    Elf32_Sym * sym;

    sym_index = find_elf_sym(&symtab_section, name);
    sym = &((Elf32_Sym *)symtab_section.syms)[sym_index];
    if (!sym_index || !sym->section)
    {
        if (err)
        {
            tcc_error_puts("%s not defined", name);
        }
        return 0;
    }
    return sym->st_value;
}

// return elf symbol value or error
static void * tcc_get_symbol_err(TCCState * s, char * name)
{
    return (void *)(u32)get_elf_sym_addr(s, name, 1);
}

// Remove relocations for section S->reloc starting at oldrelocoffset
// that are to the same place, retaining the last of them.  As side effect
// the relocations are sorted.  Possibly reduces the number of relocs.
static void squeeze_multi_relocs(Section * s, u32 oldrelocoffset)
{
    //rel_Section * sr = s->reloc;
    Relocation * r;
    Relocation * dest;
    i32 a;
    u32 addr;

    if ((oldrelocoffset + sizeof(Relocation)) >= s->relocs.u.r.c)
    {
        return;
    }
    // The relocs we're dealing with are the result of initializer parsing. // So they will be mostly in order and there aren't many of them. // Secondly we need a stable sort. We use // a simple insertion sort.
    a = oldrelocoffset + sizeof(Relocation);
    for (; a < (i32)s->relocs.u.r.c; a += sizeof(Relocation))
    {
        i32 i = a - sizeof(Relocation);
        addr = ((Relocation *)(s->relocs.u.r.p + a))->offset;

        while (
            (i >= (i32)oldrelocoffset) &&
            (((Relocation *)(s->relocs.u.r.p + i))->offset > addr)
        )
        {
            Relocation tmp = *(Relocation *)(s->relocs.u.r.p + a);
            *(Relocation *)(s->relocs.u.r.p + a) = *(Relocation *)(s->relocs.u.r.p + i);
            *(Relocation *)(s->relocs.u.r.p + i) = tmp;
            i -= sizeof(Relocation);
        }
    }

    r = (Relocation *)(s->relocs.u.r.p + oldrelocoffset);
    dest = r;
    for (; r < (Relocation *)(s->relocs.u.r.p + s->relocs.u.r.c); r++)
    {
        if (dest->offset != r->offset)
        {
            dest++;
        }
        *dest = *r;
    }
    s->relocs.u.r.c = dest - s->relocs.u.r.p + 1;
}

// relocate symbol table, resolve undefined symbols if do_resolve is
// true and output error if undefined symbol.
static void relocate_syms(TCCState * s1, Symbols * symtab, i32 do_resolve)
{
    (void)s1;

    Elf32_Sym * sym;
    Section * section;
    char * name;

    sym = (Elf32_Sym *)symtab->syms + 1;
    for (
        ;
        sym < (Elf32_Sym *)(symtab->syms + symtab->c_syms);
        ++sym
        )
    {
        section = sym->section;
        if (!section)
        {
            name = (char *)symtab_section.names + sym->st_name;
            // Use ld.so to resolve symbol for us (for tcc -run)
            if (do_resolve)
            {
                // if dynamic symbol exist, it will be used in relocate_section
            }
            // XXX: _fp_hw seems to be part of the ABI, so we ignore // it
            if (!strcmp_(name, "_fp_hw"))
            {
                continue;
            }

            print_error_with_line_number("undefined symbol -");
            print_error_with_line_number(name);
        }
        else
        {
            // add section base
            sym->st_value += section->reloc_base_addr;
        }
    }
}

// relocate a given section (CPU dependent) by applying the relocations
// in the associated relocation section
static void relocate(i32 type, u8 * ptr, u32 addr, u32 val);
static void relocate_section(TCCState * s1, Section * s)
{
    (void)s1;

    //rel_Section * sr = s->reloc;
    Relocation * rel;
    Elf32_Sym * sym;
    i32 is_pc_relative;
    i32 sym_index;
    u8 * ptr;
    u32 tgt;
    u32 addr;

    rel = (Relocation *)s->relocs.u.r.p;
    for (
        ;
        rel < (Relocation *)(s->relocs.u.r.p + s->relocs.u.r.c);
        ++rel
        )
    {
        ptr = s->bytes.p + rel->offset;
        sym_index = rel->sym;
        sym = &((Elf32_Sym *)symtab_section.syms)[sym_index];
        is_pc_relative = rel->is_pc_relative;
        tgt = sym->st_value;
        addr = s->reloc_base_addr + rel->offset;
        relocate(is_pc_relative, ptr, addr, tgt);
    }
}

// XXX: make it faster ?
static void g(i32 c)
{
    if (nocode_wanted)
    {
        return;
    }

    bytes_ensure_size(
        &cur_text_section->bytes,
        ind + 1);
    cur_text_section->bytes.p[ind] = (u8)c;
    ++ind;
}

static void o(u32 c)
{
    while (c)
    {
        g(c);
        c = c >> 8;
    }
}

static void gen_le32(i32 c)
{
    g(c);
    g(c >> 8);
    g(c >> 16);
    g(c >> 24);
}

// output a symbol and patch all calls to it
static void gsym_addr(i32 t, i32 a)
{
    while (t)
    {
        u8 * ptr = cur_text_section->bytes.p + t;
        u32 n = read32le(ptr); // next value
        write32le(ptr, a - t - 4);
        t = n;
    }
}

static void gsym(i32 t)
{
    gsym_addr(t, ind);
}

// instruction + 4 bytes data. Return the address of the data
static i32 oad(i32 c, i32 s)
{
    i32 t;
    if (nocode_wanted)
    {
        return s;
    }
    o(c);
    t = ind;
    gen_le32(s);
    return t;
}

// output constant with relocation if 'r & VT_SYM' is true
static void gen_addr32(i32 r, Sym * sym, i32 c)
{
    if (r & VT_SYM)
    {
        greloc_direct(cur_text_section, sym, ind);
    }
    gen_le32(c);
}

// generate a modrm reference. 'op_reg' contains the additional 3
// opcode bits
static void gen_modrm(i32 op_reg, i32 r, Sym * sym, i32 c)
{
    op_reg = op_reg << 3;
    if ((r & VT_VALMASK) == VT_CONST)
    {
        // constant memory reference
        o(0x05 | op_reg);
        gen_addr32(r, sym, c);
    }
    else if ((r & VT_VALMASK) == VT_LOCAL)
    {
        // currently, we use only ebp as base
        if (c == (char)c)
        {
            // short reference
            o(0x45 | op_reg);
            g(c);
        }
        else
        {
            oad(0x85 | op_reg, c);
        }
    }
    else
    {
        g(0x00 | op_reg | (r & VT_VALMASK));
    }
}

static i32 invert_rel_op(i32 op)
{
    switch (op)
    {
    case TOK_ULT:
        return TOK_UGE;
    case TOK_UGE:
        return TOK_ULT;
    
    case TOK_ULE:
        return TOK_UGT;
    case TOK_UGT:
        return TOK_ULE;

    case TOK_EQ:
        return TOK_NE;
    case TOK_NE:
        return TOK_EQ;
    
    case TOK_LT:
        return TOK_GE;
    case TOK_GE:
        return TOK_LT;
    
    case TOK_LE:
        return TOK_GT;
    case TOK_GT:
        return TOK_LE;
    }

    op_error(op, "invert_rel_op");

    return op;
}

static i32 x86_condition_code_from_op(i32 op)
{
    // these 'condition codes' are used in the encoding of
    //  the x86 SETcc and Jcc instructions (and others probably)

    // The terms �above� and �below� are associated with the CF flag 
    // and refer to the relationship between two unsigned integer values.

    // The terms �greater� and �less� are associated with the SF
    // and OF flags and refer to the relationship between two signed integer values.

    // https://www.felixcloutier.com/x86/jcc
    // https://www.felixcloutier.com/x86/setcc

    switch (op)
    {
    
    // not using 0x00 : overflow (OF=1)
    // not using 0x01 : not overflow (OF=0)
    
    case TOK_ULT: 
        return 0x02; // jump/set if 'below' (CF=1)
    case TOK_UGE: 
        return 0x03; // jump/set if 'above' or equal (CF=0)
    case TOK_EQ: 
        return 0x04; // jump/set if equal (ZF=1)
    case TOK_NE: 
        return 0x05; // jump/set if not equal (ZF=0)
    case TOK_ULE: 
        return 0x06; // jump/set if 'below' or equal (CF=1 or ZF=1)
    case TOK_UGT: 
        return 0x07; // jump/set if 'above' (CF=0 and ZF=0)
    
    // not using 0x08 : sign (SF=1)
    // not using 0x09 : not sign (SF=0)
    // not using 0x0a : parity even (PF=1)
    // not using 0x0b : parity odd (PF=0)
   
    case TOK_LT: 
        return 0x0c; // jump/set if 'less' (SF != OF)
    case TOK_GE: 
        return 0x0d; // jump/set if 'greater' or equal (SF=OF)
    case TOK_LE: 
        return 0x0e; // jump/set if 'less' or equal (ZF=1 or SF != OF)
    case TOK_GT: 
        return 0x0f; // jump/set if 'greater' (ZF=0 and SF=OF)
    }
    
    op_error(op, "x86_condition_code_from_op");

    return op;
}

static void gen_setcc(i32 op, i32 reg)
{
    // https://www.felixcloutier.com/x86/setcc

    i32 cc = x86_condition_code_from_op(op);

    if ((reg < 0) || (reg > 7))
    {
        die_with_line_number("gen_setcc : reg was supposed to be u3 here");
    }

    o(0x0f);
    o(0x90 | cc);

    // modrm:
    // The reg field of the ModR/M byte is not used for the SETCC instruction 
    // and those opcode bits are ignored by the processor.

    // | Mod | Reg | R/M |
    // |-----|-----|-----|
    // | xx  | xxx | xxx |
    // u2 mod = 0b11; // we use mod 0b11 here, to set the bit into a register
    // u3 reg = 0;
    // u3 rm = cc;
    // putting those together gives
    // (mod << 6) + (0 << 3) + cc
    // or
    // 0b1100000 + cc
    // and 0b1100000 = 0xc0

    o(0xc0 | reg);
}

static void gen_jcc_rel8(i32 op, i32 rel8)
{
    // https://www.felixcloutier.com/x86/jcc

    i32 cc = x86_condition_code_from_op(op);

    g(0x70 | cc);
    g(rel8);
}

static i32 gen_jcc_rel32(i32 op, i32 rel32)
{
    // note : returns instruction stream addr
    //  of the emited rel32 immidiate,
    //  for potential patching

    // https://www.felixcloutier.com/x86/jcc

    g(0x0f);

    i32 cc = x86_condition_code_from_op(op);
    return oad(0x80 | cc, rel32);
}

// load 'r' from value 'sv'
static void load(i32 r, SValue * sv)
{
    i32 v;
    i32 t;
    i32 ft;
    i32 fc;
    i32 fr;

    fr = sv->r;
    ft = sv->type._tv_.bits & ~VT_DEFSIGN;
    fc = (i32)sv->c.i;

    ft &= ~(VT_VOLATILE | VT_CONSTANT);

    v = fr & VT_VALMASK;
    if (fr & VT_LVAL)
    {
        if (v == VT_LLOCAL)
        {
            SValue v1;
            v1.type._tv_.bits = VT_INT;
            v1.r = VT_LOCAL | VT_LVAL;
            v1.c.i = fc;
            fr = r;
            if (!(reg_classes[fr] & RC_INT))
            {
                fr = get_reg(RC_INT);
            }
            load(fr, &v1);
        }
        if (
            ((ft & VT_TYPE) == VT_BYTE) ||
            ((ft & VT_TYPE) == VT_BOOL)
        )
        {
            o(0xbe0f);   // movsbl
        }
        else if ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED))
        {
            o(0xb60f);   // movzbl
        }
        else if ((ft & VT_TYPE) == VT_SHORT)
        {
            o(0xbf0f);   // movswl
        }
        else if ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED))
        {
            o(0xb70f);   // movzwl
        }
        else
        {
            o(0x8b);     // movl
        }
        gen_modrm(r, fr, sv->sym, fc);
    }
    else
    {
        if (v == VT_CONST)
        {
            o(0xb8 + r); // mov xx, r
            gen_addr32(fr, sv->sym, fc);
        }
        else if (v == VT_LOCAL)
        {
            if (fc)
            {
                o(0x8d); // lea xxx(%ebp), r
                gen_modrm(r, VT_LOCAL, sv->sym, fc);
            }
            else
            {
                o(0x89);
                o(0xe8 + r); // mov %ebp, r
            }
        }
        else if (v == VT_CMP)
        {
            oad(0xb8 + r, 0); // mov 0, r
            gen_setcc(fc, r);
        }
        else if ((v == VT_JMP) || (v == VT_JMPI))
        {
            t = v & 1;
            oad(0xb8 + r, t); // mov 1, r
            o(0x05eb); // jmp after
            gsym(fc);
            oad(0xb8 + r, t ^ 1); // mov 0, r
        }
        else if (v != r)
        {
            o(0x89);
            o(0xc0 + (r + (v * 8))); // mov v, r
        }
    }
}

// store register 'r' in lvalue 'v'
static void store(i32 r, SValue * v)
{
    i32 fr;
    i32 bt;
    TYPE_VAL _tv_;
    i32 fc;

    _tv_ = v->type._tv_;
    fc = (i32)v->c.i;
    fr = v->r & VT_VALMASK;
    _tv_.bits &= ~(VT_VOLATILE | VT_CONSTANT);
    bt = _tv_.bits & VT_BTYPE;
    {
        if (bt == VT_SHORT)
        {
            o(0x66);
        }
        if ((bt == VT_BYTE) || (bt == VT_BOOL))
        {
            o(0x88);
        }
        else
        {
            o(0x89);
        }
    }
    if ((fr == VT_CONST) ||
        (fr == VT_LOCAL) ||
        (v->r & VT_LVAL))
    {
        gen_modrm(r, v->r, v->sym, fc);
    }
    else if (fr != r)
    {
        o(0xc0 + (fr + (r * 8))); // mov r, fr
    }
}

static void gadd_sp(i32 val)
{
    if (val == (char)val)
    {
        o(0xc483);
        g(val);
    }
    else
    {
        oad(0xc481, val); // add xxx, %esp
    }
}

// 'is_jmp' is '1' if it is a jump
static void gcall_or_jmp(i32 is_jmp)
{
    i32 r;
    if (((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) && (vtop->r & VT_SYM))
    {
        // constant and relocation case
        greloc_pc_relative(cur_text_section, vtop->sym, ind + 1);
        oad(0xe8 + is_jmp, (i32)(vtop->c.i - 4)); // call/jmp im
    }
    else
    {
        // otherwise, indirect call
        r = gv(RC_INT);
        o(0xff); // call/jmp *r */
        o(0xd0 + r + (is_jmp << 4));
    }
    if (!is_jmp)
    {
        TYPE_VAL _tv_;
        // extend the return value to the whole register if necessary // visual studio and gcc do not always set the whole eax register // when assigning the return value of a function
        _tv_ = vtop->type.ref->type._tv_;
        switch (_tv_.bits & VT_BTYPE)
        {
        case VT_BYTE:
            if (_tv_.bits & VT_UNSIGNED)
            {
                o(0xc0b60f); // movzx %al, %eax
            }
            else
            {
                o(0xc0be0f); // movsx %al, %eax
            }
            break;
        case VT_SHORT:
            if (_tv_.bits & VT_UNSIGNED)
            {
                o(0xc0b70f); // movzx %ax, %eax
            }
            else
            {
                o(0xc0bf0f); // movsx %ax, %eax
            }
            break;
        default:
            break;
        }
    }
}

// Generate function call. The function address is pushed first, then
// all the parameters in call order. This functions pops all the
// parameters and the function address.
static void gfunc_call(i32 nb_args)
{
    i32 size;
    i32 align;
    i32 r;
    i32 args_size;
    i32 i;
    i32 func_call;
    Sym * func_sym;

    args_size = 0;
    i = 0;
    for (; i < nb_args; i++)
    {
        if (BTYPES_MATCH(vtop->type._tv_, VT_STRUCT))
        {
            size = type_size(&vtop->type, &align);
            // align to stack align size
            size = (size + 3) & ~3;
            // allocate the necessary size on stack
            oad(0xec81, size); // sub xxx, %esp
            // generate structure store
            r = get_reg(RC_INT);
            o(0x89); // mov %esp, r
            o(0xe0 + r);
            vset(&vtop->type, r | VT_LVAL, 0);
            vswap();
            vstore();
            args_size += size;
        }
        else
        {
            // simple type (currently always same size)
            // XXX: implicit cast ?
            r = gv(RC_INT);
            if (BTYPES_MATCH(vtop->type._tv_, VT_LLONG))
            {
                size = 8;
                o(0x50 + vtop->r2); // push r
            }
            else
            {
                size = 4;
            }
            o(0x50 + r); // push r
            args_size += size;
        }
        vtop--;
    }
    save_regs(0); // save used temporary registers
    func_sym = vtop->type.ref;
    func_call = func_sym->f.func_call;
    gcall_or_jmp(0);

    if (args_size && (func_call != FUNC_STDCALL))
    {
        gadd_sp(args_size);
    }
    vtop--;
}

// generate function prolog of type 't'
static i32 func_vc = {0};
static void gfunc_prolog(
    CType * func_type,
    u32 * func_sub_sp_offset,
    i32 * func_ret_sub)
{
    i32 addr;
    i32 align;
    i32 size;
    i32 func_call;
    i32 param_index;
    i32 param_addr;
    Sym * sym;
    CType * type;

    sym = func_type->ref;
    func_call = sym->f.func_call;
    addr = 8;
    loc = 0;
    func_vc = 0;

    param_index = 0;

    ind += FUNC_PROLOG_SIZE;
    *func_sub_sp_offset = ind;
    // if the function returns a structure, then add an // implicit pointer parameter
    func_vt = sym->type;
    size = type_size(&func_vt, &align);
    if ((BTYPES_MATCH(func_vt._tv_, VT_STRUCT))
        && ((size > 8) || (size & (size - 1))))
    {
        // XXX: fastcall case ?
        func_vc = addr;
        addr += 4;
        param_index++;
    }
    // define parameters
    for (;;)
    {
        sym = sym->next;
        if (sym == NULL)
        {
            break;
        }

        type = &sym->type;
        size = type_size(type, &align);
        size = (size + 3) & ~3;

        {
            param_addr = addr;
            addr += size;
        }
        sym_push(sym->v & ~SYM_FIELD, type,
            VT_LOCAL | lvalue_type(type->_tv_.bits), param_addr);
        param_index++;
    }
    *func_ret_sub = 0;
    // pascal type call ?
    if ((func_call == FUNC_STDCALL))
    {
        *func_ret_sub = addr - 8;
    }
}

// generate function epilog
static void gfunc_epilog(
    u32 func_sub_sp_offset,
    i32 func_ret_sub)
{
    u32 v;
    u32 saved_ind;

    // align local size to word & save local variables
    v = (-loc + 3) & -4;

    o(0xc9); // leave
    if (func_ret_sub == 0)
    {
        o(0xc3); // ret
    }
    else
    {
        o(0xc2); // ret n
        g(func_ret_sub);
        g(func_ret_sub >> 8);
    }
    saved_ind = ind;
    ind = func_sub_sp_offset - FUNC_PROLOG_SIZE;
    if (v >= 4096)
    {
        die_with_line_number("ack?! TOK___chkstk");
    }
    else
    {
        o(0xe58955);  // push %ebp, mov %esp, %ebp
        o(0xec81);  // sub esp, stacksize
        gen_le32(v);
        o(0x90);  // adjust to FUNC_PROLOG_SIZE
    }
    o(0); // push ebx
    ind = saved_ind;
}

// generate a jump to a label
static i32 gjmp(i32 t)
{
    return oad(0xe9, t);
}

// generate a jump to a fixed address
static void gjmp_addr(i32 a)
{
    i32 r;
    r = a - ind - 2;
    if (r == (char)r)
    {
        g(0xeb);
        g(r);
    }
    else
    {
        oad(0xe9, a - ind - 5);
    }
}

static void gtst_addr(i32 inv, i32 a)
{
    i32 v = vtop->r & VT_VALMASK;
    if (v == VT_CMP)
    {
        i32 op = (i32)vtop->c.i;
        if (inv)
        {
            op = invert_rel_op(op);
        }

        --vtop;

        a -= ind + 2;
        if (a == (char)a)
        {
            gen_jcc_rel8(op, a);
        }
        else
        {
            // XXX the -4 here is rather opaque

            gen_jcc_rel32(op, a - 4);
        }
    }
    else if ((v & ~1) == VT_JMP)
    {
        if ((v & 1) != inv)
        {
            gjmp_addr(a);
            gsym((i32)vtop->c.i);
        }
        else
        {
            gsym((i32)vtop->c.i);
            o(0x05eb);
            gjmp_addr(a);
        }
        vtop--;
    }
}

// generate a test. set 'inv' to invert test. Stack entry is popped
static i32 gtst(i32 inv, i32 t)
{
    i32 v = vtop->r & VT_VALMASK;
    if (nocode_wanted)
    {
        ;
    }
    else if (v == VT_CMP)
    {
        // fast case : can jump directly since flags are set

        i32 op = (i32)vtop->c.i;
        if (inv)
        {
            op = invert_rel_op(op);
        }

        t = gen_jcc_rel32(op, t);
    }
    else if ((v == VT_JMP) || (v == VT_JMPI))
    {
        // && or || optimization
        if ((v & 1) == inv)
        {
            // insert vtop->c jump list in t
            u32 n1;
            u32 n = (u32)vtop->c.i;
            if (n)
            {
                for (;;)
                {
                    n1 = read32le(cur_text_section->bytes.p + n);
                    if (!n1)
                    {
                        break;
                    }
                    n = n1;
                }

                write32le(cur_text_section->bytes.p + n, t);
                t = (i32)vtop->c.i;
            }
        }
        else
        {
            t = gjmp(t);
            gsym((i32)(vtop->c.i));
        }
    }
    vtop--;
    return t;
}

// generate an integer binary operation
static void gen_opi(i32 op)
{
    i32 r;
    i32 fr;
    i32 opc;
    i32 c;

    switch (op)
    {
    case '+':
    case TOK_ADDC1: // add with carry generation
        opc = 0;
    gen_op8:
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
        {
            // constant case
            vswap();
            r = gv(RC_INT);
            vswap();
            c = (i32)vtop->c.i;
            if (c == (char)c)
            {
                // generate inc and dec for smaller code
                if ((c == 1) && (opc == 0) && (op != TOK_ADDC1))
                {
                    o(0x40 | r); // inc
                }
                else if ((c == 1) && (opc == 5) && (op != TOK_SUBC1))
                {
                    o(0x48 | r); // dec
                }
                else
                {
                    o(0x83);
                    o(0xc0 | (opc << 3) | r);
                    g(c);
                }
            }
            else
            {
                o(0x81);
                oad(0xc0 | (opc << 3) | r, c);
            }
        }
        else
        {
            gv2(RC_INT, RC_INT);
            r = vtop[-1].r;
            fr = vtop[0].r;
            o((opc << 3) | 0x01);
            o(0xc0 + (r + (fr * 8)));
        }
        vtop--;
        if (is_rel_op(op))
        {
            vtop->r = VT_CMP;
            vtop->c.i = op;
        }
        break;
    case '-':
    case TOK_SUBC1: // sub with carry generation
        opc = 5;
        goto gen_op8;
    case TOK_ADDC2: // add with carry use
        opc = 2;
        goto gen_op8;
    case TOK_SUBC2: // sub with carry use
        opc = 3;
        goto gen_op8;
    case '&':
        opc = 4;
        goto gen_op8;
    case '^':
        opc = 6;
        goto gen_op8;
    case '|':
        opc = 1;
        goto gen_op8;
    case '*':
        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        o(0xaf0f); // imul fr, r
        o(0xc0 + (fr + (r * 8)));
        break;
    case TOK_SHL:
        opc = 4;
        goto gen_shift;
    case TOK_SHR:
        opc = 5;
        goto gen_shift;
    case TOK_SAR:
        opc = 7;
    gen_shift:
        opc = 0xc0 | (opc << 3);
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
        {
            // constant case
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i & 0x1f;
            o(0xc1); // shl/shr/sar xxx, r
            o(opc | r);
            g(c);
        }
        else
        {
            // we generate the shift in ecx
            gv2(RC_INT, RC_ECX);
            r = vtop[-1].r;
            o(0xd3); // shl/shr/sar %cl, r
            o(opc | r);
        }
        vtop--;
        break;
    case '/':
    case TOK_UDIV:
    case TOK_PDIV:
    case '%':
    case TOK_UMOD:
    case TOK_UMULL:
        // first operand must be in eax
        // XXX: need better constraint for second operand
        gv2(RC_EAX, RC_ECX);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        save_reg(TREG_EDX);
        // save EAX too if used otherwise
        save_reg_upstack(TREG_EAX, 1);
        if (op == TOK_UMULL)
        {
            o(0xf7); // mul fr
            o(0xe0 + fr);
            vtop->r2 = TREG_EDX;
            r = TREG_EAX;
        }
        else
        {
            if ((op == TOK_UDIV) || (op == TOK_UMOD))
            {
                o(0xf7d231); // xor %edx, %edx, div fr, %eax
                o(0xf0 + fr);
            }
            else
            {
                o(0xf799); // cltd, idiv fr, %eax
                o(0xf8 + fr);
            }
            if ((op == '%') || (op == TOK_UMOD))
            {
                r = TREG_EDX;
            }
            else
            {
                r = TREG_EAX;
            }
        }
        vtop->r = (u16)r;
        break;
    default:
        opc = 7;
        goto gen_op8;
    }
}

// end of X86 code generator

//include "i386-link.c"

void relocate(i32 is_pc_relative, u8 * ptr, u32 addr, u32 val)
{
    if (is_pc_relative)
    {
        add32le(ptr, val - addr);
    }
    else
    {
        add32le(ptr, val);
    }
}

 // -------------------------------------------------------------
// --------------------------------------------
//----------------------------------------------------------------------------
static u32 umax(u32 a, u32 b)
{
    return ((a < b) ? b : a);
}

static u32 pe_file_align(struct pe_info * pe, u32 n)
{
    return ((n + (pe->file_align - 1)) & ~(pe->file_align - 1));
}

static u32 pe_virtual_align(struct pe_info * pe, u32 n)
{
    return ((n + (pe->section_align - 1)) & ~(pe->section_align - 1));
}

static void align_end_of_section(
    Section * s,
    u32 alignment)
{
    bytes_ensure_size(
        &s->bytes,
        align_upwards(
            s->bytes.size,
            alignment
        )
    );
}

static void pe_set_datadir(struct pe_header_t * hdr, i32 dir, u32 addr, u32 size)
{
    hdr->opthdr.DataDirectory[dir].VirtualAddress = addr;
    hdr->opthdr.DataDirectory[dir].Size = size;
}

static i32 pe_fwrite(void * data, u32 len, void * fp, u32 * psum)
{
    if (psum)
    {
        u32 sum = *psum;
        u16 * p = data;
        i32 i = len;
        for (; i > 0; i -= 2)
        {
            sum += (i >= 2) ? *p++ : *(u8 *)p;
            sum = (sum + (sum >> 16)) & ((u16)(~0));
        }
        *psum = sum;
    }
    return ((len == fwrite(data, 1, len, fp)) ? 0 : -1);
}

static void pe_fpad(void * fp, u32 new_pos)
{
    u32 pos = ftell(fp);
    while (++pos <= new_pos)
    {
        fwrite("", 1, 1, fp);
    }
}

//----------------------------------------------------------------------------
struct pe_header_t pe_template =
{
    // IMAGE_DOS_HEADER doshdr
    {
        {'M', 'Z'}, // magic
        {0},        // junk
        128,        // File address of new exe header
    },

    // uint8_t dosstub[64]
    // XXX reduce size here?
    {0},

    // nt_sig = IMAGE_NT_SIGNATURE 0x00004550
    17744,

    // IMAGE_FILE_HEADER filehdr
    {

        IMAGE_FILE_MACHINE_I386, // Machine;
        3,                       // NumberOfSections; // patched later
        0,                       // TimeDateStamp;
        0,                       // PointerToSymbolTable;
        0,                       // NumberOfSymbols;
        224,                     // SizeOfOptionalHeader;

        // Characteristics
        // IMAGE_FILE_RELOCS_STRIPPED       1 << 0
        // IMAGE_FILE_EXECUTABLE_IMAGE      1 << 1
        // IMAGE_FILE_LINE_NUMS_STRIPPED    1 << 2 
        // IMAGE_FILE_LOCAL_SYMS_STRIPPED   1 << 3
        // IMAGE_FILE_32BIT_MACHINE         1 << 8
        // IMAGE_FILE_DEBUG_STRIPPED        1 << 9
        (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 8) | (1 << 9),
    },

    // IMAGE_OPTIONAL_HEADER opthdr
    {
        // Standard fields.
        267, // Magic; // IMAGE_NT_OPTIONAL_HDR32_MAGIC 0x010B
        6,   // MajorLinkerVersion;
        0,   // MinorLinkerVersion;
        0,   // SizeOfCode;
        0,   // SizeOfInitializedData;
        0,   // SizeOfUninitializedData;
        0,   // AddressOfEntryPoint;
        0,   // BaseOfCode;
        0,   // BaseOfData;

        // NT additional fields.
        4096 * 1024, // ImageBase;
        4096,        // SectionAlignment;
        512,         // FileAlignment;
        4,           // MajorOperatingSystemVersion;
        0,           // MinorOperatingSystemVersion;
        0,           // MajorImageVersion;
        0,           // MinorImageVersion;
        4,           // MajorSubsystemVersion;
        0,           // MinorSubsystemVersion;
        0,           // Win32VersionValue;
        0,           // SizeOfImage;
        512,         // SizeOfHeaders;
        0,           // CheckSum; //patched later
        2,           // Subsystem; // IMAGE_SUBSYSTEM_WINDOWS_GUI
        0,           // DllCharacteristics;
        4096 * 256,  // SizeOfStackReserve;
        4096,        // SizeOfStackCommit;
        4096 * 256,  // SizeOfHeapReserve;
        4096,        // SizeOfHeapCommit;
        0,           // LoaderFlags;
        16,          // NumberOfRvaAndSizes;

        // IMAGE_DATA_DIRECTORY DataDirectory[16];
        {0}
    }
};
static i32 pe_write(struct pe_info * pe)
{
    struct pe_header_t pe_header = pe_template;

    i32 i;
    void * op;
    u32 file_offset;
    u32 sum;
    struct section_info * si;
    IMAGE_SECTION_HEADER * psh;

    op = fopen(pe->filename, "wb");
    if (NULL == op)
    {
        print_error_with_line_number("could not write");
        print_error_with_line_number(pe->filename);
        return -1;
    }

    pe->sizeofheaders = pe_file_align(pe,
        sizeof(struct pe_header_t)
        + (pe->sec_count * sizeof(IMAGE_SECTION_HEADER))
    );

    file_offset = pe->sizeofheaders;

    i = 0;
    for (; i < pe->sec_count; ++i)
    {
        si = pe->sec_info + i;

        char * sh_name = NULL;

        if (si->s == &text_section)
        {
            sh_name = ".text";
        }
        else if (si->s == &data_section)
        {
            sh_name = ".data";
        }
        else
        {
            die_with_line_number("unexpected section !!!");
        }

        u32 addr = si->sh_addr - pe->imagebase;
        u32 size = si->sh_size;
        psh = &si->ish;

        pe_header.opthdr.BaseOfCode = addr;

        if (si->s == &data_section)
        {
            if (pe->imp_size)
            {
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_IMPORT,
                    pe->imp_offs + addr, pe->imp_size);
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_IAT,
                    pe->iat_offs + addr, pe->iat_size);
            }
            if (pe->exp_size)
            {
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_EXPORT,
                    pe->exp_offs + addr, pe->exp_size);
            }
        }

        strcpy_((char *)psh->Name, sh_name);

        if (si->s == &text_section)
        {
            // IMAGE_SCN_MEM_READ
            u32 chs = 1 << 30;

            // IMAGE_SCN_MEM_EXECUTE
            chs |= 1 << 29;

            // IMAGE_SCN_CNT_CODE
            chs |= 1 << 5;

            psh->Characteristics = chs;
        }
        else if (si->s == &data_section)
        {
            // IMAGE_SCN_MEM_WRITE
            u32 chs = 1;
            chs <<= 31;

            // IMAGE_SCN_MEM_READ
            chs |= 1 << 30;

            // IMAGE_SCN_CNT_INITIALIZED_DATA
            chs |= 1 << 6;

            psh->Characteristics = chs;
        }
        else
        {
            die_with_line_number("unexpected section!");
        }

        psh->VirtualAddress = addr;
        psh->Misc.VirtualSize = size;
        pe_header.opthdr.SizeOfImage =
            umax(pe_virtual_align(pe, size + addr), pe_header.opthdr.SizeOfImage);

        if (si->data_size)
        {
            psh->PointerToRawData = file_offset;
            file_offset = pe_file_align(pe, file_offset + si->data_size);
            psh->SizeOfRawData = file_offset - psh->PointerToRawData;
            if (si->s == &text_section)
            {
                pe_header.opthdr.SizeOfCode += psh->SizeOfRawData;
            }
            else
            {
                pe_header.opthdr.SizeOfInitializedData += psh->SizeOfRawData;
            }
        }
    }

    //pe_header.filehdr.TimeDateStamp = time(NULL);
    pe_header.filehdr.NumberOfSections = (u16)pe->sec_count;
    pe_header.opthdr.AddressOfEntryPoint = pe->start_addr;
    pe_header.opthdr.SizeOfHeaders = pe->sizeofheaders;
    pe_header.opthdr.ImageBase = pe->imagebase;
    pe_header.opthdr.Subsystem = (u16)pe->subsystem;
    if (pe->s1->pe_stack_size)
    {
        pe_header.opthdr.SizeOfStackReserve = pe->s1->pe_stack_size;
    }
    pe_header.filehdr.Characteristics |= pe->s1->pe_characteristics;

    sum = 0;
    pe_fwrite(&pe_header, sizeof(struct pe_header_t), op, &sum);
    i = 0;
    for (; i < pe->sec_count; ++i)
    {
        pe_fwrite(&pe->sec_info[i].ish, sizeof(IMAGE_SECTION_HEADER), op, &sum);
    }
    pe_fpad(op, pe->sizeofheaders);
    i = 0;
    for (; i < pe->sec_count; ++i)
    {
        si = pe->sec_info + i;
        psh = &si->ish;
        if (si->data_size)
        {
            pe_fwrite(si->data, si->data_size, op, &sum);
            file_offset = psh->PointerToRawData + psh->SizeOfRawData;
            pe_fpad(op, file_offset);
        }
    }

    pe_header.opthdr.CheckSum = sum + file_offset;

    // get offset of opthdr.CheckSum
    struct pe_header_t * p0 = 0;
    u32 * p1 = &(p0->opthdr.CheckSum);
    i32 offset = (i32)p1;

    fseek(op, offset, SEEK_SET);
    pe_fwrite(&pe_header.opthdr.CheckSum, sizeof(u32), op, NULL);

    return 0;
}

//----------------------------------------------------------------------------
static void pe_build_imports(struct pe_info * pe)
{
    u32 rva_base = data_section.reloc_base_addr - pe->imagebase;

    i32 nb_dll_used = 0;
    i32 nb_syms_used = 0;
    for (i32 i_dll = 0; 1; ++i_dll)
    {
        ImportDll * dll = &import_dlls[i_dll];
        if (!dll->name)
            break;

        if (!dll->nb_syms_used)
            continue;

        ++nb_dll_used;
        nb_syms_used += dll->nb_syms_used;
    }

    if (!nb_syms_used)
        return;

    align_end_of_section(&data_section, 16);

    i32 dll_ptr = data_section.bytes.size;
    pe->imp_offs = dll_ptr;
    pe->imp_size = (nb_dll_used + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
    pe->iat_offs = dll_ptr + pe->imp_size;
    pe->iat_size = (nb_syms_used + nb_dll_used) * sizeof(u32);
    (void)append_new_bytes(&data_section.bytes, pe->imp_size + (2 * pe->iat_size));

    i32 thk_ptr = pe->iat_offs;
    i32 ent_ptr = pe->iat_offs + pe->iat_size;

    for (i32 i_dll = 0; 1; ++i_dll)
    {
        ImportDll * dll = &import_dlls[i_dll];
        if (!dll->name)
            break;

        if (!dll->nb_syms_used)
            continue;

        // put the dll name into the import header
        u32 v = bytes_append_str(
            &data_section.bytes,
            dll->name
        );

        IMAGE_IMPORT_DESCRIPTOR * hdr;
        hdr = (IMAGE_IMPORT_DESCRIPTOR *)(data_section.bytes.p + dll_ptr);
        hdr->FirstThunk = thk_ptr + rva_base;
        hdr->ch_or_ft.OriginalFirstThunk = ent_ptr + rva_base;
        hdr->Name = v + rva_base;

        for (i32 i_sym = 0; 1; ++i_sym)
        {
            ImportSymbol * import_sym = &dll->syms[i_sym];
            char * sym_name = import_sym->name;
            if (!sym_name)
                break;

            i32 iat_index = import_sym->iat_index;
            if (!iat_index)
                continue;

            Elf32_Sym * org_sym = (Elf32_Sym *)symtab_section.syms + iat_index;

            org_sym->st_value = thk_ptr;
            org_sym->section = &data_section;

            v = data_section.bytes.size + rva_base;
            (void)append_new_bytes(&data_section.bytes, sizeof(u16)); // hint, not used
            bytes_append_str(
                &data_section.bytes,//
                sym_name
            );

            *(u32 *)(data_section.bytes.p + thk_ptr) = v;
            *(u32 *)(data_section.bytes.p + ent_ptr) = v;
            thk_ptr += sizeof(u32);
            ent_ptr += sizeof(u32);
        }

        // last entry is zero
        *(u32 *)(data_section.bytes.p + thk_ptr) = 0;
        *(u32 *)(data_section.bytes.p + ent_ptr) = 0;
        thk_ptr += sizeof(u32);
        ent_ptr += sizeof(u32);

        dll_ptr += sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }
}

// -------------------------------------------------------------
static i32 pe_assign_addresses(struct pe_info * pe)
{
    Section * secs[2] = { &text_section, &data_section };

    u32 addr = pe->imagebase + 1;
    i32 i = 0;
    for (; i < 2; ++i)
    {
        Section * s = secs[i];
        s->reloc_base_addr = pe->imagebase;
        struct section_info * si = &pe->sec_info[pe->sec_count];

        si->s = s;
        addr = pe_virtual_align(pe, addr);
        s->reloc_base_addr = addr;
        si->sh_addr = addr;

        if (s == &data_section)
        {
            pe_build_imports(pe);
        }

        if (s->bytes.size)
        {
            si->data = s->bytes.p;
            si->data_size = s->bytes.size;

            addr += s->bytes.size;
            si->sh_size = s->bytes.size;
            ++pe->sec_count;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------
static i32 pe_try_resolve_undefined_symbols(
    struct pe_info * pe,
    char * * first_undefined)
{
    (void)pe;

    Elf32_Sym * sym;
    i32 sym_index;
    i32 sym_end;

    align_end_of_section(&text_section, 8);

    sym_end = symtab_section.c_syms / sizeof(Elf32_Sym);
    sym_index = 1;
    for (; sym_index < sym_end; ++sym_index)
    {
        sym = (Elf32_Sym *)symtab_section.syms + sym_index;
        if (!sym->section)
        {
            char * name = (char *)symtab_section.names + sym->st_name;
            if (!sym->is_func)
            {
                *first_undefined = name;
                return 0;
            }

            char * sym_name_ = symtab_name_of_sym(sym);
            ImportId id;
            find_import(sym_name_, &id);

            if (-1 == id.i_dll)
            {
                *first_undefined = name;
                return 0;
            }

            ImportDll * dll = &import_dlls[id.i_dll];
            ImportSymbol * i_sym = &dll->syms[id.i_sym];

            if (!i_sym->iat_index)
            {
                dll->nb_syms_used += 1;
            }

            char name_buf[100];
            u8 * p;

            u32 offset = text_section.bytes.size;
            // add the 'jmp IAT[x]' instruction
            p = append_new_bytes(&text_section.bytes, 8);
            p[0] = 0xFF;
            p[1] = 0x25;
            // add a helper symbol, will be patched later in // pe_build_imports
            strcpy_(name_buf, "IAT.");
            strcat_(name_buf, name);

            put_elf_sym_ARGS args;
            memset(&args, 0, sizeof(put_elf_sym_ARGS));

            if (i_sym->iat_index)
            {
                die_with_line_number("i_sym->iat_index was already set???");
            }

            i_sym->iat_index = put_elf_sym(&symtab_section, name_buf, args);

            if (!i_sym->iat_index)
            {
                die_with_line_number("i_sym->iat_index was not set???");
            }

            put_elf_reloc_direct(
                &text_section,
                offset + 2, 
                i_sym->iat_index);

            // tcc_realloc might have altered sym's address
            sym = (Elf32_Sym *)symtab_section.syms + sym_index;

            // patch the original symbol
            sym->st_value = offset;
            sym->section = &text_section;
        }
    }
    return 1;
}

static void pe_add_runtime(TCCState * s1, struct pe_info * pe)
{
    (void)s1;
    // __intrin_memmove
    i32 sym_index = find_elf_sym(&symtab_section, "__intrin_memmove");
    if (sym_index)
    {
        Elf32_Sym * sym = (Elf32_Sym *)symtab_section.syms + sym_index;
        if (!sym->section)
        {
            die_with_line_number(
                "__intrin_memmove was used, "
                "you need to provide it in your program");
        }
    }

    // start_symbol
    pe->start_symbol = "main";
    sym_index = find_elf_sym(&symtab_section, pe->start_symbol);
    if (!sym_index)
    {
        die_with_line_number("MISSING start_symbol ???");
    }
    else
    {
        Elf32_Sym * sym = (Elf32_Sym *)symtab_section.syms + sym_index;
        if (!sym->section)
        {
            die_with_line_number("MISSING start_symbol !!!");
        }
    }
}

static void pe_set_options(TCCState * s1, struct pe_info * pe)
{
    pe->imagebase = 1 << 22;
    pe->subsystem = 3;

    // Allow override via -Wl,-subsystem=... option
    if (s1->pe_subsystem != 0)
    {
        pe->subsystem = s1->pe_subsystem;
    }

    // set default file/section alignment
    if (pe->subsystem == 1)
    {
        pe->section_align = 0x20;
        pe->file_align = 0x20;
    }
    else
    {
        pe->section_align = 1 << 12;
        pe->file_align = 1 << 9;
    }

    if (s1->section_align != 0)
    {
        pe->section_align = s1->section_align;
    }
    if (s1->pe_file_align != 0)
    {
        pe->file_align = s1->pe_file_align;
    }

    if ((pe->subsystem >= 10) && (pe->subsystem <= 12))
    {
        pe->imagebase = 0;
    }

    if (s1->has_text_addr)
    {
        pe->imagebase = s1->text_addr;
    }
}

static i32 pe_output_file(TCCState * s1, char * filename)
{
    struct pe_info pe;
    i32 i;

    memset(&pe, 0, sizeof(struct pe_info));
    pe.filename = filename;
    pe.s1 = s1;

    pe_add_runtime(s1, &pe);
    pe_set_options(s1, &pe);

    char * sym_undef;
    i32 resolved = pe_try_resolve_undefined_symbols(&pe, &sym_undef);
    if (!resolved)
    {
        print_error_with_line_number("undefined symbol");
        print_error_with_line_number(sym_undef);
        return -1;
    }

    pe_assign_addresses(&pe);
    relocate_syms(s1, &symtab_section, 0);
    s1->pe_imagebase = pe.imagebase;

    Section * secs[2] = {&text_section, &data_section};

    i = 0;
    for (; i < 2; ++i)
    {
        Section * s = secs[i];
        if (s->relocs.u.r.c)
        {
            relocate_section(s1, s);
        }
    }

    void * start_sym = tcc_get_symbol_err(s1, pe.start_symbol);
    u32 start_sym_addr = (u32)start_sym;

    pe.start_addr = (u32)(start_sym_addr - pe.imagebase);

    if (g_nb_errors)
        return -1;

    return pe_write(&pe);
}

 // --------------------------------------------------------------
// XXX grumble grumble keep in sync with tcc_token
static char * tcc_keywords_[] =
{
    // keywords
    "int",
    "void",
    "char",
    "if",
    "else",
    "while",
    "break",
    "return",
    "for",
    "extern",
    "static",
    "unsigned",
    "goto",
    "do",
    "continue",
    "switch",
    "case",
    "long",
    "signed",
    "inline",

    "_Bool",
    "short",
    "struct",
    "union",
    "typedef",
    "default",
    "enum",
    "sizeof",
    "__attribute__",

    // attribute identifiers
    "__stdcall__",

    // builtin functions or variables
    // xxx kill? would lose struct assignment
    "__intrin_memmove",

    0,
};

static TYPE_VAL T_INT = {VT_INT};
static TYPE_VAL T_BYTE = {VT_BYTE};
static TYPE_VAL T_U_INT = {VT_INT | VT_UNSIGNED};
static TYPE_VAL T_FUNC = {VT_FUNC};

void real_main(void)
{
    // Parse args

    char * cmdline_str = GetCommandLineA();
    char cmdline[CHAR_BUF_LEN];
    strcpy_(cmdline, cmdline_str);

    char infile[CHAR_BUF_LEN];
    char outfile[CHAR_BUF_LEN];

    i32 ich = 0;
    while (cmdline[ich] && (cmdline[ich] != ' '))
    {
        ++ich;
    }

    i32 saw_o = 0;
    for (;;)
    {
        while (cmdline[ich] == ' ')
        {
            ++ich;
        }
        
        char * arg = &cmdline[ich];
        if (!arg[0])
            break;

        while (cmdline[ich] && (cmdline[ich] != ' '))
        {
            ++ich;
        }
        i32 at_end = (cmdline[ich] == 0);
        cmdline[ich] = 0;

        if (strcmp_(arg, "-m32") == 0)
        {
            ;
        }
        else if (strcmp_(arg, "-bootstrip") == 0)
        {
            g_bootstrip = 1;
        }
        else if (strcmp_(arg, "-o") == 0)
        {
            saw_o = 1;
        }
        else if (saw_o)
        {
            strcpy_(outfile, arg);
            saw_o = 0;
        }
        else
        {
            strcpy_(infile, arg);
        }

        if (at_end)
            break;

        ++ich;
    }

    TCCState s;
    memset(&s, 0, sizeof(TCCState));

    tcc_state = &s; //!!!

    // put a bogus symbol at the start of the symbol table,
    //  so all 'real' symbols have non-zero indices
    put_elf_sym_ARGS put_elf_sym_args;
    memset(&put_elf_sym_args, 0, sizeof(put_elf_sym_ARGS));
    put_elf_sym(
        &symtab_section,
        "___NULL_SYM___",
        put_elf_sym_args);

    // symtab->strtab
    put_data_str(
        &symtab_section.c_names,
        &symtab_section.names,
        &symtab_section.c_names_allocated,
        ""
    );

    // symtab hash stuff
    i32 * hash_ptr = hash_ptr_add_(&symtab_section, 2 * sizeof(i32));
    symtab_section.nb_buckets = 1;
    symtab_section.nb_syms = 1;
    memset(hash_ptr, 0, 2 * sizeof(i32));

    // init stuff for lexing/preproc
    {
        // init allocators
        tal_new(&tokstr_alloc, TOKSTR_TAL_LIMIT, TOKSTR_TAL_SIZE);

        char * * pp = tcc_keywords_;
        while (*pp)
        {
            char * str = *pp;
            i32 len = strlen_(str);
            intern_name(str, len);
            ++pp;
        }
    }

    // Read in file
    void * f = fopen(infile, "rb");
    if (!f)
    {
        die_with_line_number("fopen failed");
    }
    u8 * fbuf = mallocz_or_die(IO_BUF_SIZE);
    u32 len = fread(fbuf, 1, IO_BUF_SIZE, f);
    if (ferror(f))
    {
        die_with_line_number("fread failed");
    }
    if (len >= IO_BUF_SIZE)
    {
        die_with_line_number("file too large");
    }

    Span span;
    span.p = fbuf;
    span.end = fbuf + len;
    span.end[0] = '\\'; // put eob symbol
    g_span = &span;

    g_line_num = 1;

    // Init crap
    g_nb_errors = 0;
    vtop = vstack() - 1;
    pvtop = vtop;
    funcname = "";
    anon_sym = SYM_FIRST_ANOM;
    section_sym = 0;
    const_wanted = 0;
    nocode_wanted = 1 << 31;

    // define some often used types
    int_type._tv_ = T_INT;
    char_pointer_type._tv_ = T_BYTE;
    mk_pointer(&char_pointer_type);
    size_type._tv_ = T_U_INT;
    ptrdiff_type._tv_ = T_INT;

    // READ FIRST TOKEN
    next();

    // PARSE + CODE GEN
    decl(VT_CONST);

    // vstack sanity
    check_vstack();

    // replay sanity
    if (replay_stack || replay_ptr)
    {
        die_with_line_number("huh, garbage on replay_stack?");
    }

    // clear some stacks
    sym_pop(&global_stack, NULL);
    sym_pop(&local_stack, NULL);

    // At the end of compilation, convert any UNDEF syms to global, and merge
    // with previously existing symbols
    i32 nb_syms = (symtab_section.c_syms / sizeof(Elf32_Sym));
    symtab_section.c_syms = 0;
    symtab_section.c_names = 0;
    i32 * tr = mallocz_or_die(nb_syms * sizeof(i32));
    for (i32 i = 0; i < nb_syms; ++i)
    {
        Elf32_Sym * sym = (Elf32_Sym *)symtab_section.syms + i;

        put_elf_sym_ARGS args;
        memset(&args, 0, sizeof(put_elf_sym_ARGS));
        args.value = sym->st_value;
        args.is_func = sym->is_func;
        args.section = sym->section;
        u8 * name = symtab_section.names + sym->st_name;

        tr[i] = set_elf_sym(
            &symtab_section,
            (char *)name, // grumble grumble
            args
        );
    }

    // now update relocations
    Section * sections[2] = {&text_section, &data_section};
    for (i32 i = 0; i < 2; i++)
    {
        Section * sec = sections[i];
        if (!sec->relocs.u.r.c)
            continue;

        Relocation * rel = (Relocation *)(sec->relocs.u.r.p);
        Relocation * rel_end = (Relocation *)(sec->relocs.u.r.p + sec->relocs.u.r.c);
        for (; rel < rel_end; ++rel)
        {
            i32 n = rel->sym;
            if (n < 0)
            {
                die_with_line_number("internal: invalid symbol index in relocation");
            }
            rel->sym = tr[n];
        }
    }
    free(tr);

    // check for errors
    if (g_nb_errors != 0)
    {
        die_with_line_number("there were errors during compile!");
    }

    // Dump
    if (pe_output_file(tcc_state, outfile))
    {
        do_exit(1);
    }

    // check for errors (again)
    if (g_nb_errors)
    {
        do_exit(1);
    }

    do_exit(0);
}

i32 main(void)
{
    real_main();
    do_exit(1);
}

//  -------- tokens
// XXX move this global state lower... or just rationalize

static char * get_tok_str(i32 v, CValue * cv) //TOKFIXME , is passed stuff other than g _ tok
{
    static i32 str_buf_set = 0;
    static CString cstr_buf = {0};
    if (!str_buf_set)
    {
        cstr_new(&cstr_buf);
        cstr_realloc(&cstr_buf, STRING_MAX_SIZE);
        str_buf_set = 1;
    }

    char * p;

    cstr_reset(&cstr_buf);
    p = cstr_buf.data;

    switch (v)
    {
    case TOK_CINT:
    case TOK_CUINT:
    case TOK_CLONG:
    case TOK_CULONG:
    case TOK_CLLONG:
    case TOK_CULLONG:
        // XXX: not quite exact, but only useful for testing
        sputu(p, (u32)cv->i);
        break;
    case TOK_CCHAR:
        die("get_tok_str TOK_CCHAR");
        break;
    case TOK_PPNUM:
    case TOK_PPSTR:
        return (char *)cv->str.data;
    case TOK_STR:
        die("get_tok_str TOK_STR");
        break;
    case TOK_LINENUM:
        cstr_cat(&cstr_buf, "<linenumber>", 0);
        break;
    case TOK_LT:
        return strcpy_(p, "<");
    case TOK_GT:
        return strcpy_(p, ">");
    case TOK_A_SHL:
        return strcpy_(p, "<<=");
    case TOK_A_SAR:
        return strcpy_(p, ">>=");
    case TOK_EOF:
        return strcpy_(p, "<eof>");
    case TOK_LE:
        return strcpy_(p, "<=");
    case TOK_GE:
        return strcpy_(p, ">=");
    case TOK_NE:
        return strcpy_(p, "!=");
    case TOK_LAND:
        return strcpy_(p, "&&");
    case TOK_LOR:
        return strcpy_(p, "||");
    case TOK_INC:
        return strcpy_(p, "++");
    case TOK_DEC:
        return strcpy_(p, "--");
    case TOK_EQ:
        return strcpy_(p, "==");
    case TOK_SHL:
        return strcpy_(p, "<<");
    case TOK_SAR:
        return strcpy_(p, ">>");
    case TOK_A_ADD:
        return strcpy_(p, "+=");
    case TOK_A_SUB:
        return strcpy_(p, "-=");
    case TOK_A_MUL:
        return strcpy_(p, "*=");
    case TOK_A_DIV:
        return strcpy_(p, "/=");
    case TOK_A_MOD:
        return strcpy_(p, "%=");
    case TOK_A_AND:
        return strcpy_(p, "&=");
    case TOK_A_XOR:
        return strcpy_(p, "^=");
    case TOK_A_OR:
        return strcpy_(p, "|=");
    case TOK_ARROW:
        return strcpy_(p, "->");

    default:
        if (v < TOK_NAME_MIN)
        {
            *p++ = (char)v;
            *p = 0;
        }
        else if (v < (g_nb_names + TOK_NAME_MIN))
        {
            return g_names[v - TOK_NAME_MIN]->str;
        }
        else if (v >= SYM_FIRST_ANOM)
        {
            // special name for anonymous symbol
            strcpy_(p, "L.");
            sputd(p + strlen_(p), v - SYM_FIRST_ANOM);
        }
        else
        {
            // should never happen
            return NULL;
        }
        break;
    }
    return cstr_buf.data;
}

static void skip(i32 c)
{
    if (!g_tok_is(c))
    {
        tcc_error_puts("unexpected (got '%s')", get_tok_str(g_tok(), &tokc));
    }
    next();
}

// NOTES

// # 32-Bit Addressing Forms with the ModR/M Byte
// 
// this is a rough 'markdown-ification' of table Table 2-2 from  
// 
// Intel� 64 and IA-32 Architectures  
// Software Developer�s Manual  
// Volume 2 (2A, 2B, 2C, & 2D):  
// Instruction Set Reference, A-Z  
// 
// ## ModR/M Byte Structure
// 
// The ModR/M byte is an 8-bit field in x86 instructions, organized as follows:
// 
// | Bit    | 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
// |--------|-----|-----|-----|-----|-----|-----|-----|-----|
// | Usage  | Mod | Mod | Reg | Reg | Reg | R/M | R/M | R/M |
// 
// - Bits 7-6 (Mod): 
//   - Determines the addressing mode
//     - 00 - Memory addressing with no displacement (but R/M = 101, means disp32).
//     - 01 - Memory addressing with an 8-bit displacement.
//     - 10 - Memory addressing with a 32-bit displacement.
//     - 11 - Register-to-register mode (no memory addressing).
// 
// - Bits 5-3 (Reg): 
//   - Specifies the register or a part of the opcode, depending on the instruction. 
//   - It typically represents the destination register or a sub-field of the opcode.
// 
// - Bits 2-0 (R/M): 
//   - Indicates either a register or, combined with the Mod field, 
//   - selects a specific memory addressing mode.
// 
// ## Register notation
// 
// in instructions that use modrm byte, the docs will have one of these notations.  
// they determine how to intepret the Reg bits.
// 
// | Register notation            |     |     | reg0 | reg1 | reg2 | reg3 | reg4 | reg5 | reg6 | reg7 |
// |------------------------------|-----|-----|------|------|------|------|------|------|------|------|
// | r8(/r)                       |     |     | AL   | CL   | DL   | BL   | AH   | CH   | DH   | BH   |
// | r16(/r)                      |     |     | AX   | CX   | DX   | BX   | SP   | BP   | SI   | DI   |
// | r32(/r)                      |     |     | EAX  | ECX  | EDX  | EBX  | ESP  | EBP  | ESI  | EDI  |
// | mm(/r)                       |     |     | MM0  | MM1  | MM2  | MM3  | MM4  | MM5  | MM6  | MM7  |
// | xmm(/r)                      |     |     | XMM0 | XMM1 | XMM2 | XMM3 | XMM4 | XMM5 | XMM6 | XMM7 |
// | (In decimal) /digit (Opcode) |     |     | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    |
// | (In binary) REG =            |     |     | 000  | 001  | 010  | 011  | 100  | 101  | 110  | 111  |
// 
// ## Main table
// 
// the combination of Mod and R/M specify an "Effective Address".  
// Combind with the reg bits, you get the full 8 bits.  
// if you look up a given modrm byte in this table, you probably can figure out what all it encodes.  
// (unless the specific instruction decides to do something weird, I don't know man)
// 
// ### Mod 00
// 
// | Effective Address            | Mod | R/M | reg0 | reg1 | reg2 | reg3 | reg4 | reg5 | reg6 | reg7 |
// |------------------------------|-----|-----|------|------|------|------|------|------|------|------|
// | [EAX]                        | 00  | 000 | 00   | 08   | 10   | 18   | 20   | 28   | 30   | 38   |
// | [ECX]                        |     | 001 | 01   | 09   | 11   | 19   | 21   | 29   | 31   | 39   |
// | [EDX]                        |     | 010 | 02   | 0A   | 12   | 1A   | 22   | 2A   | 32   | 3A   |
// | [EBX]                        |     | 011 | 03   | 0B   | 13   | 1B   | 23   | 2B   | 33   | 3B   |
// | [--][--]                     |     | 100 | 04   | 0C   | 14   | 1C   | 24   | 2C   | 34   | 3C   |
// | disp32                       |     | 101 | 05   | 0D   | 15   | 1D   | 25   | 2D   | 35   | 3D   |
// | [ESI]                        |     | 110 | 06   | 0E   | 16   | 1E   | 26   | 2E   | 36   | 3E   |
// | [EDI]                        |     | 111 | 07   | 0F   | 17   | 1F   | 27   | 2F   | 37   | 3F   |
// 
// ### Mod 01
// 
// | Effective Address            | Mod | R/M | reg0 | reg1 | reg2 | reg3 | reg4 | reg5 | reg6 | reg7 |
// |------------------------------|-----|-----|------|------|------|------|------|------|------|------|
// | [EAX]+disp8                  | 01  | 000 | 40   | 48   | 50   | 58   | 60   | 68   | 70   | 78   |
// | [ECX]+disp8                  |     | 001 | 41   | 49   | 51   | 59   | 61   | 69   | 71   | 79   |
// | [EDX]+disp8                  |     | 010 | 42   | 4A   | 52   | 5A   | 62   | 6A   | 72   | 7A   |
// | [EBX]+disp8                  |     | 011 | 43   | 4B   | 53   | 5B   | 63   | 6B   | 73   | 7B   |
// | [--][--]+disp8               |     | 100 | 44   | 4C   | 54   | 5C   | 64   | 6C   | 74   | 7C   |
// | [EBP]+disp8                  |     | 101 | 45   | 4D   | 55   | 5D   | 65   | 6D   | 75   | 7D   |
// | [ESI]+disp8                  |     | 110 | 46   | 4E   | 56   | 5E   | 66   | 6E   | 76   | 7E   |
// | [EDI]+disp8                  |     | 111 | 47   | 4F   | 57   | 5F   | 67   | 6F   | 77   | 7F   |
// 
// ### Mod 10
// 
// | Effective Address            | Mod | R/M | reg0 | reg1 | reg2 | reg3 | reg4 | reg5 | reg6 | reg7 |
// |------------------------------|-----|-----|------|------|------|------|------|------|------|------|
// | [EAX]+disp32                 | 10  | 000 | 80   | 88   | 90   | 98   | A0   | A8   | B0   | B8   |
// | [ECX]+disp32                 |     | 001 | 81   | 89   | 91   | 99   | A1   | A9   | B1   | B9   |
// | [EDX]+disp32                 |     | 010 | 82   | 8A   | 92   | 9A   | A2   | AA   | B2   | BA   |
// | [EBX]+disp32                 |     | 011 | 83   | 8B   | 93   | 9B   | A3   | AB   | B3   | BB   |
// | [--][--]+disp32              |     | 100 | 84   | 8C   | 94   | 9C   | A4   | AC   | B4   | BC   |
// | [EBP]+disp32                 |     | 101 | 85   | 8D   | 95   | 9D   | A5   | AD   | B5   | BD   |
// | [ESI]+disp32                 |     | 110 | 86   | 8E   | 96   | 9E   | A6   | AE   | B6   | BE   |
// | [EDI]+disp32                 |     | 111 | 87   | 8F   | 97   | 9F   | A7   | AF   | B7   | BF   |
// 
// ### Mod 11
// 
// | Effective Address            | Mod | R/M | reg0 | reg1 | reg2 | reg3 | reg4 | reg5 | reg6 | reg7 |
// |------------------------------|-----|-----|------|------|------|------|------|------|------|------|
// | EAX/AX/AL/MM0/XMM0           | 11  | 000 | C0   | C8   | D0   | D8   | E0   | E8   | F0   | F8   |
// | ECX/CX/CL/MM/XMM1            |     | 001 | C1   | C9   | D1   | D9   | E1   | E9   | F1   | F9   |
// | EDX/DX/DL/MM2/XMM2           |     | 010 | C2   | CA   | D2   | DA   | E2   | EA   | F2   | FA   |
// | EBX/BX/BL/MM3/XMM3           |     | 011 | C3   | CB   | D3   | DB   | E3   | EB   | F3   | FB   |
// | ESP/SP/AH/MM4/XMM4           |     | 100 | C4   | CC   | D4   | DC   | E4   | EC   | F4   | FC   |
// | EBP/BP/CH/MM5/XMM5           |     | 101 | C5   | CD   | D5   | DD   | E5   | ED   | F5   | FD   |
// | ESI/SI/DH/MM6/XMM6           |     | 110 | C6   | CE   | D6   | DE   | E6   | EE   | F6   | FE   |
// | EDI/DI/BH/MM7/XMM7           |     | 111 | C7   | CF   | D7   | DF   | E7   | EF   | F7   | FF   |
// 
// ## extra notes
// 
// 1. 
//   - The [--][--] nomenclature means a SIB follows the ModR/M byte.
// 2. 
//   - The disp32 nomenclature denotes a 32-bit displacement that follows the ModR/M byte 
//   - (or the SIB byte if one is present) 
//   - and that is added to the index.
// 3. 
//   - The disp8 nomenclature denotes an 8-bit displacement that follows the ModR/M byte 
//   - (or the SIB byte if one is present) 
//   - and that is sign-extended and added to the index.