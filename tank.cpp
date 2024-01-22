#include "tank.h"

Bullet :: Bullet(double x, double y, double speed_x, double speed_y
        , int size, int team, SDL_Color color, SDL_Texture *bullet){
    this -> x = x;
    this -> y = y;
    this -> speed_x = speed_x;
    this -> speed_y = speed_y;
    this -> color = color;
    this -> team = team;
    
    SDL_QueryTexture(bullet, NULL, NULL, &hit.w, &hit.h);
    hit.w = hit.w / size;
    hit.h = hit.h / size;

    this -> x -= hit.w / 2;
    this -> y -= hit.h / 2;
}

void Bullet :: update(){
    x += speed_x;
    y += speed_y;
    hit.x = (int) x;
    hit.y = (int) y;
}

void Bullet :: render(SDL_Renderer *renderer, SDL_Texture *bullet){
    SDL_SetTextureColorMod(bullet, color.r, color.g, color.b);
    SDL_RenderCopyEx(renderer, bullet, NULL, &hit, angle, NULL, SDL_FLIP_NONE);
}

Tank :: Tank (int x, int y, int size, int team, SDL_Texture *tank){
    this->x = x;
    this->y = y;
    SDL_QueryTexture(tank, NULL, NULL, &hit.w, &hit.h);
    hit.w = hit.w / size;
    hit.h = hit.h / size;

    center.x = hit.w / 2;
    center.y = hit.h * 2 / 3;

    this->x -= center.x;
    this->y -= center.y;

    this->size = size;
    this->team = team;
    switch(team){
        case -1:
            color.r = 255;
            color.g = 255;
            color.b = 255;
            break;
        case 0:
            color.r = 255;
            color.g = 0;
            color.b = 255;
            break;
        case 1:
            color.r = 0;
            color.g = 255;
            color.b = 0;
            break;
        case 2:
            color.r = 0;
            color.g = 0;
            color.b = 255;
            break;
        default:
            color.r = 255;
            color.g = 255;
            color.b = 255;
    }
    color.a = 255;

    angle = 0;
    shoot_c = 0;
    dead = false;
}

void Tank :: update(int screen_w, int screen_h){
    if(x < 0)
        x = 0;
    else if(x + hit.w > screen_w)
        x = screen_w - hit.w;

    if(270 <= angle || angle < 90){
        if(y < -hit.w / 2)
        y = -hit.w / 2;
        else if(y + hit.h> screen_h)
        y = screen_h - hit.h;
    }else
        if(y + hit.w / 2 < 0)
        y = -hit.w / 2 ;
        else if(y + hit.h> screen_h)
        y = screen_h - hit.h;
        
        
    if(shoot_c > 0){
        shoot_c --;
    }

    if(angle < 0)
        angle += 360;
    else if(angle >= 360)
        angle -= 360;
    hit.x = (int) x;
    hit.y = (int) y;
}

void Tank :: render(SDL_Renderer *renderer, SDL_Texture *tank){
    hit.x = (int) x;
    hit.y = (int) y;
    SDL_SetTextureColorMod(tank, color.r, color.g, color.b);
    SDL_RenderCopyEx(renderer, tank, NULL, &hit, angle, &center, SDL_FLIP_NONE);
}

void Tank :: shoot(std::list<Bullet> *bullets, SDL_Texture *bullet_texture){
    Bullet bullet(x + center.x + (sin(angle * 3.1416 / 180) * hit.h * 2 / 3)
    , y + center.y - (cos(angle * 3.1416 / 180) * hit.h * 2 / 3)
    , sin(angle * 3.1416 / 180) * size, -cos(angle * 3.1416 / 180) * size, size, team, color, bullet_texture);
    bullets->push_front(bullet);
}

bool Tank :: collition(Bullet bullet){
    int test_x, test_y;
    double distance;
    test_x = bullet.x + (bullet.hit.w / 2);
    test_y = bullet.y + (bullet.hit.h / 2);
    distance = (test_x - center.x - (int)x) * (test_x - center.x - (int)x);
    distance += (test_y - center.y - (int)y) * (test_y - center.y - (int)y);
    return (hit.w * 1.2 / 2) * (hit.w * 1.2 / 2) >= distance;
}