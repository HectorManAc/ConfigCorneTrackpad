// Keymap de Hector para el Corne Chocolate con trackpad Cirque: Vial, ratón por teclas y trackpad, arcoíris y OLED con privacidad.
#include QMK_KEYBOARD_H
#include "transactions.h"
#include "lib/lib8tion/lib8tion.h"
#include "atomic_util.h"

#define LIGHTS_VERSION 0x54500002 // cambiarlo fuerza a reaplicar los valores por defecto de las luces

// Layout de Hector (de ConfigCorneV3/configcornev3.vil), en orden de la matriz: filas 0-3 mitad
// izquierda, 4-7 derecha (columna 0 = exterior). Capa 1 = ratón y rueda, 2 = números/multimedia/
// flechas, 3 = QK_BOOT y luces. Así cada flasheo arranca ya con estas teclas.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = {
        {  KC_TAB,     KC_Q,     KC_W,     KC_E,     KC_R,     KC_T},
        { KC_LSFT,     KC_A,     KC_S,     KC_D,     KC_F,     KC_G},
        { KC_LCTL,     KC_Z,     KC_X,     KC_C,     KC_V,     KC_B},
        {   KC_NO,    KC_NO,    KC_NO,  KC_LGUI,  KC_LALT,   KC_SPC},
        { KC_BSPC,     KC_P,     KC_O,     KC_I,     KC_U,     KC_Y},
        { KC_RSFT,  KC_SCLN,     KC_L,     KC_K,     KC_J,     KC_H},
        {  KC_ESC, KC_SLASH,   KC_DOT, KC_COMMA,     KC_M,     KC_N},
        {   KC_NO,    KC_NO,    KC_NO,    MO(2),    MO(1),   KC_ENT}
    },
    [1] = {
        {  KC_ESC,  KC_TRNS,  KC_TRNS,    MS_UP,  KC_TRNS,  KC_TRNS},
        { KC_TRNS,  KC_TRNS,  MS_LEFT,  MS_DOWN,  MS_RGHT,  KC_TRNS},
        { KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS},
        {   KC_NO,    KC_NO,    KC_NO,  KC_TRNS,  KC_TRNS,  KC_TRNS},
        { KC_TRNS,  KC_TRNS,  KC_TRNS,  MS_BTN2,  KC_TRNS,  KC_TRNS},
        { KC_TRNS,  KC_TRNS,  MS_ACL1,  MS_ACL2,  MS_BTN1,  MS_BTN3},
        { KC_TRNS,  KC_TRNS,  MS_WHLR,  MS_WHLU,  MS_WHLD,  MS_WHLL},
        {   KC_NO,    KC_NO,    KC_NO,  KC_TRNS,  KC_TRNS,  KC_TRNS}
    },
    [2] = {
        {  KC_ESC,     KC_1,     KC_2,     KC_3,     KC_4,     KC_5},
        { KC_TRNS,  KC_TRNS,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_VOLU},
        { KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_VOLD},
        {   KC_NO,    KC_NO,    KC_NO,  KC_TRNS,  KC_TRNS,  KC_TRNS},
        { KC_TRNS,     KC_0,     KC_9,     KC_8,     KC_7,     KC_6},
        { KC_TRNS,  KC_TRNS, KC_RIGHT,    KC_UP,  KC_DOWN,  KC_LEFT},
        { KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS},
        {   KC_NO,    KC_NO,    KC_NO,  KC_TRNS,  KC_TRNS,  KC_TRNS}
    },
    [3] = {
        { QK_BOOT,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO},
        { RM_TOGG,  RM_HUEU,    KC_NO,  RM_VALU,    KC_NO,    KC_NO},
        {   KC_NO,  RM_HUED,    KC_NO,  RM_VALD,    KC_NO,    KC_NO},
        {   KC_NO,    KC_NO,    KC_NO,  KC_LGUI,  KC_TRNS,   KC_SPC},
        {   KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO},
        {   KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO},
        {   KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO},
        {   KC_NO,    KC_NO,    KC_NO,  KC_RALT,  KC_TRNS,   KC_ENT}
    }
};


#ifndef AUTO_SHIFT_ENABLE
// Auto Shift está quitado para ahorrar memoria, pero QMK Settings de Vial todavía llama a esto
void set_autoshift_timeout(uint16_t timeout) {}
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
#    define MOUSE_REDRAW_MS 120 // en modo ratón, menos: cada envío a la OLED retrasa los reportes del cursor
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

static void trackpad_slave_handler(uint8_t in_len, const void *in, uint8_t out_len, void *out);
#    ifdef TRACKPAD_DIAG
static void trackpad_diag_slave(uint8_t in_len, const void *in, uint8_t out_len, void *out);
#    endif

void keyboard_post_init_user(void) {
    transaction_register_rpc(SYNC_KEY, sync_key_slave_handler);
    transaction_register_rpc(TRACKPAD_KEY, trackpad_slave_handler);
#    ifdef TRACKPAD_DIAG
    transaction_register_rpc(TRACKPAD_DIAG_KEY, trackpad_diag_slave);
#    endif
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

#    define MOUSE_SYNC_MS 100 // como mucho un aviso del ratón a la derecha cada 100 ms

static bool     mouse_sync_pending;
static uint16_t mouse_sync_timer;

// La izquierda actualiza sus flechas al momento; a la derecha le avisa en mouse_sync_task(), sin
// saturar el TRRS (el trackpad manda sus datos por ahí mismo; avisar en cada cambio daba tirones)
static void mouse_update(uint8_t state) {
    if (state == mouse_state) return;
    mouse_set(state);
    mouse_sync_pending = true;
}

static void mouse_sync_task(void) {
    if (!mouse_sync_pending || timer_elapsed(mouse_sync_timer) < MOUSE_SYNC_MS) return;
    mouse_sync_pending = false;
    mouse_sync_timer   = timer_read();
    uint8_t msg        = SYNC_MOUSE | mouse_state;
    transaction_rpc_send(SYNC_KEY, 1, &msg);
}

// Trackpad sin pérdidas entre mitades: la derecha suma el movimiento de cada lectura en tp_acc y la
// izquierda lo recoge (y vacía) por RPC en cada ciclo del pointing device.
typedef struct {
    int16_t x, y;
    int8_t  v, h;    // scroll (franja del borde derecho, solo vertical: h siempre 0)
    uint8_t buttons; // botones pulsados desde la última recogida (un toque corto no se pierde)
} tp_acc_t;

static tp_acc_t tp_acc;
static uint8_t  tp_buttons_now; // estado actual de los botones del trackpad (derecha)

// Derecha, dentro de la comunicación con la izquierda: entrega lo acumulado y lo vacía
static void trackpad_slave_handler(uint8_t in_len, const void *in, uint8_t out_len, void *out) {
    memcpy(out, &tp_acc, sizeof(tp_acc));
    tp_acc.x       = 0;
    tp_acc.y       = 0;
    tp_acc.v       = 0;
    tp_acc.h       = 0;
    tp_acc.buttons = tp_buttons_now;
}

static int8_t add_i8(int8_t a, int8_t b) {
    int16_t r = a + b;
    return r > INT8_MAX ? INT8_MAX : r < INT8_MIN ? INT8_MIN : r;
}

static mouse_xy_report_t take_xy(int16_t *pending) {
    int16_t v = *pending;
    if (v > MOUSE_REPORT_XY_MAX) v = MOUSE_REPORT_XY_MAX;
    if (v < MOUSE_REPORT_XY_MIN) v = MOUSE_REPORT_XY_MIN;
    *pending -= v; // lo que no cabe en este reporte sale en el siguiente
    return v;
}

report_mouse_t pointing_device_task_user(report_mouse_t m) {
    if (!is_keyboard_master()) {
        // Derecha: acumula y no envía nada (no tiene USB)
        ATOMIC_BLOCK_FORCEON {
            tp_acc.x += m.x;
            tp_acc.y += m.y;
            tp_acc.v = add_i8(tp_acc.v, m.v); // franja de scroll del chip (solo vertical)
            tp_acc.buttons |= m.buttons;
            tp_buttons_now = m.buttons;
        }
        return (report_mouse_t){0};
    }

    // Izquierda: recoge lo acumulado en la derecha
    static int16_t  pend_x, pend_y;
    static uint8_t  tp_prev_buttons;
    tp_acc_t        got = {0};
    if (transaction_rpc_exec(TRACKPAD_KEY, 0, NULL, sizeof(got), &got)) {
        pend_x += got.x;
        pend_y += got.y;
    }
    m.x       = take_xy(&pend_x);
    m.y       = take_xy(&pend_y);
    m.v       = got.v;
    m.h       = got.h;
    m.buttons = (m.buttons & ~tp_prev_buttons) | got.buttons; // los de las teclas de ratón se respetan
    tp_prev_buttons = got.buttons;

    // La dirección del movimiento alimenta las flechas de la pantalla
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

#    ifdef TRACKPAD_DIAG
// Diagnóstico temporal: la derecha (sin pantalla) revisa su bus I2C y se lo cuenta a la izquierda,
// que lo muestra en vez de la animación: símbolo arriba (★ trackpad iniciado, + algo responde en la
// dirección del Cirque pero no se inició, X nada responde, − no hay respuesta por el TRRS) y un
// cuadrito por cada dispositivo I2C que vea la derecha.
extern bool    touchpad_init;
static uint8_t diag_result[3]; // [dispositivos I2C, responde en la dirección del Cirque, driver iniciado]

// La derecha escanea en su bucle normal (no dentro de la comunicación con la izquierda, que corre
// en una interrupción donde los tiempos de espera de I2C no avanzan y un bus atascado la colgaría)
static void trackpad_diag_task(void) {
    static uint16_t scan_timer;
    if (is_keyboard_master() || timer_elapsed(scan_timer) < 1000) return;
    scan_timer     = timer_read();
    uint8_t n      = 0;
    for (uint8_t a = 0x08; a < 0x78; a++) {
        if (i2c_ping_address(a << 1, 5) == I2C_STATUS_SUCCESS) n++;
    }
    diag_result[0] = n;
    diag_result[1] = i2c_ping_address(CIRQUE_PINNACLE_ADDR << 1, 5) == I2C_STATUS_SUCCESS;
    diag_result[2] = touchpad_init;
}

static void trackpad_diag_slave(uint8_t in_len, const void *in, uint8_t out_len, void *out) {
    memcpy(out, diag_result, sizeof(diag_result));
}

static void render_trackpad_diag(void) {
    uint8_t r[3] = {0};
    bool    ok   = transaction_rpc_exec(TRACKPAD_DIAG_KEY, 0, NULL, sizeof(r), r);
    oled_clear();
    render_sym(!ok ? G_MINUS : r[2] ? G_STAR : r[1] ? G_PLUS : G_X, 6);
    for (uint8_t i = 0; ok && i < r[0] && i < 6; i++) {
        for (uint8_t y = 0; y < 8; y++) {
            for (uint8_t x = 0; x < 10; x++) oled_write_pixel(11 + x, 40 + i * 14 + y, true);
        }
    }
}
#    endif

void housekeeping_task_user(void) {
    if (is_keyboard_master()) mouse_sync_task();
#    ifdef TRACKPAD_DIAG
    trackpad_diag_task();
#    endif
}

bool oled_task_user(void) {
#    ifdef TRACKPAD_DIAG
    static uint16_t diag_timer;
    static bool     started;
    if (!started) { // señal inmediata: si la pantalla sigue en blanco, la izquierda no llega aquí
        started = true;
        oled_clear();
        render_sym(G_MINUS, 6);
    }
    if (is_keyboard_master() && timer_elapsed(diag_timer) > 1000) {
        diag_timer = timer_read();
        render_trackpad_diag();
    }
    return false;
#    endif
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
    if (!(log_dirty || mouse_active || !typing) || timer_elapsed(timer) < (mouse_active ? MOUSE_REDRAW_MS : REDRAW_MS)) return false;
    timer = timer_read();

    if (mouse_active) {
        // Las flechas suben 12 px por cuadro (cada 120 ms); cada 24 px entra una nueva por abajo con la
        // siguiente dirección pulsada (rotando si hay varias) o una estrella si se hace clic
        oled_clear();
        scroll += 12;
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
