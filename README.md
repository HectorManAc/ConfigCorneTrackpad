# ConfigCorneTrackpad

Firmware Vial personalizado para mi Corne con trackpad: PCB **Corne Chocolate v2.1** (firmware `crkbd/rev1`), dos Pro Micro ATmega32U4 con bootloader Caterina, pantallas OLED, LEDs RGB y un **trackpad redondo Cirque de 40 mm en la mitad derecha**.

Es una variante de mi otro Corne ([ConfigCorneV3](https://github.com/HectorManAc/ConfigCorneV3)): misma pantalla, luces y privacidad, más el trackpad.

> **Estado:** todavía sin probar en el teclado (no encendía al empezar). La conexión del trackpad (I2C, compartiendo bus con la pantalla) está supuesta y hay que confirmarla.

## Qué hace

- **Trackpad**: mueve el cursor desde la mitad derecha. Los clics se pueden hacer también con teclas (MS_BTN1/2 en Vial).
- **Vial**: 4 capas editables en [vial.rocks](https://vial.rocks) (Chrome/Edge) y *QMK Settings*.
- **Multimedia**: volumen, play/pausa, siguiente/anterior.
- **Luces**: LEDs de abajo siempre en cyan a tope; teclas en morado y la que se presiona se apaga y vuelve a encenderse (~3 s). Color, brillo y velocidad de las teclas en Vial → *Lighting* (no usar "Solid Color").
- **Pantallas** (verticales, las dos mitades):
  - En reposo: una columna de H que sube sin parar y se "descifra" al azar (kanji cambiando hasta quedar en H).
  - Al escribir, **por privacidad no se muestra la tecla**: cada pulsación sale como un kanji al azar.
  - Íconos para multimedia y ⌘/⌥+Tab; al mover el trackpad suben flechas en la dirección del cursor.

Para que cupiera el trackpad (~2,8 KB) se quitaron **las teclas de mover el ratón y la rueda con teclas**.

## Archivos

| Ruta | Qué es |
|---|---|
| `corne_trackpad.hex` | Firmware compilado (flashear en **las dos** mitades) |
| `flash.sh` | Script para flashear una mitad |
| `build.sh` | Compila desde este repo y actualiza `corne_trackpad.hex` |
| `keymap/hector_tp/` | Fuentes del keymap (va en `vial-qmk/keyboards/crkbd/keymaps/hector_tp`) |
| `patches/vial-qmk-local.patch` | 2 cambios locales en vial-qmk necesarios para que todo quepa |
| `CLAUDE.md` | Guía para trabajar en el repo (estado, suposiciones, trampas conocidas) |

## Flashear

Requiere `avrdude` (`brew install avrdude`).

1. Conecta por USB **solo una mitad**, directo a la Mac y con un cable que pase datos. Si conectas o desconectas el TRRS, hazlo siempre con el USB desenchufado.
2. Espera 2–3 s y ejecuta:

   ```sh
   FLASH_TIMEOUT=300 ./flash.sh IZQUIERDA
   ```

3. Cuando lo pida, presiona **dos veces rápido** el reset de esa mitad.
4. Repite con la otra mitad (`DERECHA`).
5. Usa el teclado con el USB en la **mitad izquierda**.

Flashear reinicia el keymap: guarda tu layout en Vial (*File → Save current layout*) y vuelve a cargarlo después.

## Compilar

Con todo instalado (ver abajo), edita `keymap/hector_tp/` y ejecuta `./build.sh`. Copia el keymap a `~/vial-qmk` (otra ruta: `VIAL_QMK=/ruta ./build.sh`), aplica el parche si falta, compila y deja el resultado en `corne_trackpad.hex`.

## Instalar todo desde cero

```sh
# Compilador AVR (versión 8: genera el firmware más pequeño)
brew tap osx-cross/avr && brew trust osx-cross/avr
brew install osx-cross/avr/avr-gcc@8

# vial-qmk (probado en el commit dd43959ae5c08d8a28d38a1acf7b04e86b14a344)
git clone https://github.com/vial-kb/vial-qmk.git ~/vial-qmk
cd ~/vial-qmk
git checkout dd43959ae5c08d8a28d38a1acf7b04e86b14a344
git submodule update --init --depth 1 lib/lufa lib/printf
python3 -m venv .venv && .venv/bin/pip install qmk -r requirements.txt

# Compilar (aplica el parche y copia el keymap automáticamente)
cd /ruta/a/ConfigCorneTrackpad && ./build.sh
```

El parche hace opcionales Auto Shift (`builddefs/build_vial.mk`) y Chordal Hold / Flow Tap (`quantum/qmk_settings.c`), que Vial activa con *QMK Settings* y no caben en el ATmega32U4.
