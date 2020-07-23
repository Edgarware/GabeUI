extern crate sdl2;

use crate::config;

mod colors {
    use sdl2::pixels::Color;

    pub const BACKGROUND: Color = Color::RGB(0x60, 0x60, 0x60);
    pub const MAIN_SEL: Color = Color::RGBA(0x00, 0x00, 0x00, 0xC0);
    pub const MENU_BACK: Color = Color::RGB(0x00, 0x8C, 0x9E);
    pub const MENU_SEL: Color = Color::RGB(0x5B, 0xC0, 0xBE);
    pub const MENU_ACTIVE: Color = Color::RGB(0x05, 0x66, 0x8D);
}

//TODO: only show menu items if active
pub fn run(canvas: &mut sdl2::render::WindowCanvas, main_list: &Vec<config::MainButton>, menu_list: &Vec<config::MenuButton>) {
    canvas.clear();

    //Draw MainButtons
    for button in main_list {
        canvas.copy(&button.texture, None, button.location).unwrap();
        if !button.selected {
            canvas.set_draw_color(colors::MAIN_SEL);
            canvas.fill_rect(button.location).unwrap();
        }
    }

    //Draw MenuButtons
    for button in menu_list {
        if button.active {
            canvas.set_draw_color(colors::MENU_ACTIVE);
        } else if button.selected {
            canvas.set_draw_color(colors::MENU_SEL);
        } else {
            canvas.set_draw_color(colors::MENU_BACK);
        }
        canvas.fill_rect(button.location).unwrap();
        canvas.copy(&button.texture, None, button.location).unwrap();

        if button.active {
            for item in &button.items {
                if item.selected {
                    canvas.set_draw_color(colors::MENU_SEL);
                } else {
                    canvas.set_draw_color(colors::MENU_BACK);
                }
                canvas.fill_rect(item.back_location).unwrap();
                canvas.copy(&item.texture, None, item.text_location).unwrap();
            }
        }
    }

    canvas.set_draw_color(colors::BACKGROUND);
    canvas.present();
}