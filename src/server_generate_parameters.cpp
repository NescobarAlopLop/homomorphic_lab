#include "utils.h"

using namespace std;
using namespace seal;

int main()
{
    ofstream ofs_parameters("parameters.dat", ios::binary);

    EncryptionParameters encryption_parameters(scheme_type::CKKS);
    std::size_t poly_modulus_degree = 8192;
    encryption_parameters.set_poly_modulus_degree(poly_modulus_degree);
//        auto bit_sizes = {40, 40, 40, 40, 40}; // 84 mb
    auto bit_sizes = {40, 40, 40, 40}; // 53 mb
    auto coefficient_modulus_degree = CoeffModulus::Create(poly_modulus_degree, bit_sizes);
    encryption_parameters.set_coeff_modulus(coefficient_modulus_degree);

    auto parameters_size = encryption_parameters.save(ofs_parameters);
    cout << "Server: saved encryption parameters: wrote " << parameters_size << " bytes" << endl;
}
