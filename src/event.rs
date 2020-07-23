extern crate sdl2;
use sdl2::event::{Event, WindowEvent};
use sdl2::keyboard::Keycode;

use crate::config;

pub fn run(event_pump: &mut sdl2::EventPump, main_list: &mut Vec<config::MainButton>, menu_list: &mut Vec<config::MenuButton>) -> bool {
    for event in event_pump.poll_iter() {
        match event {
            //Window Resize
            Event::Window { win_event: WindowEvent::Resized(w, h), .. } => {
                config::arrange_main(w as u32, h as u32, main_list);
                config::arrange_menu(w as u32, h as u32, menu_list);
            }

            //Quit Application
            Event::Quit {..} |
            Event::KeyDown { keycode: Some(Keycode::Escape), .. } => {
                return true;
            },
            _ => {}
        }
    }

    false
}