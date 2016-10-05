
#include "intersection.h"
#include "parser.h"

int main(int argc, char *argv[]) {

  //Error checking for input
  // Check width is a valid number
  if (atoi(argv[1]) <= 0) {
    fprintf(stderr, "ERROR: need a valid width greater than 0\n");
    return(1);
  }

  //check height is valid
  if (atoi(argv[2]) <= 0) {
    fprintf(stderr, "ERROR: need a valid height greater than 0\n");
    return(1);
  }

  // check argument number is correct
  if (argc != 5) {
  fprintf(stderr, "ERROR: you must include 4 arguments. `file <width> <height> <json.json> <output.ppm>`");
  return(1);
  }


// open the json file
FILE* input = fopen(argv[3], "rb");

if (input == NULL) {
  fprintf(stderr, "ERROR: Something went wrong opening the file\n");
  return(1);
}
// declare the obj
  struct omem item;

  int itemnums;

  int cam = 0;
  double camHeight;
  double camWidth;

  int sph = 1;
  double sphR, sphG, sphB;
  double sphX;
  double sphY;
  double sphZ;
  double r;

  int pln = 2;
  double plnR, plnG, plnB;
  double plnX, plnY, plnZ;
  double NormalX, NormalY, NormalZ;

  // pixel width
  int W = atoi(argv[1]);
  // pixel height
  int H = atoi(argv[2]);


  item = read_scene(input);

  // get the number of objects from json file
  itemnums = item.itemNum;

  int i;

  int ppm = 6;

  // loop through the number of objects
  for (i = 0; i < itemnums; i++) {
    // if camera set w h
    if (item.itemArray[i].camera.width && item.itemArray[i].camera.height) {
      camWidth = item.itemArray[i].camera.width;
      camHeight = item.itemArray[i].camera.height;
    }
  }

  // error check for the camera width and height
  if (!camWidth || !camHeight) {
    fprintf(stderr, "ERROR: need to correct cam width and height\n");
    exit(1);
  }

  // set the pixel width and height
  double pixheight = camHeight / W;
  double pixwidth = camWidth / H;

  // create the image
  Image *img = (Image *)malloc(sizeof(Image));
  img->height = H;
  img->width = W;
  img->color = (char*)malloc(sizeof(char) * img->height * img->width * 4);


  // loop through the pixels
  for (int y = 0; y < W; y++) {
    for (int x = 0; x < H; x++) {
      double Ro[3] = {0, 0, 0};

      double Rd[3] = { item.itemArray[i].sphere.location[0] - (camWidth/2) + pixwidth * (x + 0.5), item.itemArray[i].sphere.location[1] - (camHeight/2) + pixheight * (y + 0.5), 1 };
      normalize(Rd);

      int xcol = 0;
      double best = INFINITY;
      // loop through the objects figuring out which one to trace
      for (i=0; i < itemnums; i++) {
        double t = 0;

        // if sphere
        if (item.itemArray[i].sphere.location && item.itemArray[i].sphere.radius) {
          t = sphere_intersection(Ro, Rd, item.itemArray[i].sphere.location, item.itemArray[i].sphere.radius);
        // if plane
      } else if (item.itemArray[i].plane.location && item.itemArray[i].plane.normal) {
          t = plane_intersection(Ro, Rd, item.itemArray[i].plane.location, item.itemArray[i].plane.normal);
        }

        if (t > 0 && t < best) {
          best = t;
          xcol = i;
        }
      }
      if (best > 0 && best != INFINITY) {
        if (item.itemArray[xcol].sphere.location && item.itemArray[xcol].sphere.radius) {
          pix_coloring(item.itemArray[xcol].color, y, x, img);
          printf("Shaded Sphere\n");
        } else if (item.itemArray[i].plane.location && item.itemArray[i].plane.normal) {
          pix_coloring(item.itemArray[xcol].color, y, x, img);
          printf("Shaded plane\n");
        }
        printf("#");
      } else {
        printf(".");
      }
    }
    printf("\n");
  }

  FILE* ppmout = fopen(argv[4], "w+");
  p6Create(img, ppmout, ppm);


  return 0;
}
