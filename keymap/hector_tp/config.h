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

/* Luces: teclas encendidas y la que se presiona se apaga y vuelve (keymap.c invierte el efecto
   reactivo); LEDs de abajo siempre a tope en cyan. Teclas en morado por defecto, elegible desde Vial.
   Brillo máximo de las teclas: el de fábrica del Corne (120). Si el teclado se reinicia o se
   desconecta solo por falta de corriente (fusible de ~500 mA del Pro Micro), bajarlo aquí. */
#undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 120
#define RGB_MATRIX_KEYPRESSES
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#define RGB_MATRIX_DEFAULT_HUE 191 // teclas en morado (los LEDs de abajo van en cyan fijo)
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL RGB_MATRIX_MAXIMUM_BRIGHTNESS
#define RGB_MATRIX_SLEEP
#define SPLIT_TRANSPORT_MIRROR

/* Pantallas siempre encendidas; se envían en trozos de 4 bloques (~3 ms) cada 10 ms
   para no bloquear el escaneo de teclas */
#define OLED_TIMEOUT 0
#define OLED_UPDATE_INTERVAL 10
#define OLED_UPDATE_PROCESS_LIMIT 4

#define OLED_FONT_H "keyboards/crkbd/keymaps/hector/tinyfont.c"
#define OLED_FONT_START 32
#define OLED_FONT_END 32

/* La izquierda le envía cada letra a la derecha */
#define SPLIT_TRANSACTION_IDS_USER SYNC_KEY

/* Sin tabla de corrección de brillo (256 bytes) para que quepa todo */
#undef USE_CIE1931_CURVE

/* Con FASTLED_SCALE8_FIXED el efecto reactivo calcula un desvanecimiento >255 para las teclas
   no tocadas, da la vuelta y las deja encendidas; sin él se apagan como deben. */
#undef FASTLED_SCALE8_FIXED
#define FASTLED_SCALE8_FIXED 0

/* Chordal Hold y Flow Tap (que Vial activa con QMK Settings) no se usan: fuera para ahorrar memoria */
#undef CHORDAL_HOLD
#undef FLOW_TAP_TERM

/* Trackpad Cirque de 40 mm por I2C (comparte bus con la OLED) en la mitad derecha */
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_RIGHT
#define CIRQUE_PINNACLE_DIAMETER_MM 40
