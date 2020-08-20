#include <windows.h>
#include <assert.h>

#define NAKED __attribute__((naked))

/* dxvk's version */

typedef HRESULT(__stdcall* create_dxgi_factory_t)(REFIID id, void** factory);
static create_dxgi_factory_t original_CreateDXGIFactory;

static FARPROC original_CreateDXGIFactory1;
static FARPROC original_CreateDXGIFactory2;
static FARPROC original_DXGIDeclareAdapterRemovalSupport;
static FARPROC original_DXGIGetDebugInterface1;

HINSTANCE original_dll;

BOOL WINAPI DllMain(HINSTANCE dll_handle, DWORD reason, void *res)
{
    char dll_path[MAX_PATH];
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
            /* XXX: may need tweaking */
            GetSystemDirectory(dll_path, ARRAYSIZE(dll_path));
            strcat(dll_path, "\\dxgi.dll");

            original_dll = LoadLibrary(dll_path);
            if (original_dll == NULL) return FALSE;

            original_CreateDXGIFactory = (create_dxgi_factory_t) GetProcAddress(original_dll, "CreateDXGIFactory");
            assert(original_CreateDXGIFactory != NULL);

            original_CreateDXGIFactory1 = GetProcAddress(original_dll, "CreateDXGIFactory1");
            assert(original_CreateDXGIFactory1 != NULL);
            original_CreateDXGIFactory2 = GetProcAddress(original_dll, "CreateDXGIFactory2");
            assert(original_CreateDXGIFactory2 != NULL);
            original_DXGIDeclareAdapterRemovalSupport = GetProcAddress(original_dll, "DXGIDeclareAdapterRemovalSupport");
            assert(original_DXGIDeclareAdapterRemovalSupport != NULL);
            original_DXGIGetDebugInterface1 = GetProcAddress(original_dll, "DXGIGetDebugInterface1");
            assert(original_DXGIGetDebugInterface1 != NULL);

	    MessageBox(NULL, "we are in", "dlloxy", MB_OK);

            break;
        case DLL_PROCESS_DETACH:
            FreeLibrary(original_dll);
            break;
    }

    return TRUE;
}


HRESULT __stdcall proxy_CreateDXGIFactory(REFIID id, void** factory)
{
	MessageBox(NULL, "created factory", "dlloxy", MB_OK);
	return original_CreateDXGIFactory(id, factory);
}

NAKED void __stdcall proxy_CreateDXGIFactory1()
{
    __asm("jmp *%0"
          : /* no outputs */
          : "m" (original_CreateDXGIFactory1)
          : "%rax");
}

NAKED void __stdcall proxy_CreateDXGIFactory2()
{
    __asm("jmp *%0"
          : /* no outputs */
          : "m" (original_CreateDXGIFactory2)
          : "%rax");
}

NAKED void __stdcall proxy_DXGIDeclareAdapterRemovalSupport()
{
    __asm("jmp *%0"
          : /* no outputs */
          : "m" (original_DXGIDeclareAdapterRemovalSupport)
          : "%rax");
}

NAKED void __stdcall proxy_DXGIGetDebugInterface1()
{
    __asm("jmp *%0"
          : /* no outputs */
          : "m" (original_DXGIGetDebugInterface1)
          : "%rax");
}
