#!/usr/bin/env bash
bin/server_gen_params
bin/client_read_params_encrypt_query
bin/server_run_computation
bin/client_read_decrypt_results
