#!/bin/bash


ROOT=$(pwd)

rsa_pub_key= ${ROOT}/public_key.pem
rsa_prv_key= ${ROOT}/private_key.pem

openssl genpkey -algorithm RSA -out ${rsa_prv_key} -pkeyopt rsa_keygen_bits:2048
openssl rsa -pubout -in ${rsa_prv_key} -out ${rsa_pub_key}

echo "Agent key pair generated: (public:${rsa_pub_key}, private:${rsa_prv_key})"
