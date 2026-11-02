# Windows libcurl build and validation

Validated on Windows x86_64 on 2026-11-02. Client and dedicated server use
static libcurl 8.22.0 with Windows Schannel and the Windows certificate store.
No libcurl/OpenSSL DLLs are required by these builds. SDL2 and the already
packaged libwinpthread runtime remain separate dependencies.

## Build

From an MSYS2 MINGW64 shell at the repository root:

```sh
sh engine/misc/build-curl-mingw.sh x86_64 "$PWD/engine/build/curl-mingw-x86_64"
make -C engine release PLATFORM=mingw32 ARCH=x86_64 WINDRES=windres USE_CURL=1 USE_CURL_DLOPEN=0
```

The existing curl script pins the source version and verifies its SHA-256
before building. Make now discovers the above local prefix automatically;
an explicit CURL_PREFIX still takes precedence, including in CI. The existing
Windows CI already builds and passes a static curl prefix for x86/x86_64.

Make tracks curl settings in `.curl-config` inside each build directory.
Changing USE_CURL, USE_CURL_DLOPEN or the prefix rebuilds affected client/server
objects, rather than retaining a DLOPEN implementation from an earlier build.
The static library is also an explicit executable link prerequisite.

## Runtime evidence

Logs and probes are in `output/install-validation/`:

- `curl-build.log`: pinned source SHA-256 verified and static library built.
- `curl-tls-results.log`: an executable linked to that library completed
  HTTPS GET to curl.se with peer and hostname checks enabled: HTTP 200,
  libcurl 8.22.0, TLS backend Schannel. This is a library/TLS probe, not a
  production Ladder registration.
- `static-curl-runtime-results.json`: actual game binaries sent Ladder
  registration POSTs to a loopback fixture in both client and server. The
  fixture deliberately returned an error; both correctly parsed its marker.
  No registrations or match reports were sent to the production Ladder.
- The game downloaded a missing map PK3 from a loopback HTTP redirect server,
  produced an identical SHA-256 and subsequently entered the pure server.
  The probe stopped that client after observing ClientBegin; the separate
  client Ladder probe exited normally.
- PATH contained only the test installation and Windows system directories.
  No curl DLL loading was attempted. PE imports show only Windows system
  libraries plus the existing SDL2/libwinpthread dependencies.
- Release preflight with the current development version passes again.

The local integration fixture also exposed a response-parser bug: looking
for `"error"` with strstr matched the value in `"status":"error"` before the
actual property. Registration parsing now identifies property names, accepts
JSON whitespace and handles null/zero-length output buffers. Production-code
regressions are in `tests/test_ladder_registration_response.py`.

These checks establish Windows HTTP transport, download integrity and
registration response handling. They do not claim a real Ladder registration,
match-upload acceptance, content-catalog service availability, or native
Linux/macOS/x86 runtime validation. Final versioned packages still need the
release-candidate checks from the roadmap.
