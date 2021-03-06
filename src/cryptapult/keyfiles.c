#include <unistd.h>
#include <sodium.h>

int keygen(char *pk_fname, char *sk_fname)
{
    unsigned char pk[crypto_box_PUBLICKEYBYTES];
    unsigned char sk[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(pk, sk);
    FILE *buf;
    buf = fopen(pk_fname, "w");
    if (buf == NULL) {
        return 1;
    }
    if (fwrite(pk, sizeof(unsigned char), crypto_box_PUBLICKEYBYTES, buf)
        != crypto_box_PUBLICKEYBYTES) {
        fclose(buf);
        unlink(pk_fname);
        return 2;
    }
    fclose(buf);

    buf = fopen(sk_fname, "w");
    if (buf == NULL) {
        return 1;
    }
    if (fwrite(sk, sizeof(unsigned char), crypto_box_SECRETKEYBYTES, buf)
        != crypto_box_SECRETKEYBYTES) {
        fclose(buf);
        unlink(pk_fname);
        unlink(sk_fname);
        return 2;
    }
    fclose(buf);
    return 0;
}

/* Read whole file, expecting a certain size. pk should be at least expected bytes long
 * returns 0 on success, non-zero otherwise
 * 1 is "generic error"
 * 2 if the file is not as big as expected
 */
static int read_expected_size(char *fname, unsigned char *pk,
                              unsigned long expected)
{
    FILE *buf;
    //TODO: check that size matches
    buf = fopen(fname, "r");
    if (buf == NULL) {
        return 1;
    }
    fseek(buf, 0, SEEK_END);
    if (ftell(buf) != (signed long) expected) {
        fclose(buf);
        return 2;
    }
    rewind(buf);
    if (fread(pk, sizeof(unsigned char), expected, buf) !=
        (size_t) expected) {
        fclose(buf);
        return 1;
    }
    fclose(buf);
    return 0;
}

/* Read a public key from a file. Refer to read_expected_size as for return values */
int pk_read(char *fname, unsigned char *pk)
{
    int ret = read_expected_size(fname, pk, crypto_box_PUBLICKEYBYTES);
    if (ret == 2) {
        fprintf(stderr, "%s does not seem a public key (size mismatch)\n",
                fname);
    } else if (ret != 0) {
	fprintf(stderr, "Error reading %s\n", fname);
    }
    return ret;
}

/* Read a secret key from a file. Refer to read_expected_size as for return values */
int sk_read(char *fname, unsigned char *sk)
{
    int ret = read_expected_size(fname, sk, crypto_box_SECRETKEYBYTES);
    if (ret == 2) {
        fprintf(stderr, "%s does not seem a secret key (size mismatch)\n",
                fname);
    }
    return ret;
}
