/*
 * confcrypt.c
 * 
 * Copyright 2014 Mikael Wågberg <mickew@Ubuntu-VMDT>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>
#include <errno.h>
#include <time.h>
#include "support.h"

int c_encrypt(const char *pw, int pwlen, char **resp, int *reslenp) 
{
    char ht[20], h1[20], h2[20], h3[20], h4[20], key[24];
    const char *iv = h1;
    char *res, *enc, *tmp;
    int i, enclen = (pwlen%8) ? ((pwlen/8)+1)*8 : pwlen;

    gcry_cipher_hd_t ctx;
    time_t rawtime;
 
    time(&rawtime);
    tmp = ctime(&rawtime);

    /* h1 = SHA1 of ctime - bad source for entropy */
    gcry_md_hash_buffer(GCRY_MD_SHA1, h1, tmp, strlen(tmp));

    /* ht = temporary hash */
    memcpy(ht, h1, 20);

    /* h2 = SHA1 of modified h1*/
    ht[19]++;
    gcry_md_hash_buffer(GCRY_MD_SHA1, h2, ht, 20);

    /* h3 = SHA1 of modified h2 */
    ht[19] += 2;
    gcry_md_hash_buffer(GCRY_MD_SHA1, h3, ht, 20);

    /* key = h2 + (4 bytes of h3) */
    memcpy(key, h2, 20);
    memcpy(key+20, h3, 4);

    /* allocate buffer for in-place encryption */
    enc = malloc(enclen);
    if(enc == NULL) {
        return -1;
    }

    memcpy(enc, pw, pwlen);

    /* padding */
    for(i=pwlen; i<enclen; i++) {
        enc[i] = enclen - pwlen;
    }

    /* encrypt password with 3DES with iv = 8 bytes of h1, key = see above */
    gcry_cipher_open(&ctx, GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_CBC, 0);
    gcry_cipher_setkey(ctx, key, 24);
    gcry_cipher_setiv(ctx, iv, 8);
    gcry_cipher_encrypt(ctx, (unsigned char *)enc, enclen, NULL, 0);
    gcry_cipher_close(ctx);

    /* h4 = SHA1 of encrypted password */
    gcry_md_hash_buffer(GCRY_MD_SHA1, h4, enc, enclen);

    /* hash length */    
    *reslenp = enclen+40;
    res = malloc(*reslenp);

    /* hash = h1 | h4 | encrypted password */
    memcpy(res, h1, 20);
    memcpy(res+20, h4, 20);
    memcpy(res+40, enc, enclen);
    
    *resp = res;

    free(enc);
    return 0;
}

void bin_to_strhex(unsigned char *bin, unsigned int binsz, char **result)
{
  char          hex_str[]= "0123456789ABCDEF";
  unsigned int  i;

  *result = (char *)malloc(binsz * 2 + 1);
  (*result)[binsz * 2] = 0;

  if (!binsz)
    return;

  for (i = 0; i < binsz; i++)
    {
      (*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
      (*result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
    }  
}

int c_decrypt(char *ct, int len, char **resp, char *reslenp)
{
	const char *h1  = ct;
	const char *h4  = ct + 20;
	const char *enc = ct + 40;
	
	char ht[20], h2[20], h3[20], key[24];
	const char *iv = h1;
	char *res;
	gcry_cipher_hd_t ctx;
	int reslen;
	
	if (len < 48)
		return 0;
	len -= 40;
	
	memcpy(ht, h1, 20);
	
	ht[19]++;
	gcry_md_hash_buffer(GCRY_MD_SHA1, h2, ht, 20);
	
	ht[19] += 2;
	gcry_md_hash_buffer(GCRY_MD_SHA1, h3, ht, 20);
	
	memcpy(key, h2, 20);
	memcpy(key+20, h3, 4);
	/* who cares about parity anyway? */
	
	gcry_md_hash_buffer(GCRY_MD_SHA1, ht, enc, len);
	
	if (memcmp(h4, ht, 20) != 0)
		return -1;
	
	res = malloc(len);
	if (res == NULL)
		return -1;
	
	gcry_cipher_open(&ctx, GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_CBC, 0);
	gcry_cipher_setkey(ctx, key, 24);
	gcry_cipher_setiv(ctx, iv, 8);
	gcry_cipher_decrypt(ctx, (unsigned char *)res, len, (unsigned char *)enc, len);
	gcry_cipher_close(ctx);
	
	reslen = len - res[len-1];
	res[reslen] = '\0';
	
	if (resp)
		*resp = res;
	if (reslenp)
		*reslenp = reslen;
	return 0;
}

int hex2bin_c(unsigned int c)
{
	if ((c >= '0')&&(c <= '9'))
		return c - '0';
	if ((c >= 'A')&&(c <= 'F'))
		return c - 'A' + 10;
	if ((c >= 'a')&&(c <= 'f'))
		return c - 'a' + 10;
	return -1;
}

int hex2bin(const char *str, char **bin, int *len)
{
	char *p;
	int i, l;
	
	if (!bin)
		return EINVAL;
	
	for (i = 0; str[i] != '\0'; i++)
		if (hex2bin_c(str[i]) == -1)
			return EINVAL;
	
	l = i;
	if ((l & 1) != 0)
		return EINVAL;
	l /= 2;
	
	p = malloc(l);
	if (p == NULL)
		return ENOMEM;
	
	for (i = 0; i < l; i++)
		p[i] = hex2bin_c(str[i*2]) << 4 | hex2bin_c(str[i*2+1]);
	
	*bin = p;
	if (len)
		*len = l;
	
	return 0;
}

int conf_encrypt(const char *pw, char **result)
{
	int ret = 0, pwlen, hashlen;
	char *hash;

	pwlen = strlen(pw)+1;
	gcry_check_version("1.6.2");
	ret = c_encrypt(pw, pwlen, &hash, &hashlen);
	if (ret != 0)
	{
		free(hash);
		return ret;
	}
	bin_to_strhex((unsigned char *)hash, hashlen, result);
	free(hash);
	return ret;
}

int conf_decrypt(const char *encryptedpw, char **result)
{
	int len, ret = 0;
	char *bin;
	
	gcry_check_version("1.6.2");
	ret = hex2bin(encryptedpw, &bin, & len);
	if (ret != 0)
	{
		free(bin);
		return ret;
	}
	ret = c_decrypt(bin, len, result, NULL);
	free(bin);
	return ret;
}

