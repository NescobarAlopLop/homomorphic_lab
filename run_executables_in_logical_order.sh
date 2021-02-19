#!/usr/bin/env bash
HOMOMORPHIC_ROOT=$(pwd)

mkdir -p "${HOMOMORPHIC_ROOT}"/build && cd "${HOMOMORPHIC_ROOT}"/build || exit
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 4
cd "${HOMOMORPHIC_ROOT}" || exit

bin/server_generate_parameters
bin/client_read_parameters_encrypt_query
bin/server_run_computation
bin/client_read_results_decrypt_query_result
