#ifndef MOUSE_H
#define MOUSE_H

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>	/* for input_event and input constants */
#include <linux/fb.h>		/* for frame buffer properties */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Mouse {
public:
	static void init() {
		// if the file descriptor is defined, no need to initialize
		if (isAvailable()) return;

		// reset all event_id with -1
		memset(event_id, 255, sizeof(event_id));

		// detect event number which handle mouse
		FILE *fdevices = fopen("/proc/bus/input/devices", "r");

		if (fdevices == NULL) {
			printf("Error reading input devices\n");
			exit(1);
		}

		char line[999];
		while (!feof(fdevices)) {
			fgets(line, 999, fdevices);
			char info[999];
			int temp_mouse_id = -1;
			int temp_event_id = -1;
			// looking for handler
			if (sscanf(line, "H: Handlers=%[^\n]s", info) == 1) {
				char *part = strtok(info, " ");
				while (part != NULL) {
					int temp;
					if (sscanf(part, "mouse%d", &temp) == 1) {
						temp_mouse_id = temp;
					} else if (sscanf(part, "event%d", &temp) == 1) {
						temp_event_id = temp;
					}
					part = strtok(NULL, " ");
				}
			}
			if (temp_mouse_id != -1) {
				event_id[temp_mouse_id] = temp_event_id;
			}
		}
		fclose(fdevices);

		char event_filename[20];

		// find the first available mouse
		bool available = false;
		for (int i = 0; i < 31; ++i) {
			if (event_id[i] != -1) {
				sprintf(event_filename, "/dev/input/event%d", event_id[i]);
				available = true;
				// access the device file
				mfd[i] = open(event_filename, O_RDONLY | O_NONBLOCK);
				if (mfd[i] == -1) {
					printf("Error opening device information (%s)\n", event_filename);
					exit(1);
				}
				else {
					// printf("mfd[%d] = %d, event%d\n", i, mfd[i], event_id[i]);
				}

			}
		}

		if (!available) {
			printf("No mouse is available\n");
			exit(1);
		}

		// reset anything
		x = y = 0;
		left = middle = right = 0;
		wheel = 0;

		// final touch: access /dev/input/fb0 to find out screen resolution
		int fbfd = open("/dev/fb0",O_RDONLY);
		struct fb_var_screeninfo vinfo;
		if (!fbfd) {
			printf("Error: cannot open framebuffer device.\n");
			exit(1);
		}
		// get variable screen information
		if (ioctl (fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
			printf("Error reading variable information.\n");
			exit(3);
		}

		xres = vinfo.xres;
		yres = vinfo.yres;

		close(fbfd);
	}

	static bool isAvailable() {
		bool available = false;
		for (int i = 0; i < 30; ++i) {
			if (mfd[i] != -1) {
				available = true;
			}
		}
		return available;
	}

	static void update() {
		if (!isAvailable()) init();

		struct input_event event;
		int ret;
		for (int i = 0; i < 30; ++i)
		{
			if (mfd[i] == -1) continue;
			while (ret = read(mfd[i], &event, sizeof(event)) != -1) {
				switch (event.type) {
					// nothing happened
					case EV_SYN:
						break;

					// key press
					case EV_KEY:
						switch (event.code) {
							// left key
							case BTN_LEFT:
								left = event.value;
								break;

							// right key
							case BTN_RIGHT:
								right = event.value;
								break;

							// middle key
							case BTN_MIDDLE:
								middle = event.value;
								break;
						}
						break;

					// relative movement
					case EV_REL:
						switch (event.code) {
							// x axis
							case REL_X:
								x += event.value;
								if (x < 0) x = 0;
								if (x >= xres) x = xres - 1;
								break;

							// y axis
							case REL_Y:
								y += event.value;
								if (y < 0) y = 0;
								if (y >= yres) y = yres - 1;
								break;

							// wheel
							case REL_WHEEL:
								wheel += event.value;
								break;
						}
						break;
				}
			}
		}
	}

	static void getAxis(int *x, int *y, int *wheel = NULL) {
		update();

		*x = Mouse::x;
		*y = Mouse::y;
		if (wheel != NULL) {
			*wheel = Mouse::wheel;
		}
	}

	static void getButton(int *left, int *middle, int *right) {
		update();

		*left = Mouse::left;
		*right = Mouse::right;
		*middle = Mouse::middle;
	}

	static int getXAxis() {
		update();

		return Mouse::x;
	}

	static int getYAxis() {
		update();

		return Mouse::y;
	}

	static int getWheelAxis() {
		update();

		return Mouse::wheel;
	}

	static int getLeftButton() {
		update();

		return Mouse::left;
	}

	static int getRightButton() {
		update();

		return Mouse::right;
	}

	static int getMiddleButton() {
		update();

		return Mouse::middle;
	}

private:
	Mouse() {}						// the constructor should be private
	static int mfd[31];					// file descriptor for mouse
	static int event_id[31];		// event identifiers for mice devices
	static int x, y;				// cursor position
	static int left, middle, right; // button stats
	static int wheel;				// wheel information
	static int xres, yres;			// screen resolution
};

// static variable initialization
int Mouse::mfd[31] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int Mouse::event_id[31];
int Mouse::x = 0;;
int Mouse::y = 0;
int Mouse::wheel = 0;
int Mouse::left = 0;
int Mouse::middle = 0;
int Mouse::right = 0;
int Mouse::xres = 0;
int Mouse::yres = 0;

#endif