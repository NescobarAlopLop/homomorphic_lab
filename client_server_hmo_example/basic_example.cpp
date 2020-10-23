#include "basic_example.h"

using namespace std;
using namespace seal;

int main()
{
    // server code:
    {

        EncryptionParameters encryption_parameters(scheme_type::CKKS);
        std::size_t poly_modulus_degree = 8192;
        encryption_parameters.set_poly_modulus_degree(poly_modulus_degree);
//        auto bit_sizes = {40, 40, 40, 40, 40}; // 84 mb
        auto bit_sizes = {40, 40, 40, 40}; // 53 mb
        auto coefficient_modulus_degree = CoeffModulus::Create(poly_modulus_degree, bit_sizes);
        encryption_parameters.set_coeff_modulus(coefficient_modulus_degree);

        ofstream ofs_parameters("./parameters.dat", ios::binary);
        auto parameters_size = encryption_parameters.save(ofs_parameters);
        cout << "Server: saved encryption parameters: wrote " << parameters_size << " bytes" << endl;
    }

    // client code
    {
        ifstream parameters_file_stream("./parameters.dat", ios::binary);
        EncryptionParameters params;
        params.load(parameters_file_stream);

        auto context = SEALContext::Create(params);

        KeyGenerator key_generator(context);
        auto public_key = key_generator.public_key();
        auto secret_key = key_generator.secret_key();

        ofstream ofs_secret_key("./secret_key.dat", ios::binary);
        secret_key.save(ofs_secret_key);

        auto relin_keys = key_generator.relin_keys_local();         // TODO: switch to non local
        auto galois_keys = key_generator.galois_keys_local();

        ofstream ifs_relin("./relin.dat", ios::binary);
        ofstream ifs_galois("./galois.dat", ios::binary);
        ofstream ifs_public_key("./public_key.dat", ios::binary);

        auto rlk_size = relin_keys.save(ifs_relin);
        auto gal_size = galois_keys.save(ifs_galois);
        auto public_key_size = public_key.save(ifs_public_key);

        Encryptor encryptor(context, public_key);
        Evaluator evaluator(context);
        Decryptor decryptor(context, secret_key);

        CKKSEncoder ckks_encoder(context);

        size_t slot_count = ckks_encoder.slot_count();
        cout << "number of slots: " << slot_count << ", should be half of the size of poly modulus degree" << endl;

        vector<double> user_query{0.25, -0.25};
        cout << "Input vector:" << endl;
        print_vector(user_query);

        Plaintext user_query_plain_text;
        double scale = pow(2, 40);
        print_line(__LINE__);
        ckks_encoder.encode(user_query, scale, user_query_plain_text);

        Ciphertext encrypted_user_query;
        encryptor.encrypt(user_query_plain_text, encrypted_user_query);
        ofstream ofs_encrypted_query("./encrypted_query.dat", ios::binary);
        encrypted_user_query.save(ofs_encrypted_query);
    }

    // server code
    {
        EncryptionParameters encryption_parameters;
        ifstream parameters_file_stream("./parameters.dat", ios::binary);
        encryption_parameters.load(parameters_file_stream);
        auto server_context = SEALContext::Create(encryption_parameters);

        vector<double> model_weights{50, -3};
        model_weights = {3, 4};
        double scale = pow(2, 40);
        Plaintext model_weights_plain_text;
        CKKSEncoder ckks_encoder(server_context);
        ckks_encoder.encode(model_weights, scale, model_weights_plain_text);
        Ciphertext encrypted_model_weights;


        ifstream ifs_relin("./relin.dat", ios::binary);
        ifstream ifs_galois("./galois.dat", ios::binary);
        ifstream ifs_public_key("./public_key.dat", ios::binary);

        PublicKey server_public_key;
        server_public_key.load(server_context, ifs_public_key);
        Encryptor encryptor(server_context, server_public_key);

        ifstream ifs_encrypted_query("./encrypted_query.dat", ios::binary);
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

        ofstream ofs_result("./result.dat", ios::binary);
        encrypted_sum_output.save(ofs_result);
    }

    // client code
    {
        EncryptionParameters parms;
        ifstream ifs_parameters("./parameters.dat", ios::binary);
        parms.load(ifs_parameters);
        auto context = SEALContext::Create(parms);

        /*
        Load back the secret key from sk_stream.
        */
        SecretKey sk;
        ifstream ifs_secret_key("./secret_key.dat", ios::binary);
        sk.load(context, ifs_secret_key);
        Decryptor decryptor(context, sk);
        CKKSEncoder encoder(context);

        Ciphertext encrypted_result;
        ifstream ifs_result("./result.dat", ios::binary);
        encrypted_result.load(context, ifs_result);

        Plaintext plain_result;
        decryptor.decrypt(encrypted_result, plain_result);
        vector<double> result;
        encoder.decode(plain_result, result);

        print_line(__LINE__);
        cout << "Result: " << endl;
        print_vector(result, 3, 7);
    }
}