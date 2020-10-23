#include "utils.h"

using namespace std;
using namespace seal;

int main()
// client code
{
    ifstream parameters_file_stream("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/common_data/parameters.dat", ios::binary);
    EncryptionParameters params;
    params.load(parameters_file_stream);

    auto context = SEALContext::Create(params);

    KeyGenerator key_generator(context);
    auto public_key = key_generator.public_key();
    auto secret_key = key_generator.secret_key();

    ofstream ofs_secret_key("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/client_data/secret_key.dat", ios::binary);
    secret_key.save(ofs_secret_key);

    auto relin_keys = key_generator.relin_keys_local();         // TODO: switch to non local
    auto galois_keys = key_generator.galois_keys_local();

    ofstream ifs_relin("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/common_data/relin.dat", ios::binary);
    ofstream ifs_galois("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/common_data/galois.dat", ios::binary);
    ofstream ifs_public_key("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/common_data/public_key.dat", ios::binary);

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
    ofstream ofs_encrypted_query("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/common_data/encrypted_query.dat", ios::binary);
    encrypted_user_query.save(ofs_encrypted_query);
}