#include "utils.h"
#include <nlohmann/json.hpp>


using namespace std;
using namespace seal;
using json = nlohmann::json;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Provide path to configurations directory" << endl;
		cout << "/bin/server_generate_parameters <path to config directory>" << endl;
		cout << "For example:" << endl;
		cout << R"("${HOMOMORPHIC_ROOT}"/bin/server_generate_parameters "${HOMOMORPHIC_ROOT}"/example_inputs)" << endl;

		return -1;
	}
	auto config_dir_path = string(argv[1]);

	cout << config_dir_path + "/encryption_parameters_config.json" << endl;
	std::ifstream i(config_dir_path + "/encryption_parameters_config.json");
	json input_json;
	i >> input_json;

	std::size_t poly_modulus_degree = input_json["poly_modulus_degree"].get<std::size_t>();
	auto bit_sizes = input_json["bit_sizes"].get<std::vector<int>>();
	cout << "Server has read the following encryption parameters configurations:" << endl;
	cout << "\tpoly_modulus_degree: " << input_json["poly_modulus_degree"] << endl;
	cout << "\tbit_sizes: " << endl;
	for (const auto& value: bit_sizes)
		std::cout << value << ' ';

    EncryptionParameters encryption_parameters(scheme_type::ckks);
    encryption_parameters.set_poly_modulus_degree(poly_modulus_degree);
    auto coefficient_modulus_degree = CoeffModulus::Create(poly_modulus_degree, bit_sizes);
    encryption_parameters.set_coeff_modulus(coefficient_modulus_degree);

	ofstream ofs_parameters(config_dir_path + "/parameters.dat", ios::binary);
	auto parameters_size = encryption_parameters.save(ofs_parameters);
    cout << endl << "Server: saved encryption parameters: wrote " << parameters_size << " bytes" << endl;
}
