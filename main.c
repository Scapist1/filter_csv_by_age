#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINE_MAX 100
#define DELIM "," // CSV delimiter

char *ifile, *ofile;
unsigned filter_age_max;
FILE *istream, *ostream;

const char USAGE[] =
    R"(Filters CSV rows, keeping only those with provided maximum age
%1$s max-age [input-file] [output-file]

Example: 
%1$s max-age 17 input.csv output.csv
%1$s max-age 10 input.csv (outputs to stdout)
%1$s max-age 54           (inputs from stdin, outputs to stdout)
)";

void filter_stream(FILE *istream, FILE *ostream) {
  char line[LINE_MAX];
  char *fgets_return;
  char *name, *age_str;
  size_t line_no = 0;

  while (
      // Read a line from `istream` and assign the return value to
      // `fgets_return`
      (fgets_return = fgets(line, LINE_MAX, istream))) {
    ++line_no;

    if (fgets_return && *fgets_return != '\n') {
      if (strlen(line) > 1) {
        // Assign `name` and `age_str` using `strtok`

        name = strtok(line, DELIM);
        age_str = strtok(NULL, DELIM);

        // Alternative to strtok:
        // sscanf(line, "%*[^,],%d", &age);
        if (!name) {
          fprintf(stderr, "Line %zu: Name not given\n", line_no);
          continue;
        }
        if (!age_str) {
          // Error message
          fprintf(stderr, "Line %zu: Age not given\n", line_no);
          continue;
        }
      }
    } else {
      // Error message
      fprintf(stderr, "Line %zu: Nothing given\n", line_no);
      continue;
    }

    // Age processing
    unsigned age;
    auto recognized_count = sscanf(age_str, "%d", &age);
    if (recognized_count == 1) {
      if (age <= filter_age_max) {
        // Forward input line to `ostream`
        fprintf(ostream, "%s,%s", name, age_str);
      }
    } else {
      // Error message
      fprintf(stderr, "Line %zu: Age is not a number\n", line_no);
    }
  }
}

int main(int argc, char *argv[]) {
  switch (argc) {
  case 4:
    // max-age ifile ofile
    ofile = argv[3];
  case 3:
    // max-age ifile
    ifile = argv[2];
  case 2:
    // max-age
    if (!sscanf(argv[1], "%u", &filter_age_max)) {
      puts("First argument is not an age.");
      exit(EXIT_FAILURE);
    }
    break;
  default:
    printf(USAGE, argv[0]);
    return EXIT_SUCCESS;
  }

  if (ifile) {
    // Open `ifile` and assign it to `istream`
    istream = fopen(ifile, "r");

    // Exit program with an error message if file cannot be opened
    if (!istream) {
      fprintf(stderr, "Can't open file\n");

      return EXIT_FAILURE;
    }
  } else {
    // Set `istream` if no file provided
    istream = stdin;
  }

  if (ofile) {
    // Open `ofile` and assign it to `ostream`
    ostream = fopen(ofile, "w");
    if (!ostream) {
      fprintf(stderr, "Can't create file\n");

      return EXIT_FAILURE;
    }
  } else {
    // Set `ostream` if no file provided
    ostream = stdout;
  }

  filter_stream(istream, ostream);
}