#include <windows.h>
#include <iostream>
using namespace std;
#include <pthread.h>
#include "src/libenjoy.h"




int axis[8] = {0, };
float faxis[8] = {0, 0, 0};
bool joy_connected = false;

pthread_t thread_joy;

void *threadJoy(void *arg);


void setup() {

	pthread_create(&thread_joy, NULL, threadJoy, NULL);
	
	while(1)
  {
    if (joy_connected == true)
    {  
		  //printf("axis\t 0:%1.2f 1:%1.2f 2:%1.2f %d\n", faxis[0], faxis[1], faxis[2], axis[0]);
    }
		delay(10);
  }
}

void loop()
{
  
}

void *threadJoy(void *arg)
{

	libenjoy_context *ctx;
	libenjoy_joy_info_list *info;

    ctx = libenjoy_init(); // initialize the library

    // Updates internal list of joysticks. If you want auto-reconnect
    // after re-plugging the joystick, you should call this every 1s or so
    libenjoy_enumerate(ctx);

    info = libenjoy_get_info_list(ctx);

    for (int i=0; i<info->count; i++)
    {
      printf("joystick %d: %s...\n",i, info->list[i]->name);
    }

    int joy_i;
    
    printf("connect num : ");
    scanf("%d", &joy_i);

    if (joy_i >= info->count)
    {
      joy_i = 0;
    }    

    if(info->count != 0) // just get the first joystick
    {
        libenjoy_joystick *joy;
        printf("Opening joystick %d:%s...", joy_i, info->list[joy_i]->name);
        joy = libenjoy_open_joystick(ctx, info->list[joy_i]->id);
        if(joy)
        {
            int counter = 0;
            libenjoy_event ev;
            joy_connected = true;

            printf("Success!\n");
            printf("Axes: %d btns: %d\n", libenjoy_get_axes_num(joy),libenjoy_get_buttons_num(joy));

            while(1)
            {
                // Value data are not stored in library. if you want to use
                // them, you have to store them

                // That's right, only polling available
                while(libenjoy_poll(ctx, &ev))
                {
                    switch(ev.type)
                    {
                    case LIBENJOY_EV_AXIS:
                    	axis[ev.part_id] =  ev.data;

                    	if (ev.data > 0)
                    	{
                    		faxis[ev.part_id] = (float)axis[ev.part_id] / 32767;
                    	}
                    	else
                    	{
                    		faxis[ev.part_id] = (float)axis[ev.part_id] / 32768;
                    	}
                        break;
                    case LIBENJOY_EV_BUTTON:
                        printf("%u: button %d val %d\n", ev.joy_id, ev.part_id, ev.data);
                        break;
                    case LIBENJOY_EV_CONNECTED:
                        printf("%u: status changed: %d\n", ev.joy_id, ev.data);
                        break;

                    default:
                      printf("ev.type %d %d %d\n", ev.type, ev.joy_id, ev.data);
                      break;
                    }
                    printf("ev.type %d %d %d\n", ev.type, ev.joy_id, ev.data);
                }
                //Sleep(1);

                counter += 50;
                if(counter >= 1000)
                {
                    libenjoy_enumerate(ctx);
                    counter = 0;
                }
            }

            // Joystick is really closed in libenjoy_poll or libenjoy_close,
            // because closing it while libenjoy_poll is in process in another thread
            // could cause crash. Be sure to call libenjoy_poll(ctx, NULL); (yes,
            // you can use NULL as event) if you will not poll nor libenjoy_close
            // anytime soon.
            libenjoy_close_joystick(joy);
        }
        else
            printf("Failed!\n");
    }
    else
        printf("No joystick available\n");

    // Frees memory allocated by that joystick list. Do not forget it!
    libenjoy_free_info_list(info);

    // deallocates all memory used by lib. Do not forget this!
    // libenjoy_poll must not be called during or after this call
    libenjoy_close(ctx);

  pthread_exit(NULL);
}


