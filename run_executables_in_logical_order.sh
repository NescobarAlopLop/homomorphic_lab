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

echo -e "\n 2. Client: read parameters and encrypt query..."
"${HOMOMORPHIC_ROOT}"/bin/client_read_parameters_encrypt_query "${HOMOMORPHIC_ROOT}"/example_inputs

echo -e "\n 3. Server: read parameters and encrypted query, and run computation on encrypted query..."
"${HOMOMORPHIC_ROOT}"/bin/server_run_computation "${HOMOMORPHIC_ROOT}"/example_inputs

echo -e "\n 4. Client: read parameters, and encrypted result, decrypt and print result"
"${HOMOMORPHIC_ROOT}"/bin/client_read_results_decrypt_query_result "${HOMOMORPHIC_ROOT}"/example_inputs
