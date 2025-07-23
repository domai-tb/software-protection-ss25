#include<stdio.h>
#include <string.h>

int validate(char *user, char *pass) {
	if (strlen(user) != strlen(pass)) {
		return 0;
	}
	else if (strlen(user) < 2) {
		return 0;
	}
	int i = 0;
	for (i = 0; i < strlen(user); ++i) {
		pass[i] ^= 0x17;
		if (user[i] != pass[i]) {
			return 0;
		}
	}
	return 1;
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		return -1;
	}
	char *user = argv[1];
	char *pass = argv[2];
	printf("user: %s\n", user);
	printf("license: %s\n", pass);
	printf("\n");
	if (validate(user, pass)) {
		printf("License accepted.\n");
	} else {
		printf("Invalid license. Please buy our software and save our children from starvation.\n");
	}
	return 0;
}
