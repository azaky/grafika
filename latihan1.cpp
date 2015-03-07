#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/conio.h"
#include "lib/framebuffer.h"

using namespace std;

FrameBuffer fb;

void rectangle(int x1, int y1, int x2, int y2, Color p, int width = 1) {
	for (int x = x1; x <= x2; ++x) {
		for (int y = -width; y <= width; ++y) {
			fb.set(x, y1 + y, p);
			fb.set(x, y2 + y, p);
		}
	}
	for (int y = y1; y <= y2; ++y) {
		for (int x = -width; x <= width; ++x) {
			fb.set(x + x1, y, p);
			fb.set(x + x2, y, p);
		}
	}
}

int main(int argc, char** argv) {
	// screen size
	int width = fb.getXSize();
	int height = fb.getYSize();

	// color picker size
	int size = 1;
	if (argc > 1) {
		size = atoi(argv[1]);
		if (size < 1) size = 1;
	}
	// generate rgb table
	int _r[256 * 6], _g[256 * 6], _b[256 * 6];
	_r[0] = 255, _g[0] = 0, _b[0] = 0;
	// red to yellow
	for (int i = 1; i < 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1] + 1;
		_b[i] = _b[i-1];
	}
	_r[256 * 1] = _r[256 * 1 - 1];
	_g[256 * 1] = _g[256 * 1 - 1];
	_b[256 * 1] = _b[256 * 1 - 1];
	// yellow to green
	for (int i = 256 + 1; i < 2 * 256; ++i) {
		_r[i] = _r[i-1] - 1;
		_g[i] = _g[i-1];
		_b[i] = _b[i-1];
	}
	_r[256 * 2] = _r[256 * 2 - 1];
	_g[256 * 2] = _g[256 * 2 - 1];
	_b[256 * 2] = _b[256 * 2 - 1];
	// green to cyan
	for (int i = 2 * 256 + 1; i < 3 * 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1];
		_b[i] = _b[i-1] + 1;
	}
	_r[256 * 3] = _r[256 * 3 - 1];
	_g[256 * 3] = _g[256 * 3 - 1];
	_b[256 * 3] = _b[256 * 3 - 1];
	// cyan to blue
	for (int i = 3 * 256 + 1; i < 4 * 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1] - 1;
		_b[i] = _b[i-1];
	}
	_r[256 * 4] = _r[256 * 4 - 1];
	_g[256 * 4] = _g[256 * 4 - 1];
	_b[256 * 4] = _b[256 * 4 - 1];
	// blue to purple
	for (int i = 4 * 256 + 1; i < 5 * 256; ++i) {
		_r[i] = _r[i-1] + 1;
		_g[i] = _g[i-1];
		_b[i] = _b[i-1];
	}
	_r[256 * 5] = _r[256 * 5 - 1];
	_g[256 * 5] = _g[256 * 5 - 1];
	_b[256 * 5] = _b[256 * 5 - 1];
	// purple to red
	for (int i = 5 * 256 + 1; i < 6 * 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1];
		_b[i] = _b[i-1] - 1;
	}
	// position for color pallette
	int position = 0, px = 256 * size - 1, py = 0, mode = 0;

	// tampilkan informasi
	system("clear");
	puts("Tekan wsda/arrow key untuk mengubah warna.");
	puts("Tekan enter untuk masuk mode memilih warna.");
	puts("Tekan escape 2 kali untuk keluar.");
	puts("Pada mode memilih warna:");
	puts("- Tekan wsda/arrow key untuk mengubah warna.");
	puts("- Tekan enter untuk mengetahui warna pada titik tersebut.");

	int cmd = ' ';
	while (true) {
		// tampilin color palette
		for (int x = 0; x < size * 256; ++x) {
			for (int y = 0; y < size * 256; ++y) {
				int r = _r[position], g = _g[position], b = _b[position];
				// ke bawah dulu
				r = r - (r * y) / (size * 256);
				g = g - (g * y) / (size * 256);
				b = b - (b * y) / (size * 256);
				// baru ke kiri
				int target = 255 - y / size;
				r = target + x * (r - target) / (size * 256);
				g = target + x * (g - target) / (size * 256);
				b = target + x * (b - target) / (size * 256);
				// set Color
				fb.set(width - 100 - size * 256 + x, 100 + y, r, g, b);
			}
		}
		// right sidebar
		for (int y = 0; y < size * 256; ++y) {
			int p = y * 6 / size;
			int r = _r[p], g = _g[p], b = _b[p];
			for (int x = 0; x < 20; ++x) {
				fb.set(width - 90 + x, 100 + y, r, g, b);
			}
		}
		// mark position
		int ypos = position * size / 6;
		rectangle(width - 90, 97 + ypos, width - 70, 103 + ypos, Color(1, 1, 1));
		if (mode == 1) {
			rectangle(width - 100 - size * 256 + px - 3, 100 + py - 3,
				width - 100 - size * 256 + px + 3, 100 + py + 3, Color(1, 1, 1));
		}
		// draw color
		Color color(1, 1, 1);
		if (mode == 1) {
			color = fb.get(width - 100 - size * 256 + px, 100 + py);
		}
		for (int x = 0; x < size * 256; ++x) {
			for (int y = 0; y < 20; ++y) {
				fb.set(width - 100 - size * 256 + x, 110 + size * 256 + y, color);
			}
		}
		fb.refresh();
		
		// baca piilhan
		cmd = getch();
		if (cmd == 27) {
			cmd = getch();
			if (cmd == 27) {
				break;
			}
			else if (cmd == 91) {
				cmd = getch();
				cmd = 64 - cmd;
			}
		}
		if (mode == 0) {
			if (cmd == 's' || cmd == 'd' || cmd == 'S' || cmd == 'D' || cmd == -2 || cmd == -3) {
				position += 6 / size;
				if (position >= 6 * 256) position = 0;
			}
			else if (cmd == 'a' || cmd == 'w' || cmd == 'A' || cmd == 'W' || cmd == -1 || cmd == -4) {
				position -= 6 / size;
				if (position < 0) position = 6 * 256 - 1;
			}
			else if (cmd == 10 || cmd == 13) {
				mode = 1;
			}
		}
		else if (mode == 1) {
			if (cmd == 10 || cmd == 13) {
				Color Color = fb.get(width - 100 - size * 256 + px, 100 + py);
				printf("warna = RGBA(%d, %d, %d, %d)\n", Color.red & 255, Color.green & 255, Color.blue & 255, Color.alpha & 255);
				mode = 0;
			}
			else if (cmd == 'a' || cmd == 'A' || cmd == -4) {
				px--;
				if (px < 0) px = 0;
			}
			else if (cmd == 'd' || cmd == 'D' || cmd == -3) {
				px++;
				if (px >= size * 256) px = size * 256 - 1;
			}
			else if (cmd == 'w' || cmd == 'W' || cmd == -1) {
				py--;
				if (py < 0) py = 0;
			}
			else if (cmd == 's' || cmd == 'S' || cmd == -2) {
				py++;
				if (py >= size * 256) py = size * 256 - 1;
			}
		}
	}
	return 0;
}
