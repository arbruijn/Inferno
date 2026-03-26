# Auto-generated from CompileShaders.cmd - do not edit
set(DIRECTXTK12_SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/DirectXTK12Shaders)
file(MAKE_DIRECTORY ${DIRECTXTK12_SHADER_OUTPUT_DIR})

set(DIRECTXTK12_SHADER_INCS)
set(SHDR_AlphaTestEffect_VSAlphaTest ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTest.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_VSAlphaTest})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTest.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tvs_5_1 /EVSAlphaTest /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTest.inc /VnAlphaTestEffect_VSAlphaTest
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_VSAlphaTest"
  VERBATIM
)

set(SHDR_AlphaTestEffect_VSAlphaTestNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_VSAlphaTestNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tvs_5_1 /EVSAlphaTestNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestNoFog.inc /VnAlphaTestEffect_VSAlphaTestNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_VSAlphaTestNoFog"
  VERBATIM
)

set(SHDR_AlphaTestEffect_VSAlphaTestVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_VSAlphaTestVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tvs_5_1 /EVSAlphaTestVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestVc.inc /VnAlphaTestEffect_VSAlphaTestVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_VSAlphaTestVc"
  VERBATIM
)

set(SHDR_AlphaTestEffect_VSAlphaTestVcNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestVcNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_VSAlphaTestVcNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestVcNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tvs_5_1 /EVSAlphaTestVcNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_VSAlphaTestVcNoFog.inc /VnAlphaTestEffect_VSAlphaTestVcNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_VSAlphaTestVcNoFog"
  VERBATIM
)

set(SHDR_AlphaTestEffect_PSAlphaTestLtGt ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestLtGt.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_PSAlphaTestLtGt})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestLtGt.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tps_5_1 /EPSAlphaTestLtGt /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestLtGt.inc /VnAlphaTestEffect_PSAlphaTestLtGt
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_PSAlphaTestLtGt"
  VERBATIM
)

set(SHDR_AlphaTestEffect_PSAlphaTestLtGtNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestLtGtNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_PSAlphaTestLtGtNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestLtGtNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tps_5_1 /EPSAlphaTestLtGtNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestLtGtNoFog.inc /VnAlphaTestEffect_PSAlphaTestLtGtNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_PSAlphaTestLtGtNoFog"
  VERBATIM
)

set(SHDR_AlphaTestEffect_PSAlphaTestEqNe ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestEqNe.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_PSAlphaTestEqNe})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestEqNe.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tps_5_1 /EPSAlphaTestEqNe /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestEqNe.inc /VnAlphaTestEffect_PSAlphaTestEqNe
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_PSAlphaTestEqNe"
  VERBATIM
)

set(SHDR_AlphaTestEffect_PSAlphaTestEqNeNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestEqNeNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_AlphaTestEffect_PSAlphaTestEqNeNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestEqNeNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx /Tps_5_1 /EPSAlphaTestEqNeNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/AlphaTestEffect_PSAlphaTestEqNeNoFog.inc /VnAlphaTestEffect_PSAlphaTestEqNeNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/AlphaTestEffect.fx
  COMMENT "Compiling AlphaTestEffect_PSAlphaTestEqNeNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasic ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasic.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasic})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasic.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasic /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasic.inc /VnBasicEffect_VSBasic
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasic"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicNoFog.inc /VnBasicEffect_VSBasicNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVc.inc /VnBasicEffect_VSBasicVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVc"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVcNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVcNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVcNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVcNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVcNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVcNoFog.inc /VnBasicEffect_VSBasicVcNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVcNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTx.inc /VnBasicEffect_VSBasicTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicTx"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicTxNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicTxNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicTxNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxNoFog.inc /VnBasicEffect_VSBasicTxNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicTxNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicTxVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicTxVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicTxVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxVc.inc /VnBasicEffect_VSBasicTxVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicTxVc"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicTxVcNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxVcNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicTxVcNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxVcNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicTxVcNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicTxVcNoFog.inc /VnBasicEffect_VSBasicTxVcNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicTxVcNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLighting.inc /VnBasicEffect_VSBasicVertexLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLighting"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLightingBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLightingBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLightingBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingBn.inc /VnBasicEffect_VSBasicVertexLightingBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLightingBn"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLightingVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLightingVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLightingVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingVc.inc /VnBasicEffect_VSBasicVertexLightingVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLightingVc"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLightingVcBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingVcBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLightingVcBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingVcBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLightingVcBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingVcBn.inc /VnBasicEffect_VSBasicVertexLightingVcBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLightingVcBn"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLightingTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLightingTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLightingTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTx.inc /VnBasicEffect_VSBasicVertexLightingTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLightingTx"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLightingTxBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLightingTxBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLightingTxBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxBn.inc /VnBasicEffect_VSBasicVertexLightingTxBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLightingTxBn"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLightingTxVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLightingTxVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLightingTxVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxVc.inc /VnBasicEffect_VSBasicVertexLightingTxVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLightingTxVc"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicVertexLightingTxVcBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxVcBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicVertexLightingTxVcBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxVcBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicVertexLightingTxVcBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicVertexLightingTxVcBn.inc /VnBasicEffect_VSBasicVertexLightingTxVcBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicVertexLightingTxVcBn"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLighting.inc /VnBasicEffect_VSBasicPixelLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLighting"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLightingBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLightingBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLightingBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingBn.inc /VnBasicEffect_VSBasicPixelLightingBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLightingBn"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLightingVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLightingVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLightingVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingVc.inc /VnBasicEffect_VSBasicPixelLightingVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLightingVc"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLightingVcBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingVcBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLightingVcBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingVcBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLightingVcBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingVcBn.inc /VnBasicEffect_VSBasicPixelLightingVcBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLightingVcBn"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLightingTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLightingTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLightingTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTx.inc /VnBasicEffect_VSBasicPixelLightingTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLightingTx"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLightingTxBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLightingTxBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLightingTxBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxBn.inc /VnBasicEffect_VSBasicPixelLightingTxBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLightingTxBn"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLightingTxVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLightingTxVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLightingTxVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxVc.inc /VnBasicEffect_VSBasicPixelLightingTxVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLightingTxVc"
  VERBATIM
)

set(SHDR_BasicEffect_VSBasicPixelLightingTxVcBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxVcBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_VSBasicPixelLightingTxVcBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxVcBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tvs_5_1 /EVSBasicPixelLightingTxVcBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_VSBasicPixelLightingTxVcBn.inc /VnBasicEffect_VSBasicPixelLightingTxVcBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_VSBasicPixelLightingTxVcBn"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasic ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasic.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasic})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasic.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasic /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasic.inc /VnBasicEffect_PSBasic
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasic"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicNoFog.inc /VnBasicEffect_PSBasicNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicTx.inc /VnBasicEffect_PSBasicTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicTx"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicTxNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicTxNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicTxNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicTxNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicTxNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicTxNoFog.inc /VnBasicEffect_PSBasicTxNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicTxNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicVertexLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicVertexLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicVertexLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLighting.inc /VnBasicEffect_PSBasicVertexLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicVertexLighting"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicVertexLightingNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicVertexLightingNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicVertexLightingNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingNoFog.inc /VnBasicEffect_PSBasicVertexLightingNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicVertexLightingNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicVertexLightingTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicVertexLightingTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicVertexLightingTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingTx.inc /VnBasicEffect_PSBasicVertexLightingTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicVertexLightingTx"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicVertexLightingTxNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingTxNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicVertexLightingTxNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingTxNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicVertexLightingTxNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicVertexLightingTxNoFog.inc /VnBasicEffect_PSBasicVertexLightingTxNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicVertexLightingTxNoFog"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicPixelLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicPixelLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicPixelLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicPixelLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicPixelLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicPixelLighting.inc /VnBasicEffect_PSBasicPixelLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicPixelLighting"
  VERBATIM
)

set(SHDR_BasicEffect_PSBasicPixelLightingTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicPixelLightingTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_BasicEffect_PSBasicPixelLightingTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicPixelLightingTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx /Tps_5_1 /EPSBasicPixelLightingTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/BasicEffect_PSBasicPixelLightingTx.inc /VnBasicEffect_PSBasicPixelLightingTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/BasicEffect.fx
  COMMENT "Compiling BasicEffect_PSBasicPixelLightingTx"
  VERBATIM
)

set(SHDR_DualTextureEffect_VSDualTexture ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTexture.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DualTextureEffect_VSDualTexture})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTexture.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx /Tvs_5_1 /EVSDualTexture /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTexture.inc /VnDualTextureEffect_VSDualTexture
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx
  COMMENT "Compiling DualTextureEffect_VSDualTexture"
  VERBATIM
)

set(SHDR_DualTextureEffect_VSDualTextureNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DualTextureEffect_VSDualTextureNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx /Tvs_5_1 /EVSDualTextureNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureNoFog.inc /VnDualTextureEffect_VSDualTextureNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx
  COMMENT "Compiling DualTextureEffect_VSDualTextureNoFog"
  VERBATIM
)

set(SHDR_DualTextureEffect_VSDualTextureVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DualTextureEffect_VSDualTextureVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx /Tvs_5_1 /EVSDualTextureVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureVc.inc /VnDualTextureEffect_VSDualTextureVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx
  COMMENT "Compiling DualTextureEffect_VSDualTextureVc"
  VERBATIM
)

set(SHDR_DualTextureEffect_VSDualTextureVcNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureVcNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DualTextureEffect_VSDualTextureVcNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureVcNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx /Tvs_5_1 /EVSDualTextureVcNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_VSDualTextureVcNoFog.inc /VnDualTextureEffect_VSDualTextureVcNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx
  COMMENT "Compiling DualTextureEffect_VSDualTextureVcNoFog"
  VERBATIM
)

set(SHDR_DualTextureEffect_PSDualTexture ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_PSDualTexture.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DualTextureEffect_PSDualTexture})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_PSDualTexture.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx /Tps_5_1 /EPSDualTexture /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_PSDualTexture.inc /VnDualTextureEffect_PSDualTexture
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx
  COMMENT "Compiling DualTextureEffect_PSDualTexture"
  VERBATIM
)

set(SHDR_DualTextureEffect_PSDualTextureNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_PSDualTextureNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DualTextureEffect_PSDualTextureNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_PSDualTextureNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx /Tps_5_1 /EPSDualTextureNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DualTextureEffect_PSDualTextureNoFog.inc /VnDualTextureEffect_PSDualTextureNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DualTextureEffect.fx
  COMMENT "Compiling DualTextureEffect_PSDualTextureNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_VSEnvMap ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMap.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_VSEnvMap})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMap.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tvs_5_1 /EVSEnvMap /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMap.inc /VnEnvironmentMapEffect_VSEnvMap
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_VSEnvMap"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_VSEnvMapBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_VSEnvMapBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tvs_5_1 /EVSEnvMapBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapBn.inc /VnEnvironmentMapEffect_VSEnvMapBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_VSEnvMapBn"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_VSEnvMapFresnel ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapFresnel.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_VSEnvMapFresnel})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapFresnel.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tvs_5_1 /EVSEnvMapFresnel /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapFresnel.inc /VnEnvironmentMapEffect_VSEnvMapFresnel
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_VSEnvMapFresnel"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_VSEnvMapFresnelBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapFresnelBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_VSEnvMapFresnelBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapFresnelBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tvs_5_1 /EVSEnvMapFresnelBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapFresnelBn.inc /VnEnvironmentMapEffect_VSEnvMapFresnelBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_VSEnvMapFresnelBn"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_VSEnvMapPixelLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapPixelLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_VSEnvMapPixelLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapPixelLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tvs_5_1 /EVSEnvMapPixelLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapPixelLighting.inc /VnEnvironmentMapEffect_VSEnvMapPixelLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_VSEnvMapPixelLighting"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_VSEnvMapPixelLightingBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapPixelLightingBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_VSEnvMapPixelLightingBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapPixelLightingBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tvs_5_1 /EVSEnvMapPixelLightingBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_VSEnvMapPixelLightingBn.inc /VnEnvironmentMapEffect_VSEnvMapPixelLightingBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_VSEnvMapPixelLightingBn"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMap ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMap.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMap})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMap.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMap /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMap.inc /VnEnvironmentMapEffect_PSEnvMap
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMap"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapNoFog.inc /VnEnvironmentMapEffect_PSEnvMapNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapSpecular ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpecular.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapSpecular})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpecular.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapSpecular /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpecular.inc /VnEnvironmentMapEffect_PSEnvMapSpecular
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapSpecular"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapSpecularNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpecularNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapSpecularNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpecularNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapSpecularNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpecularNoFog.inc /VnEnvironmentMapEffect_PSEnvMapSpecularNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapSpecularNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapPixelLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapPixelLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapPixelLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLighting.inc /VnEnvironmentMapEffect_PSEnvMapPixelLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapPixelLighting"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapPixelLightingNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapPixelLightingNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapPixelLightingNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingNoFog.inc /VnEnvironmentMapEffect_PSEnvMapPixelLightingNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapPixelLightingNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapPixelLightingFresnel ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingFresnel.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapPixelLightingFresnel})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingFresnel.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapPixelLightingFresnel /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingFresnel.inc /VnEnvironmentMapEffect_PSEnvMapPixelLightingFresnel
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapPixelLightingFresnel"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapPixelLightingFresnelNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog.inc /VnEnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapSpherePixelLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLighting.inc /VnEnvironmentMapEffect_PSEnvMapSpherePixelLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapSpherePixelLighting"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapSpherePixelLightingNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog.inc /VnEnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapSpherePixelLightingFresnel /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel.inc /VnEnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapSpherePixelLightingFresnelNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog.inc /VnEnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapDualParabolaPixelLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting.inc /VnEnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapDualParabolaPixelLightingNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog.inc /VnEnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapDualParabolaPixelLightingFresnel /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel.inc /VnEnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel"
  VERBATIM
)

set(SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx /Tps_5_1 /EPSEnvMapDualParabolaPixelLightingFresnelNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog.inc /VnEnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/EnvironmentMapEffect.fx
  COMMENT "Compiling EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog"
  VERBATIM
)

set(SHDR_SkinnedEffect_VSSkinnedVertexLightingFourBones ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedVertexLightingFourBones.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SkinnedEffect_VSSkinnedVertexLightingFourBones})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedVertexLightingFourBones.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx /Tvs_5_1 /EVSSkinnedVertexLightingFourBones /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedVertexLightingFourBones.inc /VnSkinnedEffect_VSSkinnedVertexLightingFourBones
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx
  COMMENT "Compiling SkinnedEffect_VSSkinnedVertexLightingFourBones"
  VERBATIM
)

set(SHDR_SkinnedEffect_VSSkinnedVertexLightingFourBonesBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedVertexLightingFourBonesBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SkinnedEffect_VSSkinnedVertexLightingFourBonesBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedVertexLightingFourBonesBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx /Tvs_5_1 /EVSSkinnedVertexLightingFourBonesBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedVertexLightingFourBonesBn.inc /VnSkinnedEffect_VSSkinnedVertexLightingFourBonesBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx
  COMMENT "Compiling SkinnedEffect_VSSkinnedVertexLightingFourBonesBn"
  VERBATIM
)

set(SHDR_SkinnedEffect_VSSkinnedPixelLightingFourBones ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedPixelLightingFourBones.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SkinnedEffect_VSSkinnedPixelLightingFourBones})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedPixelLightingFourBones.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx /Tvs_5_1 /EVSSkinnedPixelLightingFourBones /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedPixelLightingFourBones.inc /VnSkinnedEffect_VSSkinnedPixelLightingFourBones
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx
  COMMENT "Compiling SkinnedEffect_VSSkinnedPixelLightingFourBones"
  VERBATIM
)

set(SHDR_SkinnedEffect_VSSkinnedPixelLightingFourBonesBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedPixelLightingFourBonesBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SkinnedEffect_VSSkinnedPixelLightingFourBonesBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedPixelLightingFourBonesBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx /Tvs_5_1 /EVSSkinnedPixelLightingFourBonesBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_VSSkinnedPixelLightingFourBonesBn.inc /VnSkinnedEffect_VSSkinnedPixelLightingFourBonesBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx
  COMMENT "Compiling SkinnedEffect_VSSkinnedPixelLightingFourBonesBn"
  VERBATIM
)

set(SHDR_SkinnedEffect_PSSkinnedVertexLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedVertexLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SkinnedEffect_PSSkinnedVertexLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedVertexLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx /Tps_5_1 /EPSSkinnedVertexLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedVertexLighting.inc /VnSkinnedEffect_PSSkinnedVertexLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx
  COMMENT "Compiling SkinnedEffect_PSSkinnedVertexLighting"
  VERBATIM
)

set(SHDR_SkinnedEffect_PSSkinnedVertexLightingNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedVertexLightingNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SkinnedEffect_PSSkinnedVertexLightingNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedVertexLightingNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx /Tps_5_1 /EPSSkinnedVertexLightingNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedVertexLightingNoFog.inc /VnSkinnedEffect_PSSkinnedVertexLightingNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx
  COMMENT "Compiling SkinnedEffect_PSSkinnedVertexLightingNoFog"
  VERBATIM
)

set(SHDR_SkinnedEffect_PSSkinnedPixelLighting ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedPixelLighting.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SkinnedEffect_PSSkinnedPixelLighting})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedPixelLighting.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx /Tps_5_1 /EPSSkinnedPixelLighting /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SkinnedEffect_PSSkinnedPixelLighting.inc /VnSkinnedEffect_PSSkinnedPixelLighting
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SkinnedEffect.fx
  COMMENT "Compiling SkinnedEffect_PSSkinnedPixelLighting"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTx.inc /VnNormalMapEffect_VSNormalPixelLightingTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTx"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxBn.inc /VnNormalMapEffect_VSNormalPixelLightingTxBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxBn"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVc.inc /VnNormalMapEffect_VSNormalPixelLightingTxVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVc"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVcBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVcBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVcBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcBn.inc /VnNormalMapEffect_VSNormalPixelLightingTxVcBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVcBn"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpec ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpec.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpec})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpec.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxNoSpec /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpec.inc /VnNormalMapEffect_VSNormalPixelLightingTxNoSpec
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxNoSpec"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpecBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpecBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxNoSpecBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecBn.inc /VnNormalMapEffect_VSNormalPixelLightingTxNoSpecBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxNoSpecBn"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpec ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpec.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpec})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpec.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVcNoSpec /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpec.inc /VnNormalMapEffect_VSNormalPixelLightingTxVcNoSpec
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVcNoSpec"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVcNoSpecBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn.inc /VnNormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxBnInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxBnInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxBnInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxBnInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxBnInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxBnInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxBnInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxBnInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVcInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVcInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVcInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxVcInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVcInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVcBnInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcBnInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVcBnInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcBnInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVcBnInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcBnInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxVcBnInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVcBnInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpecInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpecInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxNoSpecInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxNoSpecInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxNoSpecInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpecBnInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecBnInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxNoSpecBnInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecBnInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxNoSpecBnInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxNoSpecBnInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxNoSpecBnInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxNoSpecBnInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpecInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpecInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVcNoSpecInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxVcNoSpecInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVcNoSpecInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBnInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBnInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBnInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBnInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSNormalPixelLightingTxVcNoSpecBnInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBnInst.inc /VnNormalMapEffect_VSNormalPixelLightingTxVcNoSpecBnInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBnInst"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSSkinnedPixelLightingTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSSkinnedPixelLightingTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSSkinnedPixelLightingTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTx.inc /VnNormalMapEffect_VSSkinnedPixelLightingTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSSkinnedPixelLightingTx"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSSkinnedPixelLightingTxBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSSkinnedPixelLightingTxBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSSkinnedPixelLightingTxBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxBn.inc /VnNormalMapEffect_VSSkinnedPixelLightingTxBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSSkinnedPixelLightingTxBn"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSSkinnedPixelLightingTxNoSpec ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxNoSpec.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSSkinnedPixelLightingTxNoSpec})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxNoSpec.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSSkinnedPixelLightingTxNoSpec /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxNoSpec.inc /VnNormalMapEffect_VSSkinnedPixelLightingTxNoSpec
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSSkinnedPixelLightingTxNoSpec"
  VERBATIM
)

set(SHDR_NormalMapEffect_VSSkinnedPixelLightingTxNoSpecBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxNoSpecBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_VSSkinnedPixelLightingTxNoSpecBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxNoSpecBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tvs_5_1 /EVSSkinnedPixelLightingTxNoSpecBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_VSSkinnedPixelLightingTxNoSpecBn.inc /VnNormalMapEffect_VSSkinnedPixelLightingTxNoSpecBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_VSSkinnedPixelLightingTxNoSpecBn"
  VERBATIM
)

set(SHDR_NormalMapEffect_PSNormalPixelLightingTx ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTx.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_PSNormalPixelLightingTx})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTx.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tps_5_1 /EPSNormalPixelLightingTx /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTx.inc /VnNormalMapEffect_PSNormalPixelLightingTx
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_PSNormalPixelLightingTx"
  VERBATIM
)

set(SHDR_NormalMapEffect_PSNormalPixelLightingTxNoFog ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoFog.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_PSNormalPixelLightingTxNoFog})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoFog.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tps_5_1 /EPSNormalPixelLightingTxNoFog /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoFog.inc /VnNormalMapEffect_PSNormalPixelLightingTxNoFog
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_PSNormalPixelLightingTxNoFog"
  VERBATIM
)

set(SHDR_NormalMapEffect_PSNormalPixelLightingTxNoSpec ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoSpec.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_PSNormalPixelLightingTxNoSpec})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoSpec.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tps_5_1 /EPSNormalPixelLightingTxNoSpec /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoSpec.inc /VnNormalMapEffect_PSNormalPixelLightingTxNoSpec
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_PSNormalPixelLightingTxNoSpec"
  VERBATIM
)

set(SHDR_NormalMapEffect_PSNormalPixelLightingTxNoFogSpec ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoFogSpec.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_NormalMapEffect_PSNormalPixelLightingTxNoFogSpec})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoFogSpec.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx /Tps_5_1 /EPSNormalPixelLightingTxNoFogSpec /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/NormalMapEffect_PSNormalPixelLightingTxNoFogSpec.inc /VnNormalMapEffect_PSNormalPixelLightingTxNoFogSpec
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/NormalMapEffect.fx
  COMMENT "Compiling NormalMapEffect_PSNormalPixelLightingTxNoFogSpec"
  VERBATIM
)

set(SHDR_PBREffect_VSConstant ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstant.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSConstant})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstant.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSConstant /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstant.inc /VnPBREffect_VSConstant
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSConstant"
  VERBATIM
)

set(SHDR_PBREffect_VSConstantInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSConstantInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSConstantInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantInst.inc /VnPBREffect_VSConstantInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSConstantInst"
  VERBATIM
)

set(SHDR_PBREffect_VSConstantVelocity ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantVelocity.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSConstantVelocity})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantVelocity.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSConstantVelocity /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantVelocity.inc /VnPBREffect_VSConstantVelocity
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSConstantVelocity"
  VERBATIM
)

set(SHDR_PBREffect_VSConstantBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSConstantBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSConstantBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantBn.inc /VnPBREffect_VSConstantBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSConstantBn"
  VERBATIM
)

set(SHDR_PBREffect_VSConstantBnInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantBnInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSConstantBnInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantBnInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSConstantBnInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantBnInst.inc /VnPBREffect_VSConstantBnInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSConstantBnInst"
  VERBATIM
)

set(SHDR_PBREffect_VSConstantVelocityBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantVelocityBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSConstantVelocityBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantVelocityBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSConstantVelocityBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSConstantVelocityBn.inc /VnPBREffect_VSConstantVelocityBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSConstantVelocityBn"
  VERBATIM
)

set(SHDR_PBREffect_VSSkinned ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSSkinned.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSSkinned})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSSkinned.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSSkinned /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSSkinned.inc /VnPBREffect_VSSkinned
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSSkinned"
  VERBATIM
)

set(SHDR_PBREffect_VSSkinnedBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSSkinnedBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_VSSkinnedBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSSkinnedBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tvs_5_1 /EVSSkinnedBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_VSSkinnedBn.inc /VnPBREffect_VSSkinnedBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_VSSkinnedBn"
  VERBATIM
)

set(SHDR_PBREffect_PSConstant ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSConstant.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_PSConstant})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSConstant.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tps_5_1 /EPSConstant /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSConstant.inc /VnPBREffect_PSConstant
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_PSConstant"
  VERBATIM
)

set(SHDR_PBREffect_PSTextured ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTextured.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_PSTextured})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTextured.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tps_5_1 /EPSTextured /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTextured.inc /VnPBREffect_PSTextured
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_PSTextured"
  VERBATIM
)

set(SHDR_PBREffect_PSTexturedEmissive ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedEmissive.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_PSTexturedEmissive})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedEmissive.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tps_5_1 /EPSTexturedEmissive /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedEmissive.inc /VnPBREffect_PSTexturedEmissive
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_PSTexturedEmissive"
  VERBATIM
)

set(SHDR_PBREffect_PSTexturedVelocity ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedVelocity.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_PSTexturedVelocity})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedVelocity.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tps_5_1 /EPSTexturedVelocity /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedVelocity.inc /VnPBREffect_PSTexturedVelocity
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_PSTexturedVelocity"
  VERBATIM
)

set(SHDR_PBREffect_PSTexturedEmissiveVelocity ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedEmissiveVelocity.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PBREffect_PSTexturedEmissiveVelocity})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedEmissiveVelocity.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx /Tps_5_1 /EPSTexturedEmissiveVelocity /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PBREffect_PSTexturedEmissiveVelocity.inc /VnPBREffect_PSTexturedEmissiveVelocity
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PBREffect.fx
  COMMENT "Compiling PBREffect_PSTexturedEmissiveVelocity"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebug ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebug.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebug})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebug.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebug /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebug.inc /VnDebugEffect_VSDebug
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebug"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebugBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebugBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebugBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugBn.inc /VnDebugEffect_VSDebugBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebugBn"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebugVc ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVc.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebugVc})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVc.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebugVc /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVc.inc /VnDebugEffect_VSDebugVc
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebugVc"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebugVcBn ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcBn.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebugVcBn})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcBn.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebugVcBn /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcBn.inc /VnDebugEffect_VSDebugVcBn
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebugVcBn"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebugInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebugInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebugInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugInst.inc /VnDebugEffect_VSDebugInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebugInst"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebugBnInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugBnInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebugBnInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugBnInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebugBnInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugBnInst.inc /VnDebugEffect_VSDebugBnInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebugBnInst"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebugVcInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebugVcInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebugVcInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcInst.inc /VnDebugEffect_VSDebugVcInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebugVcInst"
  VERBATIM
)

set(SHDR_DebugEffect_VSDebugVcBnInst ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcBnInst.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_VSDebugVcBnInst})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcBnInst.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tvs_5_1 /EVSDebugVcBnInst /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_VSDebugVcBnInst.inc /VnDebugEffect_VSDebugVcBnInst
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_VSDebugVcBnInst"
  VERBATIM
)

set(SHDR_DebugEffect_PSHemiAmbient ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSHemiAmbient.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_PSHemiAmbient})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSHemiAmbient.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tps_5_1 /EPSHemiAmbient /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSHemiAmbient.inc /VnDebugEffect_PSHemiAmbient
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_PSHemiAmbient"
  VERBATIM
)

set(SHDR_DebugEffect_PSRGBNormals ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBNormals.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_PSRGBNormals})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBNormals.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tps_5_1 /EPSRGBNormals /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBNormals.inc /VnDebugEffect_PSRGBNormals
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_PSRGBNormals"
  VERBATIM
)

set(SHDR_DebugEffect_PSRGBTangents ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBTangents.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_PSRGBTangents})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBTangents.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tps_5_1 /EPSRGBTangents /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBTangents.inc /VnDebugEffect_PSRGBTangents
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_PSRGBTangents"
  VERBATIM
)

set(SHDR_DebugEffect_PSRGBBiTangents ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBBiTangents.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_DebugEffect_PSRGBBiTangents})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBBiTangents.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx /Tps_5_1 /EPSRGBBiTangents /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/DebugEffect_PSRGBBiTangents.inc /VnDebugEffect_PSRGBBiTangents
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/DebugEffect.fx
  COMMENT "Compiling DebugEffect_PSRGBBiTangents"
  VERBATIM
)

set(SHDR_SpriteEffect_SpriteVertexShader ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpriteVertexShader.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SpriteEffect_SpriteVertexShader})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpriteVertexShader.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx /Tvs_5_1 /ESpriteVertexShader /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpriteVertexShader.inc /VnSpriteEffect_SpriteVertexShader
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx
  COMMENT "Compiling SpriteEffect_SpriteVertexShader"
  VERBATIM
)

set(SHDR_SpriteEffect_SpritePixelShader ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpritePixelShader.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SpriteEffect_SpritePixelShader})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpritePixelShader.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx /Tps_5_1 /ESpritePixelShader /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpritePixelShader.inc /VnSpriteEffect_SpritePixelShader
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx
  COMMENT "Compiling SpriteEffect_SpritePixelShader"
  VERBATIM
)

set(SHDR_SpriteEffect_SpriteVertexShaderHeap ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpriteVertexShaderHeap.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SpriteEffect_SpriteVertexShaderHeap})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpriteVertexShaderHeap.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx /Tvs_5_1 /ESpriteVertexShaderHeap /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpriteVertexShaderHeap.inc /VnSpriteEffect_SpriteVertexShaderHeap
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx
  COMMENT "Compiling SpriteEffect_SpriteVertexShaderHeap"
  VERBATIM
)

set(SHDR_SpriteEffect_SpritePixelShaderHeap ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpritePixelShaderHeap.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_SpriteEffect_SpritePixelShaderHeap})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpritePixelShaderHeap.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx /Tps_5_1 /ESpritePixelShaderHeap /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/SpriteEffect_SpritePixelShaderHeap.inc /VnSpriteEffect_SpritePixelShaderHeap
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/SpriteEffect.fx
  COMMENT "Compiling SpriteEffect_SpritePixelShaderHeap"
  VERBATIM
)

set(SHDR_PostProcess_VSQuad ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuad.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_VSQuad})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuad.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tvs_5_1 /EVSQuad /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuad.inc /VnPostProcess_VSQuad
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_VSQuad"
  VERBATIM
)

set(SHDR_PostProcess_VSQuadNoCB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuadNoCB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_VSQuadNoCB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuadNoCB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tvs_5_1 /EVSQuadNoCB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuadNoCB.inc /VnPostProcess_VSQuadNoCB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_VSQuadNoCB"
  VERBATIM
)

set(SHDR_PostProcess_VSQuadDual ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuadDual.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_VSQuadDual})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuadDual.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tvs_5_1 /EVSQuadDual /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_VSQuadDual.inc /VnPostProcess_VSQuadDual
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_VSQuadDual"
  VERBATIM
)

set(SHDR_PostProcess_PSCopy ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSCopy.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSCopy})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSCopy.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSCopy /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSCopy.inc /VnPostProcess_PSCopy
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSCopy"
  VERBATIM
)

set(SHDR_PostProcess_PSMonochrome ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSMonochrome.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSMonochrome})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSMonochrome.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSMonochrome /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSMonochrome.inc /VnPostProcess_PSMonochrome
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSMonochrome"
  VERBATIM
)

set(SHDR_PostProcess_PSSepia ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSSepia.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSSepia})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSSepia.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSSepia /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSSepia.inc /VnPostProcess_PSSepia
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSSepia"
  VERBATIM
)

set(SHDR_PostProcess_PSDownScale2x2 ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSDownScale2x2.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSDownScale2x2})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSDownScale2x2.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSDownScale2x2 /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSDownScale2x2.inc /VnPostProcess_PSDownScale2x2
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSDownScale2x2"
  VERBATIM
)

set(SHDR_PostProcess_PSDownScale4x4 ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSDownScale4x4.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSDownScale4x4})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSDownScale4x4.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSDownScale4x4 /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSDownScale4x4.inc /VnPostProcess_PSDownScale4x4
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSDownScale4x4"
  VERBATIM
)

set(SHDR_PostProcess_PSGaussianBlur5x5 ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSGaussianBlur5x5.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSGaussianBlur5x5})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSGaussianBlur5x5.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSGaussianBlur5x5 /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSGaussianBlur5x5.inc /VnPostProcess_PSGaussianBlur5x5
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSGaussianBlur5x5"
  VERBATIM
)

set(SHDR_PostProcess_PSBloomExtract ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomExtract.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSBloomExtract})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomExtract.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSBloomExtract /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomExtract.inc /VnPostProcess_PSBloomExtract
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSBloomExtract"
  VERBATIM
)

set(SHDR_PostProcess_PSBloomBlur ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomBlur.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSBloomBlur})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomBlur.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSBloomBlur /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomBlur.inc /VnPostProcess_PSBloomBlur
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSBloomBlur"
  VERBATIM
)

set(SHDR_PostProcess_PSMerge ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSMerge.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSMerge})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSMerge.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSMerge /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSMerge.inc /VnPostProcess_PSMerge
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSMerge"
  VERBATIM
)

set(SHDR_PostProcess_PSBloomCombine ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomCombine.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_PostProcess_PSBloomCombine})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomCombine.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx /Tps_5_1 /EPSBloomCombine /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/PostProcess_PSBloomCombine.inc /VnPostProcess_PSBloomCombine
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/PostProcess.fx
  COMMENT "Compiling PostProcess_PSBloomCombine"
  VERBATIM
)

set(SHDR_ToneMap_VSQuad ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_VSQuad.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_VSQuad})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_VSQuad.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tvs_5_1 /EVSQuad /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_VSQuad.inc /VnToneMap_VSQuad
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_VSQuad"
  VERBATIM
)

set(SHDR_ToneMap_PSCopy ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSCopy.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSCopy})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSCopy.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSCopy /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSCopy.inc /VnToneMap_PSCopy
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSCopy"
  VERBATIM
)

set(SHDR_ToneMap_PSSaturate ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSSaturate.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSSaturate})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSSaturate.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSSaturate /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSSaturate.inc /VnToneMap_PSSaturate
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSSaturate"
  VERBATIM
)

set(SHDR_ToneMap_PSReinhard ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSReinhard.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSReinhard})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSReinhard.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSReinhard /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSReinhard.inc /VnToneMap_PSReinhard
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSReinhard"
  VERBATIM
)

set(SHDR_ToneMap_PSACESFilmic ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSACESFilmic.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSACESFilmic})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSACESFilmic.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSACESFilmic /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSACESFilmic.inc /VnToneMap_PSACESFilmic
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSACESFilmic"
  VERBATIM
)

set(SHDR_ToneMap_PS_SRGB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PS_SRGB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PS_SRGB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PS_SRGB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPS_SRGB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PS_SRGB.inc /VnToneMap_PS_SRGB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PS_SRGB"
  VERBATIM
)

set(SHDR_ToneMap_PSSaturate_SRGB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSSaturate_SRGB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSSaturate_SRGB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSSaturate_SRGB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSSaturate_SRGB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSSaturate_SRGB.inc /VnToneMap_PSSaturate_SRGB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSSaturate_SRGB"
  VERBATIM
)

set(SHDR_ToneMap_PSReinhard_SRGB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSReinhard_SRGB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSReinhard_SRGB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSReinhard_SRGB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSReinhard_SRGB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSReinhard_SRGB.inc /VnToneMap_PSReinhard_SRGB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSReinhard_SRGB"
  VERBATIM
)

set(SHDR_ToneMap_PSACESFilmic_SRGB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSACESFilmic_SRGB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSACESFilmic_SRGB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSACESFilmic_SRGB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSACESFilmic_SRGB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSACESFilmic_SRGB.inc /VnToneMap_PSACESFilmic_SRGB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSACESFilmic_SRGB"
  VERBATIM
)

set(SHDR_ToneMap_PSHDR10 ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSHDR10})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSHDR10 /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10.inc /VnToneMap_PSHDR10
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSHDR10"
  VERBATIM
)

set(SHDR_ToneMap_PSHDR10_Saturate ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Saturate.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSHDR10_Saturate})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Saturate.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSHDR10_Saturate /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Saturate.inc /VnToneMap_PSHDR10_Saturate
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSHDR10_Saturate"
  VERBATIM
)

set(SHDR_ToneMap_PSHDR10_Reinhard ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Reinhard.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSHDR10_Reinhard})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Reinhard.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSHDR10_Reinhard /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Reinhard.inc /VnToneMap_PSHDR10_Reinhard
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSHDR10_Reinhard"
  VERBATIM
)

set(SHDR_ToneMap_PSHDR10_ACESFilmic ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_ACESFilmic.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSHDR10_ACESFilmic})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_ACESFilmic.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSHDR10_ACESFilmic /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_ACESFilmic.inc /VnToneMap_PSHDR10_ACESFilmic
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSHDR10_ACESFilmic"
  VERBATIM
)

set(SHDR_ToneMap_PSHDR10_Saturate_SRGB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Saturate_SRGB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSHDR10_Saturate_SRGB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Saturate_SRGB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSHDR10_Saturate_SRGB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Saturate_SRGB.inc /VnToneMap_PSHDR10_Saturate_SRGB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSHDR10_Saturate_SRGB"
  VERBATIM
)

set(SHDR_ToneMap_PSHDR10_Reinhard_SRGB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Reinhard_SRGB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSHDR10_Reinhard_SRGB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Reinhard_SRGB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSHDR10_Reinhard_SRGB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_Reinhard_SRGB.inc /VnToneMap_PSHDR10_Reinhard_SRGB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSHDR10_Reinhard_SRGB"
  VERBATIM
)

set(SHDR_ToneMap_PSHDR10_ACESFilmic_SRGB ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_ACESFilmic_SRGB.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_ToneMap_PSHDR10_ACESFilmic_SRGB})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_ACESFilmic_SRGB.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx /Tps_5_1 /EPSHDR10_ACESFilmic_SRGB /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/ToneMap_PSHDR10_ACESFilmic_SRGB.inc /VnToneMap_PSHDR10_ACESFilmic_SRGB
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/ToneMap.fx
  COMMENT "Compiling ToneMap_PSHDR10_ACESFilmic_SRGB"
  VERBATIM
)

set(SHDR_GenerateMips_main ${DIRECTXTK12_SHADER_OUTPUT_DIR}/GenerateMips_main.inc)
list(APPEND DIRECTXTK12_SHADER_INCS ${SHDR_GenerateMips_main})
add_custom_command(
  OUTPUT ${DIRECTXTK12_SHADER_OUTPUT_DIR}/GenerateMips_main.inc
  COMMAND ${CMAKE_COMMAND} -E env WINEDEBUG=-all ${DXC_WINETST} ${DIRECTXTK12_SRC}/Shaders/GenerateMips.hlsl /Tcs_5_1 /Emain /Fh${DIRECTXTK12_SHADER_OUTPUT_DIR}/GenerateMips_main.inc /VnGenerateMips_main
  DEPENDS ${DIRECTXTK12_SRC}/Shaders/GenerateMips.hlsl
  COMMENT "Compiling GenerateMips_main"
  VERBATIM
)
