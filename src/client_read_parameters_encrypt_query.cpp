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
		cout << "/bin/client_read_parameters_encrypt_query <path to config directory>" << endl;
		cout << "For example:" << endl;
		cout << R"("${HOMOMORPHIC_ROOT}"/bin/client_read_parameters_encrypt_query "${HOMOMORPHIC_ROOT}"/example_inputs)" << endl;

		return -1;
	}

	auto config_dir_path = string(argv[1]);

	std::ifstream i(config_dir_path + "/client_query.json");
	json input_json;
	i >> input_json;
	auto user_query = input_json["query"].get<std::vector<double>>();

	i.close();
	i.open(config_dir_path + "/encryption_parameters_config.json");
	i >> input_json;
	auto x = input_json["scale"]["x"].get<int>();
	auto y = input_json["scale"]["y"].get<int>();

	cout << "Client read client query" << endl;
	cout << "\tquery: " << endl;
	for(auto& value:user_query)
		cout << value << " ";
	cout << endl << "\tscale.x: " << x << " scale.y: " << y << endl;

    ifstream parameters_file_stream(config_dir_path + "/parameters.dat", ios::binary);
    EncryptionParameters params;
    params.load(parameters_file_stream);

    auto context = SEALContext(params);

    KeyGenerator key_generator(context);
	PublicKey public_key;
    key_generator.create_public_key(public_key);
    auto secret_key = key_generator.secret_key();

    ofstream ofs_secret_key(config_dir_path + "/secret_key.dat", ios::binary);
    secret_key.save(ofs_secret_key);

    auto relin_keys = key_generator.create_relin_keys();         // TODO: switch to non local
    auto galois_keys = key_generator.create_galois_keys();

    ofstream ifs_relin(config_dir_path + "/relin.dat", ios::binary);
    ofstream ifs_galois(config_dir_path + "/galois.dat", ios::binary);
    ofstream ifs_public_key(config_dir_path + "/public_key.dat", ios::binary);

    auto rlk_size = relin_keys.save(ifs_relin);
    auto gal_size = galois_keys.save(ifs_galois);
    auto public_key_size = public_key.save(ifs_public_key);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    CKKSEncoder ckks_encoder(context);

    size_t slot_count = ckks_encoder.slot_count();
    cout << "number of slots: " << slot_count << ", should be half of the size of poly modulus degree" << endl;

    cout << "Input vector:" << endl;
    print_vector(user_query);

    Plaintext user_query_plain_text;

    double scale = pow(x, y);
    print_line(__LINE__);
    ckks_encoder.encode(user_query, scale, user_query_plain_text);

    Ciphertext encrypted_user_query;
    encryptor.encrypt(user_query_plain_text, encrypted_user_query);
    ofstream ofs_encrypted_query(config_dir_path + "/encrypted_query.dat", ios::binary);
    encrypted_user_query.save(ofs_encrypted_query);
}
