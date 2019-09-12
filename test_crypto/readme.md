# 内核加解密相关框架

[rgw中介绍的加密](https://github.com/houwentaoff/rgw/blob/master/doc/%E5%8A%A0%E5%AF%86.md)  
[lora mqtt中的证书/单向认证，双向认证](https://github.com/houwentaoff/LoraGatewayBridge/blob/master/docs/mqtt%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E.md)  

# 内核层
* `tfm` -> `crypto_tfm` `crypto_alg->cra_init(struct crypto_tfm *tfm):->Initialize the cryptographic transformation object`
* `crypto_register_alg``crypto_unregister_alg`

# DMA
一般会使用DMA进行传输构造的描述符的传输  

# 应用层

1. 使用socket使用内核加解密。  
```c
struct sockaddr_alg sa = {  
        .salg_family = AF_ALG,  
        .salg_type = "hash",  
        .salg_name = "sha1"  
    };
    tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0);  
   
    bind(tfmfd, (struct sockaddr *)&sa, sizeof(sa));  
   
    opfd = accept(tfmfd, NULL, 0);  
   
    write(opfd, "abc", 3);  
    read(opfd, buf, 20); //buf中为内核加密后的数据
    for (i = 0; i < 20; i++) {  
        printf("%02x", (unsigned char)buf[i]);  
    }      
```
2. socket如何根据传递的
*. $(KDIR)/crypto/af_alg.c 中注册了`proto_register(&alg_proto, 0);`从而可以解析对应的AF_ALG域.
*. af_alg_register_type 注册使用的send recv等函数
```
./crypto/algif_hash.c:486:	return af_alg_register_type(&algif_type_hash);
./crypto/algif_skcipher.c:431:	return af_alg_register_type(&algif_type_skcipher);
./crypto/af_alg.c:63:int af_alg_register_type(const struct af_alg_type *type)
./crypto/af_alg.c:91:EXPORT_SYMBOL_GPL(af_alg_register_type);
./crypto/algif_rng.c:182:	return af_alg_register_type(&algif_type_rng);
./crypto/algif_aead.c:613:	return af_alg_register_type(&algif_type_aead);
./include/crypto/if_alg.h:168:int af_alg_register_type(const struct af_alg_type *type);
```
recv时候创建对象
`_skcipher_recvmsg->af_alg_wait_for_completion->crypto_skcipher_encrypt/crypto_skcipher_decrypt->tfm->encrypt(req); wait` 
这里关键是tfm和encrypt
send时看起来仅仅是把数据保存起来
