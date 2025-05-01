#ifndef ARCFOUR_H
#define ARCFOUR_H

/*
https://stuff.mit.edu/afs/athena/contrib/crypto/src/ssh-1.2.27/arcfour.h
https://stuff.mit.edu/afs/athena/contrib/crypto/src/ssh-1.2.27/arcfour.c

ARCFOUR cipher (based on a cipher posted on the Usenet in Spring-95).
This cipher is widely believed and has been tested to be equivalent
with the RC4 cipher from RSA Data Security, Inc.  (RC4 is a trademark
of RSA Data Security)
*/

typedef struct
{
   unsigned int x;
   unsigned int y;
   unsigned char state[256];
} ArcfourContext;

/* Initializes the context and sets the key. */
void arcfour_init(ArcfourContext *ctx, const unsigned char *key, 
		  unsigned int keylen);

/* Returns the next pseudo-random byte from the arcfour (pseudo-random 
   generator) stream. */
unsigned int arcfour_byte(ArcfourContext *ctx);

/* Encrypts data. */
void arcfour_encrypt(ArcfourContext *ctx, unsigned char *dest, 
		     const unsigned char *src, unsigned int len);

/* Decrypts data. */
void arcfour_decrypt(ArcfourContext *ctx, unsigned char *dest, 
		     const unsigned char *src, unsigned int len);

void arcfour_init(ArcfourContext *ctx, const unsigned char *key, 
		  unsigned int key_len)
{
  unsigned int t, u;
  unsigned int keyindex;
  unsigned int stateindex;
  unsigned char* state;
  unsigned int counter;

  state = &ctx->state[0];
  ctx->x = 0;
  ctx->y = 0;
  for (counter = 0; counter < 256; counter++)
    state[counter] = counter;
  keyindex = 0;
  stateindex = 0;
  for (counter = 0; counter < 256; counter++)
    {
      t = state[counter];
      stateindex = (stateindex + key[keyindex] + t) & 0xff;
      u = state[stateindex];
      state[stateindex] = t;
      state[counter] = u;
      if (++keyindex >= key_len)
	keyindex = 0;
    }
}

unsigned int arcfour_byte(ArcfourContext *ctx)
{
  unsigned int x;
  unsigned int y;
  unsigned int sx, sy;
  unsigned char *state;

  state = ctx->state;
  x = (ctx->x + 1) & 0xff;
  sx = state[x];
  y = (sx + ctx->y) & 0xff;
  sy = state[y];
  ctx->x = x;
  ctx->y = y;
  state[y] = sx;
  state[x] = sy;
  return state[(sx + sy) & 0xff];
}

void arcfour_encrypt(ArcfourContext *ctx, unsigned char *dest, 
		     const unsigned char *src, unsigned int len)
{
  unsigned int i;
  for (i = 0; i < len; i++)
    dest[i] = src[i] ^ arcfour_byte(ctx);
}

void arcfour_decrypt(ArcfourContext *ctx, unsigned char *dest, 
		 const unsigned char *src, unsigned int len)
{
  arcfour_encrypt(ctx, dest, src, len);
}

#endif /* ARCFOUR_H */
