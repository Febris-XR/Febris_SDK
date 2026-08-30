// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
// C++ half of the cross-SDK byte-parity harness (simulationintegration/parity/).
// Mirrors the C# CrossSdkEmitParityTests pipeline exactly: parse the authored
// fixture, run it through the ported StatementFactoring, emit the wire string
// through XApiJson (ordered keys, compact separators -- Newtonsoft
// Formatting.None), and write the bytes for compare.sh to diff against the C#
// output. Built by build_probe.cmd (plain cl against the FebrisCpp sources; the
// DLL exports nothing, so there is nothing to link against).
#include "pch.h"
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "usage: parity_probe <input.json> <output.json>" << std::endl;
        return 2;
    }
    std::ifstream in(argv[1], std::ios::binary);
    if (!in)
    {
        std::cerr << "cannot open input: " << argv[1] << std::endl;
        return 2;
    }
    try
    {
        json input = json::parse(in);
        Statement statement = StatementFactoring::FactorStatement(input);
        std::string wire = XApiJson::ToJsonString(statement);
        std::ofstream out(argv[2], std::ios::binary);
        if (!out)
        {
            std::cerr << "cannot open output: " << argv[2] << std::endl;
            return 2;
        }
        out << wire;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "parity probe failed: " << e.what() << std::endl;
        return 1;
    }
}
