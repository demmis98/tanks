#include "list"
#include "math.h"
#include "SDL2/SDL.h"


class Bullet {
    public:
        SDL_Rect hit;
        SDL_Color color;
        double x;
        double y;
        double speed_x;
        double speed_y;
        double angle;
        int team;

        Bullet(double x, double y, double speed_x, double speed_y
        , int size, int team, SDL_Color color, SDL_Texture *bullet);
        void update();
        void render(SDL_Renderer *renderer, SDL_Texture *bullet);
};

class Tank {
    public :
        SDL_Rect hit;
        SDL_Point center;
        SDL_Color color;
        double x;
        double y;
        int size;
        int team;
        int shoot_c;
        double angle;
        bool dead;
        int temp;

        Tank(int x, int y, int size, int team, SDL_Texture *tank);
        void update(int screen_w, int screen_h);
        void render(SDL_Renderer *renderer, SDL_Texture *tank);

        void shoot(std::list<Bullet> *bullets, SDL_Texture *bullet_texture);
        bool collition(Bullet bullet);
};