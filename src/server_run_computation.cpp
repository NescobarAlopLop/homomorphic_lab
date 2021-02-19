#include "utils.h"

using namespace std;
using namespace seal;

int main()
{
    EncryptionParameters encryption_parameters;
    ifstream parameters_file_stream("parameters.dat", ios::binary);
    encryption_parameters.load(parameters_file_stream);
    auto server_context = SEALContext::Create(encryption_parameters);

    vector<double> model_weights{50, -3};
    model_weights = {3, 4};
    double scale = pow(2, 40);
    Plaintext model_weights_plain_text;
    CKKSEncoder ckks_encoder(server_context);
    ckks_encoder.encode(model_weights, scale, model_weights_plain_text);
    Ciphertext encrypted_model_weights;


    ifstream ifs_relin("relin.dat", ios::binary);
    ifstream ifs_galois("galois.dat", ios::binary);
    ifstream ifs_public_key("public_key.dat", ios::binary);

    PublicKey server_public_key;
    server_public_key.load(server_context, ifs_public_key);
    Encryptor encryptor(server_context, server_public_key);

    ifstream ifs_encrypted_query("encrypted_query.dat", ios::binary);
    Ciphertext encrypted_query;
    encrypted_query.load(server_context, ifs_encrypted_query);

    encryptor.encrypt(model_weights_plain_text, encrypted_model_weights);

    Evaluator server_evaluator(server_context);
    Ciphertext encrypted_multiplication_result;
    server_evaluator.multiply(encrypted_query, encrypted_model_weights, encrypted_multiplication_result);

    RelinKeys server_relin_keys;
    server_relin_keys.load(server_context, ifs_relin);
    server_evaluator.relinearize_inplace(encrypted_multiplication_result, server_relin_keys);
    server_evaluator.rescale_to_next_inplace(encrypted_multiplication_result);

    GaloisKeys server_galois_keys;
    server_galois_keys.load(server_context, ifs_galois);
    vector<Ciphertext> rotations_output(encrypted_multiplication_result.size());
    for (int steps = 0; steps < encrypted_multiplication_result.size(); steps++) {
        server_evaluator.rotate_vector(encrypted_multiplication_result, steps, server_galois_keys,
                                       rotations_output[steps]);
    }
    Ciphertext encrypted_sum_output;
    server_evaluator.add_many(rotations_output, encrypted_sum_output);

    ofstream ofs_result("result.dat", ios::binary);
    encrypted_sum_output.save(ofs_result);
}
