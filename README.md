# DLLOXY

Create a proxy DLL.

For use with mingw on Linux. 64bit for now.

## What Does It Do?

DLLOXY goes through the export table of a DLL and creates a directory
containing source files that make up a proxy DLL that provides the same exact
exports and passes all the calls to the original DLL using jumps from naked
functions.

Make sure that you load the original DLL from the correct path.

## What Can I Do With It?

You can replace some of the naked functions with with your own implementation.
Sky is the limit.

See `examples/`

## How To Use It?

```bash
pip install -r requirements.txt
./dlloxy.py path/to/my.dll
cd proxy_my.dll
$EDITOR main.c
make
```

