#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "intersection.c"

typedef struct omem {
  Object *itemArray;
  size_t itemNum;
} omem;

int line = 1;

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  fscanf(json, "%lf", &value);
  // Error check this..
  return value;
}

double* next_vector(FILE* json) {
  double* v = malloc(3*sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}


struct omem read_scene(FILE* json) {
  int c;
  omem item;
  item.itemArray = NULL;
  item.itemNum = 0;

//error chk in main
  /*if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }*/

  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects

  while (1) {
    c = fgetc(json);
    if (c == ']') {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return item;
    }
    if (c == '{') {
      skip_ws(json);

      // Parse the object
      char* key = next_string(json);
      if (strcmp(key, "type") != 0) {
	       fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
	exit(1);
      }

      skip_ws(json);

      expect_c(json, ':');

      skip_ws(json);

      char* value = next_string(json);

    item.itemNum += 1;  //need to increment object counter
    item.itemArray = realloc(item.itemArray, sizeof(item)*item.itemNum); // allocate for new obj
    item.itemArray[item.itemNum-1].type = value; //set type



      if (strcmp(value, "camera") == 0) {
      } else if (strcmp(value, "sphere") == 0) {
      } else if (strcmp(value, "plane") == 0) {
      } else {
	fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
	exit(1);
      }

      skip_ws(json);

      while (1) {
	// , }
	c = next_c(json);
	if (c == '}') {
	  // stop parsing this object
	  break;
	} else if (c == ',') {
	  // read another field
	  skip_ws(json);
	  char* key = next_string(json);
	  skip_ws(json);
	  expect_c(json, ':');
	  skip_ws(json);
	  if ((strcmp(key, "width") == 0) ||
	      (strcmp(key, "height") == 0) ||
	      (strcmp(key, "radius") == 0)) {
	    double value = next_number(json);
      if(value <=0){
      fprintf(stderr, "ERROR: radius, height, or width is not correct num\n");
      exit(1);}
      else{
        // Put width, height, and radius vars in struct object
          if ((strcmp(key, "width") == 0))
          { item.itemArray[item.itemNum-1].camera.width = value; }
          else if ((strcmp(key, "height") == 0))
          { item.itemArray[item.itemNum-1].camera.height = value; }
          else if ((strcmp(key, "radius") == 0))
          { item.itemArray[item.itemNum-1].sphere.radius = value; } //sphere has radius cam doesnt
      }

	  } else if ((strcmp(key, "color") == 0) ||
		     (strcmp(key, "position") == 0) ||
		     (strcmp(key, "normal") == 0)) {
	    double* value = next_vector(json);
      if ((strcmp(key, "color") == 0)) {
           item.itemArray[item.itemNum-1].color[0] = value[0];
           item.itemArray[item.itemNum-1].color[1] = value[1];
           item.itemArray[item.itemNum-1].color[2] = value[2];
           printf("Here is the color: %lf, %lf, %lf\n", item.itemArray[item.itemNum-1].color[0],
                                                        item.itemArray[item.itemNum-1].color[1],
                                                        item.itemArray[item.itemNum-1].color[2]);
         } else if ((strcmp(key, "position") == 0)) {
           item.itemArray[item.itemNum-1].sphere.location[0] = value[0];
           item.itemArray[item.itemNum-1].sphere.location[1] = value[1];
           item.itemArray[item.itemNum-1].sphere.location[2] = value[2];
           printf("Here is the position: %lf, %lf, %lf\n", item.itemArray[item.itemNum-1].sphere.location[0],
                                                           item.itemArray[item.itemNum-1].sphere.location[0],
                                                           item.itemArray[item.itemNum-1].sphere.location[2]);
         } else if ((strcmp(key, "normal") == 0)) {
           item.itemArray[item.itemNum-1].plane.normal[0] = value[0];
           item.itemArray[item.itemNum-1].plane.normal[1] = value[1];
           item.itemArray[item.itemNum-1].plane.normal[2] = value[2];
           printf("Here is the normal: %lf, %lf, %lf\n", item.itemArray[item.itemNum-1].plane.normal[0],
                                                         item.itemArray[item.itemNum-1].plane.normal[1],
                                                         item.itemArray[item.itemNum-1].plane.normal[2]);
         }
	  } else {
	    fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",key, line);
	    //char* value = next_string(json);
      exit(1);
	  }
	  skip_ws(json);
	} else {
	  fprintf(stderr, "Error: Unexpected value on line %d\n", line);
	  exit(1);
	}
      }
      skip_ws(json);
      c = next_c(json);
      if (c == ',') {
	// noop
	skip_ws(json);
      } else if (c == ']') {
	fclose(json);
	return item;
      } else {
	fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
	exit(1);
      }
    }
  }
}
