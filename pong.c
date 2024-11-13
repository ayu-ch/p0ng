#include <stdio.h>
#include "host.h"
#include "joiner.h"

int main() {
    char choice;

    printf("Do you want to (h)ost or (j)oin a game? ");
    scanf(" %c", &choice);

    if (choice == 'h') {
        run_host();
    } else if (choice == 'j') {
	    run_joiner();
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}

