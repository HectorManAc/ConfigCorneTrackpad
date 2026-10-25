VIA_ENABLE          = yes
VIAL_ENABLE         = yes
LTO_ENABLE          = yes

# Luces: RGB matrix (teclas reactivas + LEDs de abajo siempre encendidos)
RGBLIGHT_ENABLE     = no
RGB_MATRIX_ENABLE   = yes
VIALRGB_ENABLE      = yes # elegir color/brillo/velocidad desde Vial

# Ratón + apartado de ajustes en Vial (QMK Settings -> Mouse keys)
MOUSEKEY_ENABLE     = no  # el trackpad mueve el cursor; los clics como teclas siguen (pointing device)
QMK_SETTINGS        = yes

# Pantallas con la H animada
OLED_ENABLE         = yes
OLED_DRIVER         = ssd1306

# Quitado para liberar memoria
EXTRAKEY_ENABLE     = yes # teclas multimedia (volumen, play, etc.)
TAP_DANCE_ENABLE    = no
COMBO_ENABLE        = no
KEY_OVERRIDE_ENABLE = no
CAPS_WORD_ENABLE    = no
LAYER_LOCK_ENABLE   = no
REPEAT_KEY_ENABLE   = no
MAGIC_ENABLE        = no
SPACE_CADET_ENABLE  = no
GRAVE_ESC_ENABLE    = no
CONSOLE_ENABLE      = no
COMMAND_ENABLE      = no
MUSIC_ENABLE        = no
AUDIO_ENABLE        = no
NKRO_ENABLE         = no

AUTO_SHIFT_ENABLE   = no  # requiere el cambio local en builddefs/build_vial.mk

# Trackpad Cirque Pinnacle de 40 mm por I2C en la mitad derecha
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = cirque_pinnacle_i2c
