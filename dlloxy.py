#!/usr/bin/env python3

import os
import sys
import pefile

makefile = """\
%s: main.c main.def
	x86_64-w64-mingw32-gcc -shared -static -o $@ $^"""

def_file = """\
LIBRARY %s
EXPORTS
%s"""

c_file = """\
#include <windows.h>
#include <assert.h>

#define NAKED __attribute__((naked))

%s
HINSTANCE original_dll;

BOOL WINAPI DllMain(HINSTANCE dll_handle, DWORD reason, void *res)
{
    char dll_path[MAX_PATH];
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
            /* XXX: may need tweaking */
            GetSystemDirectory(dll_path, ARRAYSIZE(dll_path));
            strcat(dll_path, "\\\\%s");

            original_dll = LoadLibrary(dll_path);
            if (original_dll == NULL) return FALSE;

%s
            break;
        case DLL_PROCESS_DETACH:
            FreeLibrary(original_dll);
            break;
    }

    return TRUE;
}
%s"""

pointer = "static FARPROC original_{};\n"
getproc = """            original_{0} = GetProcAddress(original_dll, "{0}");\n            assert(original_{0} != NULL);\n"""

func = """

NAKED void __stdcall proxy_{0}()
{{
    __asm("jmp *%0"
          : /* no outputs */
          : "m" (original_{0})
          : "%rax");
}}"""

export = """    {0}=proxy_{0} @{1}\n"""

def main():
    pointers = []
    getprocs = []
    funcs = []
    exports = []

    dll_name = os.path.split(sys.argv[1])[1]
    directory = f"proxy_{dll_name}"
    os.mkdir(directory)

    pe = pefile.PE(sys.argv[1])

    for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
        if exp.name:
            name = exp.name.decode()
            ordinal = str(exp.ordinal)
            pointers.append(pointer.format(name))
            getprocs.append(getproc.format(name))
            funcs.append(func.format(name))
            exports.append(export.format(name, ordinal))

    with open(os.path.join(directory, "main.c"), "x") as f:
        f.write(c_file % ("".join(pointers), dll_name, "".join(getprocs), "".join(funcs)))

    with open(os.path.join(directory, "main.def"), "x") as f:
        f.write(def_file % (dll_name.upper(), "".join(exports)))

    with open(os.path.join(directory, "Makefile"), "x") as f:
        f.write(makefile % (dll_name,))

main()
