# ConfigCorneTrackpad

Firmware Vial personalizado para mi Corne con trackpad: PCB **Corne Chocolate v2.1** (firmware `crkbd/rev1`), dos Pro Micro ATmega32U4 con bootloader Caterina, pantallas OLED, LEDs RGB y un **trackpad redondo Cirque de 40 mm en la mitad derecha**.

Es una variante de mi otro Corne ([ConfigCorneV3](https://github.com/HectorManAc/ConfigCorneV3)): misma pantalla, luces y privacidad, más el trackpad.

> **Estado:** funcionando en el teclado. El trackpad va por I2C en los pines de la pantalla de la mitad derecha (esa mitad no tiene pantalla). Tocar = clic y scroll por la orilla recién añadidos, pendientes de probar.

## Qué hace

- **Ratón**: con el trackpad de la mitad derecha **y** con teclas (mover, clics y rueda; asignables en Vial). En el trackpad: **tocar = clic** y **deslizar por la orilla del círculo = scroll**. Las velocidades de las teclas de ratón son fijas en el firmware.
- **Vial**: 4 capas editables en [vial.rocks](https://vial.rocks) (Chrome/Edge) y luces en *Lighting*. No tiene la pestaña *QMK Settings* (no cabía junto al trackpad).
- **Multimedia**: volumen, play/pausa, siguiente/anterior.
- **Luces**: **arcoíris** por defecto (onda de izquierda a derecha); en Vial → *Lighting* también *Cycle All* (todo cambia junto) y *Solid Color*.
- **Pantallas** (verticales, las dos mitades):
  - En reposo: una columna de H que sube sin parar y se "descifra" al azar (kanji cambiando hasta quedar en H).
  - Al escribir, **por privacidad no se muestra la tecla**: cada pulsación sale como un kanji al azar.
  - Íconos para multimedia y ⌘/⌥+Tab; al mover el ratón (trackpad o teclas) suben flechas en la dirección del cursor.

Para que cupieran el trackpad (~2,8 KB), el ratón por teclas y los gestos se quitaron **QMK Settings** y el efecto de luces reactivo del otro Corne. La inercia al soltar el dedo no cabe.

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

3. Cuando lo pida, presiona **una vez** el reset de esa mitad (con un toque basta; el script espera a que el puerto esté estable, así que dos toques también funcionan).
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
