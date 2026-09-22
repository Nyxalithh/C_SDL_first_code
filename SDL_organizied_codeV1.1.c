/*
SDL_AppInit()
    ↓
répétition :
    SDL_AppEvent()      // zéro, un ou plusieurs événements
    SDL_AppIterate()    // une mise à jour / image :)
    ↓
SDL_AppQuit()
*/


#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>

//STRUCTURE -----------------------------------------------------------------------------------------------------------

typedef struct 
{
    SDL_Surface *surface;       //surface c'est l'image source
    SDL_Texture *texture;       //l'image qu'on utilise.
    SDL_FRect rect;              //rect contient le x,y et w,d

    float speed;
} Sprite;


typedef struct 
{
    bool Tfullscreen;
    bool RUNNING;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    int mouseX, mouseY;

    Uint64 lastTime;

    Sprite nyxith;
} App;





//FONCTION DECLARATION -----------------------------------------------------------------------------------------------------------
void Render(App *app);
void ShortKeyHandle(SDL_Event *event, App *app);
void AppDestroy(App *app);
void Update(App *app, Uint64 deltaTime);



//GLOBAL VAR -----------------------------------------------------------------------------------------------------------






//START -----------------------------------------------------------------------------------------------------------

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])      //appeler au debut, une fois
{


    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("SDL_Init: %s", SDL_GetError);
        return SDL_APP_FAILURE;
    }
    int largeur = 720;
    int hauteur = 540;

    App *app = SDL_calloc(1, sizeof(App));          //cree app
        if (!app) {
            SDL_Log("SDL_Init: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        app->RUNNING = true;
        app->Tfullscreen = false;


    app->window = SDL_CreateWindow("blehh", largeur, hauteur, 0);
        if (!app->window)
        {
            SDL_Log("SDL_CreateWindow : %s", SDL_GetError());
            AppDestroy(app);
            return SDL_APP_FAILURE;
        }
    


    //creation du renderer
    app->renderer = SDL_CreateRenderer(app->window, NULL);      //NULL c'est le render, si on met null on laisse SDL choisir
        if(app->renderer == NULL)
        {
            SDL_Log("SDL_Init: %s", SDL_GetError());
            AppDestroy(app);
            return SDL_APP_FAILURE;
        }

    SDL_SetRenderLogicalPresentation(app->renderer, largeur, hauteur, SDL_LOGICAL_PRESENTATION_LETTERBOX);



    app->lastTime = SDL_GetTicks(); //on initalise lastTime pour pouvoir ensuite calculer les FPS

    

        //creation de nyxith
    app->nyxith.surface = SDL_LoadBMP("Nyx.bmp");
        if (app->nyxith.surface == NULL)
        {
            SDL_Log("Nyx couldnt load: %s", SDL_GetError());
            AppDestroy(app);
            return SDL_APP_FAILURE;
        }
    app->nyxith.rect.x = 300;
    app->nyxith.rect.y = 200;
    app->nyxith.rect.h = 100;
    app->nyxith.rect.w = 100;

    app->nyxith.speed = 300.0f;



    //creation texture
    app->texture = SDL_CreateTextureFromSurface(app->renderer, app->nyxith.surface);
          if(app->texture == NULL)
        {
            SDL_Log("SDL_Init: %s", SDL_GetError());
            AppDestroy(app);
            return SDL_APP_FAILURE;
        }

    

    SDL_DestroySurface(app->nyxith.surface);    //apres avoir cree la texture on a plus besoin de la surface donc on la supp
    app->nyxith.surface = NULL;

    SDL_SetTextureScaleMode(app->texture, SDL_SCALEMODE_PIXELART);
     

    *appstate = app;             //la struct app contien la window et la surface. on la met dans appstate et on peut la recup a chaque foncion.

    SDL_WarpMouseInWindow(app->window, largeur/2, hauteur/2);        //mettre la souris a..
    return SDL_APP_CONTINUE;
}   





//MAIN LOOP -----------------------------------------------------------------------------------------------------------

//ensuite boucle. SDL_AppIterate -> SDL_AppEvent and loop jusque ya une des deux qui return un SDL_APP_FAILURE/SUCESS

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)   //handle les event dans la queue d'event puis appel SDL_App_iterate.     
{
    App *app =appstate;     //on load la window et surface
    
    


    //pour tout type d'event
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        app->RUNNING = false;
        break;

    case SDL_EVENT_KEY_DOWN:
        ShortKeyHandle(event, app);
        break;
        
    case SDL_EVENT_MOUSE_MOTION:
        //app->mouseX = event->motion.x;
        //app->mouseY = event->motion.y;

        float x,y;
        SDL_RenderCoordinatesFromWindow(app->renderer, event->motion.x, event->motion.y,  &x,  &y);     //va prendre les coordonnée en fonction de comment est la window, sinon il peut y avoir des diff si on change la taille de la win
        app->mouseX = x;
        app->mouseY = y;
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        SDL_Log("button clicked: %d", event->button.button);
        SDL_Log("clicks: %d", event->button.clicks);
        break;

    default:
        break;
    }
    

    if (app->RUNNING == true){
        return SDL_APP_CONTINUE;
    } else {
        return SDL_APP_SUCCESS;
    }
}

SDL_AppResult SDL_AppIterate(void *appstate)        //sers a rafrechir l'ecran
{
    App *app =appstate;     //on load la window et surface qu'on a stock dans appstate

    //FPS:
        Uint64 currentTime = SDL_GetTicks();

        Uint64 tempEcoulerMilisec = currentTime - app->lastTime;
        float deltaTime = tempEcoulerMilisec / 1000.0f;        //converti tempecouler en seconde 

        app->lastTime = currentTime;
    SDL_Log("FPS: %d",tempEcoulerMilisec);






    

    Update(app, deltaTime);
    Render(app);

    

    SDL_Delay(16);

    if (app->RUNNING == true){
        return SDL_APP_CONTINUE;
    } else {
        return SDL_APP_SUCCESS;
    }
}



//MY FONCTIONS -----------------------------------------------------------------------------------------------------------


void Render(App *app)
{
    
    SDL_SetRenderDrawColor(app->renderer, 0x00, 0xAA, 0xFF, 0xFF);      //couleur du renderer
    SDL_RenderClear(app->renderer);   //va clear et set les nouveau pixel sur le render
    
    SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF);      //couleur du renderer
    SDL_RenderLine(app->renderer, (float) app->mouseX, (float) app->mouseY, 500.0f, 500.0f);
    SDL_RenderLine(app->renderer, (float) app->mouseX, (float) app->mouseY, 200.0f, 500.0f);


    //draw a texture
    SDL_FRect NyxRect;
    NyxRect.x = app->nyxith.rect.x;        //position
    NyxRect.y = app->nyxith.rect.y;
    NyxRect.w = 180;           //taille
    NyxRect.h = 180;
    if (!SDL_RenderTexture(app->renderer, app->texture, NULL, &NyxRect))
    {
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        
    }
    
    

    //ajouer les autre truc a dessiner ici
    SDL_RenderPresent(app->renderer);           //update le screen avec les chagement de render effectuer avant
  
}

void Update(App *app, Uint64 deltaTime)
{
    const bool *keys = SDL_GetKeyboardState(NULL);

    if(keys[SDL_SCANCODE_D])
    {
        app->nyxith.rect.x += app->nyxith.speed * deltaTime;
        SDL_Log("h");
    }

    if(keys[SDL_SCANCODE_A])
    {
        app->nyxith.rect.x -= app->nyxith.speed * deltaTime;
    }

}




void ShortKeyHandle(SDL_Event *event, App *app)
{
    //SDL_Log("smth was presssed: %d", event->key.key);

            switch (event->key.scancode)        //chaque touche font une action
            {
            case SDL_SCANCODE_E:
                SDL_Log("ouvrir inventaire");
                break;
            case SDL_SCANCODE_F:
                SDL_Log("ramasser");
                break;
            case SDL_SCANCODE_R:
                SDL_Log("rire (JSP OK)");   
                break;
            case SDL_SCANCODE_C:
                SDL_Log("s'acroupir ig");
                break;
            case SDL_SCANCODE_F11:
                app->Tfullscreen = !app->Tfullscreen;               
                SDL_SetWindowFullscreen(app->window, app->Tfullscreen);     //set/disapble fullscreen mode
                break;
            
            default:
                break;
            }




            switch (event->key.key)         //on peut faire a avec des num directement
            {
            case 27:                //esc == exit
                app->RUNNING = false;
                break;
            
            default:
                break;
            }
   
    
}


void AppDestroy(App *app)
{
    SDL_Log("\n[DEBUG] AppDestroy called.\n\n");
    if (!app) return;

    SDL_DestroyTexture(app->texture);
    SDL_DestroySurface(app->nyxith.surface);
    SDL_DestroyWindow(app->window);
    SDL_DestroyRenderer(app->renderer);
    SDL_free(app);
}






//QUIT -----------------------------------------------------------------------------------------------------------
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    App *app =appstate;

    AppDestroy(app);

    SDL_Log("end of program! all destroyed byeeee");
    SDL_Quit();
}





