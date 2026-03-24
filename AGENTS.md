 Completed                                                                                                  
                                                                                                                
     Build System (CMakeLists.txt)                                                                              
     - Root CMakeLists.txt with FetchContent for all dependencies (fmt, spdlog, ryml, lodepng, magic_enum,      
     DirectXMath, DirectXTK12)                                                                                  
     - Uses system-installed libvkd3d and libvkd3d-utils for DX12 API headers and library                       
     - Finds SDL3, Vulkan via pkg-config                                                                        
     - Shader compilation with DXC → SPIR-V when dxc is available                                               
     - Separate CMakeLists.txt for Imgui, Inferno.Core, and Inferno executable                                  
                                                                                                                
     Platform Abstraction (platform/linux/)                                                                     
     - WinCompat.h - Force-included header with Windows types (RECT, HWND, etc.), WRL::ComPtr, SAL              
     annotations, Win32 stub functions                                                                          
     - d3d12.h, dxgi.h, dxgi1_4.h, dxgiformat.h - Wrappers mapping to vkd3d headers                             
     - imgui_impl_sdl3.cpp - SDL3-based ImGui backend replacing Win32 backend                                   
     - sal.h, pix3.h, dwmapi.h, windowsx.h, tchar.h, wrl.h, wrl/client.h - Stubs                                
                                                                                                                
     Source Fixes                                                                                               
     - Types.h - Added noexcept to exception what() overrides                                                   
     - Utility.h - _strnicmp/CharUpperA/CharLowerA via WinCompat.h    
     - HogFile.cpp - wstring→string conversion for ifstream on Linux                                            
     - pch.h - Added <utility> and <charconv> includes                                                          
                                                                                                                
     Remaining Work (~152 errors)                                                                               
     The project has extensive Windows API dependencies that still need porting:                                
     - Audio (Audio/ directory) - Uses XAudio2/mmreg.h → needs SDL3 audio replacement                           
     - BITMAP types in CustomTextureLibrary.cpp → need GDI stubs                                                
     - Editor clipboard (Editor.Clipboard.cpp) - Uses Windows clipboard API                                     
     - DirectX.h - Includes DirectXTK12 Audio/GamePad/Keyboard/Mouse which need Windows APIs                    
     - Shell.cpp, WindowsDialogs.cpp, SoundSystem.cpp - Already excluded from Linux build but need SDL3         
     replacements                                                                                               
                                                                                                                
     To build: ./setup-linux.sh or cmake -B build && cmake --build build                
