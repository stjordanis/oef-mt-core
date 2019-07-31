#!/bin/bash
if [ $# -ne 3 ]; then
echo -e "\033[0;31m[!] Usage: ./crypto_generate_rsa_key_pair_wcert.sh <OutputDir> <KeyFileName> <NumberOfPairs>  \033[0m"
echo -e "\033[0;31m     - OutputDir     : path to where to save key pairs (e.g. ./) \033[0m"
echo -e "\033[0;31m     - KeyFileName   : prefix of the to be generated key pairs (e.g. tac_agent) \033[0m"
echo -e "\033[0;31m     - NumberOfPairs : number of pairs to generate with the same prefix (e.g. 5) \033[0m"
echo -e "\033[0;32m    e.g.: ./scripts/crypto_generate_rsa_key_pair_wcert.sh ./ tac_agent 5  \033[0m"
exit 1
fi

ROOT="$(cd $1 && pwd)"
KEY_TAG="$2"
NBR=$3

rsa_key="${ROOT}/${KEY_TAG}_key.pem"
rsa_crt="${ROOT}/${KEY_TAG}_cert.pem"
pub_lst="${ROOT}/${KEY_TAG}_pub_list"

for i in $(seq 1 1 ${NBR}); do
  if [ ${NBR} -eq 1 ]; then
    private="${ROOT}/${KEY_TAG}.pem"
    public="${ROOT}/${KEY_TAG}_pub.pem"
  else
    private="${ROOT}/${KEY_TAG}_${i}.pem"
    public="${ROOT}/${KEY_TAG}_${i}_pub.pem"
  fi
  
  echo -e "\033[0;90m"

  openssl req -x509 -newkey rsa:2048 -keyout ${rsa_key} -out ${rsa_crt} -nodes \
    -subj "/C=GB/ST=Cambridgeshire/L=Cambridge/O=Fetch.AI/OU=Open Economic Framework/CN=fetch.ai"
  cat ${rsa_key} ${rsa_crt} > "${private}"
  openssl rsa -pubout -in ${rsa_key} -out ${public}
  rm ${rsa_key} ${rsa_crt}

  echo -e "\033[0m"

  echo -e "\033[0;32m${KEY_TAG}_${i}\033[0m key pair saved in : "
  echo -e " pk:\033[0;32m${public}\033[0m"
  echo -e " sk:\033[0;31m${private}\033[0m"
  
  echo "${public}" >> "${pub_lst}"
done

if [ $? -eq 0 ]; then
  echo -e "\n\033[0;32mAll ${KEY_TAG} public keys paths listed in : ${pub_lst}\033[0m"
fi

