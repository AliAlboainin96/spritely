#include "globals.h"
#include "util.h"
#include <SDL2/SDL_ttf.h>

struct Message_Queue
{
    unsigned char front, rear, size;
    unsigned capacity;
    char **messages;
    unsigned char displaying;
};

#define MESSAGE_BOX_HEIGHT 35

Message_Queue_t Message_Queue_create(const unsigned char capacity)
{
    Message_Queue_t queue = (Message_Queue_t)malloc(sizeof(Message_Queue_t));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->messages = (char **)malloc(sizeof(char *) * capacity);
    queue->displaying = 0;
    return queue;
}

void Message_Queue_free(Message_Queue_t queue)
{
    free(queue);
}

static int Message_Queue_isFull(Message_Queue_t queue)
{
    return (queue->size == queue->capacity);
}

static int Message_Queue_isEmpty(Message_Queue_t queue)
{
    return (queue->size == 0);
}

void Message_Queue_enqueue(Message_Queue_t queue, char *message)
{
    if (Message_Queue_isFull(queue))
        return;

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->messages[queue->rear] = strdup(message);

    queue->size++;
}

static void Message_Queue_dequeue(Message_Queue_t queue)
{
    if (!Message_Queue_isEmpty(queue))
    {
        queue->size--;
        queue->front = (queue->front + 1) % queue->capacity;
    }
}

static char *Message_Queue_front(Message_Queue_t queue)
{
    if (Message_Queue_isEmpty(queue))
    {
        return NULL;
    }
    else
    {
        return queue->messages[queue->front];
    }
}

void Message_box_render(Message_Queue_t queue)
{

    if (Message_Queue_isEmpty(queue))
        return;

    if (!queue->displaying)
    {
        last_time = SDL_GetTicks();
        queue->displaying = 1;
        return;
    }

    current_time = SDL_GetTicks();
    if (last_time + 500 > current_time)
    {
        if (TTF_Init() == -1)
        {
            printf("TTF_Init: %s\n", TTF_GetError());
            exit(2);
        }

        SDL_Rect message_rect;

        message_rect.w = SCREEN_WIDTH/8;
        message_rect.h = MESSAGE_BOX_HEIGHT;
        message_rect.x = 0;
        message_rect.y = SCREEN_HEIGHT - MESSAGE_BOX_HEIGHT;

        TTF_Font *Sans = TTF_OpenFont("fonts/OpenSans/OpenSans-SemiBold.ttf", 18);
        SDL_Color White = {255, 255, 255};
        SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, Message_Queue_front(queue), White);
        SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

        SDL_SetRenderDrawColor(renderer, 74, 50, 110, 255);
        SDL_RenderFillRect(renderer, &message_rect);

        SDL_RenderCopy(renderer, Message, NULL, &message_rect);
        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(Message);
        TTF_CloseFont(Sans);
    }
    else
    {
        queue->displaying = 0;
        Message_Queue_dequeue(queue);
    }
}
