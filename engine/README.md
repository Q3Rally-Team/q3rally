Q3Rally engine
==============

## English

Q3Rally is a standalone game based on ioquake3.

There are enhancements and customizations to the engine. It is preferred to use the Q3Rally engine instead of ioquake3 to run the game.

Engine changes compared to ioquake3:

- Added Theora video playback for the Q3Rally intro video
- Changed various cvar defaults
- Changed the fallback missing sound filename
- Modified mouse handling in menus
- Changed invalid default pk3 checksums to use sv_pure 0
- Added baseq3r pk3 checksums

(This list may be incomplete.)

## Compiling
Q3Rally is compiled using GNU Make (`make`) from a terminal while in the engine directory.

`make ARCH=x86`, `make ARCH=x86_64`, `make ARCH=arm64` (ARM64/Apple Silicon) specify compiling for a specific architecture instead of the host architecture. (Windows ARM64 is unsupported.)

Compiling Linux arm64 from an x86\_64 host requires `CC=aarch64-linux-gnu-gcc`. Compiling for a different architecture than the host on Linux requires setting `SDL_CFLAGS=... SDL_LIBS=...` (see output of `pkg-config sdl2 --cflags` and `--libs`) with the paths to a build of SDL2 for that architecture.

If you have commit access and use an SSH key use `git clone git@github.com:Q3Rally-Team/q3rally.git` instead.

### Windows
There are several ways to get a MinGW-w64 build environment.

#### MSYS2
1. Install MSYS2 with packages: git, make, mingw-w64-i686-gcc, mingw-w64-x86_64-gcc
2. Open "MSYS2 MinGW 64-bit"
3. Get the Q3Rally source code with `git clone https://github.com/Q3Rally-Team/q3rally.git`
4. Change to the engine directory using `cd q3rally/engine`
5. Run `make`

#### Cygwin
_This was more relevant before MSYS2 was made._
1. Install Cygwin with packages: git, make, mingw64-i686-gcc-core, mingw64-x86_64-gcc-core
2. Open "Cygwin"
3. Get the Q3Rally source code with `git clone https://github.com/Q3Rally-Team/q3rally.git`
4. Change to the engine directory using `cd q3rally/engine`
5. Run `make`

#### WSL
1. Install Windows Subsystem for Linux.
2. Install a Linux distribution from the Windows store (e.g., Ubuntu).
3. Open the start menu entry for the distribution (e.g., Ubuntu).
4. Install Git, Make, GCC, and MinGW-w64 packages, for Ubuntu run: `sudo apt install git make gcc mingw-w64`
5. Get the Q3Rally source code with `git clone https://github.com/Q3Rally-Team/q3rally.git`
6. Change to the engine directory using `cd q3rally/engine`
7. Run `make PLATFORM=mingw32`

It's possible to compile for Linux under WSL using `make` or `./make-linux-portable.sh`. Compiling for macOS using osxcross is not supported under WSL as of writing.

### Linux
1. Install `git make gcc libsdl2-dev` packages for your Linux distribution.
2. Get the Q3Rally source code with `git clone https://github.com/Q3Rally-Team/q3rally.git`
3. Change to the engine directory using `cd q3rally/engine`
4. Run `make`

To make a release which includes SDL2 run: `./make-linux-portable.sh x86_64` (also with x86).

`make PLATFORM=mingw32` and `make PLATFORM=darwin` can be used to cross-compile for Windows and macOS from Linux, if mingw-w64 and osxcross are installed.

### macOS
1. Install Xcode from the AppStore (the GUI is optional, only the command-line tools are required).
2. Open Terminal app.
3. Get the Q3Rally source code with `git clone https://github.com/Q3Rally-Team/q3rally.git`
4. Change to the engine directory using `cd q3rally/engine`
5. Run `make`

To create an AppBundle:
* Single architecture: `./make-macosx.sh x86_64` (if needed, replace x86\_64 with arm64 (Apple Silicon), x86, or ppc).
* Modern macOS 10.9+ (Apple Silicon, x86_64): `./make-macosx-ub2.sh`
* Legacy macOS 10.5+ (x86_64, x86, ppc): `./make-macosx-ub.sh`

Modern AppBundle requires macOS 11.0 SDK or later to be installed. Legacy AppBundle must be compiled on macOS 10.6 with 10.5 and 10.6 SDKs installed.

## Release builds
Assuming you have everything set up; release builds could be done like this.

`clean` is used to do a fresh build to be sure VERSION, etc from Makefile are applied to the build.

`-j#` is the number of CPU threads to use. Set it to your CPU cores/threads for faster compiling.

```
# Windows; run on Windows or Linux with mingw-w64
make PLATFORM=mingw32 ARCH=x86 clean release -j8
make PLATFORM=mingw32 ARCH=x86_64 clean release -j8

# Linux; run on Linux or Windows (WSL)
./make-linux-portable.sh ARCH=x86 clean release -j8
./make-linux-portable.sh ARCH=x86_64 clean release -j8

# macOS Modern; run on macOS or Linux with osxcross
make PLATFORM=darwin ARCH=arm64 clean
make PLATFORM=darwin ARCH=x86_64 clean
./make-macosx-ub2.sh

# macOS Legacy: run on macOS 10.6
make PLATFORM=darwin ARCH=x86_64 clean
make PLATFORM=darwin ARCH=x86 clean
make PLATFORM=darwin ARCH=ppc clean
./make-macosx-ub.sh
```

---

## Deutsch

Q3Rally ist ein eigenständiges Spiel auf Basis von ioquake3.

Die Engine enthält verschiedene Erweiterungen und Anpassungen. Für den Spielbetrieb sollte bevorzugt die Q3Rally-Engine statt ioquake3 verwendet werden.

Änderungen gegenüber ioquake3:

- Theora-Videowiedergabe für das Q3Rally-Intro ergänzt
- Diverse CVar-Standardwerte angepasst
- Fallback-Dateiname für fehlende Sounds geändert
- Maussteuerung in Menüs überarbeitet
- Ungültige Standard-pk3-Prüfsummen so geändert, dass `sv_pure 0` verwendet wird
- Prüfsummen für baseq3r-pk3s hinzugefügt

(Diese Liste kann unvollständig sein.)

## Kompilieren
Q3Rally wird im Engine-Verzeichnis mit GNU Make (`make`) aus dem Terminal heraus gebaut.

`make ARCH=x86`, `make ARCH=x86_64`, `make ARCH=arm64` (ARM64/Apple Silicon) legen fest, dass für eine bestimmte Architektur statt für die Host-Plattform gebaut wird. (Windows ARM64 wird nicht unterstützt.)

Für einen Linux-arm64-Build auf einem x86\_64-Host ist `CC=aarch64-linux-gnu-gcc` erforderlich. Für Cross-Builds unter Linux müssen `SDL_CFLAGS=... SDL_LIBS=...` auf die Pfade einer SDL2-Build für die Zielarchitektur gesetzt werden (siehe Ausgabe von `pkg-config sdl2 --cflags` und `--libs`).

Wer Commit-Rechte besitzt und SSH-Schlüssel nutzt, kann stattdessen `git clone git@github.com:Q3Rally-Team/q3rally.git` verwenden.

### Windows
Es gibt mehrere Wege, eine MinGW-w64-Build-Umgebung einzurichten.

#### MSYS2
1. MSYS2 mit den Paketen git, make, mingw-w64-i686-gcc, mingw-w64-x86_64-gcc installieren
2. "MSYS2 MinGW 64-bit" öffnen
3. Q3Rally-Quellcode mit `git clone https://github.com/Q3Rally-Team/q3rally.git` holen
4. In das Engine-Verzeichnis wechseln: `cd q3rally/engine`
5. `make` ausführen

#### Cygwin
_Früher relevanter, bevor MSYS2 verfügbar war._
1. Cygwin mit den Paketen git, make, mingw64-i686-gcc-core, mingw64-x86_64-gcc-core installieren
2. "Cygwin" öffnen
3. Q3Rally-Quellcode mit `git clone https://github.com/Q3Rally-Team/q3rally.git` holen
4. In das Engine-Verzeichnis wechseln: `cd q3rally/engine`
5. `make` ausführen

#### WSL
1. Windows Subsystem for Linux installieren.
2. Eine Linux-Distribution aus dem Microsoft Store installieren (z. B. Ubuntu).
3. Den Startmenü-Eintrag der Distribution öffnen (z. B. Ubuntu).
4. Pakete für Git, Make, GCC und MinGW-w64 installieren, unter Ubuntu: `sudo apt install git make gcc mingw-w64`
5. Q3Rally-Quellcode mit `git clone https://github.com/Q3Rally-Team/q3rally.git` holen
6. In das Engine-Verzeichnis wechseln: `cd q3rally/engine`
7. `make PLATFORM=mingw32` ausführen

Unter WSL kann für Linux mit `make` oder `./make-linux-portable.sh` gebaut werden. macOS-Builds via osxcross werden unter WSL derzeit nicht unterstützt.

### Linux
1. Pakete `git make gcc libsdl2-dev` der Distribution installieren.
2. Q3Rally-Quellcode mit `git clone https://github.com/Q3Rally-Team/q3rally.git` holen.
3. In das Engine-Verzeichnis wechseln: `cd q3rally/engine`
4. `make` ausführen.

Für Releases mit eingebettetem SDL2: `./make-linux-portable.sh x86_64` (auch mit x86 nutzbar).

Mit installiertem mingw-w64 und osxcross lassen sich über `make PLATFORM=mingw32` bzw. `make PLATFORM=darwin` auch Windows- und macOS-Builds von Linux aus erstellen.

### macOS
1. Xcode aus dem App Store installieren (GUI optional, CLI-Tools genügen).
2. Terminal-App öffnen.
3. Q3Rally-Quellcode mit `git clone https://github.com/Q3Rally-Team/q3rally.git` holen.
4. In das Engine-Verzeichnis wechseln: `cd q3rally/engine`
5. `make` ausführen.

AppBundle erstellen:
* Einzelarchitektur: `./make-macosx.sh x86_64` (bei Bedarf x86\_64 durch arm64 (Apple Silicon), x86 oder ppc ersetzen).
* Modernes macOS 10.9+ (Apple Silicon, x86_64): `./make-macosx-ub2.sh`
* Legacy macOS 10.5+ (x86_64, x86, ppc): `./make-macosx-ub.sh`

Für das moderne AppBundle muss das macOS-11.0-SDK oder neuer installiert sein. Das Legacy-AppBundle muss auf macOS 10.6 mit installierten 10.5- und 10.6-SDKs gebaut werden.

## Release-Builds
Sind alle Voraussetzungen erfüllt, lassen sich Releases beispielsweise so erstellen.

`clean` sorgt für einen frischen Build, damit VERSION usw. aus dem Makefile sicher übernommen werden.

`-j#` legt die Anzahl der CPU-Threads fest. Stelle den Wert auf die Kern-/Threadzahl deines Systems für schnellere Builds.

```
# Windows; unter Windows oder Linux mit mingw-w64 ausführen
make PLATFORM=mingw32 ARCH=x86 clean release -j8
make PLATFORM=mingw32 ARCH=x86_64 clean release -j8

# Linux; unter Linux oder Windows (WSL) ausführen
./make-linux-portable.sh ARCH=x86 clean release -j8
./make-linux-portable.sh ARCH=x86_64 clean release -j8

# macOS Modern; unter macOS oder Linux mit osxcross ausführen
make PLATFORM=darwin ARCH=arm64 clean
make PLATFORM=darwin ARCH=x86_64 clean
./make-macosx-ub2.sh

# macOS Legacy; auf macOS 10.6 ausführen
make PLATFORM=darwin ARCH=x86_64 clean
make PLATFORM=darwin ARCH=x86 clean
make PLATFORM=darwin ARCH=ppc clean
./make-macosx-ub.sh
```
