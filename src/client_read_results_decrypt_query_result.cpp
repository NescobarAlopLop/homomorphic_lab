#include "utils.h"
#include <json.hpp>


using namespace std;
using namespace seal;
using json = nlohmann::json;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Provide path to configurations directory" << endl;
		cout << "/bin/client_read_results_decrypt_query_result <path to config directory>" << endl;
		cout << "For example:" << endl;
		cout << R"("${HOMOMORPHIC_ROOT}"/bin/client_read_results_decrypt_query_result "${HOMOMORPHIC_ROOT}"/example_inputs)" << endl;

		return -1;
	}

	auto config_dir_path = string(argv[1]);

	EncryptionParameters parms;
    ifstream ifs_parameters(config_dir_path + "/parameters.dat", ios::binary);
    parms.load(ifs_parameters);
    auto context = SEALContext::Create(parms);

    /*
    Load back the secret key from sk_stream.
    */
    SecretKey sk;
    ifstream ifs_secret_key(config_dir_path + "/secret_key.dat", ios::binary);
    sk.load(context, ifs_secret_key);
    Decryptor decryptor(context, sk);
    CKKSEncoder encoder(context);

    Ciphertext encrypted_result;
    ifstream ifs_result(config_dir_path + "/result.dat", ios::binary);
    encrypted_result.load(context, ifs_result);

    Plaintext plain_result;
    decryptor.decrypt(encrypted_result, plain_result);
    vector<double> result;
    encoder.decode(plain_result, result);

    print_line(__LINE__);
    cout << "Result: " << endl;
    print_vector(result, 3, 7);

	std::ifstream i(config_dir_path + "/client_query.json");
	json input_json;
	i >> input_json;
    cout << "For testing only:" << endl << "\texpected value: " << input_json["expected_value"] << endl;
    cout << "\tresult value: " << result[0] << endl;
}
