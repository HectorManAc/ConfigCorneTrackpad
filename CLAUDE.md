# ConfigCorneTrackpad — guía para trabajar en este repo

Firmware Vial para el **segundo** Corne de Hector: PCB **Corne Chocolate v2.1** (firmware `crkbd/rev1`, keymap `hector_tp`), dos Pro Micro ATmega32U4 con Caterina, OLED y LEDs RGB, y un **trackpad redondo Cirque de 40 mm en la mitad derecha**. Parte de la configuración de su otro teclado (repo `HectorManAc/ConfigCorneV3`, carpeta `~/Documents/CorneV3LCD`); si algo de pantallas/luces no está explicado aquí, ese repo tiene la historia. Hector escribe en español y prueba cada cambio en el teclado real.

## Estado (2026-10-25)

- Funciona en el hardware: teclas, arcoíris en ambas mitades, OLED izquierda y trackpad. **La mitad derecha no tiene OLED**: el Cirque (40 mm, cubierta **curva**) va por I2C en los pines de la OLED (dirección 0x2A, confirmado con el diagnóstico).
- Tocar = clic y scroll por la orilla (`CIRQUE_PINNACLE_TAP_ENABLE`, `POINTING_DEVICE_GESTURES_SCROLL_ENABLE`) recién añadidos: **pendientes de probar**.
- Hector quiere que el trackpad se sienta como el de su laptop; aún nota micro brincos leves. Siguientes pasos posibles: no animar las flechas de la OLED con el trackpad, o modo relativo del Cirque. La inercia (`CURSOR_GLIDE`, ~1,1 KB) no cabe.

## Flujo de trabajo

1. **Editar** solo en `keymap/hector_tp/` (este repo es la fuente; `~/vial-qmk/keyboards/crkbd/keymaps/hector_tp` se sobrescribe al compilar). El keymap `hector` del otro teclado es independiente.
2. **Compilar**: `./build.sh` → copia `keymap/hector_tp` a `~/vial-qmk`, aplica `patches/vial-qmk-local.patch` si falta, compila y deja `corne_trackpad.hex` aquí. Revisa la línea de tamaño: el límite es **28 672 bytes**.
3. **Flashear** cada mitad (ver abajo), primero la izquierda y luego la derecha: ambas mitades llevan el mismo `.hex`.
4. **Commit y push** cuando Hector confirme que funciona. Actualiza el README si cambia lo que hace el teclado.

## Flashear (Caterina, una mitad a la vez)

- Hector conecta el USB en una mitad (el TRRS puede quedarse conectado; nunca se conecta/desconecta el TRRS con USB enchufado).
- Espera 2–3 s y comprueba que la placa está en modo normal: `ioreg -p IOUSB -l -w0 | grep '"USB Product Name" = "Corne"'`. Si aparece "Arduino Leonardo" está en bootloader: espera a que salga, porque `flash.sh` ignora los puertos que ya existían al arrancar.
- Lanza en segundo plano **antes** de que Hector presione reset:
  `FLASH_TIMEOUT=300 ./flash.sh IZQUIERDA > log 2>&1 &` (por defecto flashea `corne_trackpad.hex`)
- Hector presiona el reset de esa mitad **una vez**: en estos Pro Micro un toque ya entra al bootloader y el segundo lo reiniciaba a mitad de la grabación ("Device not configured"). `flash.sh` ahora exige que el puerto siga 1,5 s antes de grabar. Confirma con el log (`bytes of flash verified`).
- Si una grabación se corta, el firmware queda a medias pero el bootloader no se toca: la placa se queda en bootloader ("Arduino Leonardo", puerto fijo) y se graba directo con `avrdude -p atmega32u4 -c avr109 -P <puerto> -U flash:w:corne_trackpad.hex:i`, sin reset.
- Flashear reinicia el keymap de Vial. Este teclado aún no tiene `.vil` guardado; el de su otro Corne (`ConfigCorneV3/configcornev3.vil`) usa teclas de mover el ratón que aquí no existen. Cuando Hector guarde uno, súbelo (revisa antes que `"macro"` no tenga texto sensible).

Diagnóstico si no aparece el bootloader: `ioreg -p IOUSB` para ver si la Mac ve la placa. En la primera sesión el problema era un cable solo de carga y había otro teclado Vial ("BCORNE") conectado que confundía las pruebas.

## Requisitos (ya instalados en esta Mac)

- `avrdude` (Homebrew).
- `avr-gcc@8` de `osx-cross/avr` (tap marcado como confiable); la versión 8 da el firmware más pequeño.
- `~/vial-qmk` en el commit `dd43959ae5c08d8a28d38a1acf7b04e86b14a344`, con `.venv` (qmk + requirements). Si se vuelve a clonar, seguir el README.

## Memoria: el límite manda

El ATmega32U4 deja 28 672 bytes (4 KB son del bootloader) y ~2,5 KB de RAM (`avr-size --mcu=atmega32u4 -C .build/crkbd_rev1_hector_tp.elf`). El trackpad ocupa ~2,8 KB. Ya se quitó: RGB animations, macros, tap dance, combos, key overrides, caps word, NKRO, Auto Shift, Chordal Hold, Flow Tap, tabla CIE1931, la fuente de texto del OLED y **QMK Settings** (Hector quiere ratón con teclas *y* trackpad; antes probó quitando las teclas de ratón). Por eso las velocidades de las teclas de ratón son fijas (`MOUSEKEY_*` en `config.h` si hay que ajustarlas) y el parche/stub de Auto Shift ya no influyen. Quitar VialRGB solo libera ~440 bytes; Hector quiere que todo siga funcionando con vial.rocks. Si algo no cabe, mide con `avr-nm --size-sort -S` antes de recortar y pregunta a Hector qué sacrificar.

## Trampas conocidas (no deshacer sin motivo)

- **`FASTLED_SCALE8_FIXED 0`** en `config.h`: con 1, `SOLID_REACTIVE_SIMPLE` calcula un desvanecimiento >255, da la vuelta y deja las teclas no tocadas encendidas.
- **Luces de este teclado**: arcoíris `RGB_MATRIX_CYCLE_LEFT_RIGHT` por defecto + `CYCLE_ALL`. El efecto reactivo invertido del otro Corne (con `RGB_MATRIX_KEYPRESSES`/`SPLIT_TRANSPORT_MIRROR`) se quitó para que cupieran los gestos (~1 KB); las notas de `FASTLED_SCALE8_FIXED` y "luces invertidas" de abajo son del otro Corne.
- **Luces invertidas**: con el efecto `SOLID_REACTIVE_SIMPLE` (para que VialRGB controle color/brillo/velocidad) y `rgb_matrix_indicators_advanced_user` reescribe todos los LEDs: teclas con el brillo invertido y `FADE_SLOWDOWN` más lento; LEDs de abajo (flag UNDERGLOW) en cyan fijo a 255. No uses un efecto custom: Vial no lo reconoce y al cambiar color lo apagaría.
- **`LIGHTS_VERSION`** en `keymap.c`: cambiarlo reaplica una vez los valores por defecto de luces (borra el color elegido en Vial).
- **OLED**: en split, QMK envía 1 bloque cada 50 ms por defecto; redibujar la pantalla completa cada cuadro deja la parte de abajo sin refrescar. Se usa `OLED_UPDATE_PROCESS_LIMIT 4` + `OLED_UPDATE_INTERVAL 10` y como mucho un redibujado cada 60 ms. **No animes mientras se escribe**: enviar la pantalla completa en cada cuadro hacía que se perdieran teclas. Los cambios de la H y las flechas del ratón solo animan en reposo o en modo ratón.
- **Rotación**: ambas mitades `OLED_ROTATION_270` (la derecha va montada girada).
- **Sincronización**: la mitad derecha no conoce los keycodes; la izquierda le envía cada glifo por RPC (`SPLIT_TRANSACTION_IDS_USER SYNC_KEY`); `0x80 | estado` indica estado del ratón.
- **Trackpad entre mitades (sin `SPLIT_POINTING_ENABLE`)**: el mecanismo de QMK sobrescribe el movimiento en un buzón que la izquierda lee a su ritmo y se perdían trozos del trazo. Ahora el driver corre en ambas mitades (en la izquierda no encuentra sensor y se desactiva solo); en la derecha `pointing_device_task_user` **acumula** x/y/scroll/botones en `tp_acc` y la izquierda lo recoge y vacía por RPC (`TRACKPAD_KEY`) cada `POINTING_DEVICE_TASK_THROTTLE_MS` (4 ms; con 10 ms desacompasados había brincos); el sobrante que no cabe en un reporte pasa al siguiente.
- **Scroll por la orilla** (modo absoluto, circular scroll): `cirque_pinnacle_configure_circular_scroll(30, 16, 9102, 18, false)`; solo vertical (en la derecha se acumula `v - h`). Hector probó y descartó: orilla 45 %/8 px/35°/24 (le quitaba libertad al cursor) y el modo relativo con la franja de scroll del chip (commit `5f1d8ef`; no le gustó). Si aún no le convence cómo entrar/salir, las ideas pendientes eran: aviso en la OLED al entrar en scroll, o una tecla/capa que convierta el trackpad en scroll.
- **Layout por defecto** = `ConfigCorneV3/configcornev3.vil` en orden de matriz (keycodes renombrados a los actuales de QMK: KC_MS_U→MS_UP, KC_BSPACE→KC_BSPC, etc.).
- **Cubierta curva**: `CIRQUE_PINNACLE_CURVED_OVERLAY` (sensibilidad 2X + bordes + calibración). Con 4X soltaba el dedo a mitad del trazo. No tocar el trackpad 2–3 s al conectar (calibra).
- **Diagnóstico** (`TRACKPAD_DIAG`, desactivado): la OLED izquierda muestra ★/+/X/− y un cuadrito por dispositivo I2C de la derecha. El escaneo va en `housekeeping_task_user`, nunca dentro de un handler RPC (corre en interrupción: los timeouts de I2C no avanzan y colgó la derecha).
- **OLED en modo ratón**: redibuja cada 120 ms (no 60): cada envío a la OLED retrasa los reportes del cursor.
- **Ratón → flechas en pantalla**: las teclas de ratón y el trackpad usan `mouse_update()`. `pointing_device_task_user` (corre en la izquierda con los datos de la derecha) convierte el movimiento en bits de dirección (`MOVE_MIN`, retenidos `MOVE_HOLD_MS`) y `mouse_update()` los manda a la derecha solo cuando cambian.
- **Privacidad**: nunca mostrar en pantalla la tecla pulsada (Hector escribe contraseñas en público). Cada tecla se muestra como un kanji generado desde una semilla **aleatoria por pulsación** (`KANJI | semilla`), no derivada del keycode. Solo multimedia, ⌘/⌥+Tab y el ratón usan íconos fijos.
- **Glifos**: `glyphs[]` en `keymap.c` son 5x7 (bit 0 arriba): la H (única en negrita, puntos 4x4) y los íconos (sin negrita, puntos 3x3; en negrita no se distinguían). Los kanji se generan con trazos en `kanji_cols()`, sin tabla. Para diseñar un glifo nuevo, previsualízalo en ASCII antes de compilar.
- **Reposo**: columna infinita de H que sube 1 px por cuadro (3 visibles, `H_GAP` 43); cada H entra cifrada (kanji al azar nuevos cada cuadro) y se descifra en un momento aleatorio; a veces una se re-cifra un instante. Anima siempre en reposo (no mientras se escribe). Hector descartó antes un glitch con temblores/franjas, un cambio lento de kanji sueltos y 3 H fijas.
- **Consumo**: brillo máximo de teclas 120 (el de fábrica del Corne). El Pro Micro suele tener un fusible de ~500 mA; si el teclado se reinicia o se desconecta solo, bajar `RGB_MATRIX_MAXIMUM_BRIGHTNESS`.
- **Parche de vial-qmk** (`patches/vial-qmk-local.patch`): hace opcional Auto Shift (`build_vial.mk`) y envuelve `get_chordal_hold`/`get_flow_tap_term` en `#ifdef` (`qmk_settings.c`). `keymap.c` tiene un `set_autoshift_timeout` vacío por eso. En Vial aún aparecen esos apartados, pero no hacen nada.
- **Master**: `MASTER_LEFT`; el keymap de Vial vive en la EEPROM de la mitad con USB, así que siempre se usa con el USB en la izquierda. El trackpad está en la derecha (esclava): sus lecturas llegan por el TRRS.
