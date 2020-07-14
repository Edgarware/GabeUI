extern crate json;

use std::fs;

pub fn read(config_path: &str){
    let json_string = fs::read_to_string(config_path).unwrap();
    let parsed_json = json::parse(&json_string).unwrap();

    for (name, item) in parsed_json.entries() {
        if !item.has_key("type") {
            panic!("Entry {} does not have a type", name);
        }
        match item["type"]{
            "mainbutton" => {
                println!("MainButton: {}", );
            },
            "menubutton" => {
                println!("MenuButton: {}", );
            },
            _ => panic!("Unknown type {} in entry {}", item["type"], name),
        }
    }


    //let unparsed_json = json::stringify(parsed_json);
    //println!("{}", unparsed_json);
}