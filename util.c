/* small utility functions */

#include <stdlib.h>
#include "random.h"

void randomPermutation( int N, int *nodes )
{
  int i;
  for(i = 0; i < N; i++)
    nodes[i] = i;

  for(i = 0; i < N - 1; i++) {
    int s, t;
    int tmp;
    t = N - 1 - i;
    // s = (int) ((float)( t + 1 ) * trand() / (RAND_MAX + 1.0f));
    s = trand() % (t + 1);
    tmp = nodes[t];
    nodes[t] = nodes[s];
    nodes[s] = tmp;
  }
}

void clamp( float *f, float min, float max )
{
  if(*f < min) *f = min;
  else if(*f > max) *f = max;
}
