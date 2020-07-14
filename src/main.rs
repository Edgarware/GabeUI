extern crate sdl2;

use std::time::Duration;
use sdl2::keyboard::Keycode;
use sdl2::event::Event;
use sdl2::pixels::Color;

mod config;

fn main() {
    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();

    let window = video_subsystem.window("GabeUI", 800, 600)
        .position_centered()
        .build()
        .unwrap();
    
    // Read config
    config::read("buttons.json");

    let mut canvas = window.into_canvas().build().unwrap();

    canvas.set_draw_color(Color::RGB(0, 255, 255));
    canvas.clear();
    canvas.present();

    let mut event_pump = sdl_context.event_pump().unwrap();
    'running: loop {
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit {..} |
                Event::KeyDown { keycode: Some(Keycode::Escape), .. } => {
                    break 'running
                },
                _ => {}
            }
        }

        ::std::thread::sleep(Duration::new(0, 1_000_000_000u32/60));
    }
}
