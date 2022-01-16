/*
* =====================================================================================
*       Copyright (c), 2013-2020.
*       Filename:  cipher_api.h
*
*    Description: 
 *         Others:
*
*        Version:  1.0
*        Created:  
*       Revision:  
*       Compiler:  
*
*         Author:  
*   Organization:  xxx
*
* =====================================================================================
*/
 
#ifndef __CIPHER_API_H__
#define __CIPHER_API_H__

struct crypto_skcipher *crypto_alloc_skcipher(const char *alg_name,
        u32 type, u32 mask);
/**
* skcipher_request_alloc() - allocate request data structure
* @tfm: cipher handle to be registered with the request
* @gfp: memory allocation flag that is handed to kmalloc by the API call.
*
* Allocate the request data structure that must be used with the skcipher
* encrypt and decrypt API calls. During the allocation, the provided skcipher
* handle is registered in the request data structure.
*
* Return: allocated request handle in case of success, or NULL if out of memory
*/
static inline struct skcipher_request *skcipher_request_alloc(
        struct crypto_skcipher *tfm, gfp_t gfp);
/**
* skcipher_request_set_crypt() - set data buffers
* @req: request handle
* @src: source scatter / gather list
* @dst: destination scatter / gather list
* @cryptlen: number of bytes to process from @src
* @iv: IV for the cipher operation which must comply with the IV size defined
*      by crypto_skcipher_ivsize
*
* This function allows setting of the source data and destination data
* scatter / gather lists.
*
* For encryption, the source is treated as the plaintext and the
* destination is the ciphertext. For a decryption operation, the use is
* reversed - the source is the ciphertext and the destination is the plaintext.
*/
static inline void skcipher_request_set_crypt(
       struct skcipher_request *req,
       char *src, char *dst,
       unsigned int cryptlen, void *iv);
/**
* skcipher_request_set_callback() - set asynchronous callback function
* @req: request handle
* @flags: specify zero or an ORing of the flags
*       CRYPTO_TFM_REQ_MAY_BACKLOG the request queue may back log and
*       increase the wait queue beyond the initial maximum size;
*       CRYPTO_TFM_REQ_MAY_SLEEP the request processing may sleep
* @compl: callback function pointer to be registered with the request handle
* @data: The data pointer refers to memory that is not used by the kernel
*      crypto API, but provided to the callback function for it to use. Here,
*      the caller can provide a reference to memory the callback function can
*      operate on. As the callback function is invoked asynchronously to the
*      related functionality, it may need to access data structures of the
*      related functionality which can be referenced using this pointer. The
*      callback function can access the memory via the "data" field in the
*      crypto_async_request data structure provided to the callback function.
*
* This function allows setting the callback function that is triggered once the
* cipher operation completes.
*
* The callback function is registered with the skcipher_request handle and
* must comply with the following template::
*
*    void callback_function(struct crypto_async_request *req, int error)
*/
static inline void skcipher_request_set_callback(struct skcipher_request *req,
                                          u32 flags,
                                          crypto_completion_t compl,
                                          void *data);
int crypto_skcipher_setkey(struct crypto_skcipher *tfm, const u8 *key,
        unsigned int keylen);
 
 
struct crypto_ahash *crypto_alloc_ahash(const char *alg_name, u32 type,
        u32 mask);
static inline struct ahash_request *ahash_request_alloc(
        struct crypto_ahash *tfm, gfp_t gfp);
/**
* ahash_request_set_crypt() - set data buffers
* @req: ahash_request handle to be updated
* @src: source scatter/gather list
* @result: buffer that is filled with the message digest -- the caller must
*        ensure that the buffer has sufficient space by, for example, calling
*        crypto_ahash_digestsize()
* @nbytes: number of bytes to process from the source scatter/gather list
*
* By using this call, the caller references the source scatter/gather list.
* The source scatter/gather list points to the data the message digest is to
* be calculated for.
*/
static inline void ahash_request_set_crypt(struct ahash_request *req,
        u8 *src, u8 *result,
        unsigned int nbytes);
/**
* ahash_request_set_callback() - set asynchronous callback function
* @req: request handle
* @flags: specify zero or an ORing of the flags
*       CRYPTO_TFM_REQ_MAY_BACKLOG the request queue may back log and
*       increase the wait queue beyond the initial maximum size;
*       CRYPTO_TFM_REQ_MAY_SLEEP the request processing may sleep
* @compl: callback function pointer to be registered with the request handle
* @data: The data pointer refers to memory that is not used by the kernel
*      crypto API, but provided to the callback function for it to use. Here,
*      the caller can provide a reference to memory the callback function can
*      operate on. As the callback function is invoked asynchronously to the
*      related functionality, it may need to access data structures of the
*      related functionality which can be referenced using this pointer. The
*      callback function can access the memory via the "data" field in the
*      &crypto_async_request data structure provided to the callback function.
*
* This function allows setting the callback function that is triggered once
* the cipher operation completes.
*
* The callback function is registered with the &ahash_request handle and
* must comply with the following template::
*
*    void callback_function(struct crypto_async_request *req, int error)
*/
static inline void ahash_request_set_callback(struct ahash_request *req,
        u32 flags,
        crypto_completion_t compl,
        void *data);
 
struct crypto_akcipher *crypto_alloc_akcipher(const char *alg_name, u32 type,
                      u32 mask);
/**
* akcipher_request_alloc() - allocates public key request
*
* @tfm:    AKCIPHER tfm handle allocated with crypto_alloc_akcipher()
* @gfp:    allocation flags
*
* Return: allocated handle in case of success or NULL in case of an error.
*/
static inline struct akcipher_request *akcipher_request_alloc(
    struct crypto_akcipher *tfm, gfp_t gfp);
/**
* akcipher_request_set_crypt() - Sets request parameters
*
* Sets parameters required by crypto operation
*
* @req:    public key request
* @src:    ptr to input scatter list
* @dst:    ptr to output scatter list or NULL for verify op
* @src_len:    size of the src input scatter list to be processed
* @dst_len:    size of the dst output scatter list or size of signature
*      portion in @src for verify op
*/
static inline void akcipher_request_set_crypt(struct akcipher_request *req,
                          char *src,
                          char *dst,
                          unsigned int src_len,
                          unsigned int dst_len);
/**
* akcipher_request_set_callback() - Sets an asynchronous callback.
*
* Callback will be called when an asynchronous operation on a given
* request is finished.
*
* @req:    request that the callback will be set for
* @flgs:   specify for instance if the operation may backlog
* @cmpl:   callback which will be called
* @data:   private data used by the caller
*/
static inline void akcipher_request_set_callback(struct akcipher_request *req,
                      u32 flgs,
                      crypto_completion_t cmpl,
                      void *data);
 
 
int crypto_skcipher_encrypt(struct skcipher_request *req);
int crypto_skcipher_decrypt(struct skcipher_request *req);
 
int crypto_ahash_digest(struct ahash_request *req);
/**
* crypto_akcipher_encrypt() - Invoke public key encrypt operation
*
* Function invokes the specific public key encrypt operation for a given
* public key algorithm
*
* @req: asymmetric key request
*
* Return: zero on success; error code in case of error
*/
static inline int crypto_akcipher_encrypt(struct akcipher_request *req);
static inline int crypto_akcipher_decrypt(struct akcipher_request *req);
 
#endif