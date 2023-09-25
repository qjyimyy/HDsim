#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct device {
    int devnum;
    int len;
    int id;
} device;

struct cachedev {
    device *devices;
} cachedev;

typedef struct P {
    int a;
} P;

int main() {
    struct cachedev *dev = (struct cachedev*)malloc(sizeof(struct cachedev));
    dev->devices = malloc(4 * sizeof(device));
    device *d1 = &dev->devices[1];
    d1->devnum = 1;
    d1->id = 1002;
    d1->len = 7;
    
    device *d0 = &dev->devices[0];
    d0->devnum = 2;
    d0->id = 1005;
    d0->len = 3;

    P *p1 = NULL;
    printf("%d, %d, %d\n", dev->devices->devnum, dev->devices->id, dev->devices->len);
    return 0;
}