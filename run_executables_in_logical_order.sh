#!/usr/bin/env bash
export HOMOMORPHIC_ROOT
HOMOMORPHIC_ROOT=$(pwd)


echo "Compiling executables..."
mkdir -p "${HOMOMORPHIC_ROOT}"/build && cd "${HOMOMORPHIC_ROOT}"/build || exit
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 4
cd "${HOMOMORPHIC_ROOT}" || exit
echo "Finished compilation."

echo -e "\n 1. Server: generating parameters..."
"${HOMOMORPHIC_ROOT}"/bin/server_generate_parameters "${HOMOMORPHIC_ROOT}"/example_inputs

bin/server_generate_parameters
bin/client_read_parameters_encrypt_query
bin/server_run_computation
bin/client_read_results_decrypt_query_result
