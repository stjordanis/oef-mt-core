#!/bin/bash

ROOT="$(cd $1 && pwd)"

rsa_key="${ROOT}/key.pem"
rsa_crt="${ROOT}/cert.pem"
core="${ROOT}/core.pem"
core_pub="${ROOT}/core_pub.pem"
echo ${rsa_key} ${rsa_crt} ${core} ${core_pub}

openssl req -x509 -newkey rsa:2048 -keyout ${rsa_key} -out ${rsa_crt} -nodes
cat ${rsa_key} ${rsa_crt} > "${core}"
openssl rsa -pubout -in ${rsa_key} -out ${core_pub}
rm ${rsa_key} ${rsa_crt}

echo "Core key pair saved in : (prv:${core},pub:${core_pub})"
