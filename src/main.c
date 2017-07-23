#include "draw.h"
#include "event.h"
#include "config.h"

#define FPS_LIMIT 60.0f

void LogDialogWindow(void* userdata, int category, SDL_LogPriority priority, const char* message){
    SDL_MessageBoxButtonData buttons[] = { {0, 0, "OK"} };
    SDL_MessageBoxData messagedata = {
        0,       /*flags*/
        NULL,    /*window*/
        NULL,    /*title*/
        message, /*message*/
        1,       /*numbuttons*/
        buttons, /*buttons*/
        NULL     /*colorScheme*/
    };

    //Use priority to determine message type
    if(priority == SDL_LOG_PRIORITY_VERBOSE || priority == SDL_LOG_PRIORITY_DEBUG || priority == SDL_LOG_PRIORITY_INFO){
        messagedata.flags = SDL_MESSAGEBOX_INFORMATION;
        messagedata.title = "INFO";
    } else if(priority == SDL_LOG_PRIORITY_WARN){
        messagedata.flags = SDL_MESSAGEBOX_WARNING;
        messagedata.title = "WARNING";
    } else {
        messagedata.flags = SDL_MESSAGEBOX_ERROR;
        messagedata.title = "ERROR";
    }
    if(SDL_ShowMessageBox(&messagedata, NULL) != 0){
        //Well we cant present the error in GUI, just print it I guess...
        printf("%s\n", message);
    }
}

void Main_Cleanup(){
    int i;
    Draw_Cleanup();
    for(i = 0; i < button_num; i++){
        if(button_list[i].type <= BUTTON_TYPE_NONE)
            continue;
        SDL_DestroyTexture(button_list[i].button.texture);
        //probably have to free more shit
    }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char** argv){
    SDL_Window *window;
    SDL_Renderer *renderer;
    uint32_t delay_msec, fps_limit_msec;
    uint64_t time_pre_loop, time_post_loop, time_last;

    //Setup Errors/Logging
    SDL_LogSetOutputFunction(LogDialogWindow, NULL);

    //Init SDL and Components
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) != 0){
		SDL_Log("SDL failed to initialize");
		return 1;
	}
    if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != (IMG_INIT_JPG | IMG_INIT_PNG)){
        SDL_Log("IMG failed to initialize");
		return 1;
    }
	if(TTF_Init() != 0){
		SDL_Log("TTF failed to initialize");
		return 1;
	}

    //Make the Window/Renderer
    if(SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_RESIZABLE | WINDOW_FLAGS, &window, &renderer)){
        SDL_Log("Could not create Window/Render");
        return 2;
    }
    SDL_SetWindowMinimumSize(window, 640, 480); //TODO: Doesnt do jack
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //Handle Transparency
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); //How we Scale

    atexit(Main_Cleanup);

    /* Read Config File */
    Config_ReadConfig("buttons.json", renderer);

    /* Arrange Buttons */
    Config_OrganizeButtons(renderer);

    /* Event Loop */
    Draw_Init(renderer);
    Event_Init(window);
    fps = 0.0f;
    fps_limit_msec = 1000.0f*(1.0f/FPS_LIMIT);
    time_last = time_pre_loop = 0;
    while(1){
        time_last = time_pre_loop;
        time_pre_loop = SDL_GetPerformanceCounter();
        if(Event_Handle() != 0){
            break;
        }
        if(did_resize == SDL_TRUE){
            Config_OrganizeButtons(renderer);
        }

        if(Draw_Screen(renderer) != 0){
            break;
        }

        //Calculate the FPS
        fps = (float)SDL_GetPerformanceFrequency()/(float)(time_pre_loop - time_last);
        time_post_loop = SDL_GetPerformanceCounter();

        //Calculate how long we need to delay to hit our target FPS
        delay_msec = 1000.f*((float)(time_post_loop - time_pre_loop)/(float)SDL_GetPerformanceFrequency());
        delay_msec = fps_limit_msec - delay_msec;
        if(delay_msec <= fps_limit_msec){ //Dont delay if we overran the desired FPS
            //printf("DELAYED\n")
            SDL_Delay(delay_msec);
        }
    }

    //Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}