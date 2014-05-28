#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gst/gst.h>


char * Gethome(char *path)
{
	char *ch;
	ch = malloc(sizeof(char)*500);
	strcpy(ch,getenv("HOME"));
	return strcat(ch,path);
}


void SetPidFile(char* Filename)
{
    FILE* f;

    f = fopen(Filename, "w+");
    if (f)
    {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}

void play()
{
	char *home = Gethome("/.local/battery.mp3");
	char path[500] = "file://";
	strcat(path,home);

	gst_init (0,0);
   
	GstElement *playbin2;
	GstBus *bus;
	GstMessage *msg;


	playbin2 = gst_element_factory_make ("playbin", "player");

	g_object_set (G_OBJECT(playbin2), "uri", path, NULL);
   	gst_element_set_state (GST_ELEMENT(playbin2), GST_STATE_PLAYING);
   
	bus = gst_element_get_bus (playbin2);
	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
   

	if (msg != NULL)
		gst_message_unref (msg);
	gst_object_unref (bus);
	gst_element_set_state (playbin2, GST_STATE_NULL);
	gst_object_unref (playbin2);
}

void work(int threshold)
{

	int c;
	unsigned short flag = 0;
	char inf[128], *p, buf[5];
	int stat;
	char *path = Gethome("/.cache/acpi_info.txt");

	for (;;)
	{
		if ((c = system("acpi > ~/.cache/acpi_info.txt")) == -1)
			exit(-1);

		FILE *acpiinf = fopen(path,"r");
		if (!acpiinf) exit(-1);

		fgets(inf,128,acpiinf);
		p = strtok(inf,",");
		p = strtok(NULL,"%");
		strcpy(buf,p);
		stat = atoi(buf);
		
		if (stat <= threshold && flag == 0)
		{
			play();
			flag = 1;
		}
		else if (stat > threshold)
			flag = 0;
		fclose(acpiinf);
		sleep(5);
	}
}
		
int main(int argc, char** argv)
{
	unsigned short th;

	if (argc != 2)
	{
		printf("Usage: ./demon X (X from 0 to 100)");
		exit(-1);
	}

	th = atoi(argv[1]);
	
	if (daemon(0,0) == -1) exit(-1);
	work(th);

	return 0;
}
