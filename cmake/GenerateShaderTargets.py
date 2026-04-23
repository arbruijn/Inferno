#!/usr/bin/env python3
"""Parse DirectXTK12 CompileShaders.cmd and generate CMake shader compilation targets."""

import re
import sys
import os

def parse_compile_shaders(cmd_path):
    """Parse CompileShaders.cmd and extract shader compilation info."""
    with open(cmd_path) as f:
        content = f.read()

    shaders = []

    # Match: call :CompileShader%1 <EffectName> <vs|ps> <EntryPoint>
    # %1 is a batch variable suffix (empty for PC, xbox, dxil, gxdk)
    # Lines with xbox/gxdk prefixes are skipped by the batch :skipxboxonly logic
    for m in re.finditer(r'call :CompileShader(?:%1)?\s+(\S+)\s+(vs|ps)\s+(\S+)', content):
        effect, stype, entry = m.groups()
        shaders.append((effect, stype, entry))

    # Match: call :CompileComputeShader%1 <ShaderName> <EntryPoint>
    for m in re.finditer(r'call :CompileComputeShader(?:%1)?\s+(\S+)\s+(\S+)', content):
        shader_name, entry = m.groups()
        shaders.append((shader_name, 'cs', entry))

    return shaders

def generate_cmake(shaders, dxc_exe_var, output_dir_var, cmake_output):
    """Generate CMake code for shader compilation."""
    lines = []
    lines.append("# Auto-generated from CompileShaders.cmd - do not edit")
    lines.append(f"set(DIRECTXTK12_SHADER_OUTPUT_DIR ${{CMAKE_BINARY_DIR}}/DirectXTK12Shaders)")
    lines.append("file(MAKE_DIRECTORY ${DIRECTXTK12_SHADER_OUTPUT_DIR})")
    lines.append("")
    lines.append("set(DIRECTXTK12_SHADER_INCS)")

    # Group by shader file
    by_file = {}
    for effect, stype, entry in shaders:
        if stype == 'cs':
            key = f"{effect}.hlsl"
        else:
            key = f"{effect}.fx"
        by_file.setdefault(key, []).append((effect, stype, entry))

    for shader_file, entries in by_file.items():
        for effect, stype, entry in entries:
            var_name = f"SHDR_{effect}_{entry}".replace('-', '_')
            if stype == 'cs':
                target = "cs_5_1"
                src = f"${{DIRECTXTK12_SRC}}/Shaders/{effect}.hlsl"
            else:
                target = f"{stype}_5_1"
                src = f"${{DIRECTXTK12_SRC}}/Shaders/{effect}.fx"

            inc_file = f"${{DIRECTXTK12_SHADER_OUTPUT_DIR}}/{effect}_{entry}.inc"

            lines.append(f"set({var_name} {inc_file})")
            lines.append(f"list(APPEND DIRECTXTK12_SHADER_INCS ${{{var_name}}})")
            lines.append(f"add_custom_command(")
            lines.append(f"  OUTPUT {inc_file}")
            lines.append(f"  COMMAND ${{CMAKE_COMMAND}} -E env WINEDEBUG=-all ${{DXC_WINETST}} {src} /T{target} /E{entry} /Fh{inc_file} /Vn{effect}_{entry}")
            lines.append(f"  DEPENDS {src}")
            lines.append(f"  COMMENT \"Compiling {effect}_{entry}\"")
            lines.append(f"  VERBATIM")
            lines.append(f")")
            lines.append("")

    with open(cmake_output, 'w') as f:
        f.write('\n'.join(lines))

if __name__ == '__main__':
    cmd_path = sys.argv[1] if len(sys.argv) > 1 else 'deps/DirectXTK12/Src/Shaders/CompileShaders.cmd'
    output_path = sys.argv[2] if len(sys.argv) > 2 else 'cmake/DirectXTK12Shaders.cmake'
    shaders = parse_compile_shaders(cmd_path)
    print(f"Found {len(shaders)} PC shader targets")
    generate_cmake(shaders, 'DXC_EXECUTABLE', 'SHADER_OUTPUT_DIR', output_path)
    print(f"Generated {output_path}")
