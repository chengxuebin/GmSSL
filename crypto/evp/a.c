
/* crypto/evp/e_sms4.c */
#include <stdio.h>
#include "../cryptlib.h"

#ifndef OPENSSL_NO_SMS4
#include <openssl/evp.h>
#include <openssl/objects.h>
#include "evp_locl.h"
#include <openssl/sms4.h>
#define SMS4_IV_LENGTH	SMS4_BLOCK_SIZE

typedef struct {
	sms4_key_t ks;
} EVP_SMS4_KEY;

static int sms4_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
	const unsigned char *iv, int enc)
{
	if (!enc) {
		if (EVP_CIPHER_CTX_mode(ctx) == EVP_CIPH_OFB_MODE)
			enc = 1;
		else if (EVP_CIPHER_CTX_mode(ctx) == EVP_CIPH_CFB_MODE)
			enc = 1;  //encrypt key == decrypt key
	}

	if (enc)
                sms4_set_encrypt_key(ctx->cipher_data, key);
	else	sms4_set_decrypt_key(ctx->cipher_data, key);


	return 1;
}
static int 
sms4_cbc_cipher(EVP_CIPHER_CTX * ctx, unsigned char *out, const unsigned char *in, size_t inl)
{
	while (inl >= ((size_t) 1 << (sizeof(long) * 8 - 2))) {
		sms4_cbc_encrypt(in, out, (long)((size_t) 1 << (sizeof(long) * 8 - 2)), &((EVP_SMS4_KEY *) ctx->cipher_data)->ks, ctx->iv, ctx->encrypt);
		inl -= ((size_t) 1 << (sizeof(long) * 8 - 2));
		in += ((size_t) 1 << (sizeof(long) * 8 - 2));
		out += ((size_t) 1 << (sizeof(long) * 8 - 2));
	} if (inl)
		sms4_cbc_encrypt(in, out, (long)inl, &((EVP_SMS4_KEY *) ctx->cipher_data)->ks, ctx->iv, ctx->encrypt);
	return 1;
} static int	sms4_cfb128_cipher(EVP_CIPHER_CTX * ctx, unsigned char *out, const unsigned char *in, size_t inl){
	size_t		chunk = ((size_t) 1 << (sizeof(long) * 8 - 2));
	if (128 == 1)
		chunk >>= 3;
	if (inl < chunk)
		chunk = inl;
	while (inl && inl >= chunk) {
		sms4_cfb128_encrypt(in, out, (long)((128 == 1) && !(ctx->flags & 0x2000) ? inl * 8 : inl), &((EVP_SMS4_KEY *) ctx->cipher_data)->ks, ctx->iv, &ctx->num, ctx->encrypt);
		inl -= chunk;
		in += chunk;
		out += chunk;
		if (inl < chunk)
			chunk = inl;
	} return 1;
} static int	sms4_ecb_cipher(EVP_CIPHER_CTX * ctx, unsigned char *out, const unsigned char *in, size_t inl){
	size_t		i      , bl;
	bl = ctx->cipher->block_size;
	if (inl < bl)
		return 1;
	inl -= bl;
	for (i = 0; i <= inl; i += bl)
		sms4_ecb_encrypt(in + i, out + i, &((EVP_SMS4_KEY *) ctx->cipher_data)->ks, ctx->encrypt);
	return 1;
} static int	sms4_ofb_cipher(EVP_CIPHER_CTX * ctx, unsigned char *out, const unsigned char *in, size_t inl){
	while (inl >= ((size_t) 1 << (sizeof(long) * 8 - 2))) {
		sms4_ofb128_encrypt(in, out, (long)((size_t) 1 << (sizeof(long) * 8 - 2)), &((EVP_SMS4_KEY *) ctx->cipher_data)->ks, ctx->iv, &ctx->num);
		inl -= ((size_t) 1 << (sizeof(long) * 8 - 2));
		in += ((size_t) 1 << (sizeof(long) * 8 - 2));
		out += ((size_t) 1 << (sizeof(long) * 8 - 2));
	} if (inl)
		sms4_ofb128_encrypt(in, out, (long)inl, &((EVP_SMS4_KEY *) ctx->cipher_data)->ks, ctx->iv, &ctx->num);
	return 1;
} static const EVP_CIPHER sms4_cbc = {978, 16, 16, 16, 0 | 0x2, sms4_init_key, sms4_cbc_cipher, ((void *)0), sizeof(EVP_SMS4_KEY), ((void *)0), ((void *)0), ((void *)0), ((void *)0)};
const EVP_CIPHER *EVP_sms4_cbc(void){
	return &sms4_cbc;
} static const EVP_CIPHER sms4_cfb128 = {982, 1, 16, 16, 0 | 0x3, sms4_init_key, sms4_cfb128_cipher, ((void *)0), sizeof(EVP_SMS4_KEY), ((void *)0), ((void *)0), ((void *)0), ((void *)0)};
const EVP_CIPHER *EVP_sms4_cfb128(void){
	return &sms4_cfb128;
} static const EVP_CIPHER sms4_ofb = {981, 1, 16, 16, 0 | 0x4, sms4_init_key, sms4_ofb_cipher, ((void *)0), sizeof(EVP_SMS4_KEY), ((void *)0), ((void *)0), ((void *)0), ((void *)0)};
const EVP_CIPHER *EVP_sms4_ofb(void){
	return &sms4_ofb;
} static const EVP_CIPHER sms4_ecb = {977, 16, 16, 0, 0 | 0x1, sms4_init_key, sms4_ecb_cipher, ((void *)0), sizeof(EVP_SMS4_KEY), ((void *)0), ((void *)0), ((void *)0), ((void *)0)};
const EVP_CIPHER *
EVP_sms4_ecb(void)
{
	return &sms4_ecb;
}
#endif
