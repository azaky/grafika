# grafika
IF3260 - Grafika

## Kompilasi

Terdapat 7 program, yaitu latihan[1-7].cpp dan uts.cpp. Petunjuk kompilasi adalah sebagai berikut:

	g++ namafile.cpp -o namafile -lpthread

`-lpthread` diperlukan karena terdapat kelas-kelas yang membutuhkan *multithreading*.

## Cara menjalankan

Program hanya dapat dijalankan pada *console view* di Linux. Tekan Ctrl + Alt + F[1-6] untuk membuka *console view*. Karena program akan mengakses file-file seperti `/dev/fb0.h` dan `/dev/input/event*`, maka dibutuhkan akses root untuk menjalankan program. Beberapa program hanya dapat dijalankan pada resolusi tepat 1366*768 pixel.
