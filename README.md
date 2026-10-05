# webinar_bp_arm_cmake

Projekt demonstracyjny — budowanie i programowanie STM32F411 (Cortex-M4)
przy użyciu CMake i toolchainu ARM GCC bez żadnego IDE.

---

## Wymagania

| Narzędzie | Minimalna wersja | Strona |
|-----------|-----------------|--------|
| Git | 2.x | https://git-scm.com |
| CMake | 3.20 | https://cmake.org |
| Ninja (zalecane) | dowolna | https://ninja-build.org |
| arm-none-eabi-gcc | 10.x | https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads |
| OpenOCD | 0.12 | https://github.com/openocd-org/openocd/releases |
| ST-Link driver | patrz niżej | https://www.st.com/en/development-tools/stsw-link009.html |

---

## 1. Klonowanie repozytorium z submodułami

Projekt korzysta z dwóch submodułów:

- `common/cmsis` — CMSIS_5 (ARM)
- `common/cmsis-device-f4` — nagłówki urządzeń STM32F4 (STMicroelectronics)

```bash
# Klonowanie z automatycznym pobraniem submodułów
git clone --recurse-submodules <URL_REPO>

# Jeśli repozytorium zostało sklonowane bez submodułów, dobierz je ręcznie:
git submodule update --init --recursive
```

---

## 2. Instalacja toolchainu GCC, OpenOCD i sterownika ST-Link

### Metoda zalecana — STM32CubeIDE

Najprostszym sposobem na uzyskanie wszystkich wymaganych narzędzi naraz jest
instalacja **STM32CubeIDE** firmy STMicroelectronics:

https://www.st.com/en/development-tools/stm32cubeide.html

STM32CubeIDE instaluje w jednym kroku:
- toolchain `arm-none-eabi-gcc`,
- OpenOCD wraz z pełnym zestawem skryptów,
- sterownik ST-Link (STSW-LINK009).

Po instalacji narzędzia znajdą się w podkatalogu `plugins\` wewnątrz katalogu
instalacyjnego STM32CubeIDE, typowo:

```
C:\ST\STM32CubeIDE_<wersja>\STM32CubeIDE\plugins\
  com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.<wersja>\tools\bin\
  com.st.stm32cube.ide.mcu.externaltools.openocd.<wersja>\tools\bin\openocd.exe
  com.st.stm32cube.ide.mcu.externaltools.openocd.<wersja>\tools\scripts\
```

Ścieżki te należy następnie wpisać do `CMakeLists.txt` i zmiennych systemowych
zgodnie z sekcjami 4 i 5 poniżej.

### Metoda alternatywna — instalacja ręczna

Jeśli nie chcesz instalować STM32CubeIDE, każde narzędzie można pobrać osobno.

#### arm-none-eabi-gcc

1. Pobierz archiwum ZIP/instalator dla Windows ze strony ARM:
   https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
   (wybierz wariant `arm-none-eabi` dla systemu Windows x86_64).
2. Wypakuj lub zainstaluj toolchain, np. do:
   ```
   C:\tools\arm-gnu-toolchain\bin
   ```

#### OpenOCD

1. Pobierz gotową paczkę binarną dla Windows (np. z xPack lub oficjalnych releases):
   https://github.com/openocd-org/openocd/releases
2. Wypakuj archiwum, np. do:
   ```
   C:\tools\openocd
   ```
   Po rozpakowaniu powinny być widoczne:
   ```
   C:\tools\openocd\bin\openocd.exe
   C:\tools\openocd\scripts\
   ```

#### Sterownik ST-Link

Przed podłączeniem płytki należy zainstalować sterownik ST-Link, który umożliwia
systemowi Windows wykrycie programatora wbudowanego w płytki Nucleo/Discovery.

1. Pobierz paczkę **STSW-LINK009** ze strony ST:
   https://www.st.com/en/development-tools/stsw-link009.html
   (wymagana rejestracja lub konto ST).
2. Rozpakuj archiwum i uruchom `dpinst_amd64.exe` (system 64-bit).
3. Postępuj zgodnie z kreatorem instalacji.
4. Podłącz płytkę przez USB — w Menedżerze urządzeń powinna pojawić się
   pozycja **STMicroelectronics STLink dongle** (bez żółtego wykrzyknika).

---

## 4. Ustawienie zmiennych środowiskowych systemowych

Otwórz **Zmienne środowiskowe** (Win + S → "Zmienne środowiskowe" → *Zmienne środowiskowe systemowe*).

### 4.1 Dodanie toolchainu i OpenOCD do PATH

W sekcji **Zmienne systemowe** kliknij zmienną `Path` → **Edytuj** → **Nowy**
i dodaj kolejno:

```
C:\tools\arm-gnu-toolchain\bin
C:\tools\openocd\bin
```

Dostosuj ścieżki do swojego katalogu instalacji.

Weryfikacja (nowe okno cmd/PowerShell po zmianach):

```bash
arm-none-eabi-gcc --version
openocd --version
```

### 4.2 Zmienna OPENOCD_SCRIPTS

OpenOCD musi wiedzieć, gdzie leżą jego pliki konfiguracyjne (skrypty).
Dodaj **nową** zmienną systemową:

| Nazwa | Wartość (przykład) |
|-------|--------------------|
| `OPENOCD_SCRIPTS` | `C:\tools\openocd\scripts` |

Ścieżka ta powinna zawierać podkatalogi `interface\`, `target\` itp.

---

## 5. Ustawienie ścieżek w CMakeLists.txt

Na górze pliku `CMakeLists.txt` znajduje się sekcja **USTAWIENIA ZMIENNYCH
ŚRODOWISKOWYCH**. Uzupełnij trzy wartości:

```cmake
# Katalog z kompilatorem arm-none-eabi-gcc
set(TOOLCHAIN_PATH "C:/tools/arm-gnu-toolchain/bin")

# OpenOCD: pełna ścieżka do pliku wykonywalnego
set(OPENOCD         "C:/tools/openocd/bin/openocd.exe")

# Katalog ze skryptami OpenOCD (ten sam co OPENOCD_SCRIPTS)
set(OPENOCD_SCRIPTS "C:/tools/openocd/scripts")
```

> **Uwaga:** W ścieżkach używaj ukośników `/` lub podwójnych `\\` —
> CMake na Windows akceptuje oba formaty.

---

## 6. Konfiguracja i budowanie projektu

```bash
# Wejdź do katalogu projektu
cd webinar_bp_arm_cmake

# Konfiguracja (Debug)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Budowanie
cmake --build build
```

Po poprawnym zbudowaniu w katalogu `build/` pojawią się pliki:
- `webinar_arm.elf` — plik z symbolami debugowania
- `webinar_arm.bin` — obraz binarny do wgrania na mikrokontroler

---

## 7. Programowanie mikrokontrolera

Podłącz płytkę Nucleo/Discovery przez USB, a następnie:

```bash
cmake --build build --target flash
```

Target `flash` wywołuje OpenOCD z konfiguracją z pliku `debug.cfg`
(STM32F411, interfejs ST-Link DAP), wgrywa plik `.elf`, weryfikuje
zapis i resetuje mikrokontroler.
