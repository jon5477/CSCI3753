#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define DEVICE "/dev/simple_character_device"
#define BUFFER_SIZE 1024

int main () {
	char command, buffer[BUFFER_SIZE];
	int file = open(DEVICE, O_RDWR);
	while (1) {
		printf("\nr) Read from device\nw) Write to device\ne) Exit device\nAnything else to continue reading and writing\n\nEnter command: ");
		scanf("%c", &command);
		switch (command) {
			case 'w': 
			case 'W':
				printf("Enter data you want to write to the device: ");
				scanf("%s", buffer);
				write(file, buffer, BUFFER_SIZE);
				while (getchar() != '\n');
				break;
			case 'r': 
			case 'R':
				read(file, buffer, BUFFER_SIZE);
				printf("Device output: %s\n", buffer);
				while (getchar() != '\n');
				break;
			case 'e':
			case 'E':
				return 0;
			default:
				while (getchar() != '\n');
		}
	}
	close(file);
	return 0;
}
