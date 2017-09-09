#include "config.h"
#include "jsmn/jsmn.h"
#include <stdio.h>
#include <string.h>

#define GET_SUBSTRING(in_string, out_string, token) strncpy(out_string, in_string + token.start, token.end-token.start)
#define MAX(x, y) (x > y) ? x : y

//UI Offsets & Scales
#define UI_PAD 0.03f
#define MENU_PAD 20
#define BOTTOMROW_PAD 80

uint32_t appbutton_num;
uint32_t menubutton_num;

//TODO: Make sure to update this whenever the button format changes
void CleanButton(union TopButton *button){
    button->type = BUTTON_TYPE_NONE;
    button->button.name = "";
    button->button.state = BUTTON_STATE_UNSELECTED;
    button->button.texture = NULL;
    button->button.pos.x = 0;
    button->button.pos.y = 0;
    button->button.pos.w = 0;
    button->button.pos.h = 0;
    button->button.base_size.x = 0;
    button->button.base_size.y = 0;
    button->button.base_size.w = 0;
    button->button.base_size.h = 0;
    button->button.directions[BUTTON_DIR_DOWN]  = NULL;
    button->button.directions[BUTTON_DIR_UP]    = NULL;
    button->button.directions[BUTTON_DIR_LEFT]  = NULL;
    button->button.directions[BUTTON_DIR_RIGHT] = NULL;

    button->appbutton.application = "";
    button->appbutton.arguments = "";

    button->menubutton.menu_num = 0;
}

void CleanMenuItem(struct MenuItem *item){
    item->type = MENUITEM_NONE;
    item->name = "";
    item->state = BUTTON_STATE_UNSELECTED;
    item->texture = NULL;
    item->pos.x = 0;
    item->pos.y = 0;
    item->pos.w = 0;
    item->pos.h = 0;
    item->text_size.x = 0;
    item->text_size.y = 0;
    item->text_size.w = 0;
    item->text_size.h = 0;
    item->application = "";
    item->arguments = "";
}

void ButtonList_Cleanup(){
    int i, j;
    for(i = 0; i < button_num; i++){
        if(button_list[i]->type == BUTTON_TYPE_NONE)
            continue;
        SDL_DestroyTexture(button_list[i]->button.texture);
        if(button_list[i]->type == BUTTON_TYPE_MENUBUTTON){
            for(j = 0; j < button_list[i]->menubutton.menu_num; j++){
                SDL_DestroyTexture(button_list[i]->menubutton.menu[j]->texture);
                free(button_list[i]->menubutton.menu[j]);
            }
        }
        free(button_list[i]);
    }
}

void PopulateGridPointers(union TopButton **appgrid, union TopButton **menugrid, int app_col, int app_row){
    int i, j;

    //Populate appbutton pointers
    for(i = 0; i < app_row; i++){
        for(j = 0; j < app_col; j++){
            if(j + (i * app_col) >= appbutton_num)
                break;

            if(i > 0){
                appgrid[j + (i * app_col)]->button.directions[BUTTON_DIR_UP] = appgrid[j + ((i - 1) * app_col)];
            }
            if(j > 0){
                appgrid[j + (i * app_col)]->button.directions[BUTTON_DIR_LEFT] = appgrid[(j - 1) + (i * app_col)];
            }
            if(j + 1 < app_col && (j + 1) + (i * app_col) < appbutton_num){
                appgrid[j + (i * app_col)]->button.directions[BUTTON_DIR_RIGHT] = appgrid[(j + 1) + (i * app_col)];
            }
            if(i + 1 < app_row && j + ((i + 1) * app_col) < appbutton_num){
                appgrid[j + (i * app_col)]->button.directions[BUTTON_DIR_DOWN] = appgrid[j + ((i + 1) * app_col)];
            } else {
                appgrid[j + (i * app_col)]->button.directions[BUTTON_DIR_DOWN] = menugrid[menubutton_num - 1];
            }
        }
    }
    //Populate menubutton pointers
    for(i = 0; i < menubutton_num; i++){
        menugrid[i]->button.directions[BUTTON_DIR_UP] = appgrid[appbutton_num - 1];
        if(i > 0) {
            menugrid[i]->button.directions[BUTTON_DIR_RIGHT] = menugrid[i - 1];
        }
        if(i + 1 < menubutton_num){
            menugrid[i]->button.directions[BUTTON_DIR_LEFT] = menugrid[i + 1];
        }
    }
}

void Config_ReadConfig(const char* filename, SDL_Renderer* renderer){
    FILE* conf_file;
    char *conf_text;
    char *temp_string;
    int64_t conf_length, temp;
    int i;
    jsmn_parser json_parser;
    jsmntok_t json_tokens[100];
    SDL_bool do_free;
    SDL_bool is_menuitem;
    SDL_Surface* tmp_surface;
    union TopButton *temp_button = NULL;
    struct MenuItem *temp_item = NULL;

    //TODO:
    // I kinda want it to support hot-loading at some point, pipe dream

    //If this is a rerun of the config read we need to clean up the old stuff
    ButtonList_Cleanup();
    memset(button_list, 0, sizeof(union TopButton *) * BUTTON_LIST_MAX);

    //Initialize some variables
    button_num = 0;
    appbutton_num = 0;
    menubutton_num = 0;

    //Read File into a single string
    conf_file = fopen(filename, "rb");
    if(conf_file == NULL){
        SDL_Log("Failed to read file %s", filename);
        return;
    }
    fseek(conf_file, 0, SEEK_END);
    conf_length = ftell(conf_file);
    rewind(conf_file);
    conf_text = (char *) malloc(sizeof(char) * conf_length + 1);
    if(conf_text == NULL){
        SDL_Log("Failed to allocate text buffer");
        return;
    }
    if((temp = fread(conf_text, sizeof(char), conf_length, conf_file)) != conf_length){
        SDL_Log("Error reading file");
        return;
    }
    //fread doesnt null terminate string because whoever wrote the C std libraries is insane
    conf_text[conf_length] = '\0';
    fclose(conf_file);

    //Tokenize string
    jsmn_init(&json_parser);
    if((temp = jsmn_parse(&json_parser, conf_text, strlen(conf_text), json_tokens, 100)) < 0){
        SDL_Log("Error tokenizing json: %ld", temp);
        return;
    }

    //TODO: Make this code less of a pain
    //      Present parse errors to user in a better fasion
    //          Line number
    //          Line
    //          Issue

    //Go through tokens and parse out info
    is_menuitem = SDL_FALSE;
    for(i = 1; i < temp; i++){
        do_free = SDL_TRUE;

        //Handle Objects
        if(json_tokens[i].type == JSMN_OBJECT){
            //Previous string was the type
            i--;
            temp_string = (char *)malloc(json_tokens[i].end - json_tokens[i].start + 1);
            GET_SUBSTRING(conf_text, temp_string, json_tokens[i]);
            temp_string[json_tokens[i].end - json_tokens[i].start] = '\0';

            //Main/MenuButtons
            if(strcmp(temp_string, "mainbutton") == 0 || strcmp(temp_string, "menubutton") == 0){
                //There is a maximum number of buttons
                if(button_num == BUTTON_LIST_MAX){
                    SDL_Log("Tried to add too many buttons");
                    return;
                }

                if(is_menuitem == SDL_TRUE)
                    is_menuitem = SDL_FALSE;

                temp_button = malloc(sizeof(union TopButton));
                button_list[button_num] = temp_button;
                button_num+=1;
                CleanButton(temp_button);

                if(strcmp(temp_string, "mainbutton") == 0){
                    temp_button->type = BUTTON_TYPE_APPBUTTON;
                    appbutton_num += 1;
                }
                else if(strcmp(temp_string, "menubutton") == 0){
                    temp_button->type = BUTTON_TYPE_MENUBUTTON;
                    memset(temp_button->menubutton.menu, 0, sizeof(struct MenuItem*) * MENUITEM_LIST_MAX);
                    menubutton_num += 1;
                }

            //MenuItems
            } else if(strcmp(temp_string, "menuitem") == 0) {
                if(temp_button->type != BUTTON_TYPE_MENUBUTTON){
                    printf("BAD MENUITEM\n");
                    continue;
                }
                //There is a maximum number of menuitems
                if(temp_button->menubutton.menu_num == MENUITEM_LIST_MAX){
                    SDL_Log("Tried to add too many MenuItems");
                    return;
                }

                temp_item = malloc(sizeof(struct MenuItem));
                temp_button->menubutton.menu[temp_button->menubutton.menu_num] = temp_item;
                temp_button->menubutton.menu_num += 1;
                CleanMenuItem(temp_item);
                is_menuitem = SDL_TRUE;
            }
            i++;
            free(temp_string);
        }

        //String Fields
        if(json_tokens[i].type == JSMN_STRING){
            temp_string = (char *)malloc(json_tokens[i].end - json_tokens[i].start + 1);
            GET_SUBSTRING(conf_text, temp_string, json_tokens[i]);
            temp_string[json_tokens[i].end - json_tokens[i].start] = '\0';

            if(strcmp(temp_string, "mainbutton") == 0 || strcmp(temp_string, "menubutton") == 0 || strcmp(temp_string, "menuitem") == 0){
                //Just ignore this, it'll be caught again as an object
                is_menuitem = SDL_FALSE;
            
            //Name
            } else if(strcmp(temp_string, "name") == 0) {
                //Get next string
                free(temp_string);
                i += 1;
                if(json_tokens[i].type != JSMN_STRING){
                    printf("Whoopsies\n");
                    continue;
                }

                temp_string = (char *)malloc(json_tokens[i].end - json_tokens[i].start + 1);
                GET_SUBSTRING(conf_text, temp_string, json_tokens[i]);
                temp_string[json_tokens[i].end - json_tokens[i].start] = '\0';

                if(is_menuitem == SDL_TRUE){
                    temp_item->name = temp_string;
                    //We get the texture when we do the size calculation
                } else {
                    temp_button->button.name = temp_string;
                }
                do_free = SDL_FALSE;

            //Image
            } else if(strcmp(temp_string, "image") == 0) {
                //Get next string
                free(temp_string);
                i += 1;
                if(json_tokens[i].type != JSMN_STRING){
                    printf("Whoopsies\n");
                    continue;
                }

                temp_string = (char *)malloc(json_tokens[i].end - json_tokens[i].start + 1);
                GET_SUBSTRING(conf_text, temp_string, json_tokens[i]);
                temp_string[json_tokens[i].end - json_tokens[i].start] = '\0';

                //Load that image
                if(is_menuitem == SDL_FALSE){
                    tmp_surface = IMG_Load(temp_string);
                    temp_button->button.texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
                    if(temp_button->appbutton.texture == NULL){
                        SDL_Log("Failed to load image: %s", SDL_GetError());
                    }
                    SDL_FreeSurface(tmp_surface);

                    //Make note of our texture's size
                    SDL_QueryTexture(temp_button->button.texture, NULL, NULL, &temp_button->button.base_size.w, &temp_button->button.base_size.h);
                }
            } else if(strcmp(temp_string, "command") == 0){
                //Get next string
                free(temp_string);
                i += 1;
                if(json_tokens[i].type != JSMN_STRING){
                    printf("Whoopsies\n");
                    continue;
                }

                temp_string = (char *)malloc(json_tokens[i].end - json_tokens[i].start + 1);
                GET_SUBSTRING(conf_text, temp_string, json_tokens[i]);
                temp_string[json_tokens[i].end - json_tokens[i].start] = '\0';

                if(temp_button->type == BUTTON_TYPE_APPBUTTON){
                    temp_button->appbutton.application = temp_string;
                } else if(is_menuitem == SDL_TRUE) {
                    temp_item->application = temp_string;
                } else {
                    printf("BAD APP\n");
                }
                do_free = SDL_FALSE;
            } else if(strcmp(temp_string, "args") == 0){
                //Get next string
                free(temp_string);
                i += 1;
                if(json_tokens[i].type != JSMN_STRING){
                    printf("Whoopsies\n");
                    continue;
                }

                temp_string = (char *)malloc(json_tokens[i].end - json_tokens[i].start + 1);
                GET_SUBSTRING(conf_text, temp_string, json_tokens[i]);
                temp_string[json_tokens[i].end - json_tokens[i].start] = '\0';

                if(temp_button->type == BUTTON_TYPE_APPBUTTON){
                    temp_button->appbutton.arguments= temp_string;
                } else if(is_menuitem == SDL_TRUE) {
                    temp_item->arguments = temp_string;
                } else {
                    printf("BAD ARG\n");
                }
                do_free = SDL_FALSE;
            } else if(strcmp(temp_string, "type") == 0){

                //Get next string
                free(temp_string);
                i += 1;
                if(json_tokens[i].type != JSMN_STRING){
                    printf("Whoopsies\n");
                    continue;
                }

                if(is_menuitem == SDL_FALSE){
                    continue;
                }

                temp_string = (char *)malloc(json_tokens[i].end - json_tokens[i].start + 1);
                GET_SUBSTRING(conf_text, temp_string, json_tokens[i]);
                temp_string[json_tokens[i].end - json_tokens[i].start] = '\0';

                if(strcmp(temp_string, "quit") == 0){
                    temp_item->type = MENUITEM_QUIT;
                } else if(strcmp(temp_string, "restart") == 0){
                    temp_item->type = MENUITEM_RESTART;
                } else if(strcmp(temp_string, "shutdown") == 0){
                    temp_item->type = MENUITEM_SHUTDOWN;
                } else if(strcmp(temp_string, "application") == 0){
                    temp_item->type = MENUITEM_APPLICATION;
                } else {
                    printf("BAD TYPE\n");
                }
            }

            if(do_free == SDL_TRUE)
                free(temp_string);
        } //END IF STRING
    }

    //Set starting selected button
    for(i = 0; i < button_num; i++){
        if(button_list[i]->type == BUTTON_TYPE_APPBUTTON){
            button_list[i]->button.state = BUTTON_STATE_SELECTED;
            break;
        }
    }

    free(conf_text);
}

void Config_OrganizeButtons(SDL_Renderer *renderer) {
    SDL_Surface* tmp_surface;
    SDL_Color menuitem_color = {0xFF, 0xFF, 0xFF, 0xFF};
    union TopButton **appgrid;
    union TopButton **menugrid;
    int i, j, window_w, window_h, ui_pad, menu_pad;
    int appbutton_padx, appbutton_pady, xx;
    int temp;

    int num_rows = 0;
    int num_cols = 0;
    int app_width = 0;
    int app_height = 0;
    int menubutton_pad = 0;
    int row_num = 0;
    int col_num = 0;
    int height_max = 0;

    //Loop through list of buttons and arrange
    SDL_GetRendererOutputSize(renderer, &window_w, &window_h);

    //TODO: resize font based on window size
    if(font != NULL){
        TTF_CloseFont(font);
    }
	font = TTF_OpenFont("Assets/calibri.ttf", 42);

    //Determine base padding values
    ui_pad = MAX(UI_PAD * window_w, UI_PAD * window_h);

    //Reset Directional Pointers
    for(i = 0; i < button_num; i++){
        if(button_list[i]->type == BUTTON_TYPE_APPBUTTON){
            button_list[i]->button.directions[BUTTON_DIR_UP] = NULL;
            button_list[i]->button.directions[BUTTON_DIR_DOWN] = NULL;
            button_list[i]->button.directions[BUTTON_DIR_LEFT] = NULL;
            button_list[i]->button.directions[BUTTON_DIR_RIGHT] = NULL;
        }
    }

    //Determine maximum number of columns
    //this is the number of buttons we can fit width-wise WITHOUT shrinking (and with minimum padding)
    temp = ui_pad;
    for(i = 0; i < button_num; i++){
        if(button_list[i]->type == BUTTON_TYPE_APPBUTTON){
            temp += button_list[i]->button.base_size.w + ui_pad;
            num_cols += 1;
            if(temp > window_w)
                break;
        }
    }

    //Use the number of columns to determine width of each element
    app_width = (window_w - ((num_cols+1) * ui_pad)) / num_cols;

    //USING THAT WIDTH, determine the height we have to be to support it
    appbutton_pady = ui_pad + BOTTOMROW_PAD;
    for(i = 0; i < button_num; i++){
        if(button_list[i]->type == BUTTON_TYPE_APPBUTTON){
            temp = (int)((float)button_list[i]->button.base_size.h/(float)button_list[i]->button.base_size.w * (float)app_width);

            if(temp > height_max){
                height_max = temp;
            }

            col_num += 1;
            if(col_num >= num_cols){
                num_rows += 1;
                appbutton_pady += height_max + ui_pad;
                height_max = 0;
            }
        }
    }

    //IF WE DONT FIT, size elements based on maximum HEIGHT rather than maximum width
    if(appbutton_pady > window_h){
        app_height = (window_h - ((num_rows+1) * ui_pad) - BOTTOMROW_PAD) / num_rows; //Hight required to fit the rows
        app_width = 0;

        //Determine how much we need to pad on the Left & Right to center the view
        //This one is harder to do, since we need to recalculate how wide a whole row is going to be
        appbutton_padx = 0;
        col_num = 0;
        for(i = 0; i < button_num; i++){
            if(button_list[i]->type == BUTTON_TYPE_APPBUTTON){
                temp = (int)((float)button_list[i]->button.base_size.w/(float)button_list[i]->button.base_size.h * (float)app_height);

                appbutton_padx += temp + ui_pad;
                col_num += 1;
                if(col_num >= num_cols)
                    break;
            }
        }
        appbutton_pady = ui_pad;
        appbutton_padx = xx = (int)((float)window_w/2.0f - (float)(appbutton_padx - ui_pad)/2.0f);
    } else {
        //Determine how much we need to pad on the top & bottom to center the view
        appbutton_padx = ui_pad;
        appbutton_pady = (int)((float)window_h/2.0f - (float)(appbutton_pady - ui_pad)/2.0f);
        xx = ui_pad;
    }

    //Allocate matricies for use in directional movement
    appgrid = malloc(sizeof(union TopButton*) * num_cols * num_rows);
    menugrid = malloc(sizeof(union TopButton*) * menubutton_num);

    //Set the position data of each button
    height_max = 0;
    col_num = row_num = 0;
    for(i = 0; i < button_num; i++){
        if(button_list[i]->type == BUTTON_TYPE_APPBUTTON){
            if(app_height == 0){
                button_list[i]->button.pos.h =(int)((float)button_list[i]->button.base_size.h/(float)button_list[i]->button.base_size.w * (float)app_width);
                button_list[i]->button.pos.w = app_width;
            } else {
                button_list[i]->button.pos.w =(int)((float)button_list[i]->button.base_size.w/(float)button_list[i]->button.base_size.h * (float)app_height);
                button_list[i]->button.pos.h = app_height;
            }
            button_list[i]->button.pos.x = appbutton_padx;
            button_list[i]->button.pos.y = appbutton_pady;

            if(button_list[i]->button.pos.h > height_max){
                height_max = button_list[i]->button.pos.h;
            }
            appbutton_padx += button_list[i]->button.pos.w + ui_pad;

            //Add button to grid
            appgrid[col_num + (row_num * num_cols)] = button_list[i];

            if(col_num == num_cols - 1){
                col_num = 0;
                row_num += 1;
                appbutton_pady += height_max + ui_pad;
                appbutton_padx = xx;
                height_max = 0;
            } else {
                col_num += 1;
            }
        }
    }

    //Menubuttons are much less complex
    col_num = 0;
    menu_pad = MENU_PAD;
    for(i = 0; i < button_num; i++){
        //Align to bottom-right
        if(button_list[i]->type == BUTTON_TYPE_MENUBUTTON){
            button_list[i]->button.pos.w = button_list[i]->button.base_size.w;
            button_list[i]->button.pos.h = button_list[i]->button.base_size.h;
            button_list[i]->button.pos.x = window_w - ui_pad - button_list[i]->button.pos.w - menubutton_pad;
            button_list[i]->button.pos.y = window_h - ui_pad - button_list[i]->button.pos.h;
            if(button_list[i]->button.pos.x < 0 || button_list[i]->button.pos.y < 0){
                //TODO: if window is too small to hold our button, scale it
                SDL_Log("BAD POSITION CALCULATION");
            }
            menubutton_pad += window_w - button_list[i]->button.pos.x;
            menugrid[col_num] = button_list[i];
            col_num += 1;

            //Do menu layout
            temp = 0;
            height_max = button_list[i]->button.pos.y;
            //Get largest width
            for(j = 0; j < button_list[i]->menubutton.menu_num; j++){
                if(button_list[i]->menubutton.menu[j]->texture != NULL){
                    SDL_DestroyTexture(button_list[i]->menubutton.menu[j]->texture);
                }
                //Generate texture from name
                tmp_surface = TTF_RenderText_Blended(font, button_list[i]->menubutton.menu[j]->name, menuitem_color);
                if(tmp_surface == NULL){
                    SDL_Log("Failed to load image: %s", SDL_GetError());
                }
                button_list[i]->menubutton.menu[j]->texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
                if(button_list[i]->menubutton.menu[j]->texture == NULL){
                    SDL_Log("Failed to load image: %s", SDL_GetError());
                }
                SDL_FreeSurface(tmp_surface);
                //Make note of our texture's size
                SDL_QueryTexture(button_list[i]->menubutton.menu[j]->texture, NULL, NULL, &button_list[i]->menubutton.menu[j]->text_size.w, &button_list[i]->menubutton.menu[j]->text_size.h);
                if(button_list[i]->menubutton.menu[j]->text_size.w > temp){
                    temp = button_list[i]->menubutton.menu[j]->text_size.w;
                }
            }
            temp += menu_pad * 2;
            //Set the positions of things
            for(j = 0; j < button_list[i]->menubutton.menu_num; j++){
                button_list[i]->menubutton.menu[j]->pos.w = temp;
                button_list[i]->menubutton.menu[j]->pos.h = button_list[i]->menubutton.menu[j]->text_size.h + menu_pad * 2;
                height_max -= button_list[i]->menubutton.menu[j]->pos.h;

                button_list[i]->menubutton.menu[j]->pos.x = button_list[i]->button.pos.x + button_list[i]->button.pos.w - temp;
                button_list[i]->menubutton.menu[j]->pos.y = height_max;
                button_list[i]->menubutton.menu[j]->text_size.x = button_list[i]->menubutton.menu[j]->pos.x + menu_pad;
                button_list[i]->menubutton.menu[j]->text_size.y = button_list[i]->menubutton.menu[j]->pos.y + menu_pad;
            }
        }
    }

    //Populate directional grid pointers & handle selection state
    PopulateGridPointers(appgrid, menugrid, num_cols, num_rows);

    //Cleanup
    free(appgrid);
    free(menugrid);
}