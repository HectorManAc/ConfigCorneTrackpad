// Keymap de Hector: Vial + ratón, teclas en cyan al presionar y una H animada en las pantallas.
#include QMK_KEYBOARD_H
#include "transactions.h"
#include "lib/lib8tion/lib8tion.h"

#define LIGHTS_VERSION 0x48000003 // cambiarlo fuerza a reaplicar los valores por defecto de las luces

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_split_3x6_3(
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
      KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_ESC,
                                          KC_LGUI, TL_LOWR,  KC_SPC,     KC_ENT, TL_UPPR, KC_RALT
  ),

  // Lower: números, ratón a la izquierda, flechas a la derecha
  [1] = LAYOUT_split_3x6_3(
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
      KC_LCTL, MS_WHLU, MS_BTN2,   MS_UP, MS_BTN1, XXXXXXX,                      KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
      KC_LSFT, MS_WHLD, MS_LEFT, MS_DOWN, MS_RGHT, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
  ),

  [2] = LAYOUT_split_3x6_3(
       KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
  ),

  // Adjust (Lower + Raise): bootloader y brillo/color de las luces
  [3] = LAYOUT_split_3x6_3(
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      RM_TOGG, RM_HUEU, XXXXXXX, RM_VALU, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      XXXXXXX, RM_HUED, XXXXXXX, RM_VALD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
  )
};


#ifndef AUTO_SHIFT_ENABLE
// Auto Shift está quitado para ahorrar memoria, pero QMK Settings de Vial todavía llama a esto
void set_autoshift_timeout(uint16_t timeout) {}
#endif

#ifdef RGB_MATRIX_ENABLE
// Efecto reactivo invertido: las teclas están encendidas y la que se presiona se apaga y vuelve
// a encenderse 6 veces más lento que el efecto normal (~3 s). El efecto activo sigue siendo
// SOLID_REACTIVE_SIMPLE (así Vial controla color, brillo y velocidad de las teclas) y aquí se
// reemplaza su brillo. Los LEDs de abajo (sin tecla) quedan siempre en cyan a tope.
#    define UNDERGLOW_HUE 128 // cyan
#    define FADE_SLOWDOWN 6
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    hsv_t   hsv      = rgb_matrix_get_hsv();
    if (hsv.v > RGB_MATRIX_MAXIMUM_BRIGHTNESS) hsv.v = RGB_MATRIX_MAXIMUM_BRIGHTNESS; // por si la EEPROM trae un brillo mayor
    uint8_t speed    = qadd8(rgb_matrix_get_speed(), 1);
    for (uint8_t i = led_min; i < led_max; i++) {
        hsv_t led = hsv;
        if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_UNDERGLOW)) {
            led.h = UNDERGLOW_HUE;
            led.v = 255;
        } else {
            uint16_t offset = 255; // sin pulsación registrada: encendida
            for (int8_t j = g_last_hit_tracker.count - 1; j >= 0; j--) {
                if (g_last_hit_tracker.index[j] == i) { // la más reciente de esta tecla
                    offset = scale16by8(g_last_hit_tracker.tick[j], speed) / FADE_SLOWDOWN; // 0 al presionar
                    break;
                }
            }
            led.v = scale8(offset > 255 ? 255 : offset, hsv.v);
        }
        rgb_t rgb = hsv_to_rgb(led);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return false;
}
#endif

#ifdef OLED_ENABLE
// Pantallas verticales (32x128).
// - En reposo: una columna de H que sube sin parar. Cada H entra "cifrada" por abajo (cambia muy
//   rápido entre símbolos "tipo kanji" al azar) y se descifra en un momento aleatorio; de vez en
//   cuando una H al azar se vuelve a cifrar un instante, como en las películas.
// - Al escribir: por privacidad no se muestra la tecla; cada pulsación entra abajo como un símbolo
//   "tipo kanji" elegido al azar (la misma tecla sale distinta cada vez) y empuja a los anteriores
//   hacia arriba. Tras 5 s sin teclear vuelve la H. Sin animación mientras se escribe: redibujar
//   la pantalla en cada cuadro hacía que se perdieran teclas.
// - Multimedia y cambio de app (⌘/⌥+Tab) salen como íconos; al mover el trackpad suben flechas en
//   la dirección del cursor como en un juego de baile (estrella al hacer clic con una tecla) hasta
//   1,5 s después de parar.
// La izquierda le manda a la derecha cada símbolo por el TRRS, así ambas muestran lo mismo.

#    define LOG_BOTTOM 105 // y del símbolo más nuevo
#    define LOG_PITCH 24   // separación vertical entre símbolos
#    define REDRAW_MS 60    // como mucho un redibujado cada 60 ms
#    define LOG_LEN 5
#    define LOG_X 6        // la H en negrita mide 19 px; centrada en 32
#    define H_GAP 43       // separación entre H (22 px de alto): 3 visibles a la vez
#    define H_SLOTS 4      // posiciones dibujadas (3 visibles + la que entra/sale)
#    define IDLE_MS 5000    // tiempo sin teclear para volver a la H
#    define MOUSE_MS 1500   // tiempo que siguen las flechas tras soltar el ratón
#    define LANE_LEN 6      // flechas en pantalla (+1 que sale por arriba)

// Bits de mouse_state
#    define M_UP 1
#    define M_DOWN 2
#    define M_LEFT 4
#    define M_RIGHT 8
#    define M_CLICK 16

// Valores de un símbolo (en el registro y en el mensaje a la derecha):
//   < KANJI           índice en glyphs[]
//   KANJI | semilla   símbolo tipo kanji generado a partir de la semilla (6 bits)
//   SYNC_MOUSE | bits solo en el mensaje: estado del ratón
#    define KANJI 0x40
#    define SYNC_MOUSE 0x80

// Glifos 5x7 (5 columnas, bit 0 arriba). Solo la H va en negrita.
static const uint8_t PROGMEM glyphs[][5] = {
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // G_H
    {0x63, 0x14, 0x08, 0x14, 0x63}, // G_X (silencio)
    {0x04, 0x7E, 0x7F, 0x7E, 0x04}, // G_UP
    {0x10, 0x3F, 0x7F, 0x3F, 0x10}, // G_DOWN
    {0x08, 0x1C, 0x3E, 0x1C, 0x1C}, // G_LEFT
    {0x1C, 0x1C, 0x3E, 0x1C, 0x08}, // G_RIGHT
    {0x2A, 0x1C, 0x7F, 0x1C, 0x2A}, // G_STAR (clic)
    {0x7F, 0x3E, 0x1C, 0x08, 0x00}, // G_PLAY
    {0x3E, 0x1C, 0x08, 0x00, 0x3E}, // G_NEXT
    {0x3E, 0x00, 0x08, 0x1C, 0x3E}, // G_PREV
    {0x1C, 0x1C, 0x3E, 0x7F, 0x00}, // G_SPK
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // G_PLUS
    {0x08, 0x08, 0x08, 0x08, 0x08}, // G_MINUS
    {0x3C, 0x24, 0x3F, 0x09, 0x0F}, // G_APP (cambio de app)
};

enum { G_H, G_X, G_UP, G_DOWN, G_LEFT, G_RIGHT, G_STAR, G_PLAY, G_NEXT, G_PREV, G_SPK, G_PLUS, G_MINUS, G_APP };

static uint8_t  log_keys[LOG_LEN]; // símbolo + 1 (0 = vacío), [0] es el más nuevo
static bool     log_dirty = true;
static bool     typing;            // false = mostrando la H
static uint16_t last_key_timer;
static uint8_t  mouse_state;       // teclas de ratón pulsadas ahora (bits M_*)
static bool     mouse_active;
static uint16_t mouse_timer;
static uint8_t  lane[LANE_LEN];    // flechas del modo ratón, glifo + 1 (0 = vacío), [0] es la de abajo
static uint16_t rng;

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270; // la derecha va montada girada, así que usa la misma rotación
}

static uint16_t hash16(uint16_t x) {
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    return x;
}

static uint8_t next_random(void) {
    rng = hash16(rng + timer_read() + 1);
    return rng;
}

static void log_push(uint8_t sym) {
    memmove(&log_keys[1], &log_keys[0], LOG_LEN - 1);
    log_keys[0]    = sym + 1;
    log_dirty      = true;
    typing         = true;
    last_key_timer = timer_read();
    mouse_active   = false;
}

static void mouse_set(uint8_t state) {
    mouse_state  = state;
    mouse_active = true;
    mouse_timer  = timer_read();
}

// La mitad derecha recibe cada símbolo desde la izquierda por el cable TRRS
static void sync_key_slave_handler(uint8_t in_len, const void *in, uint8_t out_len, void *out) {
    uint8_t msg = *(const uint8_t *)in;
    if (msg & SYNC_MOUSE) {
        mouse_set(msg & ~SYNC_MOUSE);
    } else {
        log_push(msg);
    }
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(SYNC_KEY, sync_key_slave_handler);
#    ifdef RGB_MATRIX_ENABLE
    // Solo la primera vez con este firmware: efecto reactivo y color por defecto.
    // Después se respeta lo que se elija y guarde en Vial.
    if (eeconfig_read_user() != LIGHTS_VERSION) {
        eeconfig_update_user(LIGHTS_VERSION);
        eeconfig_update_rgb_matrix_default(); // modo, color y brillo de config.h
    }
#    endif
}

static void log_send(uint8_t sym) {
    log_push(sym);
    transaction_rpc_send(SYNC_KEY, 1, &sym);
}

#    define MOVE_HOLD_MS 150 // tras el último movimiento, cuánto sigue contando la dirección
#    define MOVE_MIN 2       // movimiento mínimo por lectura para contar como dirección

// Envía a la derecha el estado del ratón solo cuando cambia
static void mouse_update(uint8_t state) {
    if (state == mouse_state) return;
    mouse_set(state);
    uint8_t msg = SYNC_MOUSE | state;
    transaction_rpc_send(SYNC_KEY, 1, &msg);
}

// El trackpad (mitad derecha) llega aquí en la izquierda: su dirección alimenta las flechas
report_mouse_t pointing_device_task_user(report_mouse_t m) {
    static uint16_t move_timer;
    static uint8_t  move_dir;
    uint8_t         dir = 0;
    if (m.y <= -MOVE_MIN) dir |= M_UP;
    if (m.y >= MOVE_MIN) dir |= M_DOWN;
    if (m.x <= -MOVE_MIN) dir |= M_LEFT;
    if (m.x >= MOVE_MIN) dir |= M_RIGHT;
    if (dir) {
        move_dir   = dir;
        move_timer = timer_read();
    } else if (timer_elapsed(move_timer) > MOVE_HOLD_MS) {
        move_dir = 0;
    }
    mouse_update((mouse_state & M_CLICK) | move_dir);
    return m;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (IS_MOUSE_KEYCODE(keycode)) {
        uint8_t bit;
        switch (keycode) {
            case MS_UP: case MS_WHLU: bit = M_UP; break;
            case MS_DOWN: case MS_WHLD: bit = M_DOWN; break;
            case MS_LEFT: case MS_WHLL: bit = M_LEFT; break;
            case MS_RGHT: case MS_WHLR: bit = M_RIGHT; break;
            case MS_ACL0 ... MS_ACL2: return true;
            default: bit = M_CLICK; break; // botones
        }
        mouse_update(record->event.pressed ? mouse_state | bit : mouse_state & ~bit);
        return true;
    }
    if (!record->event.pressed) return true;
    switch (keycode) {
        case KC_MPLY: log_send(G_PLAY); break;
        case KC_MNXT: case KC_MFFD: log_send(G_NEXT); break;
        case KC_MPRV: case KC_MRWD: log_send(G_PREV); break;
        case KC_VOLU: log_send(G_SPK); log_send(G_PLUS); break;
        case KC_VOLD: log_send(G_SPK); log_send(G_MINUS); break;
        case KC_MUTE: log_send(G_SPK); log_send(G_X); break;
        default:
            if (keycode == KC_TAB && (get_mods() & (MOD_MASK_GUI | MOD_MASK_ALT))) {
                log_send(G_APP); // ⌘/⌥ + Tab = cambio de app
            } else if (IS_BASIC_KEYCODE(keycode) || IS_QK_MODS(keycode)) {
                log_send(KANJI | (next_random() & 0x3F)); // cualquier otra tecla: símbolo al azar
            }
            break;
    }
    return true;
}

// Símbolo tipo kanji a partir de una semilla: 1-2 trazos horizontales y 1-2 verticales
static void kanji_cols(uint8_t seed, uint8_t *cols) {
    uint16_t h     = hash16(hash16(seed));
    uint8_t  row1  = 1 << (h % 7);                          // horizontal
    uint8_t  row2  = (h & 0x1000) ? 1 << ((h >> 3) % 7) : 0; // segundo horizontal
    uint8_t  vcol  = (h >> 6) % 5;                          // vertical
    uint8_t  vcol2 = (h & 0x2000) ? (h >> 9) % 5 : 0xFF;    // vertical corto
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t bits = row1;
        if (col) bits |= row2;
        if (col == vcol) bits |= 0x7F;
        if (col == vcol2) bits |= (h & 0x4000) ? 0x0F : 0x78;
        if ((h & 0x8000) && !col) bits |= 0x41; // remates
        cols[col] = bits;
    }
}

// Dibuja 5 columnas al triple de tamaño con la esquina en (x0, y0). En negrita cada columna se
// funde con la anterior y los puntos son de 4x4 (19x22); si no, puntos de 3x3 (15x21).
static void draw_cols(const uint8_t *cols, uint8_t x0, int16_t y0, bool bold) {
    uint8_t dot  = bold ? 4 : 3;
    uint8_t prev = 0;
    for (uint8_t col = 0; col < 6; col++) {
        uint8_t cur  = col < 5 ? cols[col] : 0;
        uint8_t bits = bold ? cur | prev : cur;
        prev         = cur;
        for (uint8_t row = 0; row < 7; row++) {
            if (!(bits & (1 << row))) continue;
            for (uint8_t dy = 0; dy < dot; dy++) {
                int16_t y = y0 + row * 3 + dy;
                if (y < 0 || y >= 128) continue;
                for (uint8_t dx = 0; dx < dot; dx++) oled_write_pixel(x0 + col * 3 + dx, y, true);
            }
        }
    }
}

// Símbolo del registro (glifo o kanji) centrado en la columna
static void render_sym(uint8_t sym, int16_t y0) {
    uint8_t cols[5];
    if (sym >= KANJI) {
        kanji_cols(sym, cols);
    } else {
        memcpy_P(cols, glyphs[sym], 5);
    }
    draw_cols(cols, LOG_X + 2, y0, false);
}

static void render_h(int16_t y0) {
    uint8_t cols[5];
    memcpy_P(cols, glyphs[G_H], 5);
    draw_cols(cols, LOG_X, y0, true);
}

bool oled_task_user(void) {
    static uint16_t timer;
    static uint8_t  scroll, turn, h_off;
    static uint8_t  cipher[H_SLOTS]; // cuadros que le quedan cifrada a cada H (0 = H)

    if (mouse_active && !mouse_state && timer_elapsed(mouse_timer) > MOUSE_MS) {
        mouse_active = false;
        log_dirty    = true;
    }
    if (typing && timer_elapsed(last_key_timer) > IDLE_MS) {
        typing = false;
        memset(log_keys, 0, sizeof(log_keys));
        log_dirty = true;
    }
    if (!(log_dirty || mouse_active || !typing) || timer_elapsed(timer) < REDRAW_MS) return false;
    timer = timer_read();

    if (mouse_active) {
        // Las flechas suben 6 px por cuadro; cada 24 px entra una nueva por abajo con la
        // siguiente dirección pulsada (rotando si hay varias) o una estrella si se hace clic
        oled_clear();
        scroll += 6;
        if (scroll >= LOG_PITCH) {
            scroll -= LOG_PITCH;
            memmove(&lane[1], &lane[0], LANE_LEN - 1);
            uint8_t next = 0;
            if (mouse_state & M_CLICK) {
                next = G_STAR + 1;
            } else if (mouse_state) {
                do {
                    turn = (turn + 1) & 3;
                } while (!(mouse_state & (1 << turn)));
                next = G_UP + turn + 1;
            }
            lane[0] = next;
        }
        for (uint8_t i = 0; i < LANE_LEN; i++) {
            if (lane[i]) render_sym(lane[i] - 1, LOG_BOTTOM + LOG_PITCH * (1 - i) - scroll);
        }
        return false;
    }

    log_dirty = false;
    if (!typing) {
        // La columna sube 1 px por cuadro; al salir una H por arriba entra otra cifrada por abajo
        if (++h_off >= H_GAP) {
            h_off = 0;
            memmove(&cipher[0], &cipher[1], H_SLOTS - 1);
            cipher[H_SLOTS - 1] = 10 + (next_random() & 31); // se descifra en 0,6-2,5 s
        }
        uint8_t r = next_random();
        if (r < 6) cipher[r & 3] = 4 + (r & 7); // a veces una H se vuelve a cifrar un instante
        oled_clear();
        for (uint8_t i = 0; i < H_SLOTS; i++) {
            int16_t y = 4 + i * H_GAP - h_off;
            if (cipher[i]) {
                cipher[i]--;
                render_sym(KANJI | (next_random() & 0x3F), y);
            } else {
                render_h(y);
            }
        }
        return false;
    }

    oled_clear();
    for (uint8_t i = 0; i < LOG_LEN; i++) {
        if (log_keys[i]) render_sym(log_keys[i] - 1, LOG_BOTTOM - i * LOG_PITCH);
    }
    return false;
}
#endif
