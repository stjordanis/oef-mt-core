#!/bin/bash

pubkey_file="${1}"

openssl rsa -inform PEM -pubin -in ${pubkey_file} -text -noout
