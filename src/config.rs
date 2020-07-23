extern crate json;
extern crate sdl2;

use sdl2::image::LoadTexture;
use sdl2::pixels::Color;
use std::fs;
use std::path::Path;


pub struct MainButton<'a> {
    pub name: String,
    pub command: String,
    pub args: String,
    pub selected: bool,

    pub texture: sdl2::render::Texture<'a>,
    pub location: sdl2::rect::Rect
}

pub struct MenuItem <'a> {
    pub name: String,
    pub r#type: String,
    pub command: String,
    pub args: String,
    pub selected: bool,

    pub texture: sdl2::render::Texture<'a>,
    pub back_location: sdl2::rect::Rect,
    pub text_location: sdl2::rect::Rect
}

pub struct MenuButton<'a> {
    pub name: String,
    pub items: Vec<MenuItem<'a>>,
    pub selected: bool,
    pub active: bool,

    pub texture: sdl2::render::Texture<'a>,
    pub location: sdl2::rect::Rect
}


//TODO: arrange functions can probably be cleaned up a lot since it's mostly a copy paste of the old stuff
pub fn arrange_main(window_w: u32, window_h: u32, list: &mut Vec<MainButton>){
    
    const UI_PAD : f32 = 0.03;
    const BOTTOMROW_PAD: u32 = 80;
    
    let mut num_rows: u32 = 0;
    let mut num_cols: u32 = 0;
    let mut main_width: u32;
    let mut main_height: u32  = 0;
    let mut row_num: u32;
    let mut col_num: u32 = 0;
    let mut height_max: u32 = 0;
    let xx: u32;

    //Determine base padding values
    let ui_pad: u32 = (UI_PAD * (window_w as f32)).max(UI_PAD * (window_h as f32)) as u32;

    //Determine maximum number of columns
    //this is the number of buttons we can fit width-wise WITHOUT shrinking (and with minimum padding)
    let temp = ui_pad;
    
    for item in &(*list) {
        let props = item.texture.query();
        let temp = temp + props.width + ui_pad;
        num_cols = num_cols + 1;
        if temp > window_w {
            break;
        }
    }
    
    //Use the number of columns to determine width of each element
    main_width = (window_w - ((num_cols+1)* ui_pad)) / num_cols;

    //USING THAT WIDTH, determine the height we have to be to support it
    let mut appbutton_padx: u32;
    let mut appbutton_pady = ui_pad + BOTTOMROW_PAD;
    for item in &(*list) {
        let props = item.texture.query();
        let temp = ((props.height as f32)/(props.width as f32) * (main_width as f32)) as u32;
        
        if temp > height_max {
            height_max = temp;
        }

        col_num = col_num + 1;
        if col_num >= num_cols {
            num_rows = num_rows + 1;
            appbutton_pady = appbutton_pady + height_max + ui_pad;
            height_max = 0;
        }
    }

    //IF WE DONT FIT, size elements based on maximum HEIGHT rather than maximum width
    if appbutton_pady > window_h {
        main_height = (window_h - ((num_rows+1) * ui_pad) - BOTTOMROW_PAD) / num_rows; //Hight required to fit the rows
        main_width = 0;

        //Determine how much we need to pad on the Left & Right to center the view
        //This one is harder to do, since we need to recalculate how wide a whole row is going to be
        appbutton_padx = 0;
        col_num = 0;
        for item in &(*list) {
            let props = item.texture.query();
            let temp = ((props.width as f32)/(props.height as f32) * (main_width as f32)) as u32;

            appbutton_padx = appbutton_padx + temp + ui_pad;
            col_num = col_num + 1;
            if col_num >= num_cols {
                break;
            }
        }
        appbutton_pady = ui_pad;
        appbutton_padx = (((window_w as f32)/2.0) - (((appbutton_padx - ui_pad) as f32)/2.0)) as u32;
        xx = appbutton_padx;
    } else {
        //Determine how much we need to pad on the top & bottom to center the view
        appbutton_padx = ui_pad;
        appbutton_pady = (((window_h as f32)/2.0) - (((appbutton_pady - ui_pad) as f32)/2.0)) as u32;
        xx = ui_pad;
    }

    //Set the position data of each button
    height_max = 0;
    col_num = 0;
    row_num = 0;
    for item in &mut (*list) {
        let props = item.texture.query();
        if main_height == 0 {
            item.location.set_height(((props.height as f32)/(props.width as f32) * (main_width as f32)) as u32);
            item.location.set_width(main_width);
        } else {
            item.location.set_width(((props.width as f32)/(props.height as f32) * (main_width as f32)) as u32);
            item.location.set_height(main_height);
        }
        item.location.set_x(appbutton_padx as i32);
        item.location.set_y(appbutton_pady as i32);

        if item.location.height() > height_max {
            height_max = item.location.height();
        }
        appbutton_padx = appbutton_padx + item.location.width() + ui_pad;

        if col_num == num_cols - 1{
            col_num = 0;
            row_num = row_num + 1;
            appbutton_pady = appbutton_pady + height_max + ui_pad;
            appbutton_padx = xx;
            height_max = 0;
        } else {
            col_num = col_num + 1;
        }
    }

}

pub fn arrange_menu(window_w: u32, window_h: u32, list: &mut Vec<MenuButton>) {
    const UI_PAD : f32 = 0.03;
    const MENU_PAD: u32 = 20;

    let mut menubutton_pad: u32 = 0;

    let ui_pad: u32 = (UI_PAD * (window_w as f32)).max(UI_PAD * (window_h as f32)) as u32;

    for item in &mut (*list) {
        let props = item.texture.query();

        item.location.set_width(props.width);
        item.location.set_height(props.height);
        item.location.set_x((window_w - ui_pad - props.width - menubutton_pad) as i32);
        item.location.set_y((window_h - ui_pad - props.height) as i32);
        if item.location.x() < 0 || item.location.y() < 0 {
            //TODO: if window is too small to hold our button, scale it
            print!("BAD POSITION CALCULATION");
        }
        menubutton_pad += window_w - item.location.x() as u32;

        //Do menu layout
        let mut temp = 0;
        let mut height_max = item.location.y();

        //Get largest width
        for menu_item in &item.items {
            let props = menu_item.texture.query();
            if props.width > temp {
                temp = props.width;
            }
        }
        temp += MENU_PAD * 2;

        //Set the positions of things
        for menu_item in &mut item.items {
            let props = menu_item.texture.query();

            menu_item.back_location.set_width(temp);
            menu_item.back_location.set_height(props.height + MENU_PAD * 2);
            height_max -= menu_item.back_location.height() as i32;

            menu_item.back_location.set_x(item.location.x() + item.location.width() as i32 - temp as i32);
            menu_item.back_location.set_y(height_max);

            menu_item.text_location.set_width(props.width);
            menu_item.text_location.set_height(props.height);
            menu_item.text_location.set_x(menu_item.back_location.x() + MENU_PAD as i32);
            menu_item.text_location.set_y(menu_item.back_location.y() + MENU_PAD as i32);
        }
    }
}

//TODO: More robust error handling
pub fn read<'a>(font: &sdl2::ttf::Font, texture_creator: &'a sdl2::render::TextureCreator<sdl2::video::WindowContext>, config_path: &str) -> Result<(Vec<MainButton<'a>>, Vec<MenuButton<'a>>), String>{
    let mut main_list: Vec<MainButton> = Vec::new();
    let mut menu_list: Vec<MenuButton> = Vec::new();

    //TODO: Some day my completion will work...
    //let jsn_to_str = |name: &str, entity: &json::JsonValue, key: &str| -> Result<&str, String>{
    //    entity[key].as_str().ok_or_else(|| format!("element {} of {} is not string", key, name))
    //};

    let json_path = Path::new(config_path);
    let json_str = fs::read_to_string(json_path).unwrap();
    let parsed_json = json::parse(&json_str).unwrap();

    for (name, entry) in parsed_json.entries() {
        if !entry.has_key("type") {
            return Err(format!("{} does not have a type tag", name));
        }
        let type_val = entry["type"].as_str().ok_or_else(|| "type is not string")?;

        match type_val{
            "mainbutton" => {
                let args = if entry.has_key("args") {entry["args"].as_str().ok_or_else(|| "args is not string")?} else {""};
                let img_path = entry["image"].as_str().ok_or_else(|| "image is not string")?.to_string();

                main_list.push(
                    MainButton{
                        name: name.to_string(),
                        command: entry["command"].as_str().ok_or_else(|| "command is not string")?.to_string(),
                        args: args.to_string(),
                        texture: texture_creator.load_texture(img_path).unwrap(),
                        location: sdl2::rect::Rect::new(0, 0, 0, 0),
                        selected: false,
                    }
                );
            },
            "menubutton" => {
                let mut item_list: Vec<MenuItem> = Vec::new();
                let img_path = entry["image"].as_str().ok_or_else(|| "image is not string")?.to_string();                

                for item in entry["items"].members() {
                    let r#type = item["type"].as_str().ok_or_else(|| "type is not string")?;
                    let command = if r#type == "application" {item["command"].as_str().ok_or_else(|| "command is not string")?} else {""};
                    let args = if r#type == "application" && item.has_key("args") {item["args"].as_str().ok_or_else(|| "application is not string")?} else {""};
                    let item_name = item["name"].as_str().ok_or_else(|| "name is not string")?;


                    item_list.push(MenuItem {
                        name: item_name.to_string(),
                        r#type: item["type"].as_str().ok_or_else(|| "type is not string")?.to_string(),
                        command: command.to_string(),
                        args: args.to_string(),
                        texture: texture_creator.create_texture_from_surface(font.render(item_name).solid(Color::RGB(255, 255, 255)).unwrap()).unwrap(),
                        back_location: sdl2::rect::Rect::new(0, 0, 0, 0),
                        text_location: sdl2::rect::Rect::new(0, 0, 0, 0),
                        selected: false,
                    });
                }

                menu_list.push(
                    MenuButton {
                        name: name.to_string(),
                        texture: texture_creator.load_texture(img_path).unwrap(),
                        items: item_list,
                        location: sdl2::rect::Rect::new(0, 0, 0, 0),
                        selected: false,
                        active: false,
                    }
                );
            },
            _ => panic!("Unknown type {} in entry {}", type_val, name),
        }
    }
    main_list[0].selected = true;

    Ok((main_list, menu_list))
}