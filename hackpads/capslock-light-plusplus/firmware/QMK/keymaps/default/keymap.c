// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _CONFIG
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     *
     * The wiring is VERY convoluted!
     *
     * [6][7][8]
     * [3][4][5]
     * [0][1][2]
     *
     * Maps to a physical layout of:
     *
     * [OLED]   [6]
     * [0][2][4][7]
     * [1][3][5][8]
     *
     * 6 is a EC11 encoder
     */

    [_BASE] = LAYOUT(
        KC_AUDIO_MUTE, KC_F19, TG(_CONFIG),
        KC_F16, KC_F17, KC_F18,
        KC_F13, KC_F14, KC_F15
    ),

    [_CONFIG] = LAYOUT(
        QK_BOOT, UG_TOGG, TG(_CONFIG),
        UG_SATD, UG_VALU, UG_VALD,
        UG_HUEU, UG_HUED, UG_SATU 
    )
};

const uint16_t PROGMEM encoder_map[][1][2] = {
    [0] = {ENCODER_CCW_CW(KC_AUDIO_VOL_UP, KC_AUDIO_VOL_DOWN)},
    [1] = {ENCODER_CCW_CW(QK_REP, QK_AREP)}
};

// /* Code for non-existant RGB lighting
const rgblight_segment_t PROGMEM capslock_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 1, HSV_RED}
);

const rgblight_segment_t PROGMEM background_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 1, HSV_PURPLE}
);

const rgblight_segment_t* const PROGMEM the_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    capslock_layer,
    background_layer
);

bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(0, led_state.caps_lock);
    rgblight_set();
    return true;
}
// */

// map alternate repeat keys
uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
    switch (keycode) {
        // rgblight adjustments
        case UG_HUEU: return UG_HUED;
        case UG_SATU: return UG_SATD;  
        case UG_VALU: return UG_VALD;  
        case UG_HUED: return UG_HUEU;  
        case UG_SATD: return UG_SATU;  
        case UG_VALD: return UG_VALU;  
    }

    return KC_TRNS;
}

void keyboard_post_init_user(void) {
    // Enable the LED layers
    gpio_set_pin_output(11);
    gpio_write_pin_high(11);
    rgblight_layers = the_rgb_layers;
}

#ifdef OLED_ENABLE
layer_state_t layer_state_set_user(layer_state_t state) {
    oled_clear();
    oled_render_dirty(true);
    return state;
}
bool oled_task_user(void) {
    led_t led_state = host_keyboard_led_state();
    // Host Keyboard Layer Status. Also swap for icon?
    oled_set_cursor(12, 0);
    oled_write_P(PSTR("Layer: "), false);
    oled_set_cursor(13, 1);
    
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("FN Keys"), false);
/* Theory for base layer TODO: Images
 * ███ ███ ███ Layer: 
 * █A█ █1█ █S█  FN Keys
 * ███ ███ ███
 * 
 */
            oled_set_cursor(0, 0); // full size 3x3 boxes for the lock symbols
                oled_write_P(PSTR("   "), led_state.caps_lock ? true : false);
                oled_set_cursor(0, 1);
                oled_write_P(PSTR(" A "), led_state.caps_lock ? true : false);
                oled_set_cursor(0, 2);
                oled_write_P(PSTR("   "), led_state.caps_lock ? true : false);
            oled_set_cursor(4, 0);
                oled_write_P(PSTR("   "), led_state.num_lock ? true : false);
                oled_set_cursor(4, 1);
                oled_write_P(PSTR(" 1 "), led_state.num_lock ? true : false);
                oled_set_cursor(4, 2);
                oled_write_P(PSTR("   "), led_state.num_lock ? true : false);
            oled_set_cursor(8, 0);
                oled_write_P(PSTR("   "), led_state.scroll_lock ? true : false);
                oled_set_cursor(8, 1);
                oled_write_P(PSTR(" S "), led_state.scroll_lock ? true : false);
                oled_set_cursor(8, 2);
                oled_write_P(PSTR("   "), led_state.scroll_lock ? true : false);
            break;
    
        case _CONFIG:
            oled_write_P(PSTR("Config"), false);
            // oled_set_cursor(11, 2);
            // oled_write_P(PSTR("Press Dial"), false);
            // oled_set_cursor(11, 3);
            // oled_write_P(PSTR("for Bootloader"), false);
/* Theory for base layer TODO: Images
 * █A█ █1█ █S█ Layer: 
 *  ^   ^   ^   Config
 * HUE SAT VAL
 *  v   v   v
 */
            oled_set_cursor(0, 0); // Slim 3x1 boxes for the lock symbols
            oled_write_P(PSTR(" A "), led_state.caps_lock ? true : false);
            oled_set_cursor(4, 0);
            oled_write_P(PSTR(" 1 "), led_state.num_lock ? true : false);
            oled_set_cursor(8, 0);
            oled_write_P(PSTR(" S "), led_state.scroll_lock ? true : false);
            oled_set_cursor(0, 1);
            oled_write_P(PSTR(" ^   ^   ^"), false);
            oled_set_cursor(0, 2);
            oled_write_P(PSTR("HUE SAT VAL\n"), false);
            // oled_set_cursor(0, 3);
            oled_write_P(PSTR(" v   v   v "), false);
            
            break;
        default:
            oled_write_P(PSTR("Undefined"), false);
    } 
    return false;
}
#endif