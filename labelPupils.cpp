#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>

using namespace cv;

const int defaultRadius = 10;
const int minRadius = 5;
// HACK! Resize window counts the title bar height and so was cutting off the top of the window
// I do not expect this to be portable to other systems (and definitely not to other OSes
// Probably solution is to get the Qt window handle and manually get sizes - will avoid this
// unless it in necessary.
const int osx_menu_height = 22;

Mat originalImage;
int radius;
bool drag;
int dragX;
int dragY;
char * filename;

int scaleFactor;

bool imageMarked;
int markX;
int markY;
int markRadius;

void drawImage(int x, int y) {
    Mat markedImage = originalImage.clone();
    Point cursor(x , y);
    circle( markedImage, cursor, radius, Scalar( 255, 0, 255 ) );

    if (imageMarked) {
        Point markCenter( markX, markY );
        circle( markedImage, markCenter, markRadius, Scalar( 255, 255, 0) );
    }

    imshow("Mark Pupils", markedImage);
    resizeWindow("Mark Pupils", markedImage.cols, markedImage.rows + osx_menu_height);
}

void save() {
    if (imageMarked) {
        printf("[%s] %d %d %d\n", filename, markX, markY, markRadius);
    }
    free(filename);
    exit(0);
}

void call_back_function(int event, int x, int y, int , void* ) {

    if (drag) {
        int newRadius = sqrt( (x-dragX)*(x-dragX) + (y-dragY)*(y-dragY) );
        if (newRadius > minRadius) radius = newRadius;
    } 
    if ( event == EVENT_LBUTTONDOWN ) {
        // Drag to set size with right click
       dragX = x;
       dragY = y;
       drag = true;
    } else if ( event == EVENT_LBUTTONUP ) {
        // Set a marker with left click
        markX = dragX;
        markY = dragY;
        drag = false;
        markRadius = radius;
        imageMarked = true;
    } else if ( event == EVENT_MBUTTONUP ) {
        // Delete markers with middle click
        imageMarked = false;
    } else if ( event == EVENT_RBUTTONUP ) {
        // Save a marking with right click and exit
        save();
        exit(0);

    }

    if (drag) {
        drawImage(dragX, dragY);
    } else {
        drawImage(x, y);
    }
}


int main ( int argc, char** argv ) {
    if (argc != 2) {
        printf("You must supply an image to load.");
        return 1;
    }
    originalImage = imread( argv[1] );
    radius = defaultRadius;
    drag = false;
    imageMarked = false;
    markX = 0;
    markY = 0;
    dragX = 0;
    dragY = 0;
    markRadius = 0;

    if (!originalImage.data) { printf("Error loading originalImage \n"); return -1; }

    filename = (char*) malloc(strlen(argv[1]) + 1);
    strcpy( filename, argv[1] );

    namedWindow("Mark Pupils", WINDOW_NORMAL );
    imshow("Mark Pupils", originalImage);
    resizeWindow("Mark Pupils", originalImage.cols, originalImage.rows + osx_menu_height);

    setMouseCallback("Mark Pupils", call_back_function, NULL);

    waitKey(0);
    free(filename);
    return 0;
}
