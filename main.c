#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <malloc.h>

#include <openssl/md5.h>

void hexprint(const u_char *buffer, uint size) {
  const u_char *pos;
  for (pos = buffer; pos < buffer + size; ++pos) {
    printf("%02x", (uint)*pos);
  }
  puts("");
}

void hex2bin(const char *hex, u_char *bin, int binsize) {
  const char *hexpos = hex;
  u_char *binpos = bin;
  int v;
  for (; binpos < bin + binsize; ++binpos) {
    if (sscanf(hexpos, "%02x", &v))
      *binpos = v;
    hexpos += 2;
    while (*hexpos == ' ')
      ++hexpos;
  }
}

int calculate_points(u_char *org, u_char *cp, int len) {
  int points = 0, i;
  for (i = 0; i < len; i++) {
    if ((org[i] & 0xF0) == (cp[i] & 0xF0))
      ++points;
    else
      break;
    if ((org[i] & 0x0F) == (cp[i] & 0x0F))
      ++points;
    else
      break;
  }
  return points;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <file> <hash>\n", argv[0]);
    return EXIT_FAILURE;
  }

  MD5_CTX md5context;
  MD5_CTX modcontext;
  u_char md5hash[MD5_DIGEST_LENGTH];
  u_char orghash[MD5_DIGEST_LENGTH];
  char buffer[1024];
  int readcount;
  char *infilename = argv[1];
  FILE *file;

  hex2bin(argv[2], orghash, MD5_DIGEST_LENGTH);
  MD5_Init(&md5context);

  if ((file = fopen(infilename, "rb")) == NULL) {
    perror(infilename);
    return EXIT_FAILURE;
  }

  while ((readcount = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    MD5_Update(&md5context, &buffer, readcount);
  }
  fclose(file);

  memcpy(&modcontext, &md5context, sizeof(MD5_CTX));

  MD5_Final(md5hash, &md5context);
  hexprint(orghash, MD5_DIGEST_LENGTH);

  int a, b, c, best = 0, points;
  long long i;
  for (i = 0; i < 1000000000L; i++) {
    memcpy(&modcontext, &md5context, sizeof(MD5_CTX));
    a = rand();
    b = rand();
    c = rand();
    sprintf(buffer, "%d %d %d\n", a, b, c);
    MD5_Update(&modcontext, &buffer, strlen(buffer));
    MD5_Final(md5hash, &modcontext);
    points = calculate_points(orghash, md5hash, MD5_DIGEST_LENGTH);
    if (points > best) {
      printf("[%2d Points] %d %d %d: ", points, a, b, c);
      hexprint(md5hash, MD5_DIGEST_LENGTH);
      fflush(stdout); // Ctrl+C wont dump output if piping
      best = points;
    }
  }

  return 0;
}
