#include <iostream>
#include <SDL.h>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BALL_SIZE = 20;
const int radius = 10;
const int BALL_SPEED = 5.5;
const int PADDLE_WIDTH = 15;
const int PADDLE_HEIGHT = 150;
const int PADDLE_SPEED = 5;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

SDL_Window* window;
SDL_Renderer* renderer;

class Paddle {
public:
    float x, y;
    float width, height;
    int speed;

    Paddle(SDL_Renderer* renderer, float startX, float startY) {
        x = startX;
        y = startY;
        width = PADDLE_WIDTH;
        height = PADDLE_HEIGHT;
        speed = PADDLE_SPEED;
    }

    void draw_left(SDL_Renderer* renderer) {
        SDL_Rect paddle = { x, y, width, height }; 
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
        SDL_RenderFillRect(renderer, &paddle);
    }

    void draw_right(SDL_Renderer* renderer) {
        SDL_Rect paddle = { x, y, width, height };
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &paddle);
    }

    void left_update() {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W] && y > 0) // Nếu nhấn phím W và paddle chưa đến cạnh trên cửa sổ
        {
            y -= speed; // Di chuyển paddle lên
        }
        if (state[SDL_SCANCODE_S] && y + height < SCREEN_HEIGHT) // Nếu nhấn phím S và paddle chưa đến cạnh dưới cửa sổ
        {
            y += speed; // Di chuyển paddle xuống
        }
    }

    void right_update() {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP] && y > 0) // Nếu nhấn phím UP và paddle chưa đến cạnh trên cửa sổ
        {
            y -= speed; // Di chuyển paddle lên
        }
        if (state[SDL_SCANCODE_DOWN] && y + height < SCREEN_HEIGHT) // Nếu nhấn phím DOWN và paddle chưa đến cạnh dưới cửa sổ
        {
            y += speed; // Di chuyển paddle xuống
        }
    }
};

void SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = 0; w <= radius * 2; w++)
    {
        for (int h = 0; h <= radius * 2; h++)
        {
            int dx = w - radius;
            int dy = h - radius;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

class Ball {
public:
    float x, y; // Vị trí của quả bóng theo phương X, Y
    int speed_x, speed_y; // Vận tốc của quả bóng theo phương X, Y

    Ball(SDL_Renderer* renderer, float startX, float startY, int speedX, int speedY) {
        x = startX;
        y = startY;
        speed_x = speedX;
        speed_y = speedY;
    }

    // Vẽ quả bóng lên màn hình
    void draw(SDL_Renderer* renderer) {
        SDL_Rect ball = { x, y, BALL_SIZE, BALL_SIZE }; // Vị trí ban đầu của bóng
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Thiết lập màu
        SDL_RenderFillCircle(renderer, x, y, radius);
    }

    // Di chuyển quả bóng
    void update(Paddle& left_player, Paddle& right_player) {
        x += speed_x;
        y += speed_y;
        // Kiểm tra va chạm với cạnh trên và dưới của cửa sổ
        if (y < 0 || y + BALL_SIZE > SCREEN_HEIGHT)
        {
            speed_y = -speed_y;
        }

        // Kiểm tra va chạm với thanh trượt
        if (x - radius <= left_player.x + left_player.width && y >= left_player.y && y <= left_player.y + left_player.height)
        {
            speed_x = -speed_x;
        }

        if (x + radius >= right_player.x && y >= right_player.y && y <= right_player.y + right_player.height)
        {
            speed_x = -speed_x;
        }
    }
};

void render(Ball& ball, Paddle& left_player, Paddle& right_player) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int middleX = SCREEN_WIDTH / 2;
    SDL_RenderDrawLine(renderer, middleX, 0, middleX, SCREEN_HEIGHT);

    ball.draw(renderer);
    left_player.draw_left(renderer);
    right_player.draw_right(renderer);

    SDL_RenderPresent(renderer);
}

void reset_pos(Ball& ball, Paddle& left_player, Paddle& right_player) {
    // Đặt lại vị trí ban đầu cho bóng
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    ball.speed_x = BALL_SPEED * (rand() % 2 == 0 ? 1 : -1); // Đặt tốc độ ban đầu và hướng di chuyển ngẫu nhiên
    ball.speed_y = BALL_SPEED * (rand() % 2 == 0 ? 1 : -1);



    // Đặt lại vị trí ban đầu cho thanh trượt của người chơi trái
    left_player.x = 10;
    left_player.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;

    // Đặt lại vị trí ban đầu cho thanh trượt của người chơi phải
    right_player.x = SCREEN_WIDTH - 10 - PADDLE_WIDTH;
    right_player.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;

    // Đặt lại tốc độ di chuyển của thanh trượt
    left_player.speed = PADDLE_SPEED;
    right_player.speed = PADDLE_SPEED;
}

void move_ball_and_paddle(Ball& ball, Paddle& left_player, Paddle& right_player) {
    // Code di chuyển bóng và paddle
    ball.update(left_player, right_player);
    left_player.left_update();
    right_player.right_update();

    // Kiểm tra khi bóng chạm hai bên
    if (ball.x - radius == 0) {
        left_player.speed = 0;
        right_player.speed = 0;
        ball.speed_x = 0;
        ball.speed_y = 0;
        reset_pos(ball, left_player, right_player);
        SDL_Delay(1000);
    }
    else if (ball.x + radius == SCREEN_WIDTH) {
        left_player.speed = 0;
        right_player.speed = 0;
        ball.speed_x = 0;
        ball.speed_y = 0;
        reset_pos(ball, left_player, right_player);
        SDL_Delay(1000);
    }
}

bool is_paused = false;
int tempX, tempY;
void pause_game(Ball& ball, Paddle& left_player, Paddle& right_player) {
    if (is_paused == true)
    {
        // Đặt lại tốc độ của bóng
        ball.speed_x = tempX;
        ball.speed_y = tempY;
        // Đặt lại tốc độ di chuyển của thanh trượt
        left_player.speed = PADDLE_SPEED;
        right_player.speed = PADDLE_SPEED;

        is_paused = false;
    }
    else
    {   // Nhớ tốc độ trước khi pause
        tempX = ball.speed_x;
        tempY = ball.speed_y;
        // Cho bóng và thanh trượt đứng yên
        left_player.speed = 0;
        right_player.speed = 0;
        ball.speed_x = 0;
        ball.speed_y = 0;

        is_paused = true;
        SDL_Delay(100);
    }
}

void clean_up() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Ping Pong!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    Paddle left_player(renderer, 10, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);
    Paddle right_player(renderer, SCREEN_WIDTH - 10 - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);
    Ball ball(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, BALL_SPEED, BALL_SPEED);

    bool isRunning = true;
    while (isRunning) {

        Uint32 frameStart = SDL_GetTicks();
        // cập nhật và vẽ đồ họa
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
            {
                isRunning = false; // Thoát game nếu người dùng đóng cửa sổ
            }
            else if (event.type == SDL_KEYDOWN) // Nhấn phím P để tạm dừng
            {
                if (event.key.keysym.sym == SDLK_p)
                {
                    pause_game(ball, left_player, right_player);
                }
            }
        }
        //Cập nhật
        move_ball_and_paddle(ball, left_player, right_player);
        // Vẽ
        render(ball, left_player, right_player);
    }
    // Giải phóng
    clean_up();
    return 0;
}