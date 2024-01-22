#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "cmath"
#include "list"

#include "tank.h"


#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 360

using namespace std;

void init(Tank *player, list<Tank> *tanks, list<Bullet> *bullets, SDL_Texture *tank, SDL_Texture *bullet_texture);

void update(bool *running, Tank *player, list<Tank> *tanks, list<Bullet> *bullets, SDL_Texture *tank, SDL_Texture *bulletTexture);
void update_frame(Tank *player, list<Tank> *tanks, list<Bullet> *bullets, SDL_Texture *tank, SDL_Texture *bullet_texture);
void render(SDL_Renderer *renderer, SDL_Texture *tank, SDL_Texture *bullet_texture, Tank player, list<Tank> tanks, list<Bullet> bullets);


int main(){
  bool running;

  //open window
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *window = SDL_CreateWindow("tanks!", 64, 64, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

  uint32_t time_old = SDL_GetTicks(), time_new;

  //sprite sheet
  SDL_Texture *tank = IMG_LoadTexture(renderer, "img/tank.png");
  SDL_Texture *bullet_texture = IMG_LoadTexture(renderer, "img/bullet.png");
  Tank player(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1, 0, tank);

  list<Tank> tanks;
  list<Bullet> bullets;

  SDL_SetRenderDrawColor(renderer, 64, 0, 64, 255);

  init(&player, &tanks, &bullets, tank, bullet_texture);

  running = true;
  while(running){
    time_new = SDL_GetTicks();
    update(&running, &player, &tanks, &bullets, tank, bullet_texture);
    if(time_new - time_old > 1000/60){
      if(!player.dead)
        update_frame(&player, &tanks, &bullets, tank, bullet_texture);
      time_old = SDL_GetTicks();
    }
    render(renderer, tank, bullet_texture, player, tanks, bullets);
  }

  //end

  return 0;
}

void init(Tank *player, list<Tank> *tanks, list<Bullet> *bullets, SDL_Texture *tank, SDL_Texture *bullet_texture){
  *player = Tank(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1, 0, tank);
  tanks->clear();
  tanks->push_front(Tank(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, 1, 1, tank));
  tanks->push_front(Tank(SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT / 4, 1, 1, tank));
  tanks->push_front(Tank(SCREEN_WIDTH / 4, SCREEN_HEIGHT * 3 / 4, 1, 1, tank));
  tanks->push_front(Tank(SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT * 3 / 4, 1, 1, tank));
  bullets->clear();
}

void update(bool *running, Tank *player, list<Tank> *tanks, list<Bullet> *bullets, SDL_Texture *tank, SDL_Texture *bullet_texture){
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            *running = false;
            break;
          case SDLK_r:
            init(player, tanks, bullets, tank, bullet_texture);
        }
        break;
      case SDL_QUIT:
        *running = false;
        break;
    }
  }
}

void update_frame(Tank *player, list<Tank> *tanks, list<Bullet> *bullets, SDL_Texture *tank, SDL_Texture *bullet_texture){
  const Uint8* state = SDL_GetKeyboardState(nullptr);
  if(state[SDL_SCANCODE_UP]){
    player->x += sin(player->angle * 3.1416 / 180);
    player->y -= cos(player->angle * 3.1416 / 180);
  }
  else if(state[SDL_SCANCODE_DOWN]){
    player->x -= sin(player->angle * 3.1416 / 180);
    player->y += cos(player->angle * 3.1416 / 180);
  }

  if(state[SDL_SCANCODE_LEFT]){
    player->angle -=1;
  }
  else if(state[SDL_SCANCODE_RIGHT]){
    player->angle +=1;
  }

  if(state[SDL_SCANCODE_SPACE])
    if(player->shoot_c == 0){
      player->shoot(bullets, bullet_texture);
      player->shoot_c = 60;
    }

  player->update(SCREEN_WIDTH, SCREEN_HEIGHT);

  list<Tank> :: iterator t = tanks->begin();
  while(t != tanks->end() && tanks->size() != 0){
    t->update(SCREEN_WIDTH, SCREEN_HEIGHT);
    if(t->shoot_c == 0){
      t->shoot_c = rand() % 30 + 30;
      t->temp = rand()%6;

      if(t->temp < 3)
        t->shoot(bullets, bullet_texture);
    }
    switch(t->temp){
      case 0:
        t->angle += 1;
        break;
      case 1:
        t->angle -= 1;
        break;
      case 2:
        t->x += sin(t->angle * 3.1416 / 180);
        t->y -= cos(t->angle * 3.1416 / 180);
        break;
      case 3:
        t->x -= sin(t->angle * 3.1416 / 180);
        t->y += cos(t->angle * 3.1416 / 180);
        break;
    }
    if(t->dead){
      if(t->size == 3)
        tanks->erase(t++);
      else{
        t->dead = false;
        t->angle += 90;
        t->size++;
        *t = Tank(t->x, t->y, t->size, t->team, tank);
        t->temp = 2;
        tanks->push_front(*t);
        t->angle -= 180;
      }
    }
    else
      t++;
  }

  list<Bullet> :: iterator b = bullets->begin();
  while(b != bullets->end() && bullets->size() != 0){
    bool hit = false;

    b->update();
    
    if(b->team == 0)
      for(t = tanks->begin(); t != tanks->end(); t++){
        if(t->team != b->team)
          if(t->collition(*b)){
            t->dead = true;
            hit = true;
          }
      }
    else
      if(player->collition(*b)){
        player->dead = true;
        b->color.r = 255;
        b->color.g = 255;
        b->color.b = 255;
      }
    
    if(b->x < -b->hit.w || b->y < -b->hit.h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT || hit)
      bullets->erase(b++);
    else
      b++;
  }
}

void render(SDL_Renderer *renderer, SDL_Texture *tank, SDL_Texture *bullet_texture, Tank player, list<Tank> tanks, list<Bullet> bullets){
  SDL_RenderClear(renderer); //clear screen

  for(auto t : tanks)
    t.render(renderer, tank);

  player.render(renderer, tank);

  for(auto b : bullets)
    b.render(renderer, bullet_texture);

  SDL_RenderPresent(renderer);
}