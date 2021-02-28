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
		cout << "/bin/server_run_computation <path to config directory>" << endl;
		cout << "For example:" << endl;
		cout << R"("${HOMOMORPHIC_ROOT}"/bin/server_run_computation "${HOMOMORPHIC_ROOT}"/example_inputs)" << endl;

		return -1;
	}

	auto config_dir_path = string(argv[1]);

	// read a JSON file
	std::ifstream i(config_dir_path + "/model.json");
	json input_json;
	i >> input_json;

	auto model_weights = input_json["model_weights"].get<std::vector<double>>();

	i.close();
	i.open(config_dir_path + "/encryption_parameters_config.json");
	i >> input_json;
	auto x = input_json["scale"]["x"].get<int>();
	auto y = input_json["scale"]["y"].get<int>();

	cout << "model_weights: " << endl;
	for (const auto& value: model_weights)
		std::cout << value << ' ';
	cout << endl << "\tscale.x: " << x << " scale.y: " << y << endl;

    EncryptionParameters encryption_parameters;
    ifstream parameters_file_stream(config_dir_path + "/parameters.dat", ios::binary);
    encryption_parameters.load(parameters_file_stream);
    auto server_context = SEALContext(encryption_parameters);

    double scale = pow(x, y);
    Plaintext model_weights_plain_text;
    CKKSEncoder ckks_encoder(server_context);
    ckks_encoder.encode(model_weights, scale, model_weights_plain_text);
    Ciphertext encrypted_model_weights;


    ifstream ifs_relin(config_dir_path + "/relin.dat", ios::binary);
    ifstream ifs_galois(config_dir_path + "/galois.dat", ios::binary);
    ifstream ifs_public_key(config_dir_path + "/public_key.dat", ios::binary);

    PublicKey server_public_key;
    server_public_key.load(server_context, ifs_public_key);
    Encryptor encryptor(server_context, server_public_key);

    ifstream ifs_encrypted_query(config_dir_path + "/encrypted_query.dat", ios::binary);
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
    vector<Ciphertext> rotations_output(model_weights.size());
    for (int steps = 0; steps < rotations_output.size(); steps++) {
        server_evaluator.rotate_vector(encrypted_multiplication_result, steps, server_galois_keys,
                                       rotations_output[steps]);
    }
    Ciphertext encrypted_sum_output;
    server_evaluator.add_many(rotations_output, encrypted_sum_output);

    ofstream ofs_result(config_dir_path + "/result.dat", ios::binary);
    encrypted_sum_output.save(ofs_result);
}
