// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "main.h"

using namespace std;
using namespace seal;

int main()
{
    cout << "Microsoft SEAL version: " << SEAL_VERSION << endl;
    while (true)
    {
        cout << "+------------------------------------------------------------+" << endl;
        cout << "| The following examples should be executed while reading    |" << endl;
        cout << "| comments in associated files in native/examples/.          |" << endl;
        cout << "+------------------------------------------------------------+" << endl;
        cout << "| 1 Encoders                 | 2_encoders.cpp                |" << endl;
        cout << "+----------------------------+-------------------------------+" << endl;
        cout << "| 2 integer                  | example_integer_encoder.cpp   |" << endl;
        cout << "+----------------------------+-------------------------------+" << endl;
        cout << "| 3 batch encoder            | example_batch_encoder.cpp     |" << endl;
        cout << "+----------------------------+-------------------------------+" << endl;
        cout << "| 4 ckks encoder             | example_ckks_encoder.cpp      |" << endl;
        cout << "+----------------------------+-------------------------------+" << endl;
        cout << "| 5 predict                  | predict                       |" << endl;
        cout << "+----------------------------+-------------------------------+" << endl;
        cout << "| 0 Exit                     |                               |" << endl;
        cout << "+----------------------------+-------------------------------+" << endl;

        /*
        Print how much memory we have allocated from the current memory pool.
        By default the memory pool will be a static global pool and the
        MemoryManager class can be used to change it. Most users should have
        little or no reason to touch the memory allocation system.
        */
        size_t megabytes = MemoryManager::GetPool().alloc_byte_count() >> 20;
        cout << "[" << setw(7) << right << megabytes << " MB] "
             << "Total allocation from the memory pool" << endl;

        int selection = 1;
        cout << endl << "> Run example (1 ~ 7) or exit (0): ";
        cin >> selection;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (selection)
        {
            case 1:
                example_encoders2();
                break;
            case 2:
                example_integer_encoder();
                break;
            case 3:
                example_batch_encoder();
                break;
            case 4:
                example_ckks_encoder();
                break;
            case 5:
                predict();
            case 0:
                return 0;
            default:
                cout << "wrong choice" << endl;
                break;
        }
    }
}
