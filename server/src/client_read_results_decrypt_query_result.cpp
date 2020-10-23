#include "utils.h"

using namespace std;
using namespace seal;

int main()
{
    EncryptionParameters parms;
    ifstream ifs_parameters("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/common_data/parameters.dat", ios::binary);
    parms.load(ifs_parameters);
    auto context = SEALContext::Create(parms);

    /*
    Load back the secret key from sk_stream.
    */
    SecretKey sk;
    ifstream ifs_secret_key("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/client_data/secret_key.dat", ios::binary);
    sk.load(context, ifs_secret_key);
    Decryptor decryptor(context, sk);
    CKKSEncoder encoder(context);

    Ciphertext encrypted_result;
    ifstream ifs_result("/home/${USER}/Documents/homomorhpic_lab/server/bin/tmp/common_data/result.dat", ios::binary);
    encrypted_result.load(context, ifs_result);

    Plaintext plain_result;
    decryptor.decrypt(encrypted_result, plain_result);
    vector<double> result;
    encoder.decode(plain_result, result);

    print_line(__LINE__);
    cout << "Result: " << endl;
    print_vector(result, 3, 7);
}
