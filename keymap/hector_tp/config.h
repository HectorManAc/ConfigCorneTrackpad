#pragma once

#define VIAL_KEYBOARD_UID {0x3B, 0x6B, 0xA0, 0x29, 0x80, 0x56, 0xED, 0xD1}
#define VIAL_UNLOCK_COMBO_ROWS {0, 0}
#define VIAL_UNLOCK_COMBO_COLS {0, 1}

#undef DYNAMIC_KEYMAP_LAYER_COUNT
#define DYNAMIC_KEYMAP_LAYER_COUNT 4
#define TAPPING_TERM 180

#define MASTER_LEFT
#define USE_SERIAL_PD2

/* Menos memoria */
#define NO_ACTION_ONESHOT
#define LAYER_STATE_8BIT
#define DYNAMIC_KEYMAP_MACRO_COUNT 0

/* Luces: arcoíris por defecto (onda de izquierda a derecha); en Vial → Lighting también "Cycle All",
   "Cycle Up/Down", "Rainbow Moving Chevron" y "Hue Wave".
   El efecto reactivo invertido del otro Corne se quitó para que cupieran los gestos del trackpad.
   Brillo máximo: el de fábrica del Corne (120). Si el teclado se reinicia o se desconecta solo
   por falta de corriente (fusible de ~500 mA del Pro Micro), bajarlo aquí. */
#undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 120
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#define ENABLE_RGB_MATRIX_HUE_WAVE
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CYCLE_LEFT_RIGHT
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL RGB_MATRIX_MAXIMUM_BRIGHTNESS
#define RGB_MATRIX_SLEEP

/* Pantallas siempre encendidas; se envían en trozos de 4 bloques (~3 ms) cada 10 ms
   para no bloquear el escaneo de teclas */
#define OLED_TIMEOUT 0
#define OLED_UPDATE_INTERVAL 10
#define OLED_UPDATE_PROCESS_LIMIT 4

#define OLED_FONT_H "keyboards/crkbd/keymaps/hector/tinyfont.c"
#define OLED_FONT_START 32
#define OLED_FONT_END 32

/* La izquierda le envía cada letra a la derecha */
#define SPLIT_TRANSACTION_IDS_USER SYNC_KEY, TRACKPAD_KEY, TRACKPAD_DIAG_KEY

/* Sin tabla de corrección de brillo (256 bytes) para que quepa todo */
#undef USE_CIE1931_CURVE

/* Con FASTLED_SCALE8_FIXED el efecto reactivo calcula un desvanecimiento >255 para las teclas
   no tocadas, da la vuelta y las deja encendidas; sin él se apagan como deben. */
#undef FASTLED_SCALE8_FIXED
#define FASTLED_SCALE8_FIXED 0

/* Chordal Hold y Flow Tap (que Vial activa con QMK Settings) no se usan: fuera para ahorrar memoria */
#undef CHORDAL_HOLD
#undef FLOW_TAP_TERM

/* Trackpad Cirque de 40 mm por I2C en la mitad derecha (va en los pines de la OLED; esa mitad no
   tiene pantalla). No se usa SPLIT_POINTING_ENABLE: QMK sobrescribe el movimiento en un buzón que la
   izquierda lee a su ritmo y se perdían trozos del trazo. En su lugar keymap.c acumula en la derecha
   y la izquierda lo recoge por RPC (TRACKPAD_KEY). El driver corre en las dos mitades; en la
   izquierda no encuentra el sensor y se desactiva solo. */
#define CIRQUE_PINNACLE_DIAMETER_MM 40
/* El trackpad tiene cubierta curva (hundida como un platito). Sin esto el driver usa la sensibilidad
   más baja (4X) y "soltaba" el dedo a mitad del trazo; con esto usa 2X, ajusta la sensibilidad de
   los bordes y recalibra al arrancar. Si aún suelta el dedo, probar además
   #define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_1X (la máxima). */
#define CIRQUE_PINNACLE_CURVED_OVERLAY
/* La derecha lee el trackpad y la izquierda recoge lo acumulado cada 4 ms (por defecto 10 ms en las
   dos, desacompasadas: a veces una recogida venía vacía y la siguiente con dos lecturas = brinco). */
#define POINTING_DEVICE_TASK_THROTTLE_MS 4
/* Modo absoluto (el de fábrica): tocar = clic y scroll girando por la orilla del círculo (~240 y
   ~930 bytes). Hector probó el modo relativo con la franja de scroll del chip y prefirió este.
   La inercia al soltar (POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE, ~1,1 KB) no cabía. */
#define CIRQUE_PINNACLE_TAP_ENABLE
#define POINTING_DEVICE_GESTURES_SCROLL_ENABLE

/* Diagnóstico del trackpad en la pantalla izquierda (★/+/X/− y un cuadrito por dispositivo I2C de la
   derecha). Desactivado; descomentar solo para depurar: hace que el cursor dé tirones cada segundo. */
// #define TRACKPAD_DIAG
