extern crate sdl2;

use std::time::Duration;

mod config;
mod draw;
mod event;

fn main() {

    //Set up SDL environment
    let sdl_context = sdl2::init().unwrap();
    let _sdlimg_context = sdl2::image::init(sdl2::image::InitFlag::PNG | sdl2::image::InitFlag::JPG).unwrap();
    let sdlttf_context = sdl2::ttf::init().unwrap();

    sdl2::hint::set("SDL_HINT_RENDER_SCALE_QUALITY", "best");
    sdl2::hint::set_video_minimize_on_focus_loss(false);

    let video_subsystem = sdl_context.video().unwrap();
    let window = video_subsystem.window("GabeUI", 640, 480)
        .position_centered()
        .resizable()
        .build()
        .unwrap();
    
    let mut canvas = window.into_canvas().build().unwrap();
    let texture_creator = canvas.texture_creator();
    canvas.set_blend_mode(sdl2::render::BlendMode::Blend);

    let font = sdlttf_context.load_font("assets/calibri.ttf", 42).unwrap();

    let mut event_pump = sdl_context.event_pump().unwrap();

    // Read config & generate buttons
    let (mut main_list, mut menu_list) = config::read(&font, &texture_creator, "buttons.json").unwrap();

    // Arrange buttons on screen
    let tmp = canvas.viewport();
    config::arrange_main(tmp.width(), tmp.height(), &mut main_list);
    config::arrange_menu(tmp.width(), tmp.height(), &mut menu_list);

    //Main Loop
    loop {
        if event::run(&mut event_pump, &mut main_list, &mut menu_list) {
            //If told to quit
            break;
        }
        draw::run(&mut canvas, &main_list, &menu_list);
        ::std::thread::sleep(Duration::new(0, 1_000_000_000u32/60));
    }
}
