#include <bfd.h> 
#include "include/loader.h"

int load_binary(std::string &fname, Binary *bin, Binary::BinaryType type)
{
    return load_binary_bfd(fname, bin, type); 
}

void unload_binary(Binary *bin)
{
    size_t x; 
    Section *sec; 

    for(x = 0;x < bin->sections.size(); x++)
    {
        sec = &bin->sections[x]; 
        if(sec->bytes)
            free(sec->bytes); 
    }
}

static bfd *open_bfd(std::string &fname)
{
    static int bfd_inited = 0;
    bfd *bfd_prog; 

    if(!bfd_inited)
    {
        bfd_init(); 
        bfd_inited = 1; 
    }

    /*NULL: auto detect target format based on file content */ 
    bfd_prog = bfd_openr(fname.c_str(), NULL); 
    if(!bfd_prog){
        BFD__fail("failed to open binary %s", fname); 
    }

    if(!bfd_check_format(bfd_prog, bfd_object)){
        BFD__fail("file '%s' is not executable", fname); 
    }

    bfd_set_error(bfd_error_no_error); 

    if(bfd_get_flavour(bfd_prog) == bfd_target_unknown_flavour){
        BFD__fail("unrecognized format for binary '%s'", fname); 
    }

    return bfd_prog; 
}


static int load_binary(std::string &fname, Binary *bin, Binary::BinaryType type) 
{
    if (fname.empty()) {
        std::cerr << "error: filename is empty\n";
        return -1;
    }

    if (!bin) {
        std::cerr << "error: Binary pointer is null\n";
        return -1;
    }

    bfd *bfd_prog = open_bfd(fname);
    if (!bfd_prog) {
        std::cerr << "failed to open binary: " << fname << "\n";
        return -1;
    }

    const bfd_arch_info_type *bfd_info = bfd_get_arch_info(bfd_prog);
    if (!bfd_info) {
        std::cerr << "failed to get architecture info for: " << fname << "\n";
        bfd_close(bfd_prog);
        return -1;
    }

    bin->filename = fname;
    bin->entry = bfd_get_start_address(bfd_prog);

    /* give human readable format ("elf64, pei-i286)") */
    if (bfd_prog->xvec && bfd_prog->xvec->name) {
        bin->type_str = bfd_prog->xvec->name;
    } else {
        std::cerr << "error: missing bfd format name\n";
        bfd_close(bfd_prog);
        return -1;
    }

    switch (bfd_prog->xvec->flavour) {
        /* ELF - linux, most unix like systems */
        case bfd_target_elf_flavour:
            bin->type = Binary::BIN_TYPE_ELF;
            break;

        /* COFF - legacy unix/windows object files */
        case bfd_target_coff_flavour:
            bin->type = Binary::BIN_TYPE_COFF;
            break;

        /* PE - windows executables */
        case bfd_target_pe_flavour:
            bin->type = Binary::BIN_TYPE_PE;
            break;

        /* Mach-O - macOS executables */
        case bfd_target_mach_o_flavour:
            bin->type = Binary::BIN_TYPE_MACHO;
            break;

        case bfd_target_unknown_flavour:
        default:
            std::cerr << "unsupported binary type (" 
                      << (bfd_prog->xvec ? bfd_prog->xvec->name : "unknown") 
                      << ")\n";
            bfd_close(bfd_prog);
            return -1;
    }

    /* store architecture info string */
    if (bfd_info->printable_name) {
        bin->arch_str = bfd_info->printable_name;
    } else {
        std::cerr << "missing printable architecture name\n";
        bfd_close(bfd_prog);
        return -1;
    }

    switch (bfd_info->mach) {

        /* x86 32-bit and 64-bit */
        case bfd_mach_i386_i386:
            bin->arch = Binary::ARCH_X86;
            bin->bits = 32;
            break;
        case bfd_mach_x86_64:
            bin->arch = Binary::ARCH_X86;
            bin->bits = 64;
            break;

        /* ARM 32-bit and 64-bit */
        case bfd_mach_arm_4t:
        case bfd_mach_arm_5t:
        case bfd_mach_arm_6:
        case bfd_mach_arm_7:
        case bfd_mach_arm_8:
        case bfd_mach_arm_unknown:
            bin->arch = Binary::ARCH_ARM;
            bin->bits = 32;
            break;
        case bfd_mach_aarch64:
            bin->arch = Binary::ARCH_ARM;
            bin->bits = 64;
            break;

        /* RISC-V 32-bit and 64-bit */
        case bfd_mach_riscv32:
            bin->arch = Binary::ARCH_RISCV;
            bin->bits = 32;
            break;
        case bfd_mach_riscv64:
            bin->arch = Binary::ARCH_RISCV;
            bin->bits = 64;
            break;

        default:
            std::cerr << "unsupported architecture (" << bfd_info->printable_name << ")\n";
            bfd_close(bfd_prog);
            return -1;
    }

    /* load symbols */
    if (load_symbols_bfd(bfd_prog, bin) < 0) {
        std::cerr << "failed to load symbols\n";
        bfd_close(bfd_prog);
        return -1;
    }

    /* load dynamic symbols */
    if (load_dynsym_bfd(bfd_prog, bin) < 0) {
        std::cerr << "failed to load dynamic symbols\n";
        bfd_close(bfd_prog);
        return -1;
    }

    bfd_close(bfd_prog);
    return 0;
}

static int load_symbols_bfd(bfd *bfd_prog, Binary *bin)
{
    if (bfd_prog == nullptr || bin == nullptr){
        BFD_ERROR("load_symbols_bfd: invalid arguments");
    }

    long symtab_size;
    asymbol **bfd_symtab = nullptr;
    long sym_cnt;

    /* Get number of bytes needed for static symbol table */
    symtab_size = bfd_get_symtab_upper_bound(bfd_prog);
    if (symtab_size < 0){
        BFD_ERROR("load_symbols_bfd: failed to read symbol table size");
    }

    bfd_symtab = reinterpret_cast<asymbol**>(malloc(symtab_size));
    if (!bfd_symtab){
        BFD_ERROR("load_symbols_bfd: out of memory while allocating symbol table");
    }

    /* Read static symbol table into memory */
    sym_cnt = bfd_canonicalize_symtab(bfd_prog, bfd_symtab);
    if (sym_cnt < 0) {
        std::cerr << "load_symbols_bfd: failed to canonicalize symtab: "
                  << bfd_errmsg(bfd_get_error()) << "\n";
        free(bfd_symtab);
        return -1;
    }

    /* Iterate through static symbols */
    for (long x = 0; x < sym_cnt; ++x) {
        asymbol *s = bfd_symtab[x];
        if (!s)
            continue;

        bin->symbols.emplace_back();
        Symbol &sym = bin->symbols.back();

        if (s->flags & BSF_FUNCTION){
            sym.type = Symbol::SYM_TYPE_FUNC;

        }else if (s->flags & BSF_OBJECT){
            sym.type = Symbol::SYM_TYPE_OBJECT;

        }else if (s->flags & BSF_LOCAL){
            sym.type = Symbol::SYM_TYPE_LABEL;

        }else{
            sym.type = Symbol::SYM_TYPE_UNKNOWN;
        }

        sym.name = s->name ? std::string(s->name) : std::string();
        sym.addr = static_cast<uint64_t>(bfd_asymbol_value(s));
    }

    free(bfd_symtab);
    return 0;
}

/* DYNAMIC SYMBOLS LOADER (DYNSYM) */
static int load_dynsym_bfd(bfd *bfd_prog, Binary *bin)
{
    if (bfd_prog == nullptr || bin == nullptr){
        BFD_ERROR("load_dynsym_bfd: invalid arguments");
    }

    long dynsym_size;
    asymbol **bfd_dynsym = nullptr;
    long sym_cnt;

    /* Get number of bytes needed for dynamic symbol table */
    dynsym_size = bfd_get_dynamic_symtab_upper_bound(bfd_prog);
    if (dynsym_size < 0){
        BFD_ERROR("load_dynsym_bfd: failed to read dynamic symbol table size");
    }

    bfd_dynsym = reinterpret_cast<asymbol**>(malloc(dynsym_size));
    if (!bfd_dynsym){
        BFD_ERROR("load_dynsym_bfd: out of memory while allocating dynamic symbol table");
    }

    /* Read dynamic symbol table into memory */
    sym_cnt = bfd_canonicalize_dynamic_symtab(bfd_prog, bfd_dynsym);
    if (sym_cnt < 0) {
        std::cerr << "load_dynsym_bfd: failed to canonicalize dynamic symbol table: "
                  << bfd_errmsg(bfd_get_error()) << "\n";
        free(bfd_dynsym);
        return -1;
    }

    /* Iterate through dynamic symbols */
    for (long x = 0; x < sym_cnt; ++x) {
        asymbol *s = bfd_dynsym[x];
        if (!s)
            continue;

        bin->symbols.emplace_back();
        Symbol &sym = bin->symbols.back();

        if (s->flags & BSF_FUNCTION){
            sym.type = Symbol::SYM_TYPE_FUNC;

        }else if (s->flags & BSF_OBJECT){
            sym.type = Symbol::SYM_TYPE_OBJECT;

        }else if (s->flags & BSF_LOCAL){
            sym.type = Symbol::SYM_TYPE_LABEL;

        }else{
            sym.type = Symbol::SYM_TYPE_UNKNOWN;
        }

        sym.name = s->name ? std::string(s->name) : std::string();
        sym.addr = static_cast<uint64_t>(bfd_asymbol_value(s));
    }

    free(bfd_dynsym);
    return 0;
}

